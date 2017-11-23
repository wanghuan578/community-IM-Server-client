#pragma once

#include <list>
using namespace std;

class CObservable;
class CObserver
{
public:
	virtual ~CObserver(void);
	virtual void Update(CObservable *pObserable,void *arg)=0;
protected:
	CObserver(void);
};


class CObservable
{
public:
	CObservable();
	virtual ~CObservable();
	void AddObserver(CObserver *pObserver);
	
	void ClearChanged();
	int  CountObservers();
	bool HasChanged();
	void DeleteObserver(CObserver *pObserver);
	void DeleteObservers();
	
	void NotifyObservers();
	void NotifyObservers(void *arg);
	void SetChanged();

private:
	std::list< CObserver* > m_observers;
	bool   m_bChanged;
};
