// rtuReleaseToolDlg.h : header file
//

#if !defined(AFX_RTURELEASETOOLDLG_H__B7C0726A_3372_45C3_A5C2_8FA6BBF01559__INCLUDED_)
#define AFX_RTURELEASETOOLDLG_H__B7C0726A_3372_45C3_A5C2_8FA6BBF01559__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CRtuReleaseToolDlg dialog

class CRtuReleaseToolDlg : public CDialog
{
// Construction
public:
	int m_list_cnt;
	char m_cur[MAX_PATH];
	CRtuReleaseToolDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CRtuReleaseToolDlg)
	enum { IDD = IDD_RTURELEASETOOL_DIALOG };
	CListCtrl	m_list_all;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRtuReleaseToolDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CRtuReleaseToolDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButton1();
	afx_msg void OnButton2();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RTURELEASETOOLDLG_H__B7C0726A_3372_45C3_A5C2_8FA6BBF01559__INCLUDED_)
