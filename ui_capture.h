#ifndef _UI_CAPTURE_H
#define _UI_CAPTURE_H

// include the MZFC library header file
#include <mzfc_inc.h>

#include "PtAPICE.h"
#include "BarRecog.h"
#include "ui_result.h"

#define MAX_BARCODE_DATA_LEN 8000
#define MAX_BARCODE_COUNT    8

typedef enum tagDecodeSource{
	DECODE_FROM_CAMERA	=	0,
	DECODE_FROM_FILE	=	1,
}DecodeSource_t;

struct INTRABARCODEINFO
{
    DWORD      dwX1, dwY1;    //Four corners' coordinates in pixels of the barcode. 
    DWORD      dwX2, dwY2;
    DWORD      dwX3, dwY3;
    DWORD      dwX4, dwY4;
    BYTE       Data[MAX_BARCODE_DATA_LEN];         //Pointer to the buffer that contains the barcode's data.
    DWORD      dwDataLen;     //The barcode data's length in bytes.
};

struct INTRATOTALBARCODEINFO
{
    INTRABARCODEINFO  InfoList[MAX_BARCODE_COUNT];    //Pointer to the start address of the list of barcodes' info.  
    DWORD             dwTotalCount; //The number of barcode that have been decoded.
};


class UiOption : public UiButton
{
public:
    UiOption(void){ SetButtonType(MZC_BUTTON_NONE);}
	~UiOption(void) {}
	void SetStatus(bool b) { bSelected = b; }
	bool GetStatus() { return bSelected; }
protected:
	virtual void PaintWin(HDC hdcDst, RECT* prcWin, RECT* prcUpdate){
        SetBkMode(hdcDst,TRANSPARENT);
        RECT rcStatus = {prcWin->left + 5,prcWin->top,prcWin->left + 30, prcWin->bottom};
		MzDrawControl(hdcDst, &rcStatus,  bSelected ? MZC_SELECTED : MZC_UNSELECTED, 0);
        RECT rcText = {rcStatus.right + 10, prcWin->top, prcWin->right, prcWin->bottom};
        MzDrawText(hdcDst,GetText(),&rcText,DT_LEFT|DT_VCENTER);
	}
private:
	bool bSelected;
};
// Main window derived from CMzWndEx
class Ui_CaptureWnd : public CMzWndEx {
    MZ_DECLARE_DYNAMIC(Ui_CaptureWnd);
public:
	Ui_CaptureWnd();
	~Ui_CaptureWnd();
public:
	void setDecodeSource(DecodeSource_t src){
		m_source = src;
	}
	void SetImageFile(wchar_t* f);
	void SetScanRegion(RECT rc){
		m_rcScanRegion = rc;
	}
    void SetDecodeType(BarCodeType_t t){
        m_bartype = t;
    }
    void UpdateDisplayArea(){
        int width = RECT_WIDTH(m_rcScanRegion);
        int height = RECT_HEIGHT(m_rcScanRegion);
        if(width == 0 || height == 0){
	        RECT rcSquare = {(GetWidth() - 320)/2,(GetHeight() - 320)/2 - 100,
		        (GetWidth() - 320)/2 + 320,(GetHeight() - 320)/2 - 100 + 320};
	        RECT rcRect = {(GetWidth() - 400)/2,(GetHeight() - 200)/2 - 100,
		        (GetWidth() - 400)/2 + 400,(GetHeight() - 200)/2 - 100 + 200};
	        if(m_bartype == T_BAR_CODE){
		        m_rcDisplay = rcRect;
	        }else{
		        m_rcDisplay = rcSquare;
	        }
        }else{
            m_rcDisplay.left = (GetWidth() - height)>>1;
            m_rcDisplay.top = (GetHeight() - 80 - width)>>1;
            m_rcDisplay.right = m_rcDisplay.left + height;
            m_rcDisplay.bottom = m_rcDisplay.top + width;
        }
        this->Invalidate();
        this->UpdateWindow();
    }
protected:
    bool StartDecode();
	void drawRotate90(HDC hdc,RECT *prc,ImagingHelper* image);
public:
	void PaintWin(HDC hdc, RECT* prcUpdate = NULL);
	virtual int DoModal(){
		if(m_source == DECODE_FROM_CAMERA){
			reqDecode = true;
		}
		return CMzWndEx::DoModal();
	}
	void OnTimer(UINT nIDEvent);
protected:
    // Initialization of the window (dialog)
    virtual BOOL OnInitDialog();

    // override the MZFC window messages handler
    LRESULT MzDefWndProc(UINT message, WPARAM wParam, LPARAM lParam);

    // override the MZFC command handler
    virtual void OnMzCommand(WPARAM wParam, LPARAM lParam);
private:	//PtAPI
	void InitPtApi(){
		int ret = PtQRDecodeRegister("18210708290105166561");//demo
		ret = PtDMDecodeRegister("18310708290105222081");//demo
		InitBarCodeInfo();
		PtInitImage(&m_image);
		memset( &m_para, 0 ,sizeof(m_para) );
	}
	bool PtApiDecoder(BarCodeType_t type);
	bool PtApiDecoder(const TCHAR* FileName, int type);
	bool PtApiDecoder(HBITMAP hBitmap, int type);
	// Operations
	void ShowBarCodeInfo( PTTOTALBARCODEINFO* pBar );
	void InitBarCodeInfo( );
private:
	const QRCODE_CONTENT_TAGS_t* Ui_CaptureWnd::qrcodeDecideEntryTagGroup(QR_t t);
	void qrcodeAnaysis(const unsigned char* pcode,DWORD nsize,QRCODE_RECORD_ptr pdecoded);
private:
	// Attributes
   INTRATOTALBARCODEINFO     m_BarInfo;
   PTIMAGE                   m_image;
   PTDECODEPARA              m_para;
private:
	bool isInitialized;
    bool isPaused;
	RECT m_rcScanRegion;
    RECT m_rcDisplay;
	UiToolbar_Text m_Toolbar;
    UiOption m_OptionQR;
    UiOption m_OptionDM;
    UiOption m_OptionBAR;
    BarCodeType_t m_bartype;
    MzBarDecoder decoder;
	DecodeSource_t m_source;
	wchar_t* m_ImageFile;
	bool reqDecode;
};

#endif /*_UI_CAPTURE_H*/