#include "ui_main.h"
#include "resource.h"
#include "mz_commonfunc.h"
using namespace MZ_CommonFunc;

#define MZ_IDC_TOOLBAR_MAIN		101
#define MZ_IDC_BUTTON_CAPTURE	102
#define MZ_IDC_BUTTON_LOAD		103
#include <ShellNotifyMsg.h>


MZ_IMPLEMENT_DYNAMIC(Ui_MainWnd)

Ui_MainWnd::Ui_MainWnd(){
    m_pCapture = NULL;
}

Ui_MainWnd::~Ui_MainWnd(){
    if(m_pCapture){
        delete m_pCapture;
        m_pCapture = NULL;
    }
    RotateScreen(1);
}

void Ui_MainWnd::PaintWin(HDC hdc, RECT* prcUpdate){
    RotateScreen(0);    //保持横屏
    CMzWndEx::PaintWin(hdc,prcUpdate);
}

BOOL Ui_MainWnd::OnInitDialog() {
	// Must all the Init of parent class first!

	if (!CMzWndEx::OnInitDialog()) {
		return FALSE;
	}
	// Then init the controls & other things in the window
	int y = 0;

	y = GetHeight() - MZM_HEIGHT_TEXT_TOOLBAR_w720 - MZM_WIDTH_DELETE_BUTTON * 3;
	m_ButtonCapture.SetPos(GetWidth()/3,y,GetWidth()/3,MZM_WIDTH_DELETE_BUTTON);
	m_ButtonCapture.SetText(L"摄像头扫描");
	m_ButtonCapture.SetID(MZ_IDC_BUTTON_CAPTURE);
	AddUiWin(&m_ButtonCapture);

	y += MZM_WIDTH_DELETE_BUTTON;
	m_ButtonLoadFile.SetPos(GetWidth()/3,y,GetWidth()/3,MZM_WIDTH_DELETE_BUTTON);
	m_ButtonLoadFile.SetText(L"从文件读取");
	m_ButtonLoadFile.SetID(MZ_IDC_BUTTON_LOAD);
	AddUiWin(&m_ButtonLoadFile);

	m_Toolbar.SetPos(0, GetHeight() - MZM_HEIGHT_TEXT_TOOLBAR_w720, GetWidth(), MZM_HEIGHT_TEXT_TOOLBAR_w720);
	m_Toolbar.SetID(MZ_IDC_TOOLBAR_MAIN);
    m_Toolbar.SetTextBarType(TEXT_TOOLBAR_TYPE_720);
	m_Toolbar.SetButton(2, true, true, L"退出");
	AddUiWin(&m_Toolbar);

	::RegisterShellMessage(m_hWnd,WM_MZSH_ENTRY_LOCKPHONE |WM_MZSH_LEAVE_LOCKPHONE);

	return TRUE;
}


LRESULT Ui_MainWnd::MzDefWndProc(UINT message, WPARAM wParam, LPARAM lParam) {
	return CMzWndEx::MzDefWndProc(message, wParam, lParam);
}


void Ui_MainWnd::OnMzCommand(WPARAM wParam, LPARAM lParam) {
	UINT_PTR id = LOWORD(wParam);
	switch (id) {
		case MZ_IDC_BUTTON_CAPTURE:
			{
				if(m_pCapture == NULL){
					m_pCapture = new Ui_CaptureWnd;
				}
				m_pCapture->setDecodeSource(DECODE_FROM_CAMERA);
				RECT rcWork = MzGetWorkArea();
				m_pCapture->Create(rcWork.left, rcWork.top, RECT_WIDTH(rcWork), RECT_HEIGHT(rcWork), m_hWnd, 0, WS_POPUP);
				m_pCapture->DoModal();
				delete m_pCapture;
				m_pCapture = NULL;
				break;
			}
		case MZ_IDC_BUTTON_LOAD:
			{
				if(m_pCapture == NULL){
					m_pCapture = new Ui_CaptureWnd;
				}
				m_pCapture->setDecodeSource(DECODE_FROM_FILE);
				RECT rcWork = MzGetWorkArea();
				m_pCapture->Create(rcWork.left, rcWork.top, RECT_WIDTH(rcWork), RECT_HEIGHT(rcWork), m_hWnd, 0, WS_POPUP);
				m_pCapture->DoModal();
				delete m_pCapture;
				m_pCapture = NULL;
				break;
			}
		case MZ_IDC_TOOLBAR_MAIN:
			{
				int nIndex = lParam;
				if(nIndex == 2){	//确定
					PostQuitMessage(0);
					return;
				}
			}
	}
	CMzWndEx::OnMzCommand(wParam,lParam);
}

bool Ui_MainWnd::OnShellMessage(UINT message, WPARAM wParam, LPARAM lParam){
	if(message == ::GetShellNotifyMsg_EntryLockPhone() || message == ::GetShellNotifyMsg_ReadyPowerOFF()){
		if(m_pCapture){
			m_pCapture->EndModal(ID_CANCEL);
			delete m_pCapture;
			m_pCapture = NULL;
		}
		return true;
	}
	return false;
}