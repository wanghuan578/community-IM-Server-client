
#ifndef __TCP_SERVER_MANAGER_H__
#define __TCP_SERVER_MANAGER_H__

#include "SL_Seda_Stage.h"
#include "MainStageHandler.h"
#include "CenterServerManager.h"
#include "RoomGateManager.h"
#include "RoomServerManager.h"


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
	CCenterServerManager *GetCenterServerManager();
	CRoomGateManager *GetRoomGateManager();
	CRoomServerManager *GetRoomServerManager();
	SL_Seda_Stage<CMainStageHandler, LP_DB_INST> *GetSedaStageHandler();

protected:
	CTcpServerManager();
	~CTcpServerManager();

private:
	static CTcpServerManager *Instance_;
	SL_Seda_Stage<CMainStageHandler, LP_DB_INST> SedaStageHandler_;
	CCenterServerManager CenterServerManager_;
	CRoomGateManager RoomGateManager_;
	CRoomServerManager RoomServerManager_;
	
};
#endif