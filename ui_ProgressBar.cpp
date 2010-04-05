#include <Mzfc/MzProgressDialog.h> 
#include "ui_ProgressBar.h"
#include "mz_commonfunc.h"
using namespace MZ_CommonFunc;
#include "resource.h"

static bool brangeSet = false;
static bool bdlgshown = false;
static MzPopupProgress m_Progressdlg;
static HWND m_hWnd;

void SetProgressBarTitle(LPWSTR t){
	if(t == NULL){
		return;
	}
    m_Progressdlg.SetTitleText(t);
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
		m_Progressdlg.StartProgress(m_hWnd,FALSE);
		bdlgshown = true;
	}
}
void HideProgressBar(){
	m_Progressdlg.KillProgress();
	bdlgshown = false;
}

void initProgressBar(LPWSTR title = NULL, WORD rmin = 0, WORD rmax = 100){
	SetProgressBarTitle(title);
	SetProgressBarRange(rmin,rmax);
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
	m_Progressdlg.SetNoteText(info);
	m_Progressdlg.SetCurrentValue(nCount+1);
	m_Progressdlg.UpdateProgress();
	DateTime::waitms(0);
	return true;
}
