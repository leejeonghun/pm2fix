// Copyright 2014 jeonghun
#ifndef PM2FIX_FILE_DESC_H_
#define PM2FIX_FILE_DESC_H_

#include <string>

class file_desc final {
 public:
  explicit file_desc(const wchar_t *filename = nullptr);
  ~file_desc() = default;

  inline const std::wstring& get() const { return desc_; }

 private:
  std::wstring desc_;
};

#endif  // PM2FIX_FILE_DESC_H_
