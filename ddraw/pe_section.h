// Copyright 2017 jeonghun

#ifndef DDRAW_PE_SECTION_H_
#define DDRAW_PE_SECTION_H_

#include <windows.h>

IMAGE_NT_HEADERS* get_nt_header(HINSTANCE hinst);
IMAGE_SECTION_HEADER* find_pe_section(IMAGE_NT_HEADERS *nthdrs, const char *sec_name);

#endif  // DDRAW_PE_SECTION_H_
