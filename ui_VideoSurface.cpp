#include "ui_VideoSurface.h"
#include "mz_commonfunc.h"
using namespace MZ_CommonFunc;

#include "resource.h"

#include <InitGuid.h>
#include <ICamera_GUID.h>
#include <ShellNotifyMsg.h>

#define MZ_IDC_BUTTON_SHAPE    101
#define MZ_IDC_BUTTON_CAPTURE   102
#define MZ_IDC_BUTTON_EXIT   103


extern ImagingHelper *pimg[IDB_PNG_END - IDB_PNG_BEGIN + 1];
ui_VideoSurface::ui_VideoSurface(void)
{
	m_ImageFile = NULL;
	isInitialized = false;
	m_type = T_QR_CODE;
	reqInitCamera = false;
	fadeinStep = 0;
	autoEndTime = 0;
	m_pDecode = NULL;
}

ui_VideoSurface::~ui_VideoSurface(void)
{
	if(m_ImageFile) delete [] m_ImageFile;
	ReleaseCameraDevice();
}

BOOL ui_VideoSurface::OnInitDialog() {
    // Must all the Init of parent class first!

    if (!CMzWndEx::OnInitDialog()) {
        return FALSE;
    }

	this->SetBgColor(RGB(128,128,128));

	int width = GetWidth()/3;
	int height = GetHeight() - 600;
	int x = 0;
	int y = 600;
	m_ButtonChangeShape.SetPos(x,y,width,height);
	m_ButtonChangeShape.SetImage_Normal(pimg[IDB_PNG_SHAPE-IDB_PNG_BEGIN]);
	m_ButtonChangeShape.SetID(MZ_IDC_BUTTON_SHAPE);
	AddUiWin(&m_ButtonChangeShape);

	x += width;
	m_ButtonCapture.SetPos(x,y,width,height);
	m_ButtonCapture.SetImage_Normal(pimg[IDB_PNG_SHOT-IDB_PNG_BEGIN]);
	m_ButtonCapture.SetID(MZ_IDC_BUTTON_CAPTURE);
	AddUiWin(&m_ButtonCapture);

	x += width;
	m_ButtonExit.SetPos(x,y,width,height);
	m_ButtonExit.SetImage_Normal(pimg[IDB_PNG_EXIT-IDB_PNG_BEGIN]);
	m_ButtonExit.SetID(MZ_IDC_BUTTON_EXIT);
	AddUiWin(&m_ButtonExit);

    adjustCameraPos();
	SetTimer(m_hWnd,0x1001,100,NULL);
	SetTimer(m_hWnd,0x1002,200,NULL);	//OSD
	SetTimer(m_hWnd,0x1003,1000,NULL);	//auto off

	return TRUE;
}

wchar_t* osdStr[] = {
	L"QR Code Scan",
	L"DM Code Scan",
	L"L Code Scan "
};
void ui_VideoSurface::PaintWin(HDC hdc, RECT* prcUpdate){
	RECT rcScreen = { 0,
		0,
		GetWidth(),
		GetHeight()};
	RECT rcRect = { m_rcCamera.left  - 1,
		m_rcCamera.top ,
		m_rcCamera.right + 1,
		m_rcCamera.bottom + 2 };
	//绘制底色
	::DrawRect(hdc,&rcScreen,RGB(128,128,128),RGB(128,128,128));
	//绘制边框
	::DrawRect(hdc,&rcRect,RGB(128,255,128),RGB(16,0,16));

	//绘制十字
	HPEN pen = CreatePen(PS_SOLID, 2,RGB(128,255,128));
	::SelectObject(hdc,pen);
	int x1 =  m_rcCamera.left + RECT_WIDTH(m_rcCamera)/2 - 15;
	int y1 = m_rcCamera.top + RECT_HEIGHT(m_rcCamera)/2 - 15;
	::MoveToEx(hdc,x1,y1 + 15,NULL);
	::LineTo(hdc,x1 + 30, y1 + 15);
	::MoveToEx(hdc,x1 + 15,y1,NULL);
	::LineTo(hdc,x1 + 15, y1 + 30);
	::DeleteObject(pen);

	if(fadeinStep){
		fadeinStep--;
		HFONT font = FontHelper::GetFont(42,500);
		::SelectObject(hdc,font);
		COLORREF color = RGB(128,128 + 8*fadeinStep,128);
		RECT rcText = { 10,
			10,
			GetWidth() - 10,
			m_rcCamera.top  - 10};
		::SetBkMode(hdc,TRANSPARENT);
		::SetTextColor(hdc,color);
		::DrawText(hdc,osdStr[m_type],lstrlen(osdStr[m_type]),&rcText,DT_CENTER | DT_VCENTER);
		::DeleteObject(font);
	}
	CMzWndEx::PaintWin(hdc,prcUpdate);
}

LRESULT ui_VideoSurface::MzDefWndProc(UINT message, WPARAM wParam, LPARAM lParam) {
	if(OnShellMessage(message,wParam,lParam)) return 0;
	switch (message) {
        case WM_LBUTTONUP:
        {
			int x = LOWORD(lParam);
			int y = HIWORD(lParam);
			if(x > m_rcCamera.left && x < m_rcCamera.right &&
				y > m_rcCamera.top && y < m_rcCamera.bottom){
					m_pDevice->StartAF();
			}
			resetAutoOff();
		}
	}
    return CMzWndEx::MzDefWndProc(message, wParam, lParam);
}

void ui_VideoSurface::OnMzCommand(WPARAM wParam, LPARAM lParam) {
    UINT_PTR id = LOWORD(wParam);
    switch (id) {
        case MZ_IDC_BUTTON_SHAPE:
            {
				BYTE dtype = (BYTE)m_type;
				dtype++;
				if(dtype > T_BAR_CODE){
					dtype = T_QR_CODE;
				}
				m_type = (BarCodeType_t)dtype;
				fadeinStep = 16;
				adjustCameraPos();
                break;
            }
        case MZ_IDC_BUTTON_CAPTURE:
            {
				if(isInitialized){
					m_pDevice->PausePreview();
					m_pDevice->TakePhoto();
					//BITMAPINFO* RotatedBitmap = m_pDevice->GetPreviewDataInfo();
					DateTime::waitms(1000);
					if(m_pDecode == NULL){
						m_pDecode = new Ui_CaptureWnd;
					}
					RECT m_rcRegion = {m_rcCamera.top,m_rcCamera.left, m_rcCamera.bottom,m_rcCamera.right };
					m_pDecode->setDecodeSource(DECODE_FROM_CAMERA);
					m_pDecode->SetScanRegion(m_rcRegion);
                    m_pDecode->SetDecodeType(m_type);
					m_pDecode->SetImageFile(m_ImageFile);
					RECT rcWork = MzGetWorkArea();
					m_pDecode->Create(rcWork.left, rcWork.top, RECT_WIDTH(rcWork), RECT_HEIGHT(rcWork), m_hWnd, 0, WS_POPUP);
					m_pDecode->DoModal();
					delete m_pDecode;
					m_pDecode = NULL;
					m_pDevice->StartPreview();
					//EndModal(ID_OK);
				}
				break;
            }
        case MZ_IDC_BUTTON_EXIT:
            {
				EndModal(ID_CANCEL);
                break;
            }
    }
    CMzWndEx::OnMzCommand(wParam,lParam);
}

void ui_VideoSurface::OnTimer(UINT nIDEvent){
	switch(nIDEvent){
		case 0x1001:
			if(reqInitCamera){
				reqInitCamera = false;
				InitCameraDevice();
				KillTimer(m_hWnd,0x1001);
			}
			break;
		case 0x1002:
		{
			if(fadeinStep > 0){
				this->Invalidate();
				this->UpdateWindow();
			}
			break;
		}
		case 0x1003:
			{
				if(autoEndTime>0){
					autoEndTime --;
					if(autoEndTime == 0){
						EndModal(ID_CANCEL);
					}
				}
				break;
			}
		case 0x1004:
			{
            m_pDevice->IsDrawPreviewFrame(false);
            KillTimer(m_hWnd, 0x1004);
            HDC previewFrameDC = CreateCompatibleDC(GetDC(m_hWnd));
            HBITMAP hPreviewFrameBitmap = CreateCompatibleBitmap(GetDC(m_hWnd),480, 640);
            HGDIOBJ hOldPreviewBmp = SelectObject(previewFrameDC, hPreviewFrameBitmap);

            HDC standByDC = CreateCompatibleDC(GetDC(m_hWnd));
//            HBITMAP hStandByBitmap = CreateCompatibleBitmap(GetDC(m_hWnd),RECT_WIDTH(m_rcCamera), RECT_HEIGHT(m_rcCamera));
            HBITMAP hStandByBitmap = CreateCompatibleBitmap(GetDC(m_hWnd),480,640);
            HGDIOBJ hStandByOldBmp = SelectObject(standByDC, hStandByBitmap);          

			BITMAPINFO* RotatedBitmap = m_pDevice->GetPreviewDataInfo();
			int width = RotatedBitmap->bmiHeader.biWidth;
			int height = RotatedBitmap->bmiHeader.biHeight;
			int widthDest = RotatedBitmap->bmiHeader.biHeight ;
			int heightDest =  RotatedBitmap->bmiHeader.biWidth ;
			BYTE    *lpBitmapBitsDest = new BYTE[widthDest*heightDest*2];		//16bits
			BYTE    *lpBitmapBitsSrc = (BYTE*)m_pDevice->GetPreviewData()->Data;
			//逆时针翻转原始图像
			for(int iw = 0; iw < width; iw++){
				for(int ih = 0; ih < height; ih++){
					//int ps = (RotatedBitmap->bmiHeader.biWidth *(ih+1) - (iw+1))*2;
					int ps = (width *(height - ih - 1) + iw)*2;
					int pd = (height *iw + ih)*2;
					lpBitmapBitsDest[pd + 0] = lpBitmapBitsSrc[ps + 0];
					lpBitmapBitsDest[pd + 1] = lpBitmapBitsSrc[ps + 1];
				}
			}
			RotatedBitmap->bmiHeader.biWidth = widthDest;
			RotatedBitmap->bmiHeader.biHeight = heightDest;
            if(FALSE == StretchDIBits(standByDC, 0, 0, 480,640,  0, 0, 480,640, lpBitmapBitsDest, RotatedBitmap, 
              DIB_RGB_COLORS, SRCCOPY))
            {
              RETAILMSG(1, (TEXT("Stretch DIBits failed...\r\n")));      
            }

            //if(FALSE == StretchDIBits(standByDC, 0, 0, RECT_WIDTH(m_rcCamera), RECT_HEIGHT(m_rcCamera),  m_rcCamera.left,  m_rcCamera.top ,
            //  RECT_WIDTH(m_rcCamera), RECT_HEIGHT(m_rcCamera), lpBitmapBitsDest, RotatedBitmap, 
            //  DIB_RGB_COLORS, SRCCOPY))
            //{
            //  RETAILMSG(1, (TEXT("Stretch DIBits failed...\r\n")));      
            //}
			RECT rcScreen = { 0,
								0,
								GetWidth(),
								GetHeight()};
			RECT rcRect = { m_rcCamera.left  - 1,
								m_rcCamera.top ,
								m_rcCamera.right + 1,
								m_rcCamera.bottom + 2 };
			//绘制底色
			::DrawRect(previewFrameDC,&rcScreen,RGB(128,128,128),RGB(128,128,128));
			//绘制边框
			::DrawRect(previewFrameDC,&rcRect,RGB(128,255,128),RGB(16,0,16));
			BitBlt(previewFrameDC,  m_rcCamera.left , m_rcCamera.top,  RECT_WIDTH(m_rcCamera), RECT_HEIGHT(m_rcCamera), standByDC, 0, 0, SRCCOPY );
            //for ( int i = 1; i <= RECT_HEIGHT(m_rcCamera); ++i)
            //{
            //  BitBlt(previewFrameDC,  m_rcCamera.left , m_rcCamera.top + i,  RECT_WIDTH(m_rcCamera), 1, standByDC, 0, RECT_HEIGHT(m_rcCamera)-i, SRCCOPY );
            //}

			//绘制十字
			HPEN pen = CreatePen(PS_SOLID, 2,RGB(128,255,128));
			::SelectObject(previewFrameDC,pen);
			int x1 =  m_rcCamera.left + RECT_WIDTH(m_rcCamera)/2 - 15;
			int y1 = m_rcCamera.top + RECT_HEIGHT(m_rcCamera)/2 - 15;
			::MoveToEx(previewFrameDC,x1,y1 + 15,NULL);
			::LineTo(previewFrameDC,x1 + 30, y1 + 15);
			::MoveToEx(previewFrameDC,x1 + 15,y1,NULL);
			::LineTo(previewFrameDC,x1 + 15, y1 + 30);
			::DeleteObject(pen);

			if(fadeinStep){
				fadeinStep--;
				HFONT font = FontHelper::GetFont(42,500);
				::SelectObject(previewFrameDC,font);
				COLORREF color = RGB(128,128 + 8*fadeinStep,128);
				RECT rcText = { 10,
								10,
								GetWidth() - 10,
								m_rcCamera.top  - 10};
				::SetBkMode(previewFrameDC,TRANSPARENT);
				::SetTextColor(previewFrameDC,color);
				::DrawText(previewFrameDC,osdStr[m_type],lstrlen(osdStr[m_type]),&rcText,DT_CENTER | DT_VCENTER);
				::DeleteObject(font);
			}

			BitBlt(GetDC(m_hWnd), 0 , 0 ,640,480, previewFrameDC, 0, 0, SRCCOPY );


			SelectObject(standByDC, hStandByOldBmp);
            DeleteObject(hStandByBitmap);
            DeleteDC(standByDC);

            SelectObject(previewFrameDC, hOldPreviewBmp);
            DeleteObject(hPreviewFrameBitmap);
            DeleteDC(previewFrameDC);

            SetTimer(m_hWnd, 0x1004, 200, NULL);

			delete [] lpBitmapBitsDest;
			}
	}
}

void ui_VideoSurface::adjustCameraPos(){
	RECT rcCameraSquare = {(GetWidth() - 240)/2,(GetHeight() - 240 - 80)/2,
		(GetWidth() - 240)/2 + 240,(GetHeight() - 240 - 80)/2 + 240};
	RECT rcCameraRect = {(GetWidth() - 400)/2,(GetHeight() - 200 - 80)/2,
		(GetWidth() - 400)/2 + 400,(GetHeight() - 200 - 80)/2 + 200};


	if(m_type == T_BAR_CODE){
		m_rcCamera = rcCameraRect;
	}else{
		m_rcCamera = rcCameraSquare;
	}
}

bool ui_VideoSurface::InitCameraDevice(){
	if(isInitialized) return true;
    ::RegisterShellMessage(m_hWnd,WM_MZSH_ENTRY_LOCKPHONE | WM_MZSH_READY_POWEROFF);
    ::HoldShellUsingSomeKeyFunction(m_hWnd,MZ_HARDKEY_POWER);
    ::SetScreenAlwaysOn(m_hWnd);
    resetAutoOff();//60秒自动退出
	MzBeginWaitDlg(m_hWnd,&m_rcCamera);
	bool bRet = false;
//	::CoInitializeEx(0, COINIT_MULTITHREADED);
//	initUiProgressBar(L"启动摄像头",m_hWnd);
//	uiRefreshProgressBar(L"准备摄像头",0,3);
	// Then init the controls & other things in the window
	if(	SUCCEEDED(CoCreateInstance(CLSID_CameraDevice, NULL,CLSCTX_INPROC_SERVER ,IID_MZ_CameraInterface,(void **)&m_pDevice))){
//		uiRefreshProgressBar(L"初始化摄像头设备",1,3);
		if( SUCCEEDED( m_pDevice->InitDevice(m_hWnd/*m_wnd*/)) )
		{
//			uiRefreshProgressBar(L"设置摄像头参数",2,3);
			/*        
			QXGA_PHOTO_SIZE 2048 * 1536
			UXGA_PHOTO_SIZE 1600 * 1200
			XGA_PHOTO_SIZE 1024 * 768
			VGA_PHOTO_SIZE 640 * 480 
			*/
			m_pDevice->SetPhotoSize(VGA_PHOTO_SIZE);
			/*        
			BRIGHTNESS_LEVEL_1 ~ BRIGHTNESS_LEVEL_6
			*/
			C::newstrcpy(&m_ImageFile,L"/cap.jpg");
			m_pDevice->SetBrightness(BRIGHTNESS_LEVEL_6);
			m_pDevice->IsDrawPreviewFrame(true);
			m_pDevice->SetMode(MODE_TYPE_AUTO);
			m_pDevice->SetPhotoName(m_ImageFile);
			isInitialized = true;
			m_pDevice->StartPreview();
			bRet = true;
			//m_pDevice->SetPreviewAreaAlphaValue(12);
		}
	}
	MzEndWaitDlg();
//	uiRefreshProgressBar(NULL,3,3);
//	::CoUninitialize();
//	SetTimer(m_hWnd, 0x1004, 200, NULL);
	return bRet;
}

bool ui_VideoSurface::ReleaseCameraDevice(){
	bool bRet = true;
    if(isInitialized){
		bRet = false;
		isInitialized = false;
		if(m_pDevice->UnInitDevice()){
			m_pDevice->Release();
	        m_pDevice = NULL;
			bRet = true;
			::UnRegisterShellMessage(m_hWnd,WM_MZSH_ENTRY_LOCKPHONE | WM_MZSH_READY_POWEROFF);
			::UnHoldShellUsingSomeKeyFunction(m_hWnd,MZ_HARDKEY_POWER);
			::SetScreenAutoOff();
		}
    }
	return bRet;
}

bool ui_VideoSurface::OnShellMessage(UINT message, WPARAM wParam, LPARAM lParam){
	if(message == ::GetShellNotifyMsg_AllKeyEvent()){
		resetAutoOff();
		switch(wParam){
			case WPARAM_KEY_EVENT_CLICK_POWER:
			case WPARAM_KEY_EVENT_DBLCLICK_POWER:
			case WPARAM_KEY_EVENT_LONGCLICK_POWER:
				if(isInitialized){
					EndModal(ID_CANCEL);
					return true;
				}else{
					return false;
				}
				break;
		}
	}
	if(message == ::GetShellNotifyMsg_EntryLockPhone() || message == ::GetShellNotifyMsg_ReadyPowerOFF()){
		if(isInitialized){
			EndModal(ID_CANCEL);
		}
		return true;
	}
	return false;
}