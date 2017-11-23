
#pragma once

#include <stdio.h>
#include <string>

using namespace std;

#ifdef USE_SERVER_BIZ_DLL
#ifdef EXPORT_SERVER_BIZ_DLL
#define SERVER_BIZ_API __declspec(dllexport)
#else
#define SERVER_BIZ_API __declspec(dllimport)
#endif
#else
#define SERVER_BIZ_API
#endif





class SERVER_BIZ_API CServerBiz
{
public:

	static CServerBiz *Instance()
	{
		if(NULL == m_pInstance)
		{
			m_pInstance = new CServerBiz();
		}

		return m_pInstance;
	}

	static void Destroy()
	{
		delete m_pInstance;

		m_pInstance = NULL;
	}

	bool Initialize();
	void SetServerID(int id);
	void SetServerName(string name);
	void SetServerPort(int port);
	void SetBufferSize(int size);
	void SetConnectMax(int max);
	//void SetSessionAddress(string ip);
	//void SetSessionPort(int port);

private:
	CServerBiz();
	~CServerBiz();


private:
	static CServerBiz				*m_pInstance;
	int								ListenPort_;
	int								BuffSize_;
};