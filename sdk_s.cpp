#include "stdafx.h"
#include "sdk_s.h"
#include "weixin.h"
#include "sdkdef_h.h"
#include "rpcutil_s.h"

//TODO: edit the file to add interface
void RpcLoop()
{
    StartSDKServer(GetCurrentProcessId());
    OutputDebugString(L"exit...rpc");
}

int InitSDK()
{
    CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)RpcLoop, NULL, 0, NULL);
    return GetLastError();
}

void UnInitSDK()
{
    StopSDKServer();
}
