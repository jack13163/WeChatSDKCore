#pragma once

#define HELLO_API 1

#ifdef HELLO_API  
#define HELLO_API 
#else  
#define HELLO_API extern "C" __declspec(dllimport)  
#endif 


HELLO_API void HookGetFriendList();		//HOOK获取好友列表的call
HELLO_API void GetUserListInfo();		   //获取好友列表
HELLO_API void UnHookGetFriendList();	   //卸载HOOK获取好友列表的call
HELLO_API void SendUserListInfo();		//发送好友列表
HELLO_API void SendTextMessage(wchar_t* wxid, wchar_t* msg);	//发送文本消息
HELLO_API void SendFileMessage(wchar_t* wxid, wchar_t* filepath);	//发送文件消息
HELLO_API void SendImageMessage(wchar_t* wxid, wchar_t* filepath);//发送图片消息
HELLO_API void DeleteUser(wchar_t* wxid);	//删除好友
HELLO_API void SendXmlCard(wchar_t* RecverWxid, wchar_t* SendWxid, wchar_t* NickName); //发送XML名片