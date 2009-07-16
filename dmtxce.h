#pragma once

// include the MZFC library header file
#include <mzfc_inc.h>
#include "ui_main.h"
#define VER_STRING L"1.00"
#ifdef MZFC_STATIC
#define BUILD_STRING L"20090625(S)"
#else
#define BUILD_STRING L"20090625(D)"
#endif
#define APPNAME L"DMTXCE"

// Application class derived from CMzApp
class M8Dmtx : public CMzApp {
public:
	Ui_MainWnd m_MainWnd;
	// Initialization of the application
	virtual BOOL Init();
	HWND isRuning(){
		//������������
		//�������Ƿ��Ѿ�����
		HANDLE m_hCHDle = CreateMutex(NULL,true,L"M8Dmtx");
		if(GetLastError() == ERROR_ALREADY_EXISTS)
		{
			HWND pWnd=0;//FindWindow(m_MainWnd.GetMzClassName(),NULL);
			return pWnd; 
		}
		return 0;
	}
	virtual int Done();
private:
    //������Դ
    void loadImageRes();
    void LoadLangRes();
    void LoadRes();
private:
	bool isExternLangres;
};