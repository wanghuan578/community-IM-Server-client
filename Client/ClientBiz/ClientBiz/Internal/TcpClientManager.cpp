
#include "TcpClientManager.h"
#include "MessageMapping.h"
#include "CommonTypes.h"


CTcpClientManager *CTcpClientManager::Instance_ = NULL;

CTcpClientManager::CTcpClientManager()
{
}

CTcpClientManager::~CTcpClientManager()
{
	ShutDown();

	SL_Socket_CommonAPI::socket_fini();
}

bool CTcpClientManager::Open(string ip, int port)
{
	SL_Socket_CommonAPI::socket_init(2, 2);

	SedaStageHandler_.open(1, 500000, 48, 1);

	if (!LoginServerManager_.Open(ip, port))
	{
		SedaStageHandler_.close();

		SL_Socket_CommonAPI::socket_fini();

		return false;
	}

	return true;
}

void CTcpClientManager::ShutDown()
{
	LoginServerManager_.Destroy();

	GetSedaStageHandler()->close();
}

SL_Seda_Stage<CMainStageHandler> *CTcpClientManager::GetSedaStageHandler()
{
	return &SedaStageHandler_;
}

CLoginServerManager *CTcpClientManager::GetLoginServerManager()
{
	return &LoginServerManager_;
}

CRoomGateManager *CTcpClientManager::GetRoomGateManager()
{
	return &RoomGateHandler_;
}

void CTcpClientManager::Transfer_To_UI(int message_type, int error_code, void *rpc_message)
{
	if (login_server::MessageType::MT_UPDATE_AVAILABLE_RES != message_type)
	{
		UICallBack_(message_type, error_code, rpc_message);
	}
}

