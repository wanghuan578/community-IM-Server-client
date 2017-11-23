
#pragma once

#include <string>
#include "CommonTypes.h"
#include "room_server_types.h"
using namespace std;

#ifdef CLIENT_BIZ_DLL_EXPORT
#define CLIENT_BIZ_API __declspec(dllexport)
#else
#define CLIENT_BIZ_API __declspec(dllimport)
#endif

using namespace community;

class CLIENT_BIZ_API CClientBiz
{
public:
	static CClientBiz *Instance()
	{
		if(NULL == Instance_)
		{
			Instance_ = new CClientBiz();
		}

		return Instance_;
	}

	static void Destroy()
	{
		delete Instance_;

		Instance_ = NULL;
	}

	bool Open(string ip, int port);
	bool DeInit();
	int GetUserID();
	void SetUserID(int user_id);
	string GetLogName();
	string GetPassword();
	void SetPassword(string pwd);
	int GetRoomID();
	void SetRoomID(int room_id);
	int GetRoomGateID();
	void SetRoomGateID(int id);

	void SetClientRandom(int64 r);
	int64 GetClientRandom();
	void SetServerRandom(int64 r);
	int64 GetServerRandom();
	void SetSessionTicket(string ticket);
	string GetSessionTicket();
	void SetSessionKey(string key);
	string GetSessionKey();
	void SetSessionID(string id);
	string GetSessionID();

	void UserListUpdate(common::UserInfo user_info);

	int SetCallback(ClientBizToUICallback ui_callback);

	int Login(int user_id, string password);
	void PreClientPasswordLogin(int user_id, string password, string &check_sum);
	void ClientPasswordLogin(int user_id, string &checkSum);
	int EnterRoom(int room_id, string password);
	int LeaveRoom();
	int SendText(int to_user_id, const char *chat_text, bool is_public);
	vector<room_server::RoomInfo> *GetRoomList();
	vector<common::UserInfo> *GetUserList();

protected:
	CClientBiz();
	~CClientBiz();

private:
	static CClientBiz *Instance_;
	CRITICAL_SECTION g_cs;
	int UserID_;
	string LogName_;
	string Password_;
	int RoomID_;
	int RoomGateID_;

	int64 ClientRandom_;
	int64 ServerRandom_;
	string SessionKey_;
	string SessionID_;
	string SessionTicket_;

	vector<room_server::RoomInfo> RoomList_;
	vector<common::UserInfo> UserList_;
};