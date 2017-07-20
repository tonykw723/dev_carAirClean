
// rtuUpdateToolDlg.cpp : implementation file
//

#include "stdafx.h"
#include "rtuUpdateTool.h"
#include "rtuUpdateToolDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CrtuUpdateToolDlg dialog




CrtuUpdateToolDlg::CrtuUpdateToolDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CrtuUpdateToolDlg::IDD, pParent)
	, m_curpkg(_T("0"))
	, m_totalpkg(_T("0"))
	, m_info(_T(""))
	, m_result(_T(""))
	, m_verdir(_T(""))
	, m_rtu_ver(_T(""))
	, m_rtu_start_ver(_T(""))
	, m_iniver(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CrtuUpdateToolDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATIC_CURPKG, m_curpkg);
	DDX_Text(pDX, IDC_STATIC_TOTALPKG, m_totalpkg);
	DDX_Text(pDX, IDC_STATIC_INFO, m_info);
	DDX_Text(pDX, IDC_STATIC_RESULT, m_result);
	//  DDX_Control(pDX, IDC_LIST_COM, m_list_com);
	DDX_Control(pDX, IDC_COMBO1, m_combolist);
	DDX_Text(pDX, IDC_STATIC_DIR, m_verdir);
	DDX_Text(pDX, IDC_STATIC_VER, m_rtu_ver);
	DDX_Text(pDX, IDC_STATIC_START_VER, m_rtu_start_ver);
	DDX_Text(pDX, IDC_STATIC_INIVER, m_iniver);
	DDX_Control(pDX, IDC_COMBO2, m_comBr);
	DDX_Control(pDX, IDC_COMBO4, m_comParity);
	DDX_Control(pDX, IDC_COMBO3, m_comDatabit);
	DDX_Control(pDX, IDC_COMBO5, m_comStopbit);
}

BEGIN_MESSAGE_MAP(CrtuUpdateToolDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CrtuUpdateToolDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CrtuUpdateToolDlg::OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON4, &CrtuUpdateToolDlg::OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON5, &CrtuUpdateToolDlg::OnBnClickedButton5)
END_MESSAGE_MAP()

static char g_szLastSelDir[MAX_PATH] = {0};

// CrtuUpdateToolDlg message handlers
int   CALLBACK   BrowseCallbackProc(HWND   hwnd,   UINT   msg,   LPARAM   lp,   LPARAM   pData)   
{   
	if   (msg   ==   BFFM_INITIALIZED   )   
	{
		if(strlen(g_szLastSelDir) == 0)
		{
			char pBuf[MAX_PATH];
			GetCurrentDirectory(MAX_PATH,pBuf); 
			_tcscpy(g_szLastSelDir, pBuf);
		}
		::SendMessage(hwnd,BFFM_SETSELECTION,TRUE,(LPARAM)g_szLastSelDir);   
	}   
	return   0;   
} 

int CrtuUpdateToolDlg::BrowsePath(HWND hwndParent, char *pPath, char *pDisplayString)
{
	BROWSEINFO bi;//保存用户选择的目录信息
	LPITEMIDLIST  p;
	char path[MAX_PATH];

	SecureZeroMemory(&bi, sizeof(BROWSEINFO));  //清0
	//bi.pidlRoot = ParsePidlFromPath("C://"); //设置选择的根目录，如“C://”则只能选C盘的目录
	bi.hwndOwner = GetSafeHwnd();  //父窗口句柄
	bi.pszDisplayName = path;  //保存路径的buffer
	bi.lpszTitle = pDisplayString;  //标题
	bi.lpfn = BrowseCallbackProc;
	bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_EDITBOX | BIF_VALIDATE;  //对话框显示模式

	p=SHBrowseForFolder(&bi); //弹出选择路径对话框
	if(p)
	{
		if(SHGetPathFromIDList(p,path))
		{
			strcpy(pPath,path);
			_tcscpy(g_szLastSelDir, path);
			return 1;
		}
		else
			return 0;
	}
	else
		return 0;
}

void CrtuUpdateToolDlg::EnumCommPort(CComboBox *pListBox)   
{   
	HKEY hKey;   
	if(ERROR_SUCCESS ==::RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Hardware\\DeviceMap\\SerialComm",NULL, KEY_READ, &hKey ))//打开串口注册表对应的键值  
	{   
		int   i=0;   
		TCHAR  RegKeyName[128],SerialPortName[128];
		DWORD  dwLong,dwSize; 
		while(TRUE)   
		{   
			dwLong = dwSize=sizeof(RegKeyName);   
			if(ERROR_NO_MORE_ITEMS==::RegEnumValue(hKey,i,RegKeyName,&dwLong,NULL,NULL,(PUCHAR)SerialPortName, &dwSize))//枚举串口   
			{
				break;   
			}
			if(!memcmp(RegKeyName,"\\Device\\",8))//过滤虚拟串口
			{
				pListBox->AddString(SerialPortName);   //SerialPortName就是串口名字
			}
			i++;   
		}
		if(pListBox->GetCount()==0)    
		{   
			//MessageBox("在HKEY_LOCAL_MACHINE:Hardware\\DeviceMap\\SerialComm里找不到串口!!!");   
		}
		RegCloseKey(hKey);   
	}
	else
	{
		RegCloseKey(hKey);
		//MessageBox("您的计算机的注册表上没有HKEY_LOCAL_MACHINE:Hardware\\DeviceMap\\SerialComm项");
	}
}

BOOL CrtuUpdateToolDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	GetDlgItem(IDC_BUTTON2)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON5)->EnableWindow(FALSE);

	EnumCommPort(&m_combolist);

	m_comBr.AddString("300");
	m_comBr.AddString("600");
	m_comBr.AddString("1200");
	m_comBr.AddString("2400");
	m_comBr.AddString("4800");
	m_comBr.AddString("9600");
	m_comBr.AddString("19200");
	m_comBr.AddString("38400");
	m_comBr.AddString("43000");
	m_comBr.AddString("56000");
	m_comBr.AddString("57600");
	m_comBr.AddString("115200");
	m_comBr.SetCurSel(5);

	m_comParity.AddString("N");
	m_comParity.AddString("E");
	m_comParity.AddString("O");
	m_comParity.SetCurSel(0);

	m_comDatabit.AddString("7");
	m_comDatabit.AddString("8");
	m_comDatabit.SetCurSel(1);

	m_comStopbit.AddString("1");
	m_comStopbit.AddString("1.5");
	m_comStopbit.AddString("2");
	m_comStopbit.SetCurSel(0);

	pRtuUpdate = new rtpUpdateThread(this);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CrtuUpdateToolDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CrtuUpdateToolDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

LRESULT CrtuUpdateToolDlg::WindowProc(UINT  message,  WPARAM  wParam,  LPARAM  lParam)    
{  
	switch(message)  
	{  
	case  WM_READDATA:  
		{  
			UpdateData(TRUE); 
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

void CrtuUpdateToolDlg::OnBnClickedButton1()
{
	// TODO: 在此添加控件通知处理程序代码
	m_combolist.GetLBText(m_combolist.GetCurSel(),pRtuUpdate->comName);
	//pRtuUpdate->comName = "COM2";
	pRtuUpdate->startFlag = 1;
}


void CrtuUpdateToolDlg::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	pRtuUpdate->startFlag = 2;
}


void CrtuUpdateToolDlg::OnBnClickedButton4()
{
	// TODO: 在此添加控件通知处理程序代码
	char dirPath[1024] = {0};

	BrowsePath((HWND)this, dirPath, "Choose version Directory");
	pRtuUpdate->dirName = CString(dirPath);
	m_verdir = pRtuUpdate->dirName;
	this->SendMessage(WM_WRITEDATA,0,0);
}


void CrtuUpdateToolDlg::OnBnClickedButton3()
{
	// TODO: 在此添加控件通知处理程序代码
	
}


void CrtuUpdateToolDlg::OnBnClickedButton5()
{
	// TODO: 在此添加控件通知处理程序代码
	m_rtu_ver = "...";
	this->SendMessage(WM_WRITEDATA,0,0);
	pRtuUpdate->SendCheckCmd(this);
}
