
#ifndef __MAINSTAGE_HANDLER_H__
#define __MAINSTAGE_HANDLER_H__

#include "SL_Seda_StageHandler.h"
#include "SL_Seda_Stage.h"
#include <iostream>
#include <queue>

using namespace std;



class CMainStageHandler : public SL_Seda_StageHandler
{
public:
	CMainStageHandler();
	~CMainStageHandler();
	
	virtual int handle_open();
	virtual int handle_event(const SL_Seda_Event *event, void *db_inst, void *cache);
	virtual int handle_close();
	
	static int HelloNotify(int socket_handler_id, SL_Socket_Handler *socket_handler, void *seda_stagehandler, void *rpc_message, SL_Socket_INET_Addr *remote_addr, void *db_inst);
	static int LoginRequest(int socket_handler_id, SL_Socket_Handler *socket_handler, void *seda_stagehandler, void *rpc_message, SL_Socket_INET_Addr *remote_addr, void *db_inst);
	static int LoginResp(SL_Socket_Handler *socket_handler);
	static int UserRegister(int socket_handler_id, SL_Socket_Handler *socket_handler, void *seda_stagehandler, void *rpc_message, SL_Socket_INET_Addr *remote_addr, void *db_inst);
	static int KeepAliveRequest(int socket_handler_id, SL_Socket_Handler *socket_handler, void *seda_stagehandler, void *rpc_message, SL_Socket_INET_Addr *remote_addr, void *db_inst);
	static int ServiceDisconnect(int socket_handler_id, SL_Socket_Handler *socket_handler, void *seda_stagehandler, void *rpc_message, SL_Socket_INET_Addr *remote_addr, void *db_inst);
};
#endif