
// RPC Client DemoDlg.h : 头文件
//

#pragma once

#include "SL_DataType.h"
#include "SL_Sync_Condition.h"
#include "BusinessMessage.h"

using namespace std;

//#ifdef _DEBUG
//#define DEBUG_CLIENTBLOCK   new( _CLIENT_BLOCK, __FILE__, __LINE__)
//#else
//#define DEBUG_CLIENTBLOCK
//#endif
//#define _CRTDBG_MAP_ALLOC
//#include <crtdbg.h>
//#ifdef _DEBUG
//#define new DEBUG_CLIENTBLOCK
//#endif
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define SERVER_IP_ADDRESS		"116.62.159.79"
//#define SERVER_IP_ADDRESS		"127.0.0.1"
#define SERVER_PORT				9090
#define DEFAULT_USERNAME		10000003
#define DEFAULT_PASSWORD		123456
#define SENDING_INTERVAL_MS		30
#define DEFAULT_MESSAGE_LEN (1024 * 16)
#define THREAD_NUM				1
#define CHAT_TIMES				-1

enum 
{
	LIST_INFO_TYPE_SYSTEM,
	LIST_INFO_TYPE_ROOMLIST,
	LIST_INFO_TYPE_USERLIST,
	LIST_INFO_TYPE_ROOM,
};

enum
{
	TIMER_MESSAGE_BLINK,
};

typedef struct RoomInfo
{
	int room_id;
	string room_name;
};

// CRPCClientDemoDlg 对话框
class CRPCClientDemoDlg : public CDialogEx, public CBusinessMessageObserver
{
// 构造
public:
	CRPCClientDemoDlg(CWnd* pParent = NULL);	// 标准构造函数
	virtual ~CRPCClientDemoDlg();

// 对话框数据
	enum { IDD = IDD_RPCCLIENTDEMO_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnRPCMessage(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedLogin();
	afx_msg void OnBnClickedLogout();
	afx_msg void OnBnClickedChatRequest();
	afx_msg void OnBnClickedIsBoradcast();
	afx_msg void OnBnClickedStop();
	BOOL PreTranslateMessage(MSG* pMsg);
	LRESULT OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

public:
	void InitListCtrl();
	void AppendLog(int log_type, const CString info);
	void AppendLog(char *info, int value);
	void AppendLog(char *info, string value);
	void RefreshLog();
	int	StartGetVideoData();
	int	RefreshData();
	static unsigned int WINAPI thread_proc(void *arg);
	static unsigned int WINAPI refresh_proc(void *arg);
	void UpdateBusinessMessage(CBusinessMessage *pObserable, void *arg);
	int InitEnterRoomCtrl(BOOL b);

private:
	void OnSetMessageBlinkTimer();

private:
	CListCtrl	*m_ListInfo;
	CListCtrl	*m_ListRoomList;
	CListCtrl	*m_ListRoom;
	CListCtrl	*m_ListUserList;


	int m_Times;
	int m_ClientMax;
	uint64 m_StartTime;
	//SL_Sync_Condition m_SycnEvent;
	bool m_IsSequence;
	int64 m_CurrentIndex;
	std::list<f64> consuption_list;
	int interval;
	//CClientBiz *m_Client;
	string Username_;
	string Password_;
	int SystemlogIndex_;
	int RoomListIndex_;
	int UserListIndex_;
	int RoomIndex_;

public:
	afx_msg void OnBnClickedConnect();
	afx_msg void OnBnClickedDisconnect();
	afx_msg void OnBnClickedCommit();
	afx_msg void OnBnClickedChatSequence();
	afx_msg void OnIdclose();
	afx_msg void OnNMDblclkListRoom(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkListRoomlist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemchangedListInfo(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedPrivate();
	afx_msg void OnBnClickedLeave();
	afx_msg void OnBnClickedSend();
};
