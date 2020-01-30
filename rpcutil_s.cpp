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
#include <list>


using namespace std;

typedef tuple <
	//wxid1
	wstring,
	//wxName
	wstring,
	//v1
	wstring,
	//nickName
	wstring
> USER_INFO;

//定义7000个用户列表
list<USER_INFO> userInfoList(1);


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
	int HOOKGetPeoples(rpc_conn conn)
	{
		HookGetFriendList();
		return 0;
	}

	// 获取好友列表
	list<USER_INFO> GetPeoples(rpc_conn conn)
	{
		return GetFriendList();
	}

};



// api接口
dummy d;

// hardware_concurrency: 正常返回支持的并发线程数，若值非良定义或不可计算，则返回 ​0​
rpc_server* server;

// 开启RPC服务
int StartSDKServer()
{
	try
	{
		server = new rpc_server(9000, std::thread::hardware_concurrency());
		// 注册远程调用方法
		server->register_handler("add", &dummy::add, &d);
		server->register_handler("GetQrCode", &dummy::GetQrCode, &d);
		server->register_handler("LogoutWx", &dummy::LogoutWx, &d);
		server->register_handler("SendTextMsg", &dummy::SendTextMsg, &d);
		server->register_handler("DeletePeople", &dummy::DeletePeople, &d);
		server->register_handler("AddPeople", &dummy::AddPeople, &d);
		server->register_handler("HOOKGetPeoples", &dummy::HOOKGetPeoples, &d);
		server->register_handler("GetPeoples", &dummy::GetPeoples, &d);
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