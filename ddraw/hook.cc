// Copyright 2016 jeonghun

#include "hook.h"
#include "iat_hooker.h"
#include "postposition.h"

namespace hook {

iat_hooker createfontindirecta;
HFONT WINAPI hook_createfontindirecta(CONST LOGFONTA *lplf) {
  enum { HEIGHT_CORRECTION = 7 };
  LOGFONTA *lf_ptr = const_cast<LOGFONTA*>(lplf);
  if (lf_ptr != nullptr) {
    strcpy_s(lf_ptr->lfFaceName, "Malgun Gothic");
    lf_ptr->lfHeight += HEIGHT_CORRECTION;
    lf_ptr->lfWeight = FW_SEMIBOLD;
  }

  return createfontindirecta.call_origin<decltype(&CreateFontIndirectA)>(lplf);
}

iat_hooker textouta;
BOOL WINAPI hook_textouta(HDC hdc, int x, int y, LPCSTR lpString, int c) {
  static const char* prev_ptr = nullptr;
  static int prev_c = 0;

  if ((prev_ptr + prev_c) != lpString) {
    postposition_correction(const_cast<LPSTR>(lpString));
  }

  prev_ptr = lpString;
  prev_c = c;

  return textouta.call_origin<decltype(&TextOutA)>(hdc, x, y, lpString, c);
}

bool install() {
  mark_bad_postposition();

  HINSTANCE hinstance = GetModuleHandle(nullptr);
  return createfontindirecta.hook(hinstance, "gdi32.dll", "CreateFontIndirectA", hook_createfontindirecta) &&
    textouta.hook(hinstance, "gdi32.dll", "TextOutA", hook_textouta);
}

bool uninstall() {
  return createfontindirecta.unhook() && textouta.unhook();
}

}  // namespace hook
