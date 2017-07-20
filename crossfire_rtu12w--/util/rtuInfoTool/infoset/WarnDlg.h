#pragma once
#include "afxwin.h"

// WarnDlg 对话框

class WarnDlg : public CDialogEx
{
	DECLARE_DYNAMIC(WarnDlg)

public:
	WarnDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~WarnDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
