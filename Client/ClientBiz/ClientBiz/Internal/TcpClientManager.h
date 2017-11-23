
#ifndef __TCP_CLIENT_MANAGER_H__
#define __TCP_CLIENT_MANAGER_H__

#include "MainStageHandler.h"
#include "LoginServerManager.h"
#include "RoomGateManager.h"

class CTcpClientManager
{
public:
	static CTcpClientManager *Instance()
	{
		if(NULL == Instance_)
		{
			Instance_ = new CTcpClientManager();
		}

		return Instance_;
	}

	void Destroy()
	{
		delete Instance_;

		Instance_ = NULL;
	}

	bool Open(string ip, int port);
	SL_Seda_Stage<CMainStageHandler> *GetSedaStageHandler();
	CLoginServerManager *GetLoginServerManager();
	CRoomGateManager *GetRoomGateManager();

	ClientBizToUICallback UICallBack_;
	void Transfer_To_UI(int message_type, int error_code, void *rpc_message);

protected:
	CTcpClientManager();
	~CTcpClientManager();

	void ShutDown();

private:
	static CTcpClientManager *Instance_;
	SL_Seda_Stage<CMainStageHandler> SedaStageHandler_;

	CLoginServerManager LoginServerManager_;
	CRoomGateManager RoomGateHandler_;


};
#endif