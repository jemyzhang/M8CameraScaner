#include "ui_main.h"
#include "resource.h"
#include "mz_commonfunc.h"
using namespace MZ_CommonFunc;

#include <InitGuid.h>
#include <ICamera_GUID.h>

#define MZ_IDC_TOOLBAR_MAIN 101


MZ_IMPLEMENT_DYNAMIC(Ui_CaptureWnd)

#include "libdmtx.h"
#include <assert.h>

wchar_t* chr2wch(const char* buffer, wchar_t** wbuf)
{
      size_t len = strlen(buffer); 
      size_t wlen = MultiByteToWideChar(CP_ACP, 0, (const char*)buffer, int(len), NULL, 0); 
      wchar_t *wBuf = new wchar_t[wlen + 1]; 
      MultiByteToWideChar(CP_ACP, 0, (const char*)buffer, int(len), wBuf, int(wlen));
	  *wbuf = wBuf;
	  return wBuf;
} 

bool Ui_CaptureWnd::DataMatrixDecode(unsigned char* px,int w, int h){
	bool bRet = false;

	DmtxImage      *img;
	DmtxDecode     *dec;
	DmtxRegion     *reg;
	DmtxMessage    *msg;
	int flipMethods[3] = {DmtxFlipY, DmtxFlipX, DmtxFlipNone};
	int flipIdx = 0;
	while(bRet == false && flipIdx < 3){
		img = dmtxImageCreate(px, w, h, DmtxPack24bppRGB);
		assert(img != NULL);
		dmtxImageSetProp(img, DmtxPropImageFlip, flipMethods[flipIdx++]);

		dec = dmtxDecodeCreate(img, 1);
		assert(dec != NULL);

		DmtxTime tm = dmtxTimeNow();
		tm.sec += 5;
		tm.usec = 0;
		reg = dmtxRegionFindNext(dec, &tm);
		if(reg != NULL) {
			msg = dmtxDecodeMatrixRegion(dec, reg, DmtxUndefined);
			if(msg != NULL) {
	#if 0
				fputs("output: \"", stdout);
				fwrite(msg->output, sizeof(unsigned char), msg->outputIdx, stdout);
				fputs("\"\n", stdout);
	#endif
				wchar_t *woutput;
				chr2wch((const char*)msg->output,&woutput);
				MzAutoMsgBoxEx(m_hWnd,woutput,2000);
				delete [] woutput;
				dmtxMessageDestroy(&msg);
				bRet = true;
			}
			dmtxRegionDestroy(&reg);
		}

		dmtxDecodeDestroy(&dec);
		dmtxImageDestroy(&img);
	}

	return bRet;
}

void Ui_CaptureWnd::decode(HBITMAP hBitmap/*640*480*/,const char *filename)
{ 
	HDC  hScrDC, hMemDC;        
	int  width, height; 

	//the pointer will save all pixel point's color value
	BYTE  *lpBitmapBits = NULL;

	//creates a device context for the screen device
	hScrDC = CreateCompatibleDC(NULL);
	SelectObject(hScrDC, hBitmap);
	//get the screen point size
	POINT pos = this->GetWindowPos();
	width = GetWidth();
	height = GetHeight();

	//creates a memory device context (DC) compatible with the screen device(hScrDC) 
	hMemDC = CreateCompatibleDC(hScrDC);

	//initialise the struct BITMAPINFO for the bimap infomation£¬
	//in order to use the function CreateDIBSection
	// on wince os, each pixel stored by 24 bits(biBitCount=24) 
	//and no compressing(biCompression=0)
	BITMAPINFO RGB24BitsBITMAPINFO;
	ZeroMemory(&RGB24BitsBITMAPINFO, sizeof(BITMAPINFO));
	RGB24BitsBITMAPINFO.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	RGB24BitsBITMAPINFO.bmiHeader.biWidth = width;
	RGB24BitsBITMAPINFO.bmiHeader.biHeight = height;
	RGB24BitsBITMAPINFO.bmiHeader.biPlanes = 1;
	RGB24BitsBITMAPINFO.bmiHeader.biBitCount = 24;

	//use the function CreateDIBSection and SelectObject 
	//in order to get the bimap pointer : lpBitmapBits
	HBITMAP directBmp = CreateDIBSection(hMemDC, (BITMAPINFO*)&RGB24BitsBITMAPINFO,
		DIB_RGB_COLORS, (void **)&lpBitmapBits, NULL, 0);
	HGDIOBJ previousObject = SelectObject(hMemDC, directBmp);

	// copy the screen dc to the memory dc
	BitBlt(hMemDC, 0, 0, width, height, hScrDC, pos.x,pos.y, SRCCOPY);

	//if you only want to get the every pixel color value,
	//you can begin here and the following part of this function will be unuseful;
	//the following part is in order to write file;

	//bimap file header in order to write bmp file
	BITMAPFILEHEADER bmBITMAPFILEHEADER;
	ZeroMemory(&bmBITMAPFILEHEADER, sizeof(BITMAPFILEHEADER));
	bmBITMAPFILEHEADER.bfType = 0x4d42;  //bmp 
	bmBITMAPFILEHEADER.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	bmBITMAPFILEHEADER.bfSize = bmBITMAPFILEHEADER.bfOffBits + ((width*height)*3); ///3=(24 / 8)

	//write into file
	if(filename){
		FILE *mStream = NULL;
		if((mStream = fopen(filename, "wb")))
		{  
			//write bitmap file header
			fwrite(&bmBITMAPFILEHEADER, sizeof(BITMAPFILEHEADER), 1, mStream);
			//write bitmap info
			fwrite(&(RGB24BitsBITMAPINFO.bmiHeader), sizeof(BITMAPINFOHEADER), 1, mStream);
			//write bitmap pixels data
			fwrite(lpBitmapBits, 3*width*height, 1, mStream);
			//close file
			fclose(mStream);
		}
	}
	DataMatrixDecode(lpBitmapBits,width,height);
	//delete
	DeleteObject(hMemDC);
	DeleteObject(hScrDC);
	DeleteObject(directBmp);
	DeleteObject(previousObject);
} 


Ui_CaptureWnd::Ui_CaptureWnd(){
	isInitialized = false;
	isPaused = false;
}

Ui_CaptureWnd::~Ui_CaptureWnd(){
	if(isInitialized){
		m_pDevice->UnInitDevice();
		m_pDevice->Release();
		m_pDevice = NULL;
	}

}

BOOL Ui_CaptureWnd::OnInitDialog() {
	// Must all the Init of parent class first!

	if (!CMzWndEx::OnInitDialog()) {
		return FALSE;
	}
	// Then init the controls & other things in the window
	if(	SUCCEEDED(CoCreateInstance(CLSID_CameraDevice, NULL,CLSCTX_INPROC_SERVER ,IID_MZ_CameraInterface,(void **)&m_pDevice))){
		if( SUCCEEDED( m_pDevice->InitDevice(m_hWnd/*m_wnd*/)) )
		{
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
			m_pDevice->SetBrightness(BRIGHTNESS_LEVEL_6);
			m_pDevice->IsDrawPreviewFrame(true);
			m_pDevice->SetMode(MODE_TYPE_AUTO);
			m_pDevice->SetEffect(EFFECT_TYPE_NORMAL);
			m_pDevice->SetPhotoName(L"/Disk/123.jpg");
			isInitialized = true;
			m_pDevice->StartPreview();
			//m_pDevice->SetPreviewAreaAlphaValue(5);
		}
	}
	return TRUE;
}


LRESULT Ui_CaptureWnd::MzDefWndProc(UINT message, WPARAM wParam, LPARAM lParam) {
	return CMzWndEx::MzDefWndProc(message, wParam, lParam);
}

bool Ui_CaptureWnd::StartDecode(){
	if(isInitialized){
		isPaused = true;
		m_pDevice->StartAF();
		DateTime::waitms(100);
		while(!m_pDevice->IsAFEnd()){
			DateTime::waitms(0);
		}
		m_pDevice->PausePreview();

		m_pDevice->TakePhoto();
		ImagingHelper m_Image;

		DateTime::waitms(1000);
		m_Image.LoadImageW(L"/Disk/123.jpg",true,false);//,true,true);

		int height = m_Image.GetImageHeight();
		int width = m_Image.GetImageWidth();
		HBITMAP hBitmap= m_Image.CreateBitmap();

		decode(hBitmap);
		m_pDevice->StartPreview();
	}
	return false;
}

void Ui_CaptureWnd::OnMzCommand(WPARAM wParam, LPARAM lParam) {
	CMzWndEx::OnMzCommand(wParam,lParam);
}

