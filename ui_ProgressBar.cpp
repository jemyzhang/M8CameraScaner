#include <Mzfc/MzProgressDialog.h> 
#include "ui_ProgressBar.h"
#include "mz_commonfunc.h"
using namespace MZ_CommonFunc;
#include "resource.h"

static bool brangeSet = false;
static bool bdlgshown = false;
static MzProgressDialog m_Progressdlg;
static HWND m_hWnd;

void SetProgressBarTitle(LPWSTR t){
	if(t == NULL){
		m_Progressdlg.SetShowTitle(false);
		return;
	}
	m_Progressdlg.SetTitle(t);
	m_Progressdlg.SetShowTitle(true);
}
void SetProgressBarRange(WORD rmin, WORD rmax){
	if(brangeSet) return;
	if(rmin > rmax){
		WORD temp = rmin;
		rmin = rmax;
		rmax = temp;
	}
	m_Progressdlg.SetRange(rmin,rmax);
	brangeSet = true;
}
void ShowProgressBar(){
	if(!bdlgshown){
		m_Progressdlg.BeginProgress(m_hWnd);
		bdlgshown = true;
	}
}
void HideProgressBar(){
	m_Progressdlg.EndProgress();
	bdlgshown = false;
}

void initProgressBar(LPWSTR title = NULL, WORD rmin = 0, WORD rmax = 100){
	SetProgressBarTitle(title);
	SetProgressBarRange(rmin,rmax);
	m_Progressdlg.SetShowInfo(true);
	brangeSet = false;
	bdlgshown = false;
}

void initUiProgressBar(wchar_t* title,HWND hwnd){
    m_hWnd = hwnd;
	initProgressBar(title);
}


bool uiRefreshProgressBar(wchar_t* info,WORD nCount,WORD nSize){
	if(info == NULL){
		HideProgressBar();
		return false;
	}
	ShowProgressBar();
	SetProgressBarRange(0,nSize);
	m_Progressdlg.SetInfo(info);
	m_Progressdlg.SetCurValue(nCount+1);
	m_Progressdlg.UpdateProgress();
	DateTime::waitms(0);
	return true;
}
