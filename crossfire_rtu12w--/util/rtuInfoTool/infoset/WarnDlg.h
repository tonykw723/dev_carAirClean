#pragma once
#include "afxwin.h"

// WarnDlg �Ի���

class WarnDlg : public CDialogEx
{
	DECLARE_DYNAMIC(WarnDlg)

public:
	WarnDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~WarnDlg();

// �Ի�������
	enum { IDD = IDD_DIALOG2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
};
