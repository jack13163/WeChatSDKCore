#pragma once

bool GetFileVersion(LPTSTR lpszFilePath, LPTSTR version);
void Patch(PVOID addr, DWORD size, PVOID code);
void InlineHookE8(PVOID src, PVOID target, PVOID* orgfun);
wchar_t* SplitStr(const wchar_t* src, const wchar_t* f1, const wchar_t* f2);

// 解决字符串编码问题：“const char*”类型的实参与“LPCWSTR”类型的形参不兼容
wchar_t* char2wchar(const char* cchar);
char* wchar2char(const wchar_t* wchar);