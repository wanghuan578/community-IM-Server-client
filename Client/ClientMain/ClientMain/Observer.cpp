#include "StdAfx.h"
#include "Observer.h"


///////////////////////////////////////
//Observer

CObserver::CObserver(void)
{
}

CObserver::~CObserver(void)
{
}



///////////////////////////////////////
//Observable

CObservable::CObservable(void)
{
	m_bChanged = false;
}

CObservable::~CObservable(void)
{
	DeleteObservers();
}

void CObservable::AddObserver(CObserver *pObserver)
{
	m_observers.push_back(pObserver);
}

void CObservable::ClearChanged()
{
	m_bChanged = false;
}

int  CObservable::CountObservers()
{
	return m_observers.size();
}

bool CObservable::HasChanged()
{
	return m_bChanged;
}

void CObservable::DeleteObserver(CObserver *pObserver)
{
	m_observers.remove(pObserver);
}

void CObservable::DeleteObservers()
{
	m_observers.clear();
}

void CObservable::NotifyObservers()
{
	NotifyObservers(NULL);
}

void CObservable::NotifyObservers(void *arg)
{
	if(m_bChanged)
	{
		list<CObserver*>::iterator iter;
		for(iter = m_observers.begin();iter!=m_observers.end();iter++)
		{
			(*iter)->Update(this,arg);
		}
		ClearChanged();
	}
}

void CObservable::SetChanged()
{
	m_bChanged = true;
}