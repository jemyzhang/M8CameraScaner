#include "ui_main.h"
#include "resource.h"
#include "mz_commonfunc.h"
using namespace MZ_CommonFunc;

#define MZ_IDC_TOOLBAR_MAIN 101


MZ_IMPLEMENT_DYNAMIC(Ui_MainWnd)

Ui_MainWnd::Ui_MainWnd(){
    m_pCapture = NULL;
}

Ui_MainWnd::~Ui_MainWnd(){
    if(m_pCapture){
        delete m_pCapture;
        m_pCapture = NULL;
    }
	DEVMODE  DevMode;
	memset(&DevMode, 0, sizeof(DevMode));
	DevMode.dmSize = sizeof(DevMode);
	DevMode.dmFields = DM_DISPLAYORIENTATION;
	DevMode.dmDisplayOrientation = 1;
	if(DISP_CHANGE_FAILED == ChangeDisplaySettingsEx(NULL, &DevMode, NULL, 0, NULL))
	{
		int err = GetLastError();
		RETAILMSG(1, (_T("::::::::::::::: Unable to read current rotation. Rotation disabled (%d)::::::::::::::::\n"),err));				
	}
}

BOOL Ui_MainWnd::OnInitDialog() {
	// Must all the Init of parent class first!

	if (!CMzWndEx::OnInitDialog()) {
		return FALSE;
	}
	// Then init the controls & other things in the window
	int y = 0;
	m_Toolbar.SetPos(0, GetHeight() - MZM_HEIGHT_TEXT_TOOLBAR_w720, GetWidth(), MZM_HEIGHT_TEXT_TOOLBAR_w720);
	m_Toolbar.SetID(MZ_IDC_TOOLBAR_MAIN);
    m_Toolbar.SetTextBarType(TEXT_TOOLBAR_TYPE_720);
	m_Toolbar.SetButton(1, true, true, L"开始");
	m_Toolbar.SetButton(2, true, true, L"退出");
	AddUiWin(&m_Toolbar);

	return TRUE;
}


LRESULT Ui_MainWnd::MzDefWndProc(UINT message, WPARAM wParam, LPARAM lParam) {
	return CMzWndEx::MzDefWndProc(message, wParam, lParam);
}


void Ui_MainWnd::OnMzCommand(WPARAM wParam, LPARAM lParam) {
	UINT_PTR id = LOWORD(wParam);
	switch (id) {
		case MZ_IDC_TOOLBAR_MAIN:
			{
				int nIndex = lParam;
				if(nIndex == 1){	//
#if 1
					if(m_pCapture == NULL){
						m_pCapture = new Ui_CaptureWnd;
					}
                        RECT rcWork = MzGetWorkArea();
						RECT rcCamera = {(GetWidth() - 240)/2,(GetHeight() - 240 - MZM_HEIGHT_TEXT_TOOLBAR_w720)/2,
						(GetWidth() - 240)/2 + 240,(GetHeight() - 240 - MZM_HEIGHT_TEXT_TOOLBAR_w720)/2 + 240};
						m_pCapture->setCameraPos(rcCamera);
                        m_pCapture->Create(rcWork.left, rcWork.top, RECT_WIDTH(rcWork), RECT_HEIGHT(rcWork), m_hWnd, 0, WS_POPUP);
						m_pCapture->DoModal();
						delete m_pCapture;
						m_pCapture = NULL;
#else
                    if(m_pCapture == NULL){
                        m_pCapture = new Ui_CaptureWnd;
                        RECT rcWork = MzGetWorkArea();
						RECT rcCamera = {(GetWidth() - 240)/2,(GetHeight() - 240 - MZM_HEIGHT_TEXT_TOOLBAR_w720)/2,
						(GetWidth() - 240)/2 + 240,(GetHeight() - 240 - MZM_HEIGHT_TEXT_TOOLBAR_w720)/2 + 240};
						m_pCapture->setCameraPos(rcCamera);
                        m_pCapture->Create(rcWork.left, rcWork.top, RECT_WIDTH(rcWork), RECT_HEIGHT(rcWork), m_hWnd, 0, 0);
                        //int w = m_pCapture->GetWidth();
                        //int h = m_pCapture->GetHeight();
                        //m_pCapture->SetBgColor(RGB(16,0,16));
                        //m_pCapture->SetSupportDShow(true);
                        m_pCapture->Show(true,true);
                    }else{
                        m_pCapture->Show(false,false);
                        delete m_pCapture;
                        m_pCapture = NULL;
                    }
#endif
				}
				if(nIndex == 2){	//确定
					PostQuitMessage(0);
					return;
				}
			}
	}
	CMzWndEx::OnMzCommand(wParam,lParam);
}

