
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
	
	int handle_open();
	int handle_event(const SL_Seda_Event *event);
	int handle_close();

	static int HelloNotify(int socket_handler_id, SL_Socket_Handler *socket_handler, void *seda_stagehandler, void *rpc_message, SL_Socket_INET_Addr *remote_addr);
	static int LoginRequest(int socket_handler_id, SL_Socket_Handler *socket_handler, void *seda_stagehandler, void *rpc_message, SL_Socket_INET_Addr *remote_addr);
	static int RoomGateUpdateNotify(int socket_handler_id, SL_Socket_Handler *socket_handler, void *seda_stagehandler, void *rpc_message, SL_Socket_INET_Addr *remote_addr);
	static int KeepAliveRequest(int socket_handler_id, SL_Socket_Handler *socket_handler, void *seda_stagehandler, void *rpc_message, SL_Socket_INET_Addr *remote_addr);
	static int CenterUserLoginRes(int socket_handler_id, SL_Socket_Handler *socket_handler, void *seda_stagehandler, void *rpc_message, SL_Socket_INET_Addr *remote_addr);
	static int CS_HelloNotify(int socket_handler_id, SL_Socket_Handler *socket_handler, void *seda_stagehandler, void *rpc_message, SL_Socket_INET_Addr *remote_addr);
	static int CS_HelloRes(int socket_handler_id, SL_Socket_Handler *socket_handler, void *seda_stagehandler, void *rpc_message, SL_Socket_INET_Addr *remote_addr);
	static int ServiceDisconnect(int socket_handler_id, SL_Socket_Handler *socket_handler, void *seda_stagehandler, void *rpc_message, SL_Socket_INET_Addr *remote_addr);
	static int DBProxyReadyNotify(int socket_handler_id, SL_Socket_Handler *socket_handler, void *seda_stagehandler, void *rpc_message, SL_Socket_INET_Addr *remote_addr);
	static int DBProxyHelloNotify(int socket_handler_id, SL_Socket_Handler *socket_handler, void *seda_stagehandler, void *rpc_message, SL_Socket_INET_Addr *remote_addr);
	static int LoginCheckRes(int socket_handler_id, SL_Socket_Handler *socket_handler, void *seda_stagehandler, void *rpc_message, SL_Socket_INET_Addr *remote_addr);
	static int LoginGetUserInfoRes(int socket_handler_id, SL_Socket_Handler *socket_handler, void *seda_stagehandler, void *rpc_message, SL_Socket_INET_Addr *remote_addr);
public:
	int LoginResp(SL_Socket_Handler *socket_handler);
};
#endif