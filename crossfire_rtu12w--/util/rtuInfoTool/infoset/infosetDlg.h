
// infosetDlg.h : 头文件
//

#pragma once
#include "afxwin.h"


// CinfosetDlg 对话框
class CinfosetDlg : public CDialogEx
{
// 构造
public:
	CinfosetDlg(CWnd* pParent = NULL);	// 标准构造函数
	void CinfosetDlg::EnumCommPort(CComboBox *pListBox);
// 对话框数据
	enum { IDD = IDD_INFOSET_DIALOG };

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
	afx_msg void OnBnClickedQuery();
	//	afx_msg void OnCbnSelchangeCombo1();
	CString COM1;
	afx_msg void OnCbnSelchangeComnumber();
	CComboBox m_com_select;
	CString m_rt_sn;
	CString m_rtu_doman;
	//	CString m_lamp_num;
	short m_pwm_frequnce;
	//	CString m_pwm_value;
	CString m_project_name;
	afx_msg void OnBnClickedQuery2();
	CString m_uid;
	CString m_hardware_version;
	CString m_solfware_version;
	CComboBox m_lamp_type_select;
	//	afx_msg void OnCbnSelchangeCombo2();
	CComboBox m_pwm_type_select;
	CComboBox m_lamp_swap_selcet;
	//	afx_msg void OnCbnSelchangeCombo4();
	CComboBox m_pwm_delay_opened_select;
	//	afx_msg void OnCbnSelchangeCombo5();

	//afx_msg void OnEnChangeEdit9();
	CString m_group_1;
	CString m_group_2;
	CString m_group_3;
	//afx_msg void OnEnChangeEdit9();
	//	afx_msg void OnCbnSelchangeCombo5();
	CComboBox m_lamp_type2;
	int m_pwm_value;
	int m_lamp_num;
};
