#include "ui_main.h"
#include "resource.h"
#include "mz_commonfunc.h"
using namespace MZ_CommonFunc;
#include <ShellNotifyMsg.h>
#include <InitGuid.h>
#include <IMzUnknown.h>
#include <IMzUnknown_IID.h>
#include <IFileBrowser.h>
#include <IFileBrowser_GUID.h>

#define MZ_IDC_TOOLBAR_MAIN		101
#define MZ_IDC_BUTTON_CAPTURE	102
#define MZ_IDC_BUTTON_LOAD		103


MZ_IMPLEMENT_DYNAMIC(Ui_MainWnd)

Ui_MainWnd::Ui_MainWnd(){
    m_pCapture = NULL;
	m_pDecode = NULL;
}

Ui_MainWnd::~Ui_MainWnd(){
    if(m_pCapture){
        delete m_pCapture;
        m_pCapture = NULL;
    }
	::UnRegisterShellMessage(m_hWnd,::GetShellNotifyMsg_EntryLockPhone() | ::GetShellNotifyMsg_ReadyPowerOFF());
}

void Ui_MainWnd::PaintWin(HDC hdc, RECT* prcUpdate){
    //RotateScreen(SCREEN_ORIENTATION_90);    //保持横屏
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
	m_ButtonCapture.SetPos(GetWidth()/4,y,GetWidth()/2,MZM_WIDTH_DELETE_BUTTON);
	m_ButtonCapture.SetText(L"摄像头扫描");
	m_ButtonCapture.SetID(MZ_IDC_BUTTON_CAPTURE);
	AddUiWin(&m_ButtonCapture);

	y += MZM_WIDTH_DELETE_BUTTON;
	m_ButtonLoadFile.SetPos(GetWidth()/4,y,GetWidth()/2,MZM_WIDTH_DELETE_BUTTON);
	m_ButtonLoadFile.SetText(L"从文件读取");
	m_ButtonLoadFile.SetID(MZ_IDC_BUTTON_LOAD);
	AddUiWin(&m_ButtonLoadFile);

	m_Toolbar.SetPos(0, GetHeight() - MZM_HEIGHT_TEXT_TOOLBAR_w720, GetWidth(), MZM_HEIGHT_TEXT_TOOLBAR_w720);
	m_Toolbar.SetID(MZ_IDC_TOOLBAR_MAIN);
    m_Toolbar.SetTextBarType(TEXT_TOOLBAR_TYPE_720);
	m_Toolbar.SetButton(2, true, true, L"退出");
	AddUiWin(&m_Toolbar);

	::RegisterShellMessage(m_hWnd, ::GetShellNotifyMsg_EntryLockPhone() | ::GetShellNotifyMsg_ReadyPowerOFF());
	return TRUE;
}


LRESULT Ui_MainWnd::MzDefWndProc(UINT message, WPARAM wParam, LPARAM lParam) {
	if(OnShellMessage(message,wParam,lParam)) return 0;
	return CMzWndEx::MzDefWndProc(message, wParam, lParam);
}


void Ui_MainWnd::OnMzCommand(WPARAM wParam, LPARAM lParam) {
	UINT_PTR id = LOWORD(wParam);
	switch (id) {
		case MZ_IDC_BUTTON_CAPTURE:
			{
				::EnterFullScreen(true);

				if(m_pCapture == NULL){
					m_pCapture = new ui_VideoSurface;//Ui_CaptureWnd;
				}
				RotateScreen(SCREEN_ORIENTATION_90);    //横屏
				//m_pCapture->setDecodeSource(DECODE_FROM_CAMERA);
				RECT rcWork = MzGetWorkArea();
				m_pCapture->Create(rcWork.left, rcWork.top, RECT_WIDTH(rcWork), RECT_HEIGHT(rcWork), m_hWnd, 0, WS_POPUP);
				int nRet = m_pCapture->DoModal();
				C::newstrcpy(&m_ImageFile,m_pCapture->getCapturePath());
				RotateScreen(SCREEN_ORIENTATION_0);    //横屏
				::EnterFullScreen(false);


				if(nRet == ID_OK){
					if(m_pDecode == NULL){
						m_pDecode = new Ui_CaptureWnd;
					}
					m_pDecode->setDecodeSource(DECODE_FROM_CAMERA);
					//TODO: m_pDecode->SetScanRegion();
					m_pDecode->SetImageFile(m_ImageFile);
					RECT rcWork = MzGetWorkArea();
					m_pDecode->Create(rcWork.left, rcWork.top, RECT_WIDTH(rcWork), RECT_HEIGHT(rcWork), m_hWnd, 0, WS_POPUP);
					m_pDecode->DoModal();
					delete m_pDecode;
					m_pDecode = NULL;
				}
				delete m_pCapture;
				m_pCapture = NULL;
				break;
			}
		case MZ_IDC_BUTTON_LOAD:
			{
				if(GetImageFile()){
					if(m_pDecode == NULL){
						m_pDecode = new Ui_CaptureWnd;
					}
					m_pDecode->setDecodeSource(DECODE_FROM_FILE);
					m_pDecode->SetImageFile(m_ImageFile);
					RECT rcWork = MzGetWorkArea();
					m_pDecode->Create(rcWork.left, rcWork.top, RECT_WIDTH(rcWork), RECT_HEIGHT(rcWork), m_hWnd, 0, WS_POPUP);
					m_pDecode->DoModal();
					delete m_pDecode;
					m_pDecode = NULL;
				}
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
	if(	message == ::GetShellNotifyMsg_EntryLockPhone() ||
		message == ::GetShellNotifyMsg_ReadyPowerOFF()){
			if(this->m_pCapture){
				m_pCapture->EndModal(ID_CANCEL);
				return true;
			}else{
				return false;
			}
	}
	return false;
}

bool Ui_MainWnd::GetImageFile(){
	bool nRet = false;
	IFileBrowser *pFile = NULL;
//	RotateScreen(SCREEN_ORIENTATION_0);
	CoInitializeEx(NULL, COINIT_MULTITHREADED );
	IMzSelect *pSelect = NULL; 
	if ( SUCCEEDED( CoCreateInstance( CLSID_FileBrowser, NULL,CLSCTX_INPROC_SERVER ,IID_MZ_FileBrowser,(void **)&pFile ) ) )
	{     
		if( SUCCEEDED( pFile->QueryInterface( IID_MZ_Select, (void**)&pSelect ) ) )
		{
			TCHAR file[ MAX_PATH ] = { 0 };
			pFile->SetParentWnd( m_hWnd );
			pFile->SetOpenDirectoryPath( L"\\Disk" );
			pFile->SetExtFilter( L"*.jpg;*.gif;*.png;*.bmp");                                      
			pFile->SetOpenDocumentType(DOCUMENT_SELECT_SINGLE_FILE);
			CMzString fileDlgTitle = L"选择图片";
			pFile->SetTitle((TCHAR*)fileDlgTitle.C_Str());

			if( pSelect->Invoke() ) 
			{						
				C::newstrcpy( &m_ImageFile, pFile->GetSelectedFileName() );
				nRet = true;
			}
			pSelect->Release();
		}     
		pFile->Release();
	}	
	CoUninitialize();
//	RotateScreen(SCREEN_ORIENTATION_90);
	return nRet;
}