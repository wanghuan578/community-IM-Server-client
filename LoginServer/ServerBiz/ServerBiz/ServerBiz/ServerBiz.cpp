
#include "ServerBiz.h"
#include "TcpServerManager.h"



CServerBiz *CServerBiz::m_pInstance = NULL;

CServerBiz::CServerBiz()
{
}

CServerBiz::~CServerBiz()
{
	CTcpServerManager::Instance()->Destroy();
}

bool CServerBiz::Initialize()
{
	return CTcpServerManager::Instance()->Initialize();
}

void CServerBiz::SetServerID(int id)
{
	CTcpServerManager::Instance()->GetLoginServerManager()->SetServerID(id);
}

void CServerBiz::SetServerName(string name)
{
	CTcpServerManager::Instance()->GetLoginServerManager()->SetServerName(name);
}

void CServerBiz::SetServerPort(int port)
{
	CTcpServerManager::Instance()->GetLoginServerManager()->SetListenPort(port);
}

void CServerBiz::SetBufferSize(int size)
{
	CTcpServerManager::Instance()->GetLoginServerManager()->SetBufferSize(size);

	//CTcpServerManager::Instance()->GetCenterServerManager()->SetBufferSize(size);
}

void CServerBiz::SetConnectMax(int max)
{
	CTcpServerManager::Instance()->GetLoginServerManager()->SetMaxConnect(max);
}

//void CServerBiz::SetSessionAddress(string ip)
//{
//	//CTcpServerManager::Instance()->GetCenterServerManager()->SetIpAddress(ip);
//}

//void CServerBiz::SetSessionPort(int port)
//{
//	CTcpServerManager::Instance()->GetCenterServerManager()->SetPort(port);
//}



