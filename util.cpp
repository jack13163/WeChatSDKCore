#include "stdafx.h"
#include "util.h"
#include <string>
#include <locale>
#include <codecvt>
#include <windows.h>
#include <tlhelp32.h>	//进程快照函数头文件
#include <stdio.h>

using namespace std;

bool GetFileVersion(LPTSTR lpszFilePath, LPTSTR version)
{

	if (_tcslen(lpszFilePath) > 0 && PathFileExists(lpszFilePath))
	{
		VS_FIXEDFILEINFO *pVerInfo = NULL;
		DWORD dwTemp, dwSize;
		BYTE *pData = NULL;
		UINT uLen;

		dwSize = GetFileVersionInfoSize(lpszFilePath, &dwTemp);
		if (dwSize == 0)
		{
			return false;
		}

		pData = new BYTE[dwSize + 1];
		if (pData == NULL)
		{
			return false;
		}

		if (!GetFileVersionInfo(lpszFilePath, 0, dwSize, pData))
		{
			delete[] pData;
			return false;
		}

		if (!VerQueryValue(pData, TEXT("\\"), (void **)&pVerInfo, &uLen))
		{
			delete[] pData;
			return false;
		}

		DWORD verMS = pVerInfo->dwFileVersionMS;
		DWORD verLS = pVerInfo->dwFileVersionLS;
		DWORD major = HIWORD(verMS);
		DWORD minor = LOWORD(verMS);
		DWORD build = HIWORD(verLS);
		DWORD revision = LOWORD(verLS);
		delete[] pData;

		StringCbPrintf(version, 0x20, TEXT("%d.%d.%d.%d"), major, minor, build, revision);

		return true;
	}

	return false;
}

void Patch(PVOID addr, DWORD size, PVOID code)
{
	DWORD lpOldPro = 0;
	if (!VirtualProtect((LPVOID)addr, size, PAGE_EXECUTE_READWRITE, &lpOldPro))
	{
		return;
	}
	memcpy((char*)addr, (char*)code, size);

	if (!VirtualProtect((LPVOID)addr, size, lpOldPro, &lpOldPro))
	{
	}
}

PVOID NextAddr(PVOID addr, DWORD asm_size /*= 5*/)
{
    return (char*)addr + asm_size;
}

LONG JmpOffset(PVOID dst, PVOID next)
{
    return (LONG)dst - (LONG)next;
}

LONG CallOffset(PVOID addr, DWORD callsize)
{
    return *((LONG*)((char*)addr + callsize));
}

LONGLONG CallOffset64(PVOID addr, DWORD callsize)
{
    return *((LONGLONG*)((char*)addr + callsize));
}

PVOID CallTarget(PVOID addr, DWORD callsize, DWORD codesize)
{
    PVOID next = NextAddr(addr, codesize);
    LONG offset = CallOffset(addr, callsize);
    return (PVOID)((LONG)next + offset);
}

//E8 D0 FC 22 00 call    sub_104830B0
void InlineHookE8(PVOID src, PVOID target, PVOID* orgfun)
{
    BYTE code[5] = { 0xE8, 0x00, 0x00, 0x00, 0x00 };

    PVOID next = NextAddr((PVOID)src, 5);
    LONG offset = JmpOffset(target, next);
    
    if (orgfun) {
        *orgfun = CallTarget(src, 1, 5);
    }

    *((DWORD*)&code[1]) = offset;
    Patch(src, 5, code);
}

wchar_t* SplitStr(const wchar_t* src, const wchar_t* f1, const wchar_t* f2)
{
    const wchar_t* p = wcsstr(src, f1);
    if (p == NULL) {
        return NULL;
    }
    p = p + wcslen(f1);
    const wchar_t* p1 = wcsstr(p, f2);
    if (p1 == NULL) {
        return NULL;
    }

    int len = p1 - p;
    if (len <= 0) {
        return NULL;
    }
    wchar_t* dst = new wchar_t[len + 2];
    if (dst == NULL) {
        return NULL;
    }
    memset(dst, 0, (len + 2) * sizeof(wchar_t));
    memcpy(dst, p, len * sizeof(wchar_t));

    return dst;
}


wchar_t* char2wchar(const char* cchar)
{
    wchar_t* m_wchar;
    int len = MultiByteToWideChar(CP_ACP, 0, cchar, strlen(cchar), NULL, 0);
    m_wchar = new wchar_t[len + 1];
    MultiByteToWideChar(CP_ACP, 0, cchar, strlen(cchar), m_wchar, len);
    m_wchar[len] = '\0';
    return m_wchar;
}


char* wchar2char(const wchar_t* wchar)
{
    char* m_char;
    int len = WideCharToMultiByte(CP_ACP, 0, wchar, wcslen(wchar), NULL, 0, NULL, NULL);
    m_char = new char[len + 1];
    WideCharToMultiByte(CP_ACP, 0, wchar, wcslen(wchar), m_char, len, NULL, NULL);
    m_char[len] = '\0';
    return m_char;
}

// 中文string则采用：
string chswstring2string(const wstring& wstr)
{
    string result;
    int len = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), NULL, 0, NULL, NULL);
    if (len <= 0)return result;
    char* buffer = new char[len + 1];
    if (buffer == NULL)return result;
    WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), wstr.size(), buffer, len, NULL, NULL);
    buffer[len] = '\0';
    result.append(buffer);
    delete[] buffer;
    return result;
}

wstring chsstring2wstring(const string& str)
{
    wstring result;
    int len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), NULL, 0);
    if (len < 0)return result;
    wchar_t* buffer = new wchar_t[len + 1];
    if (buffer == NULL)return result;
    MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.size(), buffer, len);
    buffer[len] = '\0';
    result.append(buffer);
    delete[] buffer;
    return result;
}


// 获取微信进程的个数
int numberOfWechat()
{
    int countProcess = 0;                                    //当前进程数量计数变量
    PROCESSENTRY32 currentProcess;                        //存放快照进程信息的一个结构体
    currentProcess.dwSize = sizeof(currentProcess);        //在使用这个结构之前，先设置它的大小
    HANDLE hProcess = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);//给系统内的所有进程拍一个快照

    if (hProcess == INVALID_HANDLE_VALUE)
    {
        printf("CreateToolhelp32Snapshot()调用失败!\n");
        return -1;
    }

    bool bMore = Process32First(hProcess, &currentProcess);    //获取第一个进程信息
    while (bMore)
    {
        if (strcmp(wchar2char(currentProcess.szExeFile), "WeChat.exe") == 0)
        {
            countProcess++;
        }
        bMore = Process32Next(hProcess, &currentProcess);    //遍历下一个
    }

    CloseHandle(hProcess);    //清除hProcess句柄
    return countProcess;
}