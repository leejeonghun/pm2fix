// Copyright 2017 jeonghun

#ifndef LAUNCHER_UNINST_REG_H_
#define LAUNCHER_UNINST_REG_H_

#include <string>

class uninst_reg final {
 public:
  explicit uninst_reg(const wchar_t *key_name, bool wow64_64key);
  ~uninst_reg() = default;

  bool chk_exist() const;
  std::wstring get_instloc() const;

 private:
  // https://msdn.microsoft.com/ko-kr/library/windows/desktop/ms724872(v=vs.85).aspx
  enum { REGKEY_MAX_LENGTH = 255 + 1 };

  wchar_t regkey_[REGKEY_MAX_LENGTH];
  const bool wow64_64key_;
};

#endif  // LAUNCHER_UNINST_REG_H_
