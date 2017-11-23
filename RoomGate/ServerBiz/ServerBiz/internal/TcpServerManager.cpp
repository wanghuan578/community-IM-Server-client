
#include "TcpServerManager.h"
#include "MessageMapping.h"
#include "ServerBiz.h"
#include "RoomServerManager.h"

CTcpServerManager *CTcpServerManager::Instance_ = NULL;

CTcpServerManager::CTcpServerManager()
{

}

CTcpServerManager::~CTcpServerManager()
{
	RoomGateManager_.Destroy();

	RoomServerManager_.Destroy();

	CenterServerManager_.Destroy();

	GetSedaStageHandler()->close();

	SL_Socket_CommonAPI::socket_fini();
}

bool CTcpServerManager::Initialize()
{
	bool ret = false;

	SL_Socket_CommonAPI::socket_init(2, 2);

	string host = "116.62.159.79";
	unsigned int port = 3306;
	string username = "root";
	string passwd = "spirit";
	string db = "community";
	SedaStageHandler_.set_db_param(host, port, username, passwd, db);
	SedaStageHandler_.open(2, 500000, 48, 1);

	if (!RoomGateManager_.Init())
	{
		SedaStageHandler_.close();

		SL_Socket_CommonAPI::socket_fini();

		return false;
	}

	CenterServerManager_.SetConnectTimeout(5000);

	if (!CenterServerManager_.Init())
	{
		SL_Seda_TimerExpireEvent time_expire;
		time_expire.slot = TIMER_SLOT_CENTER_SERVER_CONNECT;
		SedaStageHandler_.push_event(&time_expire);
	}

	return ret;
}

CCenterServerManager *CTcpServerManager::GetCenterServerManager()
{
	return &CenterServerManager_;
}

CRoomGateManager *CTcpServerManager::GetRoomGateManager()
{
	return &RoomGateManager_;
}

CRoomServerManager *CTcpServerManager::GetRoomServerManager()
{
	return &RoomServerManager_;
}

SL_Seda_Stage<CMainStageHandler, LP_DB_INST> *CTcpServerManager::GetSedaStageHandler()
{
	return &SedaStageHandler_;
}

