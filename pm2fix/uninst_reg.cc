// Copyright 2017 jeonghun

#include "uninst_reg.h"

uninst_reg::uninst_reg(const wchar_t *key_name, bool wow64_64key)
    : regkey_{ 0 }, wow64_64key_(wow64_64key) {
  if (key_name != nullptr) {
    wcscpy_s(regkey_,
      L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\");
    wcscat_s(regkey_, key_name);
  }
}

bool uninst_reg::chk_exist() const {
  bool exist_reg = false;

  HKEY hkey = NULL;
  if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, regkey_, 0,
      KEY_READ | (wow64_64key_ ? KEY_WOW64_64KEY : 0),
      &hkey) == ERROR_SUCCESS && hkey != NULL) {
    RegCloseKey(hkey);
    exist_reg = true;
  }

  return exist_reg;
}

std::wstring uninst_reg::get_instloc() const {
  std::wstring instloc;

  HKEY hkey = NULL;
  if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, regkey_, 0,
      KEY_READ | (wow64_64key_ ? KEY_WOW64_64KEY : 0),
      &hkey) == ERROR_SUCCESS && hkey != NULL) {
    wchar_t instloc_buf[MAX_PATH] = { 0 };
    DWORD value_type = 0;
    DWORD value_size = sizeof(instloc_buf);
    if (RegQueryValueEx(hkey, L"InstallLocation", 0, &value_type,
        reinterpret_cast<BYTE*>(instloc_buf), &value_size) == ERROR_SUCCESS) {
      instloc = instloc_buf;
    }
    RegCloseKey(hkey);
  }

  return instloc;
}
