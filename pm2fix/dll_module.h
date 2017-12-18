// Copyright 2014 jeonghun
#ifndef LAUNCHER_DLL_MODULE_H_
#define LAUNCHER_DLL_MODULE_H_

#include <windows.h>
#include <cassert>
#include <utility>

class dll_module final {
 public:
  explicit dll_module(const wchar_t *dll_name) {
    assert(dll_name != nullptr);
    module_ = GetModuleHandleW(dll_name);
    assert(module_ != NULL);
  }

  ~dll_module() = default;

  dll_module(const dll_module&) = delete;
  void operator=(const dll_module&) = delete;

  template <typename T>
  T get_func_ptr(const char* api_name) const {
    assert(module_ != NULL);
    assert(api_name != nullptr);
    return reinterpret_cast<T>(GetProcAddress(module_, api_name));
  }

  template <typename R, typename... ARGS>
  typename R call(const char* api_name, ARGS ...args) const {
    using func_ptr_t = R(_stdcall *)(ARGS...);
    assert(module_ != NULL);
    assert(api_name != nullptr);
    func_ptr_t func_ptr = reinterpret_cast<func_ptr_t>(GetProcAddress(module_, api_name));
    return func_ptr ? func_ptr(args...) : R();
  }

 private:
  HMODULE module_ = NULL;
};

#endif  // LAUNCHER_DLL_MODULE_H_
