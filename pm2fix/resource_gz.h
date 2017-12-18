// Copyright 2014 jeonghun
#ifndef _TRUNK_ORANGE_BASE_RESOURCE_FILE_H_
#define _TRUNK_ORANGE_BASE_RESOURCE_FILE_H_

#include <windows.h>
#include <cstdint>

class resource_gz final {
 public:
  explicit resource_gz(uint32_t rsc_id, HINSTANCE hinst = GetModuleHandle(nullptr));
  ~resource_gz() = default;

  bool extract(const wchar_t *filepath) const;

 private:
  LPBYTE mem_ptr_ = nullptr;
  uint32_t size_ = 0;
};

#endif  // _TRUNK_ORANGE_BASE_RESOURCE_FILE_H_
