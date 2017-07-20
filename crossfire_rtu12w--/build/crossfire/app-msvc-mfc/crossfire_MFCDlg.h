
// crossfire_MFCDlg.h : 头文件
//

#pragma once
#include "afxwin.h"


// Ccrossfire_MFCDlg 对话框
class Ccrossfire_MFCDlg : public CDialogEx
{
// 构造
public:
	Ccrossfire_MFCDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_CROSSFIRE_MFC_DIALOG };

	HANDLE initThreadHandle;

	DWORD ThreadID;

	int paramData;

	HANDLE mfcThreadHandle;

	unsigned int win32msgRespThreadID;

	HANDLE mfcRespThreadHandle;

	HANDLE mfcTestThreadHandle;

	LRESULT Ccrossfire_MFCDlg::WindowProc(UINT  message,  WPARAM  wParam,  LPARAM  lParam);

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
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	int m_light_connected;
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();
	int m_dimming1;
	afx_msg void OnEnChangeEdit1();
	int m_index1;
	int m_index2;
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedCheck4();
	int m_check_control;
};
