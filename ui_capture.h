#ifndef _UI_CAPTURE_H
#define _UI_CAPTURE_H

// include the MZFC library header file
#include <mzfc_inc.h>
#include <ICameraApp.h> 
#include <ICameraDeviceInterface.h>

#include "PtAPICE.h"
#include "BarRecog.h"

#define MAX_BARCODE_DATA_LEN 8000
#define MAX_BARCODE_COUNT    8

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


#define QR_MAX_ENTRIES	10

typedef enum QR_TYPE {
	QR_TXT		=	0,		//纯文本
	QR_NAMECARD	=	1,		//名片
	QR_SMS		=	2,		//短信
	QR_MAIL		=	3,		//邮件
	QR_URL		=	4,		//网站链接
	QR_TEXT		=	5,		//带标题的文本
    QR_AD       =   6,      //非标准 WAP或电话呼叫
    QR_BLOG     =   7,      //...Blog
    QR_GIS      =   8,      //地图
    QR_ENC      =   9,      //加密文字
	QR_UNKNOWN	=	0xff,	//未知
}QR_t;

typedef enum QR_ENTRY_TYPE {
	QR_CARD_NAME		=	0,
	QR_CARD_TITLE		=	1,
	QR_CARD_DIVISION	=	2,
	QR_CARD_COMPANY		=	3,
	QR_CARD_ADDRESS		=	4,
	QR_CARD_ZIPCODE		=	5,
	QR_CARD_MOBILE		=	6,
	QR_CARD_TEL			=	7,
	QR_CARD_FAX			=	8,
	QR_CARD_EMAIL		=	9,

	QR_SMS_RECEIVER		=	0,
	QR_SMS_CONTENT		=	1,

	QR_MAIL_RECEIVER	=	0,
	QR_MAIL_SUBJECT		=	1,
	QR_MAIL_CONTENT		=	2,

	QR_TEXT_SUBJECT		=	0,
	QR_TEXT_CONTENT		=	1,

	QR_SITE_TITLE		=	0,
	QR_SITE_URL			=	1,

	QR_TXT_TEXT			=	0,

    QR_AD_SUB           =   0,
    QR_AD_URL           =   1,
    QR_AD_CT            =   2,

    QR_BLOG_SUB         =   0,
    QR_BLOG_URL         =   1,

    QR_GIS_URL          =   0,

    QR_ENC_TXT          =   0,
    QR_ENC_PWD          =   1,

	QR_ENTRY_UNKNOWN	=	0xff
}QR_ENTRY_t;

typedef struct QRCODE_TAGS {
	QR_t type;
	DWORD	tag;
}QRCODE_TAGS_t,*QRCODE_TAGS_ptr;

typedef struct QRCODE_CONTENT_TAGS {
	QR_ENTRY_t type;
	DWORD	tag;
}QRCODE_CONTENT_TAGS_t,*QRCODE_CONTENT_TAGS_ptr;

typedef struct QRCODE_ENTRY {
	QR_ENTRY_t	type;
	wchar_t*	content;
	QRCODE_ENTRY(){
		type = QR_ENTRY_UNKNOWN;
		content = NULL;
	}
	void Reset(){
		type = QR_ENTRY_UNKNOWN;
		if(content) delete [] content;
		content = NULL;
	}
	~QRCODE_ENTRY(){
		Reset();
	}
}QRCODE_ENTRY_t,*QRCODE_ENTRY_ptr;

typedef struct QRCODE_RECORD {
	QR_t type;
	QRCODE_ENTRY_ptr entries[QR_MAX_ENTRIES];
	DWORD	nEntry;
	QRCODE_RECORD(){
		type = QR_UNKNOWN;
		nEntry = 0;
		for(int i = 0; i < QR_MAX_ENTRIES; i++){
			entries[i] = NULL;
		}
	}
	void Reset(){
		type = QR_UNKNOWN;
		for(DWORD i = 0; i < nEntry; i++){
			delete entries[i];
			entries[i] = 0;
		}
		nEntry = 0;
	}
	~QRCODE_RECORD(){
		Reset();
	}
}QRCODE_RECORD_t, *QRCODE_RECORD_ptr;

typedef struct QRCODE_NAMES{
	QR_t type;
	wchar_t* tname;
	WORD	esize;
	wchar_t* enames[QR_MAX_ENTRIES];
}QRCODE_NAMES_t;

typedef enum BarCodeType{
	T_QR_CODE	=	0,
	T_DATAMATRIX_CODE	=	1,
    T_BAR_CODE  =   3,
	T_UNKNOWN	=	0xff,
}BarCodeType_t;

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
        RECT rcText = {rcStatus.right + 5, prcWin->top, prcWin->right, prcWin->bottom};
        MzDrawText(hdcDst,GetText(),&rcText,GetDrawTextFormat());
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

    bool StartDecode();
	void adjustCameraPos(){
        RECT rcCameraSquare = {(GetWidth() - 240)/2,(GetHeight() - 240 - MZM_HEIGHT_TEXT_TOOLBAR_w720)/2,
						(GetWidth() - 240)/2 + 240,(GetHeight() - 240 - MZM_HEIGHT_TEXT_TOOLBAR_w720)/2 + 240};
        RECT rcCameraRect = {(GetWidth() - 320)/2,(GetHeight() - 240 - MZM_HEIGHT_TEXT_TOOLBAR_w720)/2,
						(GetWidth() - 320)/2 + 320,(GetHeight() - 240 - MZM_HEIGHT_TEXT_TOOLBAR_w720)/2 + 240};
        if(m_bartype == T_BAR_CODE){
            m_rcCamera = rcCameraRect;
        }else{
            m_rcCamera = rcCameraSquare;
        }
        this->Invalidate();
        this->UpdateWindow();
	}
	void PaintWin(HDC hdc, RECT* prcUpdate = NULL);
public:
	ICameraDeviceInterface *m_pDevice;
protected:
    // Initialization of the window (dialog)
    virtual BOOL OnInitDialog();

    // override the MZFC window messages handler
    LRESULT MzDefWndProc(UINT message, WPARAM wParam, LPARAM lParam);

    // override the MZFC command handler
    virtual void OnMzCommand(WPARAM wParam, LPARAM lParam);
private:	//PtAPI
	void InitPtApi(){
		int ret = PtQRDecodeRegister    ("18210708290105166561");//demo
		ret = PtDMDecodeRegister    ("18310708290105222081");//demo
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
//   float                     m_scale;
private:
	bool isInitialized;
    bool isPaused;
	RECT m_rcCamera;
	UiToolbar_Text m_Toolbar;
    UiOption m_OptionQR;
    UiOption m_OptionDM;
    UiOption m_OptionBAR;
    BarCodeType_t m_bartype;
    MzBarDecoder decoder;
};

// Main window derived from CMzWndEx
class Ui_ResultWnd : public CMzWndEx {
    MZ_DECLARE_DYNAMIC(Ui_ResultWnd);
public:
	Ui_ResultWnd(){
		m_type = T_QR_CODE;
		m_pqrrecord = NULL;
		m_pEntryTitles = NULL;
		m_pMultiLineEdit = NULL;
	}
	~Ui_ResultWnd();
public:
	void setDecodeType(BarCodeType_t t){
		m_type = t;
	}
	void setQRCodeRecord(QRCODE_RECORD_ptr pr){
		m_pqrrecord = pr;
	}
	void setupUi();
protected:
    // Initialization of the window (dialog)
    virtual BOOL OnInitDialog();

    // override the MZFC window messages handler
    //LRESULT MzDefWndProc(UINT message, WPARAM wParam, LPARAM lParam);

    // override the MZFC command handler
    virtual void OnMzCommand(WPARAM wParam, LPARAM lParam);
private:	//PtAPI
	UiScrollWin m_ScrollWin;
	UiCaption m_Title;
	UiStatic *m_pEntryTitles;
	UiMultiLineEdit *m_pMultiLineEdit;
	UiToolbar_Text m_Toolbar;
private:
	BarCodeType_t m_type;
	QRCODE_RECORD_ptr m_pqrrecord;
};


#endif /*_UI_CAPTURE_H*/