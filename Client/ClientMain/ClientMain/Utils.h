#pragma once





// singleton
#define DECLARE_SINGLETON(x)	\
	static x *m_pInstance;			\
	static x *Instance()			\
{								\
	if (!m_pInstance)			\
	m_pInstance = new x;	\
	return m_pInstance;			\
}								\
	static void ReleaseInstance()	\
{								\
	delete m_pInstance;			\
	m_pInstance = NULL;			\
}

#define IMPLEMENT_SINGLETON(x)	x *x::m_pInstance = NULL;



// usage: #pragma mesage(__FILELINE__, "message-text")
#define __STR2__(x) #x
#define __STR1__(x) __STR2__(x)
#define __FILELINE__ __FILE__ "("__STR1__(__LINE__)") : "


class CInstanceLockBase
{
public:
	CRITICAL_SECTION m_cs;
	void Lock() { EnterCriticalSection(&m_cs); }
	void Unlock() { LeaveCriticalSection(&m_cs); }

} ;

class CInstanceLock
{
public:
	CInstanceLockBase * _pObj;

public:
	CInstanceLock(CInstanceLockBase * pObj)
	{
		_pObj = pObj;
		if (NULL != _pObj)
			_pObj->Lock();
	}
	~CInstanceLock()
	{
		if (NULL != _pObj)
			_pObj->Unlock();
	}
};



/*
CSIDL_BITBUCKET   回收站    
CSIDL_CONTROLS   控制面板    
CSIDL_DESKTOP   Windows   桌面Desktop    
CSIDL_DESKTOPDIRECTORY   Desktop的目录    
CSIDL_DRIVES   我的电脑    
CSIDL_FONTS   字体目录    
CSIDL_NETHOOD   网上邻居    
CSIDL_NETWORK   网上邻居虚拟目录    
CSIDL_PERSONAL   我的文档    
CSIDL_PRINTERS   打印机    
CSIDL_PROGRAMS   程序组    
CSIDL_RECENT   最近打开的文档    
CSIDL_SENDTO   “发送到”菜单项    
CSIDL_STARTMENU   任务条启动菜单项    
CSIDL_STARTUP   启动目录    
CSIDL_TEMPLATES   文档模板 
*/
wstring GetSysDirectory(int nCsidl);

LPCTSTR GetModuleFilePath();

int duMessageBox(HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType);


UINT GetRandom(UINT nLowerLimit, UINT UpperLimit);

wstring&   replace_all(wstring& str,const wstring& old_value,const wstring& new_value);

wstring&   replace_all_distinct(wstring& str,const wstring& old_value,const wstring& new_value);