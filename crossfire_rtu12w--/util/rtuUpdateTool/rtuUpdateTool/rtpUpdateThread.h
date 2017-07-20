#pragma once
class rtpUpdateThread
{
public:
	int SendCheckCmd(LPVOID arg);
	rtpUpdateThread(LPVOID arg);
	~rtpUpdateThread(void);
	HANDLE procThread;
	CString comName;
	CString dirName;
	LPVOID dlg;
	int startFlag;
};

