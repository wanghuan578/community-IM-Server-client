
#ifndef __SERVER_CONTROL_BASE_ADAPTOR_H__
#define __SERVER_CONTROL_BASE_ADAPTOR_H__

#include "SL_Socket_SendControl_HandlerManager.h"
#include "SL_ObjectPool.h"
#include "SL_Socket_TcpServer.h"


template <typename TServerHandler, typename TSocketRunner>
class CServerControlBaseAdaptor
{
public:
	CServerControlBaseAdaptor()
		:InitChunkSize_(1000),
		MaxChunkSize_(5000),
		SocketBufferSize_(1024 * 16),
		IoProcesserNum_(GetSystemProcessors() * 2)
	{
	}

	virtual ~CServerControlBaseAdaptor()
	{
		Destroy();
	}

	inline bool Initialize()
	{
		UserHandlerPool_.init(MaxChunkSize_, InitChunkSize_, 10);

		SocketRunner_.open(SL_Socket_Handler::ALL_EVENT_MASK, ConnectMax_, 10, IoProcesserNum_);

		SendControl_.open();

		TcpServerHandler_.set_interface(&UserHandlerPool_, &SocketRunner_);
		
		TcpServerHandler_.set_config(1, true, ConnectMax_, SocketBufferSize_, SocketBufferSize_, 4, 1);

		TcpServerHandler_.open(ListenPort_, 128);

		printf("Listening Port %d...\n", ListenPort_);

		return true;
	}

	inline void Destroy()
	{
		TcpServerHandler_.close();
		SendControl_.close();
		SocketRunner_.thread_stop();
		SocketRunner_.thread_wait();
		SocketRunner_.close();
	}

	inline int GetSystemProcessors()
	{
		SYSTEM_INFO si;

		GetSystemInfo(&si);

		return si.dwNumberOfProcessors;
	}

	inline void SetIoProcesserNum(int num)
	{
		IoProcesserNum_ = num;
	}

	inline void SetHostIP(string ip)
	{
		Host_ = ip;
	}

	inline string GetHostIP()
	{
		return Host_;
	}

	inline void SetListenPort(int port)
	{
		ListenPort_ = port;
	}

	inline int GetListenPort()
	{
		return ListenPort_;
	}

	inline void SetMaxConnect(int max)
	{
		ConnectMax_ = max;
	}

	inline void SetBufferSize(int size)
	{
		SocketBufferSize_ = size;
	}

	inline SL_Socket_SendControl_HandlerManager *GetSendControl()
	{
		return &SendControl_;
	}

	inline SL_ObjectPool_SimpleEx<TServerHandler, SL_Sync_ThreadMutex> *GetUserHandlerPool()
	{
		return &UserHandlerPool_;
	}

	inline SL_Socket_TcpServer<TServerHandler, SL_ObjectPool_SimpleEx<TServerHandler, SL_Sync_ThreadMutex>> *GetTcpServerHandler()
	{
		return &TcpServerHandler_;
	}

	inline TSocketRunner *GetSocketRunner()
	{
		return &SocketRunner_;
	}

private:
	TSocketRunner SocketRunner_;
	SL_Socket_SendControl_HandlerManager SendControl_;
	SL_ObjectPool_SimpleEx<TServerHandler, SL_Sync_ThreadMutex> UserHandlerPool_;
	SL_Socket_TcpServer<TServerHandler, SL_ObjectPool_SimpleEx<TServerHandler, SL_Sync_ThreadMutex>> TcpServerHandler_;

	int IoProcesserNum_;
	string Host_;
	int ListenPort_;
	int ConnectMax_;
	int SocketBufferSize_;
	unsigned int MaxChunkSize_;
	unsigned int InitChunkSize_;
};
#endif