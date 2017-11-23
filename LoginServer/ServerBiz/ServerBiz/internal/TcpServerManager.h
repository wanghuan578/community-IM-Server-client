
#ifndef __TCP_SERVER_MANAGER_H__
#define __TCP_SERVER_MANAGER_H__

#include "MainStageHandler.h"
//#include "CenterServerManager.h"
#include "LoginServerManager.h"
//#include "DBProxyManager.h"


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
	SL_Seda_Stage<CMainStageHandler, LP_DB_INST> *GetSedaStageHandler();
	//CCenterServerManager *GetCenterServerManager();
	CLoginServerManager *GetLoginServerManager();
	//CDBProxyManager *GetDBProxyManager();
	
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
	SL_Seda_Stage<CMainStageHandler, LP_DB_INST> SedaStageHandler_;
	
	//CCenterServerManager CenterServerManager_;
	CLoginServerManager LoginServerManager_;
	//CDBProxyManager DBProxyManager_;
};
#endif