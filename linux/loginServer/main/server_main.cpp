// ServerMain.cpp : 定义控制台应用程序的入口点。
//

#include "serveBiz.h"
#include "config.h"

int main()
{
	CServerBiz::Instance()->SetBufferSize(TCP_RECVBUFF_LEN);
	CServerBiz::Instance()->SetConnectMax(CLIENT_CONNECT_MAX);
	
	CServerBiz::Instance()->SetServerName("LoginServer_1");

	CServerBiz::Instance()->SetServerID(10000);

	CServerBiz::Instance()->SetServerPort(9000);

	CServerBiz::Instance()->SetSessionAddress("127.0.0.1");

	CServerBiz::Instance()->SetSessionPort(9200);

	CServerBiz::Instance()->Initialize();

	return 0;
}
