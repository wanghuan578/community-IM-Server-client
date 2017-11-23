
#include "StdAfx.h"
#include "BusinessMessage.h"


IMPLEMENT_SINGLETON(CBusinessMessage)

bool CBusinessMessage::m_bChanged = false;
list<CBusinessMessageObserver*> CBusinessMessage::m_observers;
CCriticalSection *CBusinessMessage::m_pSection = NULL;

CBusinessMessage::CBusinessMessage(void)
{
	m_pSection = new CCriticalSection();
}

CBusinessMessage::~CBusinessMessage(void)
{
	if(m_pSection)
	{
		delete m_pSection;

		m_pSection = NULL;
	}
}

int CBusinessMessage::UIProc(int message_type, int error_code, void *message)
{
	if (NULL != m_pSection)
	{
		m_pSection->Lock();

		CClientBiz *pClientBiz = CClientBiz::Instance();			

		if (NULL != pClientBiz)
		{
			CBusinessMessageItem *pMessageItem = new CBusinessMessageItem;

			if (NULL != pMessageItem)
			{
				pMessageItem->m_nMessageType = message_type;
				pMessageItem->m_nEerrorCode = error_code;
				pMessageItem->m_pMessage = message;

				SetChanged();

				NotifyObservers((void*)pMessageItem);

				delete pMessageItem;
			}
		}
		
		m_pSection->Unlock();
	}
	
	return 0;
}

BOOL CBusinessMessage::InitClientBizCallBack()
{
	CClientBiz *pClientBiz = CClientBiz::Instance();

	if (NULL != pClientBiz)
	{
		pClientBiz->SetCallback(UIProc);

		return true;
	}
	
	return false;
};

void CBusinessMessage::AddObserver(CBusinessMessageObserver *pObserver)
{
	m_pSection->Lock();
	m_observers.push_back(pObserver);
	m_pSection->Unlock();
}

void CBusinessMessage::ClearChanged()
{
	m_bChanged = false;
}

int  CBusinessMessage::CountObservers()
{
	return m_observers.size();
}

bool CBusinessMessage::HasChanged()
{
	return m_bChanged;
}

void CBusinessMessage::DeleteObserver(CBusinessMessageObserver *pObserver)
{
	m_pSection->Lock();
	m_observers.remove(pObserver);
	m_pSection->Unlock();
}

void CBusinessMessage::DeleteObservers()
{
	m_pSection->Lock();
	m_observers.clear();
	m_pSection->Unlock();
}

void CBusinessMessage::NotifyObservers()
{
	NotifyObservers(NULL);
}

void CBusinessMessage::NotifyObservers(void *arg)
{
	m_pSection->Lock();

	if(m_bChanged)
	{
		for(list<CBusinessMessageObserver*>::iterator iter = m_observers.begin(); iter != m_observers.end(); iter++)
		{
			CBusinessMessageObserver *pBusinessMessageObserver = (*iter);

			pBusinessMessageObserver->UpdateBusinessMessage(m_pInstance, arg);
		}

		ClearChanged();
	}

	m_pSection->Unlock();
}

void CBusinessMessage::SetChanged()
{
	m_bChanged = true;
}
