// Copyright 2017 jeonghun

#ifndef LAUNCHER_ENCRYPTED_STRING_H_
#define LAUNCHER_ENCRYPTED_STRING_H_

#include <utility>

template <typename T>
constexpr T cipher(const T ch) {
  return ch ^ 0xc0de;
}

template <typename T, size_t... I>
class encrypted_string final {
public:
  constexpr explicit encrypted_string(const T(&plain_str)[sizeof...(I)])
    : buffer_{ cipher(plain_str[I])... } {}
  ~encrypted_string() = default;

  inline operator const T*() {
    for (auto& ch : buffer_) ch = cipher(ch);
    return buffer_;
  }

private:
  T buffer_[sizeof...(I)];
};

template <typename T> struct make_type;
template <typename T, size_t... I> struct make_type<std::integer_sequence<T, I...>> {
  template <typename U>
  using type = encrypted_string<U, I...>;
};

template <typename T, size_t N>
constexpr decltype(auto) encrypt(const T(&plain_str)[N]) {
  return make_type<std::make_index_sequence<N>>::type<T>(plain_str);
}

#endif  // LAUNCHER_ENCRYPTED_STRING_H_
