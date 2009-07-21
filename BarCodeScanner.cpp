#include "BarCodeScanner.h"
#include "resource.h"
#include "mz_commonfunc.h"

// The global variable of the application.
M8BarCodeScanner theApp;
HINSTANCE LangresHandle;
HINSTANCE ImgresHandle;
ImagingHelper *pimg[IDB_PNG_END - IDB_PNG_BEGIN + 1];
//ImagingHelper *imgArrow;
//AppConfigIni appconfig;


void M8BarCodeScanner::loadImageRes(){
	ImgresHandle = MzGetInstanceHandle();
	for(int i = 0; i < sizeof(pimg) / sizeof(pimg[0]); i++){
		pimg[i] = LOADIMAGE(IDB_PNG_BEGIN + i);
	}

/*	HINSTANCE MzresHandle = GetMzResModuleHandle();
	imgArrow = ImagingHelper::GetImageObject(MzresHandle, MZRES_IDR_PNG_ARROW_RIGHT, true);
	*/
}

void M8BarCodeScanner::LoadLangRes(){
    //载入资源
    LangresHandle = LoadLibrary(L"language.dll");
    if(LangresHandle){
        isExternLangres = true;
    }else{
        LangresHandle = MzGetInstanceHandle();
        isExternLangres = false;
    }
}

void M8BarCodeScanner::LoadRes(){
    //载入图像资源
    loadImageRes();
    //载入语言资源
    LoadLangRes();
}

BOOL M8BarCodeScanner::Init() {
    // Init the COM relative library.
    CoInitializeEx(0, COINIT_MULTITHREADED);

    //载入资源
    LoadRes();
    //检测程序是否已经运行
    HWND pWnd = isRuning();
    if(pWnd)
    {
        SetForegroundWindow(pWnd);
        PostMessage(pWnd,WM_NULL,NULL,NULL);
        PostQuitMessage(0);
        return true; 
    }
//    RotateScreen(SCREEN_ORIENTATION_90);    //横屏

    // Create the main window
    RECT rcWork = MzGetWorkArea();
    m_MainWnd.Create(rcWork.left, rcWork.top, RECT_WIDTH(rcWork), RECT_HEIGHT(rcWork), 0, 0, 0);
//	m_MainWnd.SetSupportDShow(true);
    m_MainWnd.Show();

    return TRUE;
}

int M8BarCodeScanner::Done(){
    FreeMzResModule();
    if(isExternLangres) FreeLibrary(LangresHandle);
    return CMzApp::Done();
}