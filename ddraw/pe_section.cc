// Copyright 2017 jeonghun

#include "pe_section.h"

IMAGE_NT_HEADERS* get_nt_header(HINSTANCE hinst) {
  IMAGE_NT_HEADERS *result = nullptr;

  auto dh_ptr = reinterpret_cast<IMAGE_DOS_HEADER*>(hinst);
  if (dh_ptr != nullptr && dh_ptr->e_magic == IMAGE_DOS_SIGNATURE) {
    auto nh_ptr = reinterpret_cast<IMAGE_NT_HEADERS*>(
      reinterpret_cast<DWORD_PTR>(dh_ptr) + dh_ptr->e_lfanew);
    if (nh_ptr != nullptr && nh_ptr->Signature == IMAGE_NT_SIGNATURE) {
      result = nh_ptr;
    }
  }

  return result;
}

IMAGE_SECTION_HEADER* find_pe_section(IMAGE_NT_HEADERS *nthdrs, const char *sec_name) {
  IMAGE_SECTION_HEADER *result = nullptr;

  IMAGE_SECTION_HEADER *sec_hdr = IMAGE_FIRST_SECTION(nthdrs);
  WORD num_of_sec = nthdrs->FileHeader.NumberOfSections;

  while (num_of_sec--) {
    if (strcmp(reinterpret_cast<char*>(sec_hdr->Name), sec_name) == 0) {
      result = sec_hdr;
      break;
    }
    sec_hdr++;
  }

  return result;
}
