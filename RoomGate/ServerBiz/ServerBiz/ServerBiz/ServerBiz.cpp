
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
	CTcpServerManager::Instance()->GetRoomGateManager()->SetServerID(id);
}

void CServerBiz::SetServerName(string name)
{
	CTcpServerManager::Instance()->GetRoomGateManager()->SetServerName(name);
}

void CServerBiz::SetServerPort(int port)
{
	CTcpServerManager::Instance()->GetRoomGateManager()->SetListenPort(port);
}

void CServerBiz::SetServiceWeight(int weight)
{
	CTcpServerManager::Instance()->GetRoomGateManager()->SetServiceWeight(weight);
}

void CServerBiz::SetBufferSize(int size)
{
	CTcpServerManager::Instance()->GetRoomGateManager()->SetBufferSize(size);

	CTcpServerManager::Instance()->GetCenterServerManager()->SetBufferSize(size);
}

void CServerBiz::SetConnectMax(int max)
{
	CTcpServerManager::Instance()->GetRoomGateManager()->SetMaxConnect(max);
}

void CServerBiz::SetSessionAddress(string ip)
{
	CTcpServerManager::Instance()->GetCenterServerManager()->SetIpAddress(ip);
}

void CServerBiz::SetSessionPort(int port)
{
	CTcpServerManager::Instance()->GetCenterServerManager()->SetPort(port);
}



