
// rtuUpdateToolDlg.h : header file
//

#pragma once

#include "rtpUpdateThread.h"
#include "resource.h"
#include "afxwin.h"

const int WM_READDATA=WM_USER+60; 
const int WM_WRITEDATA=WM_USER+61; 

// CrtuUpdateToolDlg dialog
class CrtuUpdateToolDlg : public CDialogEx
{
// Construction
public:
	int CrtuUpdateToolDlg::BrowsePath(HWND hwndParent, char *pPath, char *pDisplayString);
	void CrtuUpdateToolDlg::EnumCommPort(CComboBox *pListBox);
	CrtuUpdateToolDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_RTUUPDATETOOL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	rtpUpdateThread* pRtuUpdate;
	afx_msg void OnBnClickedButton2();
	LRESULT CrtuUpdateToolDlg::WindowProc(UINT  message,  WPARAM  wParam,  LPARAM  lParam);
	CString m_curpkg;
	CString m_totalpkg;
	CString m_info;
	CString m_result;
//	CListBox m_list_com;
	CComboBox m_combolist;
	afx_msg void OnBnClickedButton4();
	CString m_verdir;
	afx_msg void OnBnClickedButton3();
	CString m_rtu_ver;
	afx_msg void OnBnClickedButton5();
	CString m_rtu_start_ver;
	CString m_iniver;
	CComboBox m_comBr;
	CComboBox m_comParity;
	CComboBox m_comDatabit;
	CComboBox m_comStopbit;
};
