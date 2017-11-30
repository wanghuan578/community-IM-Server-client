
#ifndef __TCPSERVER_MANAGER_H__
#define __TCPSERVER_MANAGER_H__

#include "main_stage_handler.h"
#include "center_server_manager.h"
#include "login_server_manager.h"
#include "dbproxy_manager.h"


class CTcpServerManager
{
public:

	static CTcpServerManager *Instance()
	{
		if(NULL == Instance_)
		{
			Instance_ = new CTcpServerManager();
		}

		return Instance_;
	}

	void Destroy()
	{
		delete Instance_;

		Instance_ = NULL;
	}

	bool Initialize();
	SL_Seda_Stage<CMainStageHandler> *GetSedaStageHandler();
	CCenterServerManager *GetCenterServerManager();
	CLoginServerManager *GetLoginServerManager();
	CDBProxyManager *GetDBProxyManager();
	
protected:
	CTcpServerManager();
	~CTcpServerManager();

	void ShutDown();
	int SedaStageInit();
	int LoginServerInit();
	int CenterServerConnect();
	int ConnectTimeoutDetect();
	
private:
	static CTcpServerManager *Instance_;
	SL_Seda_Stage<CMainStageHandler> SedaStageHandler_;
	
	CCenterServerManager CenterServerManager_;
	CLoginServerManager LoginServerManager_;
	CDBProxyManager DBProxyManager_;
};
#endif
