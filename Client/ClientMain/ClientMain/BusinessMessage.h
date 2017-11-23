#pragma once

#include "Observer.h"

#include "../../ClientBiz/ClientBiz/ClientBiz.h"
#include "SL_Rpc_Message.h"
#include "../../ClientBiz/Internal/MessageMapping.h"
//#include "atlsync.h"
#include "afxmt.h"
#include "utils.h"
#include<iostream>


#define BUSINESSMESSAGE_TRAVERSELIST -10000
#define WM_BUSINESSMESSAGE WM_USER + 3000


class CBusinessMessageItem
{
public:
	CBusinessMessageItem()
	{
		m_nMessageType = 0;
		m_nEerrorCode = 0;
		m_pMessage = NULL;
		m_nListType = 0;
	};

	~CBusinessMessageItem()
	{
		if(m_pMessage)
		{
			if(BUSINESSMESSAGE_TRAVERSELIST != m_nMessageType)
			{
				//ClientBiz::instance()->release_message( m_nMessageType, m_pMessage);	
			}
		}
	};

public:
	int m_nMessageType;			//如果为 -10000号，那么这个消息就是人员列表消息
	int m_nEerrorCode;			//错误代码
	void *m_pMessage;			//具体消息内容，需要根据m_nMessageType去反查
	short m_nListType;			//当m_nMessageType为-10000号的时候才用得上

};


class CBusinessMessage;
class CBusinessMessageObserver
{
public:
	CBusinessMessageObserver(){};
	~CBusinessMessageObserver(){};
	virtual void UpdateBusinessMessage(CBusinessMessage *pObserable, void *arg) = 0;

};



class CBusinessMessage: public CInstanceLockBase
{
public:
	DECLARE_SINGLETON(CBusinessMessage)

	CBusinessMessage(void);
	~CBusinessMessage(void);

	BOOL InitClientBizCallBack();
	

public:
	static int UIProc(int message_type, int error_code, void *message);
	//static int Traverselist(short list_type, void *message=NULL);	

	static void AddObserver(CBusinessMessageObserver *pObserver);				//增加观察者，其它地方只需要用到这一对函数

	static void ClearChanged();													
	static int  CountObservers();
	static bool HasChanged();
	static void DeleteObserver(CBusinessMessageObserver *pObserver);
	static void DeleteObservers();												//删除观察者，其它地方只需要用到这一对函数

	static void NotifyObservers();
	static void NotifyObservers(void *arg);
	static void SetChanged();

private:
	static list< CBusinessMessageObserver* > m_observers;
	static bool   m_bChanged;
	
	static CCriticalSection *m_pSection;

};

