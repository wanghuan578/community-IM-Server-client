
#pragma once

#include "SL_Seda_StageHandler.h"
#include "SL_Seda_Stage.h"
#include "MessageMapping.h"
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

	static int LS_HelloNotify(int socket_handler_id, SL_Socket_Handler *socket_handler, void *seda_stagehandler, void *rpc_message, SL_Socket_INET_Addr *remote_addr);
	static int RG_HelloNotify(int socket_handler_id, SL_Socket_Handler *socket_handler, void *seda_stagehandler, void *rpc_message, SL_Socket_INET_Addr *remote_addr);
	static int ConnectRes(int socket_handler_id, SL_Socket_Handler *socket_handler, void *seda_stagehandler, void *rpc_message, SL_Socket_INET_Addr *remote_addr);
	static int GetRoomGateListRes(int socket_handler_id, SL_Socket_Handler *socket_handler, void *seda_stagehandler, void *rpc_message, SL_Socket_INET_Addr *remote_addr);
	static int RegisterRes(int socket_handler_id, SL_Socket_Handler *socket_handler, void *seda_stagehandler, void *rpc_message, SL_Socket_INET_Addr *remote_addr);
	static int LoginRes(int socket_handler_id, SL_Socket_Handler *socket_handler, void *seda_stagehandler, void *rpc_message, SL_Socket_INET_Addr *remote_addr);
	static int ServiceDisconnect(int socket_handler_id, SL_Socket_Handler *socket_handler, void *seda_stagehandler, void *rpc_message, SL_Socket_INET_Addr *remote_addr);

	static int EnterRoomRes(int socket_handler_id, SL_Socket_Handler *socket_handler, void *seda_stagehandler, void *rpc_message, SL_Socket_INET_Addr *remote_addr);
	static int EnterRoomNotify(int socket_handler_id, SL_Socket_Handler *socket_handler, void *seda_stagehandler, void *rpc_message, SL_Socket_INET_Addr *remote_addr);
	static int LeaveRoomRes(int socket_handler_id, SL_Socket_Handler *socket_handler, void *seda_stagehandler, void *rpc_message, SL_Socket_INET_Addr *remote_addr);
	static int GetUserListRes(int socket_handler_id, SL_Socket_Handler *socket_handler, void *seda_stagehandler, void *rpc_message, SL_Socket_INET_Addr *remote_addr);
	static int ChatNotify(int socket_handler_id, SL_Socket_Handler *socket_handler, void *seda_stagehandler, void *rpc_message, SL_Socket_INET_Addr *remote_addr);
	
protected:
	void ConnectSucceed(int socket_handler_id, SL_Socket_Handler *socket_handler);
	//int DecryptSessionTicket(string &ticket);
	void DecryptUserInfo(void *res, void *user_info);

private:


	
};