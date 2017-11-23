
// RPC Client DemoDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "RPC Client Demo.h"
#include "RPC Client DemoDlg.h"
#include "afxdialogex.h"
#include "CommonTypes.h"

#include <iostream>
#include "../ClientBiz/ClientBiz.h"



#pragma comment(lib,"../debug/ClientBiz.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//#define TCP_RECVBUFF_LEN	(1024*300 + 1024*50)
// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CRPCClientDemoDlg 对话框




CRPCClientDemoDlg::CRPCClientDemoDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CRPCClientDemoDlg::IDD, pParent),
	SystemlogIndex_(0),
	RoomListIndex_(0),
	UserListIndex_(0),
	RoomIndex_(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	//_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
}

CRPCClientDemoDlg::~CRPCClientDemoDlg()
{
	//CClientBiz::Destroy();
}

void CRPCClientDemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CRPCClientDemoDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_LOGIN, &CRPCClientDemoDlg::OnBnClickedLogin)
	ON_BN_CLICKED(IDC_LOGOUT, &CRPCClientDemoDlg::OnBnClickedLogout)
	ON_BN_CLICKED(IDC_CHAT_REQUEST, &CRPCClientDemoDlg::OnBnClickedChatRequest)
	ON_BN_CLICKED(IDC_BROADCAST, &CRPCClientDemoDlg::OnBnClickedIsBoradcast)
	ON_MESSAGE(WM_RPC_MESSAGE, &CRPCClientDemoDlg::OnRPCMessage)
	ON_BN_CLICKED(IDC_CONNECT, &CRPCClientDemoDlg::OnBnClickedConnect)
	ON_BN_CLICKED(IDC_DISCONNECT, &CRPCClientDemoDlg::OnBnClickedDisconnect)
	ON_BN_CLICKED(IDC_COMMIT, &CRPCClientDemoDlg::OnBnClickedCommit)
	ON_BN_CLICKED(IDC_CHAT_SEQUENCE, &CRPCClientDemoDlg::OnBnClickedChatSequence)
	ON_COMMAND(IDCLOSE, &CRPCClientDemoDlg::OnIdclose)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_ROOM, &CRPCClientDemoDlg::OnNMDblclkListRoom)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_ROOMLIST, &CRPCClientDemoDlg::OnNMDblclkListRoomlist)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_INFO, &CRPCClientDemoDlg::OnLvnItemchangedListInfo)
	ON_BN_CLICKED(IDC_PRIVATE, &CRPCClientDemoDlg::OnBnClickedPrivate)
	ON_BN_CLICKED(IDC_LEAVE, &CRPCClientDemoDlg::OnBnClickedLeave)
	ON_BN_CLICKED(IDC_SEND, &CRPCClientDemoDlg::OnBnClickedSend)
END_MESSAGE_MAP()


// CRPCClientDemoDlg 消息处理程序

BOOL CRPCClientDemoDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	//_CrtSetDbgFlag ( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}


	SetIcon(m_hIcon, TRUE);			
	SetIcon(m_hIcon, FALSE);		


	CBusinessMessage::Instance()->InitClientBizCallBack();
	CBusinessMessage::Instance()->AddObserver(this);

	InitEnterRoomCtrl(TRUE);

	SetDlgItemInt(IDC_USERNAME, DEFAULT_USERNAME);
	SetDlgItemInt(IDC_PASSWORD, DEFAULT_PASSWORD);
	SetDlgItemInt(IDC_PORT, SERVER_PORT);
	SetDlgItemText(IDC_IPADDRESS, _T(SERVER_IP_ADDRESS));

	GetDlgItem(IDC_LIST_ROOM)->ShowWindow(FALSE);

	GetDlgItem(IDC_THRIFT)->EnableWindow(FALSE);
	GetDlgItem(IDC_AES)->EnableWindow(FALSE);
	GetDlgItem(IDC_USERNAME)->EnableWindow(FALSE);
	GetDlgItem(IDC_PASSWORD)->EnableWindow(FALSE);

	m_ListInfo = (CListCtrl*)GetDlgItem(IDC_LIST_INFO);
	m_ListInfo->SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_ListInfo->InsertColumn(0, L"SystemLog", LVCFMT_JUSTIFYMASK, 800);

	m_ListRoomList = (CListCtrl*)GetDlgItem(IDC_LIST_ROOMLIST);
	m_ListRoomList->SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_ListRoomList->InsertColumn(0, L"RoomList", LVCFMT_CENTER, 204);

	//m_ListRoom = (CListCtrl*)GetDlgItem(IDC_LIST_ROOM);
	//m_ListRoom->SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	//m_ListRoom->InsertColumn(0, L"RoomList", LVCFMT_CENTER, 500);

	m_ListUserList = (CListCtrl*)GetDlgItem(IDC_LIST_USERLIST);
	m_ListUserList->SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_ListUserList->InsertColumn(0, L" UserList", LVCFMT_CENTER, 204);

	CButton* pBtn = (CButton*)GetDlgItem(IDC_THRIFT);
	pBtn->SetCheck(1);

	return TRUE;
}

int CRPCClientDemoDlg::InitEnterRoomCtrl(BOOL b)
{
	GetDlgItem(IDC_LIST_USERLIST)->ShowWindow(!b);
	GetDlgItem(IDC_LIST_ROOM)->ShowWindow(!b);
	GetDlgItem(IDC_BROADCAST)->ShowWindow(!b);

	GetDlgItem(IDC_STATIC_CHAT_BOX)->ShowWindow(!b);
	GetDlgItem(IDC_INPUT)->ShowWindow(!b);
	GetDlgItem(IDC_SEND)->ShowWindow(!b);
	GetDlgItem(IDC_STATIC_OPTION2_BOX)->ShowWindow(!b);
	GetDlgItem(IDC_LEAVE)->ShowWindow(!b);
	GetDlgItem(IDC_PRIVATE)->ShowWindow(!b);


	GetDlgItem(IDC_LIST_ROOMLIST)->ShowWindow(b);
	GetDlgItem(IDC_LIST_INFO)->ShowWindow(b);

	GetDlgItem(IDC_STATIC_IP_BOX)->ShowWindow(b);
	GetDlgItem(IDC_STATIC_ADDR)->ShowWindow(b);
	GetDlgItem(IDC_STATIC_PORT)->ShowWindow(b);
	GetDlgItem(IDC_IPADDRESS)->ShowWindow(b);
	GetDlgItem(IDC_PORT)->ShowWindow(b);
	GetDlgItem(IDC_CONNECT)->ShowWindow(b);
	
	GetDlgItem(IDC_STATIC_LOGIN_BOX)->ShowWindow(b);
	GetDlgItem(IDC_STATIC_OPTION_BOX)->ShowWindow(b);
	GetDlgItem(IDC_STATIC_USERNAME)->ShowWindow(b);
	GetDlgItem(IDC_STATIC_PWD)->ShowWindow(b);
	GetDlgItem(IDC_USERNAME)->ShowWindow(b);
	GetDlgItem(IDC_PASSWORD)->ShowWindow(b);
	GetDlgItem(IDC_LOGIN)->ShowWindow(b);
	GetDlgItem(IDC_LOGOUT)->ShowWindow(b);
	GetDlgItem(IDC_AES)->ShowWindow(b);
	GetDlgItem(IDC_THRIFT)->ShowWindow(b);
	GetDlgItem(IDC_KEEPALIVE)->ShowWindow(b);

	return 0;
}

void CRPCClientDemoDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

void CRPCClientDemoDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this);

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

HCURSOR CRPCClientDemoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CRPCClientDemoDlg::UpdateBusinessMessage(CBusinessMessage *pObserable, void *arg)
{
	CBusinessMessageItem *pMessageItem = (CBusinessMessageItem*)arg;

	char buff[256];

	switch (pMessageItem->m_nMessageType)
	{
	case common::MessageType::MT_HELLO_NOTIFY:
		{
			if (SERVER_REGISTER_TYPE_LOGIN == pMessageItem->m_nEerrorCode)
			{
				GetDlgItem(IDC_IPADDRESS)->EnableWindow(FALSE);
				GetDlgItem(IDC_PORT)->EnableWindow(FALSE);

				sprintf_s(buff, 256, "Login Server Connected!");

				AppendLog(LIST_INFO_TYPE_SYSTEM, CString(buff));

				GetDlgItem(IDC_USERNAME)->EnableWindow(TRUE);
				GetDlgItem(IDC_PASSWORD)->EnableWindow(TRUE);
			}
			else if (SERVER_REGISTER_TYPE_ROOMGATE == pMessageItem->m_nEerrorCode)
			{
				sprintf_s(buff, 256, "RoomGate Connected!");

				AppendLog(LIST_INFO_TYPE_SYSTEM, CString(buff));
			}
		}
		break;

	case login_server::MessageType::MT_CLIENT_LOGIN_RES:
		{
			if (common::ErrorCode::OK == pMessageItem->m_nEerrorCode)
			{
				GetDlgItem(IDC_LOGIN)->EnableWindow(FALSE);

				sprintf_s(buff, 256, "Login Succeed!");

				GetDlgItem(IDC_USERNAME)->EnableWindow(FALSE);
				GetDlgItem(IDC_PASSWORD)->EnableWindow(FALSE);
			}
			else if (common::ErrorCode::LOGNAME_EXIST == pMessageItem->m_nEerrorCode)
			{
				sprintf_s(buff, 256, "Login Name Exist!");
			}
			else
			{
				sprintf_s(buff, 256, "Login Failed! ErrorCode = %d", pMessageItem->m_nEerrorCode);
			}
			
			AppendLog(LIST_INFO_TYPE_SYSTEM, CString(buff));
		}
		break;

	case room_gate::MessageType::MT_CONNECT_RES:
		{
			if (common::ErrorCode::OK == pMessageItem->m_nEerrorCode)
			{
				sprintf_s(buff, 256, "RoomGate Connected!");
			}
			else
			{
				sprintf_s(buff, 256, "RoomGate Connected Failed! ErrorCode = %d", pMessageItem->m_nEerrorCode);
			}

			AppendLog(LIST_INFO_TYPE_SYSTEM, CString(buff));
		}
		break;

	case room_gate::MessageType::MT_GET_ROOMLIST_RES:
		{
			MsgGetRoomListRes *res = (MsgGetRoomListRes*)pMessageItem->m_pMessage;

			vector<room_gate::RoomInfo>::iterator iter;

			for (iter = res->body_.room_list.begin(); iter != res->body_.room_list.end(); iter++)
			{
				room_server::RoomInfo room_info;

				//room_info.room_id = iter->room_id;
				room_info.room_name = iter->room_name;
				CClientBiz::Instance()->GetRoomList()->push_back(room_info);
	

				sprintf_s(buff, 256, "(%d) %s", iter->room_id, iter->room_name.c_str());

				AppendLog(LIST_INFO_TYPE_ROOMLIST, CString(buff));
			}
		}
		break;

	case room_server::MessageType::MT_ENTER_ROOM_RES:
		{
			MsgEnterRoomRes *res = (MsgEnterRoomRes*)(pMessageItem->m_pMessage);

			CClientBiz::Instance()->SetRoomID(res->head_.attach_id1);

			if (common::ErrorCode::OK == res->body_.error_code)
			{
				InitEnterRoomCtrl(FALSE);

				//sprintf_s(buff, 256, "%s:  %s (%d)", res->body_.room_info.room_theme.c_str(), res->body_.room_info.room_name.c_str(), res->head_.attach_id1);

				m_ListRoom = (CListCtrl*)GetDlgItem(IDC_LIST_ROOM);
				m_ListRoom->SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
				m_ListRoom->InsertColumn(RoomIndex_++, CString(buff), LVCFMT_CENTER, 500);

				//sprintf_s(buff, 256, "                                           %s", res->body_.room_info.welcome_phrase.c_str());

				AppendLog(LIST_INFO_TYPE_ROOM, CString(buff));
			}
			else if (common::ErrorCode::ROOM_PASSORD_ERROR == res->body_.error_code)
			{
				sprintf_s(buff, 256, "%s", res->body_.error_text.c_str());

				AppendLog(LIST_INFO_TYPE_SYSTEM, CString(buff));
			}
			
		}
		break;
		
	case room_server::MessageType::MT_GET_USER_LIST_RES:
		{
			MsgGetUserListRes *res = (MsgGetUserListRes*)pMessageItem->m_pMessage;
			
			if (0 == res->body_.error_code)
			{
				//vector<common::UserInfo>::iterator iter;

				//for (iter = res->body_.user_list.begin(); iter != res->body_.user_list.end(); iter++)
				//{
				//	CClientBiz::Instance()->GetUserList()->push_back(*iter);

				//	sprintf_s(buff, 256, "%s (%d)", iter->user_name.c_str(), iter->user_id);

				//	AppendLog(LIST_INFO_TYPE_USERLIST, CString(buff));
				//}
			}
		}
		break;

	case room_server::MessageType::MT_CHAT_NOTIFY:
		{
			MsgRoomServerChatNotify *notify = (MsgRoomServerChatNotify*)pMessageItem->m_pMessage;

			if (notify->head_.source == CClientBiz::Instance()->GetUserID())
			{
				if (notify->head_.destination == CClientBiz::Instance()->GetUserID())
				{
					sprintf_s(buff, 256, "%s", "Can not talk to yourself!");
				}
				else
				{
					sprintf_s(buff, 256, "%s(%d) to %s(%d): %s", "Me", notify->head_.source, notify->body_.to_nick_name.c_str(), notify->head_.destination, notify->body_.chat_text.c_str());
				}
			}
			else
			{
				sprintf_s(buff, 256, "%s(%d) to %s(%d): %s", notify->body_.from_nick_name.c_str(), notify->head_.source, notify->body_.to_nick_name.c_str(), notify->head_.destination, notify->body_.chat_text.c_str());
			}
			
			AppendLog(LIST_INFO_TYPE_ROOM, CString(buff));

			OnSetMessageBlinkTimer();
			
		}
		break;

	case room_server::MessageType::MT_LEAVE_ROOM_RES:
		{
			MsgLeaveRoomRes *res = (MsgLeaveRoomRes*)pMessageItem->m_pMessage;

			if (common::ErrorCode::OK == res->body_.error_code)
			{
				InitEnterRoomCtrl(TRUE);

				m_ListRoom->DeleteAllItems();

				m_ListUserList->DeleteAllItems();
			}
		}
		break;

	case room_server::MessageType::MT_ENTER_ROOM_NOTIFY:
		{
			MsgEnterRoomNotify *notify = (MsgEnterRoomNotify*)pMessageItem->m_pMessage;

			sprintf_s(buff, 256, "%s (%d)", notify->body_.user_name.c_str(), notify->body_.user_id);

			AppendLog(LIST_INFO_TYPE_USERLIST, CString(buff));

			sprintf_s(buff, 256, "%s (%d) 加入聊天室.", notify->body_.user_name.c_str(), notify->body_.user_id);

			AppendLog(LIST_INFO_TYPE_ROOM, CString(buff));

		}
		break;

	default:
		break;
	}
}

LRESULT CRPCClientDemoDlg::OnTimer(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	switch(wParam)
	{
	case TIMER_MESSAGE_BLINK:
		{
			if (IsWindowVisible())  
			{
				KillTimer(TIMER_MESSAGE_BLINK);
			}
			else
			{
				FlashWindow(TRUE);
			}
		}
		break;

	default:
		break;
	}

	return 0;
}

void CRPCClientDemoDlg::OnSetMessageBlinkTimer()
{
	if (0)
	{
		SetTimer(TIMER_MESSAGE_BLINK, 500, NULL);
	}
}

void CRPCClientDemoDlg::OnBnClickedLogin()
{
	CString username;
	CString password;

	GetDlgItemText(IDC_USERNAME, username);
	GetDlgItemText(IDC_PASSWORD, password);
	Username_ = CW2A(username.GetBuffer(), CP_UTF8);
	Password_ = CW2A(password.GetBuffer(), CP_UTF8);

	CClientBiz::Instance()->Login(atoi(Username_.c_str()), Password_);
}


void CRPCClientDemoDlg::OnBnClickedLogout()
{
}


void CRPCClientDemoDlg::OnBnClickedChatRequest()
{
	m_IsSequence = false;

	int message_len = GetDlgItemInt(IDC_MESSAGE_LEN);
	CButton* pBtn = (CButton*)GetDlgItem(IDC_BROADCAST);
	int state = 1;
	if (pBtn->GetCheck())
	{
		state = 3;
	}
}


void CRPCClientDemoDlg::OnBnClickedIsBoradcast()
{
}


void CRPCClientDemoDlg::OnBnClickedStop()
{
}

LRESULT CRPCClientDemoDlg::OnRPCMessage(WPARAM wParam, LPARAM lParam)
{
	int msg_type = (int)wParam;
	char *msg = (char *)lParam;

	return 0;
}

void CRPCClientDemoDlg::InitListCtrl()
{
	
}

void CRPCClientDemoDlg::AppendLog(int log_type, const CString info)
{
	switch (log_type)
	{
	case LIST_INFO_TYPE_ROOM:
		{
			m_ListRoom->InsertItem(RoomIndex_++, info);
		}
		break;

	case LIST_INFO_TYPE_SYSTEM:
		{
			m_ListInfo->InsertItem(SystemlogIndex_++, info);
		}
		break;

	case LIST_INFO_TYPE_ROOMLIST:
		{
			m_ListRoomList->InsertItem(RoomListIndex_++, info);
		}
		break;

	case LIST_INFO_TYPE_USERLIST:
		{
			m_ListUserList->InsertItem(UserListIndex_++, info);
		}
		break;

	default:
		break;
	}
}

void CRPCClientDemoDlg::AppendLog(char *info, int value)
{
	char buff[128];

	sprintf_s(buff, 128, "%s = %d", info, value);

	CString message = CA2W(buff);

	m_ListInfo->InsertItem(0, message);
}

void CRPCClientDemoDlg::AppendLog(char *info, string value)
{
	char buff[512];

	sprintf_s(buff, 512, "%s = %s", info, value.c_str());

	CString message = CA2W(buff);

	m_ListInfo->InsertItem(0, message);
}

void CRPCClientDemoDlg::RefreshLog()
{
	m_ListInfo->DeleteAllItems();
}

void CRPCClientDemoDlg::OnBnClickedConnect()
{
	GetDlgItem(IDC_CONNECT)->EnableWindow(FALSE);
	GetDlgItem(IDC_KEEPALIVE)->EnableWindow(FALSE);
	m_ClientMax = GetDlgItemInt(IDC_THREAD);
	
	int port = GetDlgItemInt(IDC_PORT);
	CString ip_addr;
	GetDlgItemText(IDC_IPADDRESS, ip_addr);
	string ip = CW2A(ip_addr.GetBuffer(), CP_UTF8);
	char value[25];
	itoa(DEFAULT_USERNAME, value, 10);
	CClientBiz::Instance()->Open(ip, port);
}


void CRPCClientDemoDlg::OnBnClickedDisconnect()
{
	GetDlgItem(IDC_DISCONNECT)->EnableWindow(FALSE);
	GetDlgItem(IDC_IPADDRESS)->EnableWindow(TRUE);
	GetDlgItem(IDC_PORT)->EnableWindow(TRUE);
	GetDlgItem(IDC_CONNECT)->EnableWindow(TRUE);
	GetDlgItem(IDC_LOGOUT)->EnableWindow(FALSE);
	GetDlgItem(IDC_USERNAME)->EnableWindow(FALSE);
	GetDlgItem(IDC_PASSWORD)->EnableWindow(FALSE);
	GetDlgItem(IDC_LOGIN)->EnableWindow(FALSE);
}

void CRPCClientDemoDlg::OnBnClickedCommit()
{

}

void CRPCClientDemoDlg::OnBnClickedChatSequence()
{

	consuption_list.clear();
	m_CurrentIndex = 1;
	m_IsSequence = true;

	int message_len = GetDlgItemInt(IDC_MESSAGE_LEN);
	m_Times = GetDlgItemInt(IDC_TIMES);
	m_ClientMax = GetDlgItemInt(IDC_THREAD);

	interval = GetDlgItemInt(IDC_INTERVAL);

	CButton* pBtn = (CButton*)GetDlgItem(IDC_BROADCAST);
	int state = 1;
	if (pBtn->GetCheck())
	{
		state = 3;
	}


}

int	CRPCClientDemoDlg::RefreshData()
{
	//SL_Thread thread;

	//thread.start(refresh_proc, this);

	return 0;
}

unsigned int WINAPI CRPCClientDemoDlg::refresh_proc(void *arg)
{
	CRPCClientDemoDlg *UI = (CRPCClientDemoDlg*)arg;

	while (1)
	{
		Sleep(10000);
		
		UI->RefreshLog();
	}
}

int	CRPCClientDemoDlg::StartGetVideoData()
{
	//SL_Thread thread;

	//thread.start(thread_proc, this);

	return 0;
}



void CRPCClientDemoDlg::OnIdclose()
{
	//CClientBizIF::Destroy();
}


void CRPCClientDemoDlg::OnNMDblclkListRoom(NMHDR *pNMHDR, LRESULT *pResult)
{
	
}


void CRPCClientDemoDlg::OnNMDblclkListRoomlist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	room_server::RoomInfo room_info = CClientBiz::Instance()->GetRoomList()->at(pNMItemActivate->iItem);
	//RoomInfo info = RoomList_.at(pNMItemActivate->iItem);

	//CClientBiz::Instance()->EnterRoom(room_info.room_id, "");

	*pResult = 0;
}


void CRPCClientDemoDlg::OnLvnItemchangedListInfo(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
}


void CRPCClientDemoDlg::OnBnClickedPrivate()
{
	// TODO: 在此添加控件通知处理程序代码
}


void CRPCClientDemoDlg::OnBnClickedLeave()
{
	CClientBiz::Instance()->LeaveRoom();
}


void CRPCClientDemoDlg::OnBnClickedSend()
{
	POSITION pss = m_ListUserList->GetFirstSelectedItemPosition();

	int nIdx = -1;

	CString chat_content;
	GetDlgItemText(IDC_INPUT, chat_content);
	string chat_content_a = CW2A(chat_content.GetBuffer(), CP_ACP);

	bool is_private = true;

	if (NULL != pss)
	{
		while(pss)
		{  
			nIdx = m_ListUserList->GetNextSelectedItem(pss);

			//common::UserInfo user_info = CClientBiz::Instance()->GetUserList()->at(nIdx);

			//CClientBiz::Instance()->SendText(user_info.user_id, chat_content_a.c_str(), is_private);
		}
	}

	SetDlgItemText(IDC_INPUT, _T(""));
}

BOOL CRPCClientDemoDlg::PreTranslateMessage(MSG *pMsg) 
{
	if(pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
	{
		POSITION pss = m_ListUserList->GetFirstSelectedItemPosition();

		int nIdx = -1;

		CString chat_content;
		GetDlgItemText(IDC_INPUT, chat_content);
		string chat_content_a = CW2A(chat_content.GetBuffer(), CP_ACP);

		bool is_private = true;

		if (NULL != pss)
		{
			while(pss)
			{  
				nIdx = m_ListUserList->GetNextSelectedItem(pss);

				//common::UserInfo user_info = CClientBiz::Instance()->GetUserList()->at(nIdx);

				//CClientBiz::Instance()->SendText(user_info.user_id, chat_content_a.c_str(), is_private);
			}
		}

		SetDlgItemText(IDC_INPUT, _T(""));

		return true;
	}
	//CDialog::FlashWindow(FALSE);
	return CDialog::PreTranslateMessage(pMsg);
}
