#pragma once
#include "stdafx.h"


void HookQrCode(DWORD dwHookOffset);	//HOOK二维码
void UnHookQrCode(DWORD dwHookOffset);	//卸载HOOK
void ShowPic();		//显示二维码
void SaveImg(DWORD qrcode);	//保存二维码
void GotoQrCode(); //跳转到二维码
void LogoutWeChat(); // 注销登陆

BOOL IsWxVersionValid();	//检查当前微信版本
int CheckIsLogin();	//检测微信是否登陆