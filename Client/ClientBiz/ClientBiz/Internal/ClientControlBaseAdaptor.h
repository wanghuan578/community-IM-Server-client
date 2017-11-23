
#pragma once

#include "SL_Socket_TcpClient_AutoConnect.h"
#include "SL_Socket_SendControl_HandlerManager.h"
#include "SL_ObjectPool.h"
#include "SL_Socket_TcpClient.h"
#include <string>
using namespace std;

template <typename TClientHandler, typename TSocketRunner>
class CClientControlBaseAdaptor
{
public:
	CClientControlBaseAdaptor()
		:InitChunkSize_(1),
		MaxChunkSize_(2),
		ConnectTimeout_(0),
		SocketBufferSize_(1024 * 16)
	{
	}

	virtual ~CClientControlBaseAdaptor()
	{
		Destroy();
	}

	inline bool Initialize()
	{
		ClientHandlerPool_.init(MaxChunkSize_, InitChunkSize_, 1);

		SocketRunner_.set_buffer_size(SocketBufferSize_, SocketBufferSize_);

		TcpClientHandler_.set_interface(&ClientHandlerPool_, &SocketRunner_);
		TcpClientHandler_.set_config(ConnectTimeout_, false, true, SocketBufferSize_, SocketBufferSize_, 4, 1);
		TcpAutoConnect_.open();
		TcpClientHandler_.set_autoconnect(false);

		SendControl_.open(1, SocketBufferSize_, 8, 0, 50, 10, 200);
		SocketRunner_.open(SL_Socket_Handler::ALL_EVENT_MASK, 1000, 100, 4);
		SocketRunner_.thread_event_loop(100);

		if (!TcpClientHandler_.open(IPAddress_.c_str(), Port_))
		{
			return true;
		}

		return false;
	}

	inline void Destroy()
	{
		TcpClientHandler_.close();
		SendControl_.close();
		SocketRunner_.thread_stop();
		SocketRunner_.thread_wait();
		SocketRunner_.close();
	}

	inline void SetIpAddress(string ip)
	{
		IPAddress_ = ip;
	}

	inline void SetPort(int port)
	{
		Port_ = port;
	}

	inline void SetBufferSize(int size)
	{
		SocketBufferSize_ = size;
	}

	inline void SetConnectTimeout(int time_out)
	{
		ConnectTimeout_ = time_out;
	}

	inline int GetClientID()
	{
		return ServerID_;
	}

	inline void SetClientID(int id)
	{
		ServerID_ = id;
	}

	inline SL_Socket_SendControl_HandlerManager *GetSendControl()
	{
		return &SendControl_;
	}

	inline SL_ObjectPool_SimpleEx<TClientHandler, SL_Sync_ThreadMutex> *GetClientHandlerPool()
	{
		return &ClientHandlerPool_;
	}

	inline SL_Socket_TcpClient<TClientHandler, SL_ObjectPool_SimpleEx<TClientHandler, SL_Sync_ThreadMutex>> *GetTcpClientHandler()
	{
		return &TcpClientHandler_;
	}

	inline TSocketRunner *GetSocketRunner()
	{
		return &SocketRunner_;
	}

private:
	TSocketRunner SocketRunner_;
	SL_Socket_SendControl_HandlerManager SendControl_;
	SL_ObjectPool_SimpleEx<TClientHandler, SL_Sync_ThreadMutex> ClientHandlerPool_;
	SL_Socket_TcpClient_AutoConnect TcpAutoConnect_;
	SL_Socket_TcpClient<TClientHandler, SL_ObjectPool_SimpleEx<TClientHandler, SL_Sync_ThreadMutex>> TcpClientHandler_;
	
	string ServerName_;
	int ServerID_;
	string IPAddress_;
	int Port_;
	int SocketBufferSize_;
	unsigned int MaxChunkSize_;
	unsigned int InitChunkSize_;
	int ConnectTimeout_;
};