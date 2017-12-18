// Copyright 2014 jeonghun
#include "resource_gz.h"
#include <cassert>
#include "zlib/zlib.h"

resource_gz::resource_gz(uint32_t rsc_id, HINSTANCE hinst) {
  HRSRC hrsc = FindResource(hinst, MAKEINTRESOURCE(rsc_id), RT_RCDATA);
  if (hrsc != NULL) {
    size_ = SizeofResource(hinst, hrsc);
    mem_ptr_ = reinterpret_cast<LPBYTE>(LockResource(LoadResource(hinst, hrsc)));
  }
  assert(mem_ptr_ != nullptr);
  assert(size_ > 0);
}

bool resource_gz::extract(const wchar_t *filepath) const {
  bool success = false;

  HANDLE hfile = CreateFile(filepath, FILE_GENERIC_WRITE, FILE_SHARE_READ,
    nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
  if (hfile != INVALID_HANDLE_VALUE) {
    z_stream zs = { 0 };
    if (inflateInit2(&zs, 31) == Z_OK) {
      int result = Z_OK;
      bool write_failed = false;

      Bytef buffer[64 * 1024] = { 0 };
      zs.next_in = reinterpret_cast<Bytef*>(mem_ptr_);
      zs.avail_in = size_;

      do {
        zs.next_out = buffer;
        zs.avail_out = sizeof(buffer);
        result = inflate(&zs, Z_NO_FLUSH);
        if (result == Z_OK || result == Z_STREAM_END) {
          uint32_t decomp_size = sizeof(buffer) - zs.avail_out;
          if (decomp_size > 0) {
            DWORD bytes_write = 0;
            write_failed = WriteFile(hfile, buffer, decomp_size, &bytes_write, NULL) == FALSE;
          }
        }
      } while (result == Z_OK && write_failed == false);
      success = write_failed == false && (result == Z_STREAM_END || result == Z_BUF_ERROR);
      inflateEnd(&zs);
    }
    CloseHandle(hfile);
  }

  return success;
}
