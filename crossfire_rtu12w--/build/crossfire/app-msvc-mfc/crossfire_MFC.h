
// crossfire_MFC.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// Ccrossfire_MFCApp:
// �йش����ʵ�֣������ crossfire_MFC.cpp
//

class Ccrossfire_MFCApp : public CWinApp
{
public:
	Ccrossfire_MFCApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern Ccrossfire_MFCApp theApp;