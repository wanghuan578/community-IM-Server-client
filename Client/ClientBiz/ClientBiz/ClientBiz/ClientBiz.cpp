
#include "ClientBiz.h"
#include "MessageMapping.h"
#include "TcpClientManager.h"
#include "SL_Crypto_CRTRand.h"
#include "SL_Crypto_MD5.h"
#include "SL_Crypto_Hex.h"
#include "SL_Crypto_AES.h"
#include "Serialize.h"

#ifdef _DEBUG
#pragma comment(lib, "soli_100usd.lib")
#else
#pragma comment(lib, "soli_100rsd.lib")
#endif

CClientBiz *CClientBiz::Instance_ = NULL;

CClientBiz::CClientBiz()
{
	string key = "123123";
	string encode_key = CSerialize::MD5(key);
	InitializeCriticalSection(&g_cs);
}

CClientBiz::~CClientBiz()
{
	DeInit();
}

bool CClientBiz::Open(string ip, int port)
{
	CTcpClientManager::Instance()->Open(ip, port);

	return true;
}

bool CClientBiz::DeInit()
{
	return true;
}

int CClientBiz::GetUserID()
{
	return UserID_;
}

string CClientBiz::GetLogName()
{
	return LogName_;
}

string CClientBiz::GetPassword()
{
	return Password_;
}

void CClientBiz::SetUserID(int user_id)
{
	UserID_ = user_id;
}

void CClientBiz::SetPassword(string pwd)
{
	Password_ = pwd;
}

int CClientBiz::GetRoomID()
{
	return RoomID_;
}

void CClientBiz::SetRoomID(int room_id)
{
	RoomID_ = room_id;
}

int CClientBiz::GetRoomGateID()
{
	return RoomGateID_;
}

void CClientBiz::SetRoomGateID(int id)
{
	RoomGateID_ = id;
}

int CClientBiz::SetCallback(ClientBizToUICallback ui_callback)
{
	CTcpClientManager::Instance()->UICallBack_ = ui_callback;

	return 0;
}

void CClientBiz::SetClientRandom(int64 r)
{
	ClientRandom_ = r;
}

int64 CClientBiz::GetClientRandom()
{
	return ClientRandom_;
}

void CClientBiz::SetSessionTicket(string ticket)
{
	SessionTicket_ = ticket;
}

string CClientBiz::GetSessionTicket()
{
	return SessionTicket_;
}

void CClientBiz::SetSessionKey(string key)
{
	SessionKey_ = key;
}

string CClientBiz::GetSessionKey()
{
	return SessionKey_;
}

void CClientBiz::SetSessionID(string id)
{
	SessionID_ = id;
}

string CClientBiz::GetSessionID()
{
	return SessionID_;
}

void CClientBiz::SetServerRandom(int64 r)
{
	ServerRandom_ = r;
}

int64 CClientBiz::GetServerRandom()
{
	return ServerRandom_;
}

int CClientBiz::Login(int user_id, string password)
{
	UserID_ = user_id;
	Password_ = password;

	string check_sum;

	PreClientPasswordLogin(user_id, password, check_sum);

	ClientPasswordLogin(user_id, check_sum);

	return 0;
}

void CClientBiz::PreClientPasswordLogin(int user_id, string password, string &check_sum)
{
	SL_Crypto_CRTRand rand;
	rand.seed();
	SetClientRandom(rand.random_int());

	char check_hex_code[33] = {0};
	{
		login_server::ClientPasswordLoginReqChecksum login_check_sum;
		login_check_sum.user_id = user_id;
		login_check_sum.password = password;
		login_check_sum.client_random = GetClientRandom();
		login_check_sum.server_random = GetServerRandom();

		SL_ByteBuffer buf_check_sum(1024);
		SL_Crypto_MD5 md5;
		char key[16] = {0};
		SL_Rpc_MessageParser::serialize_to_buffer<login_server::ClientPasswordLoginReqChecksum, SL_ByteBuffer>(&login_check_sum, &buf_check_sum);
		md5.init();
		md5.update((const unsigned char*)buf_check_sum.data(), buf_check_sum.data_size());
		md5.final((unsigned char*)key);

		SL_Crypto_Hex hex;
		hex.set_upper_case(false);
		int len = 32;
		hex.encode((const unsigned char *)key, 16, (unsigned char *)check_hex_code, (unsigned int *)&len);
	}

	check_sum = check_hex_code;
}

void CClientBiz::ClientPasswordLogin(int user_id, string &check_sum)
{
	MsgUserPasswordLoginReq req;
	req.body_.user_id = user_id;
	req.body_.client_random = GetClientRandom();
	req.body_.client_mac = "0000000022222222333333555";
	req.body_.check_sum = check_sum;

	SL_ByteBuffer buff(1024);
	req.serialize_ex<SL_ByteBuffer>(&buff);

	CTcpClientManager::Instance()->GetLoginServerManager()->GetTcpClientHandler()->put_data(buff);
}

int CClientBiz::EnterRoom(int room_id, string password)
{
	MsgEnterRoomReq enter_room_req;
	enter_room_req.body_.room_password = password;
	enter_room_req.body_.is_robot = false;
	enter_room_req.body_.user_status = room_server::UserStatus::OFFLINE;

	enter_room_req.head_.source = CClientBiz::Instance()->GetUserID();
	enter_room_req.head_.attach_id1 = room_id;
	enter_room_req.head_.attach_id2 = CClientBiz::Instance()->GetRoomGateID();

	SL_ByteBuffer buff(1024);
	enter_room_req.serialize_ex<SL_ByteBuffer>(&buff);

	CTcpClientManager::Instance()->GetRoomGateManager()->GetTcpClientHandler()->put_data(buff);

	return 0;
}

int CClientBiz::LeaveRoom()
{
	MsgLeaveRoomReq req;
	req.head_.source = UserID_;
	req.head_.attach_id1 = RoomID_;
	req.head_.attach_id2 = RoomGateID_;

	SL_ByteBuffer buff(512);
	req.serialize_ex<SL_ByteBuffer>(&buff);

	CTcpClientManager::Instance()->GetRoomGateManager()->GetTcpClientHandler()->put_data(buff);

	return 0;
}

void CClientBiz::UserListUpdate(common::UserInfo user_info)
{
	EnterCriticalSection(&g_cs);
	UserList_.push_back(user_info);
	LeaveCriticalSection(&g_cs);
}

int CClientBiz::SendText(int to_user_id, const char *chat_text, bool is_public)
{
	MsgRoomServerChatReq req;

	req.body_.chat_text = chat_text;
	
	for (int i = 0; i < UserList_.size(); i++)
	{
		common::UserInfo user_info = UserList_.at(i);

		if (user_info.user_id == UserID_)
		{
			req.body_.from_nick_name = user_info.user_name;

			break;
		}
	}

	for (int i = 0; i < UserList_.size(); i++)
	{
		common::UserInfo user_info = UserList_.at(i);

		if (user_info.user_id == to_user_id)
		{
			req.body_.to_nick_name = user_info.user_name;

			break;
		}
	}

	req.head_.source = UserID_;
	req.head_.destination = to_user_id;
	req.head_.attach_id1 = RoomID_;
	
	SL_ByteBuffer buff(1024);
	req.serialize_ex<SL_ByteBuffer>(&buff);

	CTcpClientManager::Instance()->GetRoomGateManager()->GetTcpClientHandler()->put_data(buff);

	return 0;
}

vector<room_server::RoomInfo> *CClientBiz::GetRoomList()
{
	return &RoomList_;
}

vector<common::UserInfo> *CClientBiz::GetUserList()
{
	return &UserList_;
}