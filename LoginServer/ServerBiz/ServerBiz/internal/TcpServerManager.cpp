
#include "TcpServerManager.h"
#include "MessageMapping.h"
#include "ServerBiz.h"
#include "CommonTypes.h"
//#include "sl_db_connectPool.h"

CTcpServerManager *CTcpServerManager::Instance_ = NULL;

CTcpServerManager::CTcpServerManager()
{
}

CTcpServerManager::~CTcpServerManager()
{
	ShutDown();

	SL_Socket_CommonAPI::socket_fini();
}

bool CTcpServerManager::Initialize()
{
	SL_Socket_CommonAPI::socket_init(2, 2);

	SedaStageInit();

	//CDBConnectPool::Instance()->Open("115.159.104.21", 3306, "root", "spirit", 20);

	LoginServerInit();

	//CenterServerConnect();

	//ConnectTimeoutDetect();

	return true;
}

int CTcpServerManager::SedaStageInit()
{
	string host = "116.62.159.79";
	unsigned int port = 3306;
	string username = "root";
	string passwd = "spirit";
	string db = "community";
	SedaStageHandler_.set_db_param(host, port, username, passwd, db);

	SedaStageHandler_.open(2, 500000, 48, 1);

	return 0;
}

int CTcpServerManager::LoginServerInit()
{
	if (!LoginServerManager_.Init())
	{
		SedaStageHandler_.close();

		SL_Socket_CommonAPI::socket_fini();

		return -1;
	}

	return 0;
}

//int CTcpServerManager::CenterServerConnect()
//{
//	CenterServerManager_.SetConnectTimeout(CENTER_SERVER_CONNECT_INTERVAL);
//
//	if (!CenterServerManager_.Init())
//	{
//		SL_Seda_TimerExpireEvent time_expire;
//
//		time_expire.slot = TIMER_SLOT_CENTER_SERVER_CONNECT;
//
//		SedaStageHandler_.push_event(&time_expire);
//	}
//
//	return 0;
//}

int CTcpServerManager::ConnectTimeoutDetect()
{
	SL_Seda_TimerExpireEvent time_expire;

	time_expire.slot = TIMER_SLOT_USER_CONNECT_TIMEOUT;

	SedaStageHandler_.push_event(&time_expire);

	return 0;
}

void CTcpServerManager::ShutDown()
{
	LoginServerManager_.Destroy();

	//CenterServerManager_.Destroy();

	GetSedaStageHandler()->close();
}

SL_Seda_Stage<CMainStageHandler, LP_DB_INST> *CTcpServerManager::GetSedaStageHandler()
{
	return &SedaStageHandler_;
}

//CCenterServerManager *CTcpServerManager::GetCenterServerManager()
//{
//	return &CenterServerManager_;
//}

CLoginServerManager *CTcpServerManager::GetLoginServerManager()
{
	return &LoginServerManager_;
}

//CDBProxyManager *CTcpServerManager::GetDBProxyManager()
//{
//	return &DBProxyManager_;
//}
