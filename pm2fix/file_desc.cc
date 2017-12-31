// Copyright 2014 jeonghun
#include "file_desc.h"
#include <windows.h>
#include <cstdio>
#include <vector>

#pragma comment(lib, "version")

file_desc::file_desc(const wchar_t *filename) {
  uint32_t buf_size = 0;
  const void *buf_ptr = nullptr;
  std::vector<uint8_t> buffer;

  if (filename == nullptr) {
    HMODULE module_handle = GetModuleHandle(nullptr);
    HRSRC res_handle = FindResource(module_handle, MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION);
    if (res_handle != NULL) {
      HGLOBAL global_handle = LoadResource(module_handle, res_handle);
      buf_size = SizeofResource(module_handle, res_handle);
      buf_ptr = LockResource(global_handle);
    }
  } else {
    buf_size = GetFileVersionInfoSize(filename, nullptr);
    buffer.resize(buf_size);
    if (GetFileVersionInfo(filename, 0, buffer.size(), buffer.data()) != FALSE) {
      buf_ptr = buffer.data();
    }
  }

  if (buf_size > 0 && buf_ptr != nullptr) {
    UINT val_size = 0;
    struct {
      WORD lang;
      WORD code;
    } *trans_ptr = nullptr;

    if (VerQueryValue(buf_ptr, L"\\VarFileInfo\\Translation",
        reinterpret_cast<void**>(&trans_ptr), &val_size) != FALSE) {
      wchar_t query[64] = { 0 };
      swprintf_s(query, L"\\StringFileInfo\\%04x%04x\\FileDescription",
        trans_ptr->lang, trans_ptr->code);
      void *desc_ptr = nullptr;
      if (VerQueryValue(buf_ptr, query, &desc_ptr, &val_size) != FALSE) {
        desc_ = reinterpret_cast<wchar_t*>(desc_ptr);
      }
    }
  }  
}
