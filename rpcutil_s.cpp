#include "stdafx.h"
#include "rpcutil_s.h"

#include <rpc_server.h>
using namespace rest_rpc;
using namespace rpc_service;
#include <fstream>

#include "util.h"
#include "string.h"
#include "offset.h"
#include "Login.h"
#include "FriendList.h"
#include "Function.h"
#include "ChatRecord.h"
#include <list>
#include "../WeChatSDK/openwechat.h"


using namespace std;


// 服务列表
struct dummy {
	// 测试方法
	int add(rpc_conn conn, int a, int b)
	{
		return a + b;
	}

	// 获取登陆二维码
	int GetQrCode(rpc_conn conn)
	{
		OutputDebugString(L"sdk::GetQrCode");
		GotoQrCode();
		HookQrCode(QrCodeOffset);
		return 0;
	}

	// 退出登陆
	int LogoutWx(rpc_conn conn)
	{
		OutputDebugString(L"sdk::LogoutWx");
		LogoutWeChat();
		return 0;
	}

	// 发送文本消息
	int SendTextMsg(rpc_conn conn, string wxid, string msg)
	{
		SendTextMessage(char2wchar(wxid.data()), char2wchar(msg.data()));
		return 0;
	}

	// 删除好友
	int DeletePeople(rpc_conn conn, string wxid)
	{
		DeleteUser(char2wchar(wxid.data()));
		return 0;
	}

	// 添加好友
	int AddPeople(rpc_conn conn, string wxid, string msg)
	{
		AddWxUser(char2wchar(wxid.data()), char2wchar(msg.data()));
		return 0;
	}

	// HOOK获取好友列表
	int HookGetPeoples(rpc_conn conn)
	{
		HookGetFriendList();
		return 0;
	}

	// 获取好友列表
	string GetPeoples(rpc_conn conn)
	{
		return GetUserInfoList();
	}

	// Hook接受消息
	int HookMsgReceive(rpc_conn conn)
	{
		HookChatRecord();
		return 0;
	}

	// 检查是否已经登陆
	int IsLogin(rpc_conn conn)
	{
		return CheckIsLogin();
	}
};


// 端口列表【需要RPC服务端和客户端保持一致】
int ports[] = { 9000, 8866, 7777, 5555, 6666,9958,7765 };

// 当前端口
int currentPort = -1;

// api接口
dummy d;

// hardware_concurrency: 正常返回支持的并发线程数，若值非良定义或不可计算，则返回 ​0​
rpc_server* server;

// 开启RPC服务
int StartSDKServer()
{
	if (currentPort < 0) 
	{
		currentPort = ports[numberOfWechat() - 1];
	}
	try
	{
		server = new rpc_server(currentPort, std::thread::hardware_concurrency());
		// 注册远程调用方法
		server->register_handler("add", &dummy::add, &d);
		server->register_handler("GetQrCode", &dummy::GetQrCode, &d);
		server->register_handler("LogoutWx", &dummy::LogoutWx, &d);
		server->register_handler("SendTextMsg", &dummy::SendTextMsg, &d);
		server->register_handler("DeletePeople", &dummy::DeletePeople, &d);
		server->register_handler("AddPeople", &dummy::AddPeople, &d);
		server->register_handler("HookGetPeoples", &dummy::HookGetPeoples, &d);
		server->register_handler("GetPeoples", &dummy::GetPeoples, &d);
		server->register_handler("HookMsgReceive", &dummy::HookMsgReceive, &d);
		server->register_handler("IsLogin", &dummy::IsLogin, &d);
		// 启动RPC服务
		server->run();
	}
	catch (int)
	{
		return -1;
	}
	return 0;
}

// 结束RPC服务
int StopSDKServer()
{
	try
	{
		server->~rpc_server();
	}
	catch (int)
	{
		return -1;
	}
	return 0;
}