// Copyright 2016 jeonghun

#include <windows.h>
#include <shlobj.h>
#include <shlwapi.h>
#include "resource.h"
#include "uninst_reg.h"
#include "resource_gz.h"
#include "file_desc.h"

#pragma comment(lib, "shlwapi")

namespace {

const wchar_t CAPTION_MSG[] = L"pm2fix";
const wchar_t FILE_HOOK_DLL[] = L"ddraw.dll";
const wchar_t REG_UNINST_KEY[] = L"Steam App 523000";
const wchar_t DLL_FILE_DESC[] = L"pm2fix core";

}  // namespace

enum {
  SUCCESS_UNINSTALL = 1,
  SUCCESS_INSTALL = 0,
  ERR_GAME_NOT_EXIST = -1,
  ERR_DLL_CREATION_FAILED = -2,
  ERR_DLL_DELETION_FAILED = -3,
  ERR_DLL_CORRUPTED = -4,
  ERR_UNKNOWN = -99,
};

int check_retcode(int retcode);

int APIENTRY wWinMain(HINSTANCE hinstance, HINSTANCE, LPWSTR, int) {
  int retcode = ERR_UNKNOWN;

  BOOL wow64proc = FALSE;
  bool use_64bit_key = IsWow64Process(GetCurrentProcess(), &wow64proc) != FALSE &&
    wow64proc != FALSE;

  std::wstring inst_path;
  uninst_reg uninst(REG_UNINST_KEY, use_64bit_key);
  if (uninst.chk_exist()) {
    inst_path = uninst.get_instloc();
  }

  if (inst_path.empty() || PathFileExists(inst_path.c_str()) == FALSE) {
    retcode = ERR_GAME_NOT_EXIST;
  } else {
    wchar_t dll_path[MAX_PATH] = { 0 };
    PathCombine(dll_path, inst_path.c_str(), FILE_HOOK_DLL);
    if (PathFileExists(dll_path) != FALSE) {
      if (file_desc(dll_path).get() == DLL_FILE_DESC) {
        retcode = DeleteFile(dll_path) ? SUCCESS_UNINSTALL : ERR_DLL_DELETION_FAILED;
      } else {
        retcode = ERR_DLL_CORRUPTED;
      }
    } else {
      retcode = resource_gz(IDR_COREDLL, hinstance).extract(dll_path) ?
        SUCCESS_INSTALL : ERR_DLL_CREATION_FAILED;
    }
  }
  
  return check_retcode(retcode);
}

int check_retcode(int retcode) {
  const wchar_t *msg = nullptr;
  switch (retcode) {
  case SUCCESS_UNINSTALL:
    msg = L"��ġ�� ���� �Ϸ� �Ͽ����ϴ�.";
    break;
  case SUCCESS_INSTALL:
    msg = L"��ġ�� ���� �Ϸ� �Ͽ����ϴ�.";
    break;
  case ERR_GAME_NOT_EXIST:
    msg = L"���� ��ġ ������ ã�� �� �����ϴ�.\r\n���� ��ġ �� ������ �ֽʽÿ�.\r\n"
      L"�̹� ��ġ�� ����� �缳ġ �� �ٽ� �õ��� ���ʽÿ�.";
    break;
  case ERR_DLL_CREATION_FAILED:
    msg = IsUserAnAdmin() ? 
      L"��ġ ���뿡 ���� �Ͽ����ϴ�." :
      L"��ġ ���뿡 ���� �Ͽ����ϴ�.\r\n������ �������� �����Ͽ� �ٽ� �õ��� ���ʽÿ�.";
    break;
  case ERR_DLL_DELETION_FAILED:
    msg = IsUserAnAdmin() ?
      L"��ġ ���ſ� ���� �Ͽ����ϴ�.\r\n������ ���� ���̸� ���� �� �ٽ� �õ��� ���ʽÿ�." :
      L"��ġ ���ſ� ���� �Ͽ����ϴ�.\r\n������ �������� �����Ͽ� �ٽ� �õ��� ���ʽÿ�.";
    break;
  case ERR_DLL_CORRUPTED:
    msg = L"��ġ ���ſ� ���� �Ͽ����ϴ�.\r\n���� ���� �� �缳ġ�Ͽ� �ٽ� �õ��� ���ʽÿ�.";
    break;
  case ERR_UNKNOWN:
  default:
    msg = L"�� �� ���� ������ �߻� �Ͽ����ϴ�.";
    break;
  }

  DWORD flag = MB_OK | ((retcode < 0) ? MB_ICONERROR : 0);
  MessageBox(NULL, msg, CAPTION_MSG, flag);
  
  return retcode;
}
