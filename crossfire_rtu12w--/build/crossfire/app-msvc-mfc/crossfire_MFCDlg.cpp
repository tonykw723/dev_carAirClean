
// crossfire_MFCDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "crossfire_MFC.h"
#include "crossfire_MFCDlg.h"
#include "afxdialogex.h"

#include <list>
#include <iostream>
#include <queue>
#include <deque>

using namespace std ;

/* Includes ------------------------------------------------------------------*/
#include "unity.h"
#include <setjmp.h>
#include <stdio.h>
#include <setjmp.h>

#include "xtsys_type.h"
#include "xtsys_cfg.h"

#include "xtos_print.h"
#include "xtos_task.h"
#include "xtos_mem.h"
#include "xtos_msg.h"
#include "xtos_timer.h"

#include "ringqueue.h"

#include "xt_bsp_rtu09c.h"

#include "drv_uart.h"

#include "xt_conn.h"
#include "conn_uart.h"
#include "conn_msg.h"

#include "xt_dev.h"
#include "dev_rise.h"

#include "pro_dev_iot.h"

#include "global_para.h"
#include "pro_ap.h"
#include "xt_pro_ap.h"
#include "pro_ap_iot.h"

#include "ap.h"
#include "ap_comm.h"
#include "ap_rtu.h"
#include "ap_measure.h"
#include "misc_timer.h"

#include "config.h"

#define MFC_CTRL_MSG_TYPE_CMD		(0)
#define MFC_CTRL_MSG_TYPE_TEST		(1)
#define MFC_CTRL_MSG_TYPE_TEST_RESP	(2)

typedef PACKED struct 
{
	xt_u8			msgType;
	pro_ap_info_t	msgData;
}mfc_ctrl_info_t;

typedef queue<mfc_ctrl_info_t> CONNMSGQUEUE;

CONNMSGQUEUE connMsgQueue;
CONNMSGQUEUE connMsgRespQueue;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define WM_READDATA		WM_USER+60
#define WM_WRITEDATA	WM_USER+61

#define	WIN32_MFC_MSG	WM_USER+101

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


// Ccrossfire_MFCDlg 对话框

Ccrossfire_MFCDlg::Ccrossfire_MFCDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(Ccrossfire_MFCDlg::IDD, pParent)
	, m_light_connected(1)
	, m_dimming1(100)
	, m_index1(1)
	, m_index2(1)
	, m_check_control(1)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	if (m_check_control == 1)
	{
		Win32CtrlRespInit();
	}
}

void Ccrossfire_MFCDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK1, m_light_connected);
	DDX_Text(pDX, IDC_EDIT1, m_dimming1);
	DDV_MinMaxInt(pDX, m_dimming1, 0, 100);
	DDX_Check(pDX, IDC_CHECK2, m_index1);
	DDV_MinMaxInt(pDX, m_index1, 0, 1);
	DDX_Check(pDX, IDC_CHECK3, m_index2);
	DDV_MinMaxInt(pDX, m_index2, 0, 1);
	DDX_Check(pDX, IDC_CHECK4, m_check_control);
	DDV_MinMaxInt(pDX, m_check_control, 0, 1);
}

BEGIN_MESSAGE_MAP(Ccrossfire_MFCDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &Ccrossfire_MFCDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &Ccrossfire_MFCDlg::OnBnClickedButton2)
	ON_EN_CHANGE(IDC_EDIT1, &Ccrossfire_MFCDlg::OnEnChangeEdit1)
	ON_BN_CLICKED(IDC_BUTTON3, &Ccrossfire_MFCDlg::OnBnClickedButton3)
	ON_BN_CLICKED(IDC_CHECK4, &Ccrossfire_MFCDlg::OnBnClickedCheck4)
END_MESSAGE_MAP()

bool ShowJpgGif(CDC* pDC,CString strPath, int x, int y)
{
	IStream *pStm; 
	CFileStatus fstatus; 
	CFile file; 
	LONG cb; 
	HGLOBAL hGlobal;
	bool ret = false;

	if (file.Open(strPath,CFile::modeRead)&&file.GetStatus(strPath,fstatus)&&((cb = fstatus.m_size) != -1)) 
	{ 
		hGlobal = GlobalAlloc(GMEM_MOVEABLE, cb); 
		LPVOID pvData = NULL; 
		if (hGlobal != NULL) 
		{ 
			pvData = GlobalLock(hGlobal);
			if (pvData != NULL) 
			{ 
				file.Read(pvData, cb); 

				CreateStreamOnHGlobal(hGlobal, TRUE, &pStm); 

				//显示JPEG和GIF格式的图片，GIF只能显示一帧，还不能显示动画，
				//要显示动画GIF请使用ACTIVE控//件。
				IPicture *pPic;
				//load image from file stream
				if(SUCCEEDED(OleLoadPicture(pStm,fstatus.m_size,TRUE,IID_IPicture,(LPVOID*)&pPic)))
				{
					OLE_XSIZE_HIMETRIC hmWidth; 
					OLE_YSIZE_HIMETRIC hmHeight; 
					pPic->get_Width(&hmWidth); 
					pPic->get_Height(&hmHeight); 
					double fX,fY; 
					//get image height and width
					//fX = (double)pDC->GetDeviceCaps(HORZRES)*(double)hmWidth/((double)pDC->GetDeviceCaps(HORZSIZE)*100.0); 
					//fY = (double)pDC->GetDeviceCaps(VERTRES)*(double)hmHeight/((double)pDC->GetDeviceCaps(VERTSIZE)*100.0); 
					//use render function display image
					pDC->SetBkMode(TRANSPARENT);
					if(SUCCEEDED(pPic->Render(*pDC,0,0,(DWORD)x,(DWORD)y,0,hmHeight,hmWidth,-hmHeight,NULL))) 
					{
						ret = true;
					}
				} 
	
				pPic->Release();
				pStm->Release(); 

			}

			GlobalUnlock(hGlobal); 

		}

		GlobalFree(hGlobal);
			
	}

	file.Close();

	return ret;;
}

LRESULT Ccrossfire_MFCDlg::WindowProc(UINT  message,  WPARAM  wParam,  LPARAM  lParam)    
{  
	int paramData = 0;
	int paramDataLen = 0;
	CDC* pDC = NULL;

	switch(message)  
	{  
	case  WM_READDATA:  
		{ 
			paramData = *(int *)wParam;
			paramDataLen = (int)lParam;

			//read value from ui
			UpdateData(TRUE); 

			win32ParamSt.ledConnSt = m_light_connected;

			if( win32ParamSt.ledSt != 0 && win32ParamSt.ledConnSt != 0)
			{
				pDC = GetDlgItem(IDC_STATIC4)->GetDC();  
				ShowJpgGif(pDC,".\\res\\lighton.jpg",45,60);
			}
			else
			{
				pDC = GetDlgItem(IDC_STATIC4)->GetDC();  
				ShowJpgGif(pDC,".\\res\\lightoff.jpg",45,60);
			}
			ReleaseDC(pDC);

			
		} 
		break;
	case  WM_WRITEDATA:  
		{  
			UpdateData(FALSE); 
		} 
		break;
	}  
	return  CDialog::WindowProc(message,  wParam,  lParam);  
}

extern int main(int argc, char* argv[]);

DWORD WINAPI mfcUpdateThread(LPVOID arg)
{
	Ccrossfire_MFCDlg* pDlg = (Ccrossfire_MFCDlg *)arg;

	while(TRUE)
	{
		//pDlg->paramData = win32ParamSt.ledSt;
		pDlg->SendMessage(WM_READDATA,(WPARAM)&pDlg->paramData,sizeof(pDlg->paramData)); 
		Sleep(50);
	}
}

HANDLE hStartEvent = NULL;
HANDLE hTestEvent = NULL;

xt_s32 mfcCtrlReqAndResp(xt_u8* pSendBuf, xt_s32 sendLen, xt_u8* pRecvBuf, xt_s32* pRecvLen)
{
	xt_u8 recvbuf[256];
	xt_s32 recvlen = 0;
	xt_s32 ret = FAILURE_XT;
	xt_s32 cnt = 0;
	xt_s32 sendcnt = 0;

	sendcnt = XtBspCtrlRespWrite(pSendBuf, sendLen);

	cnt = 0;
	ret = FAILURE_XT;
	while(TRUE && sendcnt > 0)
	{
		XtBspCtrlRespRead(recvbuf,&recvlen);
		if (recvlen > 0)
		{
			TRACE("GetMessage\n");
			if(pRecvBuf != NULL && pRecvLen!= NULL)
			{
				MemMemcpyXTOS(pRecvBuf, recvbuf, recvlen);
				*pRecvLen = recvlen;
			}
			ret = SUCCESS_XT;
		}

		if (ret == SUCCESS_XT || cnt >= 1000)
		{
			break;
		}
		Sleep(10);
		cnt ++;
	}

	return ret;
}

unsigned __stdcall  mfcRespUpdateThread(void *param)
{
	xt_u8 sendbuf[256];
	xt_s32 sendlen = 0;
	xt_u8 recvbuf[256];
	xt_s32 recvlen = 0;
	xt_s32 ret = FAILURE_XT;
	xt_s32 cnt = 0;
	mfc_ctrl_info_t sendmsg = {0};
	mfc_ctrl_info_t respmsg = {0};

	MSG msg;
	char * pInfo = NULL;
	PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);

	if(!SetEvent(hStartEvent))
	{
		TRACE("set start event failed,errno:%d\n",::GetLastError());
		return 1;
	}

	while(TRUE)
	{
		if (!connMsgQueue.empty())
		{
			sendmsg = connMsgQueue.front();
			connMsgQueue.pop();

			ret = mfcCtrlReqAndResp(sendmsg.msgData.proApData, sendmsg.msgData.proApDataLen, recvbuf, &recvlen);
			if (sendmsg.msgType == MFC_CTRL_MSG_TYPE_TEST)
			{
				if (ret = SUCCESS_XT)
				{
					MemMemcpyXTOS(respmsg.msgData.proApData,recvbuf,recvlen);
					respmsg.msgData.proApDataLen = recvlen;
				}
				else
				{
					MemMemsetXTOS(&respmsg,0,sizeof(respmsg));
				}
				respmsg.msgType = MFC_CTRL_MSG_TYPE_TEST_RESP;
				
				connMsgRespQueue.push(respmsg);
			}
		}
		else
			Sleep(10);
	}

}

xt_s32 test_mfc_dev_pro_correct()
{
	xt_s32 ret = FAILURE_XT;
	pro_pkt_parse_t	testRecvData = {0};
	xt_u8 testCmd[] = {0x68,0x0,0x0,0x0,0x0,0x43,0x21,0x68,0x1,0x3,0x42,0x14,0x3,0x91,0x16};

	MemMemcpyXTOS(testRecvData.tmpBuf, testCmd, sizeof(testCmd));
	testRecvData.tmpLen = sizeof(testCmd);

	while (ProDevIotParse(&testRecvData) == SUCCESS_XT) {

		if (memcmp(testCmd, testRecvData.chkBuf, testRecvData.pktLen) == 0 
			&& testRecvData.pktLen == sizeof(testCmd))
		{
			ret = SUCCESS_XT;
		}
		testRecvData.pktLen = 0;
	}

	return ret;
}

xt_s32 test_mfc_dev_pro_error()
{
	xt_s32 ret = FAILURE_XT;
	pro_pkt_parse_t	testRecvData = {0};
	xt_u8 testCmd[] = {0x68,0x0,0x0,0x0,0x0,0x43,0x21,0x68,0x1,0x3,0x42,0x14,0x3,0x90,0x16};

	MemMemcpyXTOS(testRecvData.tmpBuf, testCmd, sizeof(testCmd));
	testRecvData.tmpLen = sizeof(testCmd);

	while (ProDevIotParse(&testRecvData) == SUCCESS_XT) {

		if (memcmp(testCmd, testRecvData.chkBuf, testRecvData.pktLen) == 0 
			&& testRecvData.pktLen == sizeof(testCmd))
		{
			ret = SUCCESS_XT;
		}
		testRecvData.pktLen = 0;
	}

	return ret;
}

xt_s32 test_mfc_light_open()
{
	xt_s32 ret = FAILURE_XT;

	xt_u8 uid[DEV_ADDR_BYTES] = {0,0,0,0,0x43,0x21};
	xt_s64 addr = 0;
	xt_u8 cmd = CMD_UNICAST;
	ctrl_data_in_t di = {0};
	pro_ap_iot_ctrl_t st = {0};
	xt_s32 len = 0;
	mfc_ctrl_info_t msg = {0};
	mfc_ctrl_info_t respmsg = {0};
	xt_u8 expectCmd[] = {0x68,0x0,0x0,0x0,0x0,0x43,0x21,0x68,0x1,0x4,0x42,0x3,0x14,0xf5,0x87,0x16};

	MemMemcpyXTOS(&addr, uid, DEV_ADDR_BYTES);
	addr = addr << 16; // uid store low 48 bit

	st.cmd = ccOpen;
	st.dimming = 20;
	st.index = 3;

	di.pDataBuf = &st;
	di.dataLen = sizeof(st);

	len = PRO_AP_PACKAGE_MAX;
	ProDevIotPack(addr, cmd, &di, msg.msgData.proApData, &len);
	msg.msgData.proApDataLen = len;
	msg.msgType = MFC_CTRL_MSG_TYPE_TEST;

	TRACE("TEST OPEN LIGHT\n");
	connMsgQueue.push(msg);

	while(TRUE)
	{
		if (!connMsgRespQueue.empty())
		{
			respmsg = connMsgRespQueue.front();
			connMsgRespQueue.pop();

			if (respmsg.msgData.proApDataLen > 0 && respmsg.msgType == MFC_CTRL_MSG_TYPE_TEST_RESP)
			{
				if (memcmp(expectCmd,respmsg.msgData.proApData,respmsg.msgData.proApDataLen) == 0)
				{
					ret = SUCCESS_XT;
				}
				
			}

			break;
		}
		else
			Sleep(10);
	}

	return ret;
}

xt_void test_mfc_dev_pro(xt_void)
{
	PrintfXTOS("in test_mfc_dev_pro #1 \n");

	TEST_ASSERT_EQUAL(SUCCESS_XT, test_mfc_dev_pro_correct());
	TEST_ASSERT_EQUAL(FAILURE_XT, test_mfc_dev_pro_error());
}

xt_void test_mfc_light(xt_void)
{
	PrintfXTOS("in test_mfc_light #1 \n");

	TEST_ASSERT_EQUAL(SUCCESS_XT, test_mfc_light_open());
}

DWORD WINAPI mfcTestThread(LPVOID arg)
{
	Ccrossfire_MFCDlg* pDlg = (Ccrossfire_MFCDlg *)arg;

	while(TRUE)
	{
		WaitForSingleObject(hTestEvent,INFINITE);

		Unity.TestFile = "mfc_cpp.c";
		
		UnityBegin();

		//RUN_TEST(test_mfc_dev_pro,26,RUN_TEST_NO_ARGS);

		RUN_TEST(test_mfc_light,26,RUN_TEST_NO_ARGS);

		UnityEnd();

		((CButton *)pDlg->GetDlgItem(IDC_BUTTON3))->EnableWindow(TRUE);
	}
}

// Ccrossfire_MFCDlg 消息处理程序

void CrossfireInitThread()
{
	char param = 'f';
	char* pParam = &param;

	main(0,&pParam);

	while(TRUE)
	{
		Sleep(1000);
	}
}

BOOL Ccrossfire_MFCDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

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

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	initThreadHandle=CreateThread(NULL,
		0,
		(LPTHREAD_START_ROUTINE)CrossfireInitThread,
		NULL,
		0,
		&ThreadID);

	mfcThreadHandle = CreateThread(NULL,0,mfcUpdateThread,this,0,NULL);

	hTestEvent = CreateEvent(0, FALSE, FALSE, 0);
	if(hTestEvent == 0)
	{
		TRACE("create test event failed,errno:%d\n",::GetLastError());
		return 1;
	}

	mfcTestThreadHandle = CreateThread(NULL,0,mfcTestThread,this,0,NULL);

	//SetEvent(hReqEvent);

	hStartEvent = CreateEvent(0, FALSE, FALSE, 0);
	if(hStartEvent == 0)
	{
		TRACE("create start event failed,errno:%d\n",::GetLastError());
		return 1;
	}

	mfcRespThreadHandle = (HANDLE)_beginthreadex(NULL, 0, &mfcRespUpdateThread, NULL, 0, &win32msgRespThreadID);
	if(mfcRespThreadHandle == 0)
	{
		TRACE("start thread failed,errno:%d\n",::GetLastError());
		CloseHandle(hStartEvent);
		return 1;
	}

	WaitForSingleObject(hStartEvent,INFINITE);
	CloseHandle(hStartEvent);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void Ccrossfire_MFCDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void Ccrossfire_MFCDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
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

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR Ccrossfire_MFCDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


BOOL Ccrossfire_MFCDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此添加专用代码和/或调用基类
	if((pMsg->message==WM_KEYDOWN) && (pMsg->wParam==VK_RETURN))
		return true;
	return CDialogEx::PreTranslateMessage(pMsg);
}


void Ccrossfire_MFCDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	xt_s32 ret = FAILURE_XT;

	xt_u8 uid[DEV_ADDR_BYTES] = {0,0,0,0,0x43,0x21};
	xt_s64 addr = 0;
	xt_u8 cmd = CMD_UNICAST;
	ctrl_data_in_t di = {0};
	pro_ap_iot_ctrl_t st = {0};
	xt_s32 len = 0;
	mfc_ctrl_info_t msg = {0};

	m_dimming1 = 100;
	UpdateData(FALSE);

	MemMemcpyXTOS(&addr, uid, DEV_ADDR_BYTES);
	addr = addr << 16; // uid store low 48 bit

	st.cmd = ccOpen;
	st.dimming = m_dimming1/5;
	st.index = m_index2<<1 | m_index1;

	di.pDataBuf = &st;
	di.dataLen = sizeof(st);

	len = PRO_AP_PACKAGE_MAX;
	ProDevIotPack(addr, cmd, &di, msg.msgData.proApData, &len);
	msg.msgData.proApDataLen = len;
	msg.msgType = MFC_CTRL_MSG_TYPE_CMD;

	TRACE("OPEN LIGHT\n");
	connMsgQueue.push(msg);
}



void Ccrossfire_MFCDlg::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	xt_s32 ret = FAILURE_XT;
	xt_u8 uid[DEV_ADDR_BYTES] = {0,0,0,0,0x43,0x21};
	xt_s64 addr = 0;
	xt_u8 cmd = CMD_UNICAST;
	ctrl_data_in_t di = {0};
	pro_ap_iot_ctrl_t st = {0};
	xt_s32 len = 0;
	mfc_ctrl_info_t msg = {0};

	m_dimming1 = 0;
	UpdateData(FALSE);

	MemMemcpyXTOS(&addr, uid, DEV_ADDR_BYTES);
	addr = addr << 16; // uid store low 48 bit

	st.cmd = ccClose;
	st.dimming = m_dimming1/5;
	st.index = m_index2<<1 | m_index1;

	di.pDataBuf = &st;
	di.dataLen = sizeof(st);

	len = PRO_AP_PACKAGE_MAX;
	ProDevIotPack(addr, cmd, &di, msg.msgData.proApData, &len);
	msg.msgData.proApDataLen = len;
	msg.msgType = MFC_CTRL_MSG_TYPE_CMD;

	TRACE("CLOSE LIGHT\n");
	connMsgQueue.push(msg);
}


void Ccrossfire_MFCDlg::OnEnChangeEdit1()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}

void Ccrossfire_MFCDlg::OnBnClickedButton3()
{
	// TODO: 在此添加控件通知处理程序代码
	SetEvent(hTestEvent);
	((CButton *)GetDlgItem(IDC_BUTTON3))->EnableWindow(FALSE);
}


void Ccrossfire_MFCDlg::OnBnClickedCheck4()
{
	// TODO: 在此添加控件通知处理程序代码
	if(BST_CHECKED==((CButton*)GetDlgItem(IDC_CHECK4))->GetCheck())
		Win32CtrlRespInit();
	else
		Win32CtrlRespDeinit();
}
