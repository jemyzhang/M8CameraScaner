#ifndef _UI_RESULT_H
#define _UI_RESULT_H

// include the MZFC library header file
#include <mzfc_inc.h>
#define QR_MAX_ENTRIES	12

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
	BAR_CODE	=	10,		//1d条形码
    QR_PHONE    =   11,     //电话号码
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
	QR_CARD_IM			=	10,

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

	QR_BAR_TYPE			=	0,
	QR_BAR_CODE			=	1,

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
    T_BAR_CODE  =   2,
	T_UNKNOWN	=	0xff,
}BarCodeType_t;


typedef enum ResultActionCmd {
	CmdNone			=	0x0000,	//无操作
	CmdSaveText		=	0x0001,	//保存到文本
	CmdSaveContact	=	0x0002,	//保存联系人
	CmdDialNumber	=	0x0004,	//拨打电话
	CmdSendSms		=	0x0008,	//发送短信
	CmdSendMail		=	0x0010,	//发送邮件
	CmdOpenSite		=	0x0020,	//打开链接
	CmdClipBoard	=	0x0040,	//复制到剪贴板
}ResultActionCmd_t;

typedef struct QRActionMap {
	QR_t type;
	uint actioncode;
}QRActionMap_t;

typedef struct QRActionName {
	ResultActionCmd_t type;
	wchar_t* name;
}QRActionName_t;

//typedef bool( *ResultActionFunc(QRCODE_RECORD_ptr rec));
//
//typedef struct ResultActions {
//	ResultActionCmd_t cmd;
//	ResultActionFunc action;
//}ResultActions_t;

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
private:
	void setupUi();
	//根据qr码类别弹出菜单
	void popupMenu(QRCODE_RECORD_ptr pr);
	bool SaveContact(QRCODE_RECORD_ptr pr);
	bool SendSMS(LPCTSTR lpNumber,LPCTSTR lpszMessage);
    void ProcGridMenu(UINT_PTR id);
protected:
    // Initialization of the window (dialog)
    virtual BOOL OnInitDialog();

    // override the MZFC window messages handler
    LRESULT MzDefWndProc(UINT message, WPARAM wParam, LPARAM lParam);

    // override the MZFC command handler
    virtual void OnMzCommand(WPARAM wParam, LPARAM lParam);
    // 重载 OnLButtonDown 在点击主窗口时隐藏弹出窗口
    void OnLButtonDown(UINT fwKeys, int xPos, int yPos)
    {
        if (yPos < (GetHeight() - MZM_HEIGHT_TOOLBARPRO))
        {
            if (m_GridMenu.IsContinue())
            {
                m_GridMenu.EndGridMenu();
            }
        }
    }
private:	//PtAPI
	UiScrollWin m_ScrollWin;
	UiHeadingBar m_Title;
	UiStatic *m_pEntryTitles;
	UiEdit *m_pMultiLineEdit;
	UiToolBarPro m_Toolbar;
    MzGridMenu m_GridMenu;
    ImageContainer m_imgContainer;
private:
	BarCodeType_t m_type;
	QRCODE_RECORD_ptr m_pqrrecord;
};


#endif /*_UI_RESULT_H*/