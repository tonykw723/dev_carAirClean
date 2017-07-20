
// infosetDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "infoset.h"
#include "infosetDlg.h"
#include "afxdialogex.h"
#include "TipDlg.h"
#include "WarnDlg.h"
#include "test.h"
#include "windows.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


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


// CinfosetDlg 对话框

CinfosetDlg::CinfosetDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CinfosetDlg::IDD, pParent)
	, m_solfware_version(_T(""))
	, COM1(_T(""))
	, m_project_name(_T(""))
	, m_rt_sn(_T(""))
	, m_rtu_doman(_T(""))
	//, m_lamp_num(_T(""))
	, m_pwm_frequnce((short)_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_pwm_frequnce = 0;
	m_project_name = _T("");
	m_uid = _T("");
	m_hardware_version = _T("");
	m_solfware_version = _T("");
	m_group_1 = _T("");
	m_group_2 = _T("");
	m_group_3 = _T("");
	m_pwm_value = 0;
	m_lamp_num = 0;
}

void CinfosetDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMNUMBER, m_com_select);
	DDX_Text(pDX, IDC_EDIT6, m_rt_sn);
	((CEdit *)GetDlgItem(IDC_EDIT6))->SetLimitText(4);    //限定输入长度
	DDX_Text(pDX, IDC_EDIT7, m_rtu_doman);
	((CEdit *)GetDlgItem(IDC_EDIT7))->SetLimitText(4); 

	//  DDX_Text(pDX, IDC_EDIT9, m_lamp_num);
	((CEdit *)GetDlgItem(IDC_EDIT9))->SetLimitText(1); 

	//DDX_Text(pDX, IDC_EDIT11, m_group_1);
	//((CEdit *)GetDlgItem(IDC_EDIT11))->SetLimitText(3); 
	DDX_Text(pDX, IDC_EDIT8, m_pwm_frequnce);
	((CEdit *)GetDlgItem(IDC_EDIT8))->SetLimitText(5); 

	//DDX_Text(pDX, IDC_EDIT5, m_pwm_value);
	//((CEdit *)GetDlgItem(IDC_EDIT5))->SetLimitText(1); 
	DDX_Text(pDX, IDC_PROJECT_NAME, m_project_name);
	DDV_MaxChars(pDX, m_project_name, 6);

	DDX_Text(pDX, IDC_UID, m_uid);
	((CEdit *)GetDlgItem(IDC_UID))->SetLimitText(12); 
	DDX_Text(pDX, IDC_EDIT10, m_hardware_version);
	((CEdit *)GetDlgItem(IDC_EDIT10))->SetLimitText(4); 

	DDX_Text(pDX, IDC_SOLFTWARE_VERSION, m_solfware_version);
	DDX_Control(pDX, IDC_COMBO2, m_lamp_type_select);
	DDX_Control(pDX, IDC_COMBO1, m_pwm_type_select);
	DDX_Control(pDX, IDC_COMBO4, m_lamp_swap_selcet);
	DDX_Control(pDX, IDC_COMBO5, m_pwm_delay_opened_select);

	DDX_Text(pDX, IDC_EDIT11, m_group_1);
	DDV_MaxChars(pDX, m_group_1, 2);
	DDX_Text(pDX, IDC_EDIT12, m_group_2);
	DDV_MaxChars(pDX, m_group_2, 2);
	DDX_Text(pDX, IDC_EDIT13, m_group_3);
	DDV_MaxChars(pDX, m_group_3, 2);
	DDX_Control(pDX, IDC_COMBO6, m_lamp_type2);
	//  DDX_Text(pDX, IDC_EDIT14, m_pwm_value);
	//  DDV_MaxChars(pDX, m_pwm_value, 2);
	DDX_Text(pDX, IDC_EDIT14, m_pwm_value);
	DDV_MinMaxInt(pDX, m_pwm_value, 0, 10);
	((CEdit *)GetDlgItem(IDC_EDIT14))->SetLimitText(3); 
	DDX_Text(pDX, IDC_EDIT9, m_lamp_num);
	DDV_MinMaxInt(pDX, m_lamp_num, 1, 10);
}

BEGIN_MESSAGE_MAP(CinfosetDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_QUERY, &CinfosetDlg::OnBnClickedQuery)
	ON_BN_CLICKED(IDC_QUERY2, &CinfosetDlg::OnBnClickedQuery2)
	ON_CBN_SELCHANGE(IDC_COMNUMBER, &CinfosetDlg::OnCbnSelchangeComnumber)
END_MESSAGE_MAP()

// CinfosetDlg 消息处理程序

BOOL CinfosetDlg::OnInitDialog()
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


	EnumCommPort(&m_com_select);
	
	/*灯具类型：00---未知，01---钠灯（普通），02---钠灯（电感），03---钠灯（电子镇流器），
	04---LED灯(小于100w)，05---LED灯(小于100w)，06---金卤灯，07---节能灯，08---无极灯，09---汞灯。*/
	
	
	m_lamp_type_select.InsertString(0,"未知"); 
	m_lamp_type_select.InsertString(1,"钠灯（普通）"); 
	m_lamp_type_select.InsertString(2,"钠灯（电感）"); 
	m_lamp_type_select.InsertString(3,"钠灯（电子镇流器）"); 
	m_lamp_type_select.InsertString(4,"LED灯(小于100w)"); 
	m_lamp_type_select.InsertString(5,"LED灯(小于100w)"); 
	m_lamp_type_select.InsertString(6,"金卤灯"); 
	m_lamp_type_select.InsertString(7,"节能灯"); 
	m_lamp_type_select.InsertString(8,"无极灯"); 
	m_lamp_type_select.InsertString(9,"汞灯"); 
	m_lamp_type_select.InsertString(10,"SOLAR_LAMP"); 

	m_lamp_type2.InsertString(0,"未知"); 
	m_lamp_type2.InsertString(1,"钠灯（普通）"); 
	m_lamp_type2.InsertString(2,"钠灯（电感）"); 
	m_lamp_type2.InsertString(3,"钠灯（电子镇流器）"); 
	m_lamp_type2.InsertString(4,"LED灯(小于100w)"); 
	m_lamp_type2.InsertString(5,"LED灯(小于100w)"); 
	m_lamp_type2.InsertString(6,"金卤灯"); 
	m_lamp_type2.InsertString(7,"节能灯"); 
	m_lamp_type2.InsertString(8,"无极灯"); 
	m_lamp_type2.InsertString(9,"汞灯"); 
	m_lamp_type_select.InsertString(10,"SOLAR_LAMP"); 

	m_pwm_type_select.InsertString(0,"正向");
	m_pwm_type_select.InsertString(1,"反向");          //PWM类型

	m_lamp_swap_selcet.InsertString(0,"无");
	m_lamp_swap_selcet.InsertString(1,"有");          //灯头号对调标志：0---没有对调，1---表示对调

	m_pwm_delay_opened_select.InsertString(0,"关");
	m_pwm_delay_opened_select.InsertString(1,"开");   

	/*CStatic* pWnd=(CStatic*)GetDlgItem(IDC_STATIC);//得到Picture Control控件句柄
	CImage image;
	image.Load(_T("skin/512.png"));
	HBITMAP hbmp=image.Detach();
	pWnd->SetBitmap(hbmp);
	pWnd->SetWindowPos(NULL,0,0,100,50,SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOZORDER);*/

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CinfosetDlg::EnumCommPort(CComboBox *pListBox)   
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
void CinfosetDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CinfosetDlg::OnPaint()
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
HCURSOR CinfosetDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CinfosetDlg::OnBnClickedQuery()
{ 
	// TODO: 在此添加控件通知处理程序代码
	int nIndex,RtuInfoGetResult=0,nGet=0;
	unsigned char uidBuf[16];
	unsigned char pronameBuf[10];
	unsigned char rtu_snBuf[10];
	unsigned char hardware_vertionBuf[10];
	//INT_PTR nRes;
	//WarnDlg tipDlg;

	memset(uidBuf,NULL,sizeof(uidBuf));
	memset(pronameBuf,NULL,sizeof(pronameBuf));
	memset(rtu_snBuf,NULL,sizeof(rtu_snBuf));
	memset(hardware_vertionBuf,NULL,sizeof(hardware_vertionBuf));
	memset(&set_pack.project_name[0],NULL,sizeof(set_pack)-1);
	RtuInfoGetResult=RtuInfoGet((_TCHAR*)portset.COM);
	m_lamp_num=set_pack.lamp_num;
	m_pwm_value=set_pack.pwm_decay_value;
	m_pwm_frequnce=set_pack.pwm_frequence;
	
	CutZero(set_pack.project_name,pronameBuf,sizeof(set_pack.project_name));
	m_project_name=pronameBuf;
	CutZero(set_pack.rtu_sn,rtu_snBuf,sizeof(set_pack.rtu_sn));
	m_rt_sn=rtu_snBuf;
	CutZero(set_pack.hardware_vertion,hardware_vertionBuf,sizeof(set_pack.hardware_vertion));
	m_hardware_version=hardware_vertionBuf;

	m_solfware_version.Format("%c%c%c%c",set_pack.software_version[0],set_pack.software_version[1],
	set_pack.software_version[2],set_pack.software_version[3]);
	HEX2Asc(uidBuf,set_pack.rtu_addr,sizeof(set_pack.rtu_addr));
	m_uid=uidBuf;
	m_group_1.Format("%X",set_pack.rtu_group[0]);
	m_group_2.Format("%X",set_pack.rtu_group[1]);
	m_group_3.Format("%X",set_pack.rtu_group[2]);
	m_rtu_doman.Format("%X%X",set_pack.rtu_domain[0],set_pack.rtu_domain[1]);

	nIndex=(int)(set_pack.lamp_type[0]);
	m_lamp_type_select.SetCurSel(nIndex);
	nIndex=(int)(set_pack.lamp_type[1]);
	m_lamp_type2.SetCurSel(nIndex);
	nIndex=(int)((set_pack.pwm_type))-1;
	m_pwm_type_select.SetCurSel(nIndex);
	nIndex=(int)((set_pack.lamp_swap));
	m_lamp_swap_selcet.SetCurSel(nIndex);
	nIndex=(int)((set_pack.pwm_decay_opened));
	m_pwm_delay_opened_select.SetCurSel(nIndex);

	UpdateData(FALSE);   
}

void CinfosetDlg::OnCbnSelchangeComnumber()  
{

	// TODO: 在此添加控件通知处理程序代码
	CString strCom;
	int nSel;
	

	 nSel=m_com_select.GetCurSel();				// 获取组合框控件的列表框中选中项的索引
	 m_com_select.GetLBText(nSel,strCom);		// 根据选中项索引获取该项字符串
	 TCHAR* szMsg; //= new TCHAR[100];
	 //其参数为CString字符串的长度
	 szMsg = strCom.GetBuffer(strCom.GetLength());
	 //portset.COM=*szMsg;
	 strcpy_s(portset.COM,szMsg);
	 strCom.ReleaseBuffer();
	 //delete []szMsg;
	 szMsg = NULL;
	 //SetDlgItemText(IDC_EDIT5,strCom);
}

void CinfosetDlg::OnBnClickedQuery2()                     //点击“设置”按钮
{
	// TODO: 在此添加控件通知处理程序代码

	INT_PTR nRes;
	int nSel;
	CTipDlg tipDlg;
	nRes=tipDlg.DoModal();
	if(IDCANCEL==nRes)
		return;
	UpdateData(TRUE); 

	//char *p = (LPSTR)(LPCTSTR)m_solfware_version;
	//memcpy(set_pack.software_version,p,m_solfware_version.GetLength());
	
	memset(&set_pack.project_name,NULL,sizeof(set_pack)-1);
	memset(&set_pack.rtu_addr,0,sizeof(set_pack.rtu_addr));

	memcpy(set_pack.project_name+(6-m_project_name.GetLength()),(LPSTR)(LPCTSTR)m_project_name,m_project_name.GetLength());
	memcpy(set_pack.hardware_vertion+(4-m_hardware_version.GetLength()),(LPSTR)(LPCTSTR)m_hardware_version,m_hardware_version.GetLength());
	memcpy(set_pack.rtu_sn+(4-m_rt_sn.GetLength()),(LPSTR)(LPCTSTR)m_rt_sn,m_rt_sn.GetLength());

	TwoASCtoHEX(&set_pack.rtu_group[0],(unsigned char*)(LPSTR)(LPCTSTR)m_group_1,m_group_1.GetLength());
	TwoASCtoHEX(&set_pack.rtu_group[1],(unsigned char*)(LPSTR)(LPCTSTR)m_group_2,m_group_2.GetLength());
	TwoASCtoHEX(&set_pack.rtu_group[2],(unsigned char*)(LPSTR)(LPCTSTR)m_group_3,m_group_3.GetLength());
	TwoASCtoHEX(set_pack.rtu_addr+(6-m_uid.GetLength()/2),(unsigned char*)(LPSTR)(LPCTSTR)m_uid,m_uid.GetLength());
	TwoASCtoHEX(set_pack.rtu_domain,(unsigned char*)(LPSTR)(LPCTSTR)m_rtu_doman,m_rtu_doman.GetLength());
	//memcpy(&set_pack.rtu_domain,(LPSTR)(LPCTSTR)m_rtu_doman,m_rtu_doman.GetLength());

	//memcpy(&set_pack.lamp_num,(LPSTR)(LPCTSTR)m_lamp_num,m_lamp_num.GetLength());
	set_pack.lamp_num=m_lamp_num;
	set_pack.pwm_decay_value=m_pwm_value;
	set_pack.pwm_frequence=m_pwm_frequnce;

	nSel=m_pwm_type_select.GetCurSel();
	set_pack.pwm_type=(unsigned char)(nSel)+1;
	nSel=m_lamp_type_select.GetCurSel();
	set_pack.lamp_type[0]=nSel;
	nSel=m_lamp_type2.GetCurSel();
	set_pack.lamp_type[1]=nSel;

	nSel=m_lamp_swap_selcet.GetCurSel();
    set_pack.lamp_swap=(unsigned char)(nSel);
	nSel=m_pwm_delay_opened_select.GetCurSel();
	set_pack.pwm_decay_opened=(unsigned char)(nSel);

	RtuInfoSet();
}




//void CinfosetDlg::OnCbnSelchangeCombo5()
//{
//	// TODO: 在此添加控件通知处理程序代码
//}
