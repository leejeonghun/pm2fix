// Copyright 2017 jeonghun

#include <windows.h>
#include <cstdint>
#include "pe_section.h"

enum FINAL_CONSONANT_TYPE {
  NOT_CHAR = 0,
  NONE,
  EXIST,
  RIEUL,
};

static const char *bad_mark[] = {
  "{{ ",
  "{{ÀÇ ",
  "}} ",
  "[[ "
};

static const char *correct[] = {
  "¿Í(°ú) ",
  "¿Í(°ú)ÀÇ ",
  "Àº(´Â) ",
  "À»(¸¦) "
};

void mark_bad_postposition() {
  static const char *bad_pattern[] = {
    "%s¿Í ",
    "%s¿ÍÀÇ ",
    "%s´Â ",
    "%sÀ» "
  };

  HINSTANCE base = GetModuleHandle(NULL);
  auto nt_header_ptr = get_nt_header(base);
  if (nt_header_ptr != nullptr) {
    IMAGE_SECTION_HEADER *sh_ptr = find_pe_section(nt_header_ptr, ".rdata");
    if (sh_ptr != nullptr) {
      char *sec_ptr = reinterpret_cast<char*>(
        reinterpret_cast<DWORD_PTR>(base) + sh_ptr->PointerToRawData);

      if (sec_ptr != nullptr) {
        const uint32_t sec_size = sh_ptr->SizeOfRawData;
        for (uint32_t i = 0; i < sec_size; i++) {
          for (uint32_t j = 0; j < _countof(bad_pattern); j++) {
            char *find_ptr = strstr(sec_ptr + i, bad_pattern[j]);
            if (find_ptr != nullptr) {
              find_ptr += 2;
              const uint32_t pattern_size = strlen(bad_mark[j]);
              DWORD old_protect = 0;
              VirtualProtect(find_ptr, pattern_size, PAGE_READWRITE, &old_protect);
              memcpy(find_ptr, bad_mark[j], pattern_size);
              VirtualProtect(find_ptr, pattern_size, old_protect, &old_protect);
            }
          }
          i += strlen(sec_ptr + i);
        }
      }
    }
  }
}

inline void replace_bad_mark(char *msg) {
  if (msg != nullptr) {
    for (int i = 0; i < _countof(bad_mark); i++) {
      while (char *find = strstr(msg, bad_mark[i])) {
        const int insert_len = strlen(correct[i]);
        const int offset = insert_len - strlen(bad_mark[i]);

        for (int i = strlen(find); i >= 0; i--) {
          find[i + offset] = find[i];
        }
        memcpy(find, correct[i], insert_len);
      }
    }
  }
}

inline wchar_t get_char_before_postposition(const char *postposition) {
  wchar_t unicode = 0;

  const char *subject = postposition;
  while (*(subject - 1) == ' ') subject--;
  subject -= 2;

  MultiByteToWideChar(949, 0, subject, 2, &unicode, 1);
  return unicode;
}

inline FINAL_CONSONANT_TYPE check_final_consonant(const wchar_t unicode) {
  FINAL_CONSONANT_TYPE type = NOT_CHAR;

  if (unicode >= L'°¡' && unicode <= L'ÆR') {
    switch ((unicode - L'°¡') % (L'°³' - L'°¡')) {
    case 0:
      type = NONE;
      break;
    case 8:
      type = RIEUL;
      break;
    default:
      type = EXIST;
      break;
    }
  }

  return type;
}

inline constexpr int make_pattern(uint16_t lch, uint16_t rch) {
  return lch << 16 | rch;
}

inline constexpr uint16_t get_mbcs_ch(const char *str) {
  return (*reinterpret_cast<const unsigned char*>(str) << 8) |
    *reinterpret_cast<const unsigned char*>(str + 1);
}

inline bool is_mbcs(unsigned char msb) {
  return msb > 0x7f;
}

inline bool find_bad_postposition(const char *str, int len, int *find_pos, bool *reverse) {
  enum { PATTERN_LENGTH = 6 };

  bool find = false;

  if (str != nullptr && find_pos != nullptr && len > PATTERN_LENGTH) {
    for (int i = 0; i + PATTERN_LENGTH <= len; str++, i++) {
      if (str[0] == '(' && is_mbcs(str[1]) && str[3] == ')' && is_mbcs(str[4])) {
        *find_pos = i;
        *reverse = false;
        find = true;
        break;
      } else if (is_mbcs(str[0]) && str[2] == '(' && is_mbcs(str[3]) && str[5] == ')') {
        *find_pos = i;
        *reverse = true;
        find = true;
        break;
      }
    }
  }

  return find;
}

bool postposition_correction(char *msg) {
  replace_bad_mark(msg);

  int len = strlen(msg);
  int pos = -1;
  bool reverse = false;
  while (find_bad_postposition(msg, len, &pos, &reverse)) {
    const int find_pattern = reverse ?
      make_pattern(get_mbcs_ch(msg + pos + 0), get_mbcs_ch(msg + pos + 3)) :
      make_pattern(get_mbcs_ch(msg + pos + 1), get_mbcs_ch(msg + pos + 4));

    bool not_matched = false;

    bool use_first = false;
    bool rieul_exception = false;
    bool use_both = false;

    switch (find_pattern) {
    case make_pattern('Àº', '´Â'):
    case make_pattern('ÀÌ', '°¡'):
    case make_pattern('À»', '¸¦'):
    case make_pattern('¾Æ', '¾ß'):
      break;

    case make_pattern('¿Í', '°ú'):
      use_first = true;
      break;

    case make_pattern('ÀÌ', '¿©'):
    case make_pattern('ÀÌ', '¶ó'):
      use_both = true;
      break;

    case make_pattern('À¸', '·Î'):
      use_both = true;
      rieul_exception = true;
      break;

    default:
      not_matched = true;
      break;
    }

    if (not_matched == false) {
      auto type = check_final_consonant(get_char_before_postposition(msg + pos));
      if (reverse) {
        switch (type) {
        case NONE:
          if (rieul_exception == false) {
            memcpy(msg + pos, msg + pos + (use_first ? 0 : 3), 2);
            memcpy(msg + pos + 2, msg + pos + 6, len - (pos + 6) + 1);
            break;
          }
        case RIEUL:
          if (rieul_exception) {
            memcpy(msg + pos, msg + pos + 3, 2);
            memcpy(msg + pos + 2, msg + pos + 6, len - (pos + 6) + 1);
            break;
          }
        case EXIST:
          memcpy(msg + pos, msg + pos + (use_first ? 3 : 0), 2);
          if (use_both == false) {
            memcpy(msg + pos + 2, msg + pos + 6, len - (pos + 6) + 1);
          } else {
            memcpy(msg + pos + 2, msg + pos + 3, 2);
            memcpy(msg + pos + 4, msg + pos + 6, len - (pos + 6) + 1);
          }
          break;
        }
      } else {
        switch (type) {
        case NONE:
          if (rieul_exception == false) {
            memcpy(msg + pos, msg + pos + (use_first ? 1 : 4), 2);
            memcpy(msg + pos + 2, msg + pos + 6, len - (pos + 6) + 1);
            break;
          }
        case RIEUL:
          if (rieul_exception) {
            memcpy(msg + pos, msg + pos + 4, 2);
            memcpy(msg + pos + 2, msg + pos + 6, len - (pos + 6) + 1);
            break;
          }
        case EXIST:
          memcpy(msg + pos, msg + pos + (use_first ? 4 : 1), 2);
          if (use_both == false) {
            memcpy(msg + pos + 2, msg + pos + 6, len - (pos + 6) + 1);
          } else {
            memcpy(msg + pos + 2, msg + pos + 4, len - (pos + 4) + 1);
          }
          break;
        }
      }
    }

    if (not_matched == false) {
      msg += (pos + 4);
      len -= (pos + 4);
    } else {
      msg++;
      len--;
    }
  }

  return 0;
}
