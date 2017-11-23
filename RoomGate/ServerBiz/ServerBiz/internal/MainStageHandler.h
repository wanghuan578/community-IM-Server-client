
#ifndef __MAIN_STAGE_HANDLER_H__
#define __MAIN_STAGE_HANDLER_H__

#include "SL_Sync_Condition.h"
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


	static int HelloNotify(int socket_handler_id, SL_Socket_Handler *socket_handler, void *seda_stagehandler, void *rpc_message, SL_Socket_INET_Addr *remote_addr, void *db);
	static int ConnectReq(int socket_handler_id, SL_Socket_Handler *socket_handler, void *seda_stagehandler, void *rpc_message, SL_Socket_INET_Addr *remote_addr, void *db);
	static int KeepAliveRequest(int socket_handler_id, SL_Socket_Handler *socket_handler, void *seda_stagehandler, void *rpc_message, SL_Socket_INET_Addr *remote_addr, void *db);
	static int ServiceDisconnect(int socket_handler_id, SL_Socket_Handler *socket_handler, void *seda_stagehandler, void *rpc_message, SL_Socket_INET_Addr *remote_addr, void *db);
	static int CS_HelloNotify(int socket_handler_id, SL_Socket_Handler *socket_handler, void *seda_stagehandler, void *rpc_message, SL_Socket_INET_Addr *remote_addr, void *db);
	static int CS_HelloRes(int socket_handler_id, SL_Socket_Handler *socket_handler, void *seda_stagehandler, void *rpc_message, SL_Socket_INET_Addr *remote_addr, void *db);
	static int RoomServerUpdateNotify(int socket_handler_id, SL_Socket_Handler *socket_handler, void *seda_stagehandler, void *rpc_message, SL_Socket_INET_Addr *remote_addr, void *db);
	static int CS_UserQueryRes(int socket_handler_id, SL_Socket_Handler *socket_handler, void *seda_stagehandler, void *rpc_message, SL_Socket_INET_Addr *remote_addr, void *db);
	
	static int RS_HelloNotify(int socket_handler_id, SL_Socket_Handler *socket_handler, void *seda_stagehandler, void *rpc_message, SL_Socket_INET_Addr *remote_addr, void *db);
	
	static int TransmitToRSRequest(int from_user_id, int to_user_id, void *transmit_message, int len);
	static int TransmitToRGRequest(int from_user_id, int to_user_id, void *transmit_message, int len);

	//int LoginResp(SL_Socket_Handler *socket_handler);
	//SL_Sync_Condition SycnEvent_;
	//int ServerID_;
};
#endif