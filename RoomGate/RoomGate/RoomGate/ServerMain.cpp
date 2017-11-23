// ServerMain.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <windows.h>
#include "ServerBiz.h"
#include "Config.h"
#include "IniParser.h"


#ifdef _DEBUG
#define DEBUG_CLIENTBLOCK   new( _CLIENT_BLOCK, __FILE__, __LINE__)
#else
#define DEBUG_CLIENTBLOCK
#endif
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#ifdef _DEBUG
#define new DEBUG_CLIENTBLOCK
#endif




#pragma comment(lib, "../Debug/ServerBiz.lib")


char *buff_ = NULL;
int interval_ = 3000;

void OnExit(void)
{
	CServerBiz::Destroy();

	delete [] buff_;
}

char *Producer()
{
	if (NULL == buff_)
	{
		buff_ = new char[TCP_RECVBUFF_LEN];

		for (int i = 0; i < TCP_RECVBUFF_LEN; i++)
		{
			*(buff_ + i) = 'a';
		}
	}

	return buff_;
}

int _tmain(int argc, _TCHAR* argv[])
{
	_CrtSetDbgFlag (_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	atexit(OnExit);

	CServerBiz::Instance()->SetBufferSize(TCP_RECVBUFF_LEN);
	CServerBiz::Instance()->SetConnectMax(CLIENT_CONNECT_MAX);

	CIniParser ini(L"..\\debug\\RoomGate.ini");

	string server_name;
	if (ini.GetProfileValue(L"RoomGate", L"ServerName", "", server_name))
	{
		CServerBiz::Instance()->SetServerName(server_name);

		printf("GetProfileValue RoomGate ServerName: %s\n", server_name.c_str());
	}

	int server_id;
	if (ini.GetProfileValue(L"RoomGate", L"ServerID", "", server_id))
	{
		CServerBiz::Instance()->SetServerID(server_id);

		printf("GetProfileValue RoomGate ServerID: %d\n", server_id);
	}

	int listen_port;
	if (ini.GetProfileValue(L"RoomGate", L"ListenPort", "", listen_port))
	{
		CServerBiz::Instance()->SetServerPort(listen_port);

		printf("GetProfileValue RoomGate Port: %d\n", listen_port);
	}

	int service_weight;
	if (ini.GetProfileValue(L"RoomGate", L"ServiceWeight", "", service_weight))
	{
		CServerBiz::Instance()->SetServiceWeight(service_weight);

		printf("GetProfileValue RoomGate ServiceWeight: %d\n", service_weight);
	}

	string ip_a;
	if (ini.GetProfileValue(L"CenterServer", L"IpAddress", "", ip_a))
	{
		CServerBiz::Instance()->SetSessionAddress(ip_a);

		printf("GetProfileValue CenterServer IpAddress: %s\n", ip_a.c_str());
	}

	if (ini.GetProfileValue(L"CenterServer", L"ListenPort", "", listen_port))
	{
		CServerBiz::Instance()->SetSessionPort(listen_port);

		printf("GetProfileValue CenterServer Port: %d\n", listen_port);
	}

	printf("RoomGate Initialize\n"); 

	CServerBiz::Instance()->Initialize();

	while(1)
	{
		Sleep(5000000);
	}

	return 0;
}



