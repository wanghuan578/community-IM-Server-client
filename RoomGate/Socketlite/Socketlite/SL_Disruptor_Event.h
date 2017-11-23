////////////////////////////////////////////////////////////////////////////////
//  basic structure of Disruptor event

#ifndef SOCKETLITE_DISRUPTOR_EVENT_H
#define SOCKETLITE_DISRUPTOR_EVENT_H
#include "SL_Seda_Event.h"

typedef SL_Seda_Event               SL_Disruptor_Event;
typedef SL_Seda_QuitEvent           SL_Disruptor_QuitEvent;
typedef SL_Seda_ThreadIdleEvent     SL_Disruptor_ThreadIdleEvent;
typedef SL_Seda_TimerExpireEvent    SL_Disruptor_TimerExpireEvent;
typedef SL_Seda_TcpSocketOpenEvent  SL_Disruptor_TcpSocketOpenEvent;
typedef SL_Seda_TcpSocketCloseEvent SL_Disruptor_TcpSocketCloseEvent;
typedef SL_Seda_TcpReadDataEvent    SL_Disruptor_TcpReadDataEvent;
typedef SL_Seda_TcpReadMessageEvent SL_Disruptor_TcpReadMessageEvent;
typedef SL_Seda_RpcMessageProc      SL_Disruptor_RpcMessageProc;
typedef SL_Seda_RpcMessageEvent     SL_Disruptor_RpcMessageEvent;

#endif

