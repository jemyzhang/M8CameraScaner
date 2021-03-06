#include "ui_main.h"
#include "ui_ProgressBar.h"
#include "resource.h"
#include "mz_commonfunc.h"
using namespace MZ_CommonFunc;

#define MZ_IDC_TOOLBAR_MAIN 101
#define MZ_IDC_OPTION_QR    102
#define MZ_IDC_OPTION_DM    103
#define MZ_IDC_OPTION_BAR   104


MZ_IMPLEMENT_DYNAMIC(Ui_CaptureWnd)

wchar_t* chr2wch(const char* buffer, wchar_t** wbuf)
{
    size_t len = strlen(buffer); 
    size_t wlen = MultiByteToWideChar(CP_ACP, 0, (const char*)buffer, int(len), NULL, 0); 
    wchar_t *wBuf = new wchar_t[wlen + 1]; 
    MultiByteToWideChar(CP_ACP, 0, (const char*)buffer, int(len), wBuf, int(wlen));
    wBuf[wlen] = 0;
    *wbuf = wBuf;
    return wBuf;
} 

void Ui_CaptureWnd::InitBarCodeInfo( )
{
    m_BarInfo.dwTotalCount = 0;
    for( int i=0; i<MAX_BARCODE_COUNT; i++)
    {
        m_BarInfo.InfoList[i].dwDataLen = 0;
        memset( m_BarInfo.InfoList[i].Data , 0, MAX_BARCODE_DATA_LEN);
        m_BarInfo.InfoList[i].dwX1 = 0;
        m_BarInfo.InfoList[i].dwX2 = 0;
        m_BarInfo.InfoList[i].dwX3 = 0;
        m_BarInfo.InfoList[i].dwX4 = 0;
        m_BarInfo.InfoList[i].dwY1 = 0;
        m_BarInfo.InfoList[i].dwY2 = 0;
        m_BarInfo.InfoList[i].dwY3 = 0;
        m_BarInfo.InfoList[i].dwY4 = 0;
    }
}
void Ui_CaptureWnd::ShowBarCodeInfo( PTTOTALBARCODEINFO* pBar ){
    int   i;
    uiRefreshProgressBar(L"解码完成",2,3);
    if( pBar->dwTotalCount<=0 )
    {
        MzMessageAutoBoxV2(m_hWnd,L"无法找到可用信息");
        return;
    }

    m_BarInfo.dwTotalCount = pBar->dwTotalCount;
    for( i=0; i<int(pBar->dwTotalCount); i++ )
    {
        m_BarInfo.InfoList[i].dwDataLen = pBar->pInfoList[i].dwDataLen;
        memcpy( m_BarInfo.InfoList[i].Data , pBar->pInfoList[i].pData, pBar->pInfoList[i].dwDataLen );
        m_BarInfo.InfoList[i].dwX1 =pBar->pInfoList[i].dwX1;
        m_BarInfo.InfoList[i].dwX2 =pBar->pInfoList[i].dwX2;
        m_BarInfo.InfoList[i].dwX3 =pBar->pInfoList[i].dwX3;
        m_BarInfo.InfoList[i].dwX4 =pBar->pInfoList[i].dwX4;
        m_BarInfo.InfoList[i].dwY1 =pBar->pInfoList[i].dwY1;
        m_BarInfo.InfoList[i].dwY2 =pBar->pInfoList[i].dwY2;
        m_BarInfo.InfoList[i].dwY3 =pBar->pInfoList[i].dwY3;
        m_BarInfo.InfoList[i].dwY4 =pBar->pInfoList[i].dwY4;
    }
    QRCODE_RECORD_t qrcode;
    this->qrcodeAnaysis(m_BarInfo.InfoList[0].Data,m_BarInfo.InfoList[0].dwDataLen,&qrcode);

    Ui_ResultWnd dlg;
    dlg.setDecodeType(m_bartype);
    dlg.setQRCodeRecord(&qrcode);

    RECT rcWork = MzGetWorkArea();
    dlg.Create(rcWork.left, rcWork.top, RECT_WIDTH(rcWork), RECT_HEIGHT(rcWork), m_hWnd, 0, WS_POPUP);
    dlg.DoModal();

}

bool Ui_CaptureWnd::PtApiDecoder(BarCodeType_t type){
    bool bRet = false;
    InitBarCodeInfo();
    PTTOTALBARCODEINFO CodeInfo;

	m_para.dwStartX = m_rcScanRegion.left;
	m_para.dwStartY = m_rcScanRegion.top;
	m_para.dwEndX = m_rcScanRegion.right;
	m_para.dwEndY = m_rcScanRegion.bottom;
    m_para.dwMaxCount = 0;
    if(type == T_QR_CODE){ //QR
        PtQRDecodeInit(&CodeInfo);
        if(  PtQRDecode ( &m_image, &m_para, &CodeInfo ) != PT_QRDECODE_SUCCESS ){
            MzMessageAutoBoxV2(m_hWnd,L"图像识别错误，无法找到可用信息");
        }else{
            ShowBarCodeInfo( &CodeInfo );
        }
        PtQRDecodeFree( &CodeInfo );//release the mem allocated while decoding
    }else if(type == T_DATAMATRIX_CODE){	//DM
        PtDMDecodeInit(&CodeInfo);
        if(  PtDMDecode ( &m_image, &m_para, &CodeInfo ) != PT_QRDECODE_SUCCESS ){
            MzMessageAutoBoxV2(m_hWnd,L"图像识别错误，无法找到可用信息");
        }else{
            ShowBarCodeInfo( &CodeInfo );
        }
        PtDMDecodeFree( &CodeInfo );//release the mem allocated while decoding
    }

    return bRet;
}

bool Ui_CaptureWnd::PtApiDecoder(const TCHAR* FileName, int type){
    bool bRet = false;
    InitBarCodeInfo();
    PTTOTALBARCODEINFO CodeInfo;
    if(type == 0){ //QR
        PtQRDecodeInit(&CodeInfo);
        if(  PtQRDecodeFromFile ( FileName, &m_para, &CodeInfo ) != PT_QRDECODE_SUCCESS ){
            MzMessageAutoBoxV2(m_hWnd,L"图像识别错误，无法找到可用信息");
        }else{
            ShowBarCodeInfo( &CodeInfo );
        }
        PtQRDecodeFree( &CodeInfo );//release the mem allocated while decoding
    }else if(type == 1){	//DM
        PtDMDecodeInit(&CodeInfo);
        if(  PtDMDecodeFromFile ( FileName, &m_para, &CodeInfo ) != PT_QRDECODE_SUCCESS ){
            MzMessageAutoBoxV2(m_hWnd,L"图像识别错误，无法找到可用信息");
        }else{
            ShowBarCodeInfo( &CodeInfo );
        }
        PtDMDecodeFree( &CodeInfo );//release the mem allocated while decoding
    }
    return bRet;
}
bool Ui_CaptureWnd::PtApiDecoder(HBITMAP hBitmap, int type){
    bool bRet = false;
    InitBarCodeInfo();
    PTTOTALBARCODEINFO CodeInfo;
    if(type == 0){ //QR
        PtQRDecodeInit(&CodeInfo);
        if(  PtQRDecodeFromBitmap ( hBitmap, &m_para, &CodeInfo ) != PT_QRDECODE_SUCCESS ){
            MzMessageAutoBoxV2(m_hWnd,L"图像识别错误，无法找到可用信息");
        }else{
            ShowBarCodeInfo( &CodeInfo );
        }
        PtQRDecodeFree( &CodeInfo );//release the mem allocated while decoding
    }else if(type == 1){	//DM
        PtDMDecodeInit(&CodeInfo);
        if(  PtDMDecodeFromBitmap ( hBitmap, &m_para, &CodeInfo ) != PT_QRDECODE_SUCCESS ){
            MzMessageAutoBoxV2(m_hWnd,L"图像识别错误，无法找到可用信息");
        }else{
            ShowBarCodeInfo( &CodeInfo );
        }
        PtDMDecodeFree( &CodeInfo );//release the mem allocated while decoding
    }
    return bRet;
}

Ui_CaptureWnd::Ui_CaptureWnd(){
    isInitialized = false;
    isPaused = false;
	m_source = DECODE_FROM_CAMERA;
	m_ImageFile = NULL;
	m_rcScanRegion.left = 0;
	m_rcScanRegion.right = 0;
	m_rcScanRegion.top = 0;
	m_rcScanRegion.bottom = 0;
	reqDecode = false;
    InitPtApi();
}

Ui_CaptureWnd::~Ui_CaptureWnd(){
    PtFreeImage(&m_image);
}

void Ui_CaptureWnd::PaintWin(HDC hdc, RECT* prcUpdate){
		if(m_ImageFile){
			ImagingHelper image;
			image.LoadImageW(m_ImageFile,true,true);
            if(m_source == DECODE_FROM_FILE){
    			image.Draw(hdc,&m_rcDisplay,true,false);
            }else{
				drawRotate90(hdc,&m_rcScanRegion,&image);
            }
		}

    CMzWndEx::PaintWin(hdc,prcUpdate);
}

void Ui_CaptureWnd::drawRotate90(HDC hdc,RECT *prc,ImagingHelper* pimg){
	//prc == null
	//image == null
	if(prc == NULL || pimg == NULL) return;
	//获取bitmap数据
	int width = RECT_WIDTH(*prc);
	int height = RECT_HEIGHT(*prc);
	HDC hSrcDC = pimg->GetDC();
	HDC hMemDC = ::CreateCompatibleDC(hSrcDC);
	BYTE    *lpBitmapBitsSrc = NULL;

	BITMAPINFO RGB24BitsBITMAPINFO;
	ZeroMemory(&RGB24BitsBITMAPINFO, sizeof(BITMAPINFO));
	RGB24BitsBITMAPINFO.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	RGB24BitsBITMAPINFO.bmiHeader.biWidth = width;
	RGB24BitsBITMAPINFO.bmiHeader.biHeight = height;
	RGB24BitsBITMAPINFO.bmiHeader.biPlanes = 1;
	RGB24BitsBITMAPINFO.bmiHeader.biBitCount = 24;

	HBITMAP hBitmap = ::CreateDIBSection(hMemDC, &RGB24BitsBITMAPINFO,
		DIB_RGB_COLORS, (void **)&lpBitmapBitsSrc, NULL, 0);
	::SelectObject(hMemDC, hBitmap);
	::BitBlt(hMemDC,0,0,width,height,hSrcDC,prc->left,prc->top,SRCCOPY);

	//90度翻转bitmap数据
	//lp[h*(0:w-1) + (0:h-1)] = lpold[w*(1:h) - (1:w)]
	int widthDest = height;
	int heightDest = width;
	BYTE    *lpBitmapBitsDest = new BYTE[widthDest*heightDest*3];
	for(int iw = 0; iw < width; iw++){
		for(int ih = 0; ih < height; ih++){
			int ps = (width*(ih+1) - (iw+1))*3;
			int pd = (height*iw + ih)*3;
			lpBitmapBitsDest[pd + 0] = lpBitmapBitsSrc[ps + 0];	//B
			lpBitmapBitsDest[pd + 1] = lpBitmapBitsSrc[ps + 1];	//G
			lpBitmapBitsDest[pd + 2] = lpBitmapBitsSrc[ps + 2];	//R
		}
	}

	//拷贝数据
	ZeroMemory(&RGB24BitsBITMAPINFO, sizeof(BITMAPINFO));
	RGB24BitsBITMAPINFO.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	RGB24BitsBITMAPINFO.bmiHeader.biWidth = widthDest;
	RGB24BitsBITMAPINFO.bmiHeader.biHeight = heightDest;
	RGB24BitsBITMAPINFO.bmiHeader.biPlanes = 1;
	RGB24BitsBITMAPINFO.bmiHeader.biBitCount = 24;

	::SetDIBitsToDevice(hdc,
		m_rcDisplay.left,m_rcDisplay.top,
		widthDest,heightDest,
		0,0,0,heightDest,
		lpBitmapBitsDest,
		&RGB24BitsBITMAPINFO,DIB_RGB_COLORS);

	//清理
	delete []lpBitmapBitsSrc;
	delete []lpBitmapBitsDest;
	::ReleaseDC(m_hWnd,hMemDC);
	::DeleteObject(hBitmap);
}

BOOL Ui_CaptureWnd::OnInitDialog() {
    // Must all the Init of parent class first!

    if (!CMzWndEx::OnInitDialog()) {
        return FALSE;
    }
	if(m_source == DECODE_FROM_FILE){
		m_OptionQR.SetPos(5,GetHeight() - MZM_HEIGHT_TOOLBARPRO - MZM_HEIGHT_SINGLELINE_EDIT,GetWidth()/3,MZM_HEIGHT_SINGLELINE_EDIT);
		m_OptionQR.SetStatus(true);
		m_OptionQR.SetText(L"QR CODE");
		m_OptionQR.SetID(MZ_IDC_OPTION_QR);
		AddUiWin(&m_OptionQR);

		m_OptionDM.SetPos(5,GetHeight() - MZM_HEIGHT_TOOLBARPRO - MZM_HEIGHT_SINGLELINE_EDIT*2,GetWidth()/3,MZM_HEIGHT_SINGLELINE_EDIT);
		m_OptionDM.SetStatus(false);
		m_OptionDM.SetText(L"DM CODE");
		m_OptionDM.SetID(MZ_IDC_OPTION_DM);
		AddUiWin(&m_OptionDM);

		m_OptionBAR.SetPos(5,GetHeight() - MZM_HEIGHT_TOOLBARPRO - MZM_HEIGHT_SINGLELINE_EDIT*3,GetWidth()/3,MZM_HEIGHT_SINGLELINE_EDIT);
		m_OptionBAR.SetStatus(false);
		m_OptionBAR.SetText(L"BAR CODE");
		m_OptionBAR.SetID(MZ_IDC_OPTION_BAR);
		AddUiWin(&m_OptionBAR);

		m_Toolbar.SetPos(0, GetHeight() - MZM_HEIGHT_TOOLBARPRO, GetWidth(), MZM_HEIGHT_TOOLBARPRO);
		m_Toolbar.SetID(MZ_IDC_TOOLBAR_MAIN);
		m_Toolbar.SetMiddleButton(true, true, L"解码",NULL,NULL,NULL);
		m_Toolbar.SetButton(TOOLBARPRO_LEFT_TEXTBUTTON, true, true, L"返回");
		AddUiWin(&m_Toolbar);
	}else{
		SetTimer(m_hWnd,0x1001,100,NULL);
	}
    UpdateDisplayArea();

    return TRUE;
}

void Ui_CaptureWnd::OnTimer(UINT nIDEvent){
	static int cnt = 0;
	switch(nIDEvent){
		case 0x1001:
			if(reqDecode){
				reqDecode = false;
				StartDecode();
				KillTimer(m_hWnd,0x1001);
			}
			break;
	}
}

LRESULT Ui_CaptureWnd::MzDefWndProc(UINT message, WPARAM wParam, LPARAM lParam) {
    return CMzWndEx::MzDefWndProc(message, wParam, lParam);
}

void Ui_CaptureWnd::SetImageFile(wchar_t *f){
	if(f){
		C::newstrcpy(&m_ImageFile,f);
	}
}

bool Ui_CaptureWnd::StartDecode(){
	int retry = 3;
	int bdecode = 1;
	initUiProgressBar(L"启动解码",m_hWnd);
	uiRefreshProgressBar(L"读取图像",0,3);
	while(retry--){
		if(m_bartype == T_BAR_CODE){
			DECODEPARA_t param;
			param.dwScanRegion = m_rcScanRegion;
			param.dwThrehold = 160;
			BYTE code[200];
			DWORD codelen;
			if(decoder.DecodeFromFile(m_ImageFile,code,&codelen,&param)){
				bdecode = 0;
			}else{
				bdecode = 2;
			}
			if(bdecode == 0){//解码完成
				PTTOTALBARCODEINFO bar;
				PTBARCODEINFO infolist;
				infolist.dwDataLen = codelen;
				infolist.pData = code;
				bar.dwTotalCount = 1;
				bar.pInfoList = &infolist;
				ShowBarCodeInfo( &bar );
				break;
			}
		}else{
			int frames=PtGetImageFrames( m_ImageFile );
			if(frames > 0){
				int ret = PtLoadImage(m_ImageFile,&m_image,frames - 1);
				bdecode = 2;
				if(ret == PT_IMAGERW_SUCCESS){
					bdecode = 0;
					uiRefreshProgressBar(L"开始解码",1,3);
					PtApiDecoder(m_bartype);
				}
			}
		}
		if(bdecode == 0){//解码完成
			break;
		}
		DateTime::waitms(200);
	}
	uiRefreshProgressBar(NULL,3,3);
	if(bdecode == 1){
		MzMessageAutoBoxV2(m_hWnd,L"图像数据错误");
	}else if(bdecode == 2){
		MzMessageAutoBoxV2(m_hWnd,L"图像识别错误，无法找到可用信息");
	}
	PtFreeImage(&m_image);
	if(m_source == DECODE_FROM_CAMERA){
		EndModal(ID_OK);
	}
    return false;
}

void Ui_CaptureWnd::OnMzCommand(WPARAM wParam, LPARAM lParam) {
	UINT_PTR id = LOWORD(wParam);
	switch (id) {
        case MZ_IDC_OPTION_BAR:
            {
                if(!m_OptionBAR.GetStatus()){
                    m_OptionBAR.SetStatus(true);
                    m_OptionQR.SetStatus(false);
                    m_OptionDM.SetStatus(false);
                    m_bartype = T_BAR_CODE;
                    UpdateDisplayArea();
                }
                break;
            }
        case MZ_IDC_OPTION_QR:
            {
                if(!m_OptionQR.GetStatus()){
                    m_OptionQR.SetStatus(true);
                    m_OptionBAR.SetStatus(false);
                    m_OptionDM.SetStatus(false);
                    m_bartype = T_QR_CODE;
                    UpdateDisplayArea();
                }
                break;
            }
        case MZ_IDC_OPTION_DM:
            {
                if(!m_OptionDM.GetStatus()){
                    m_OptionDM.SetStatus(true);
                    m_OptionBAR.SetStatus(false);
                    m_OptionQR.SetStatus(false);
                    m_bartype = T_DATAMATRIX_CODE;
                    UpdateDisplayArea();
                }
                break;
            }
        case MZ_IDC_TOOLBAR_MAIN:
			{
				int nIndex = lParam;
				if(nIndex == TOOLBARPRO_LEFT_TEXTBUTTON){
					EndModal(ID_CANCEL);
				}
				if(nIndex == TOOLBARPRO_MIDDLE_TEXTBUTTON){
					StartDecode();
				}
				break;
			}
	}
    CMzWndEx::OnMzCommand(wParam,lParam);
}

/////////////////////////////
/*
0x1,0x20，以冒号间隔标识和内容；以分号间隔栏目，遇到两个;;结束
名片 CARD 姓名N 职务 TIL 部门 DIV 公司 COR 地址 ADR 邮编 ZIP 移动电话 M 固定电话 TEL 传真 FAX 邮件 EM
短信 SMS 收件人 SM 内容 TXT
邮件 MAIL 收信人 TO 主题 SUB 正文 TXT
文本 DTXT 标题 SUB 文本 TXT
网站 SITE 标题 TITL 链接 URL
*/

const QRCODE_TAGS_t qrtags[] = {
    {QR_TXT,		0},
    {QR_NAMECARD,	'CARD'},
    {QR_SMS,		'SMS'},
    {QR_MAIL,		'MAIL'},
    {QR_URL,		'SITE'},
    {QR_TEXT,		'DTXT'},
    {QR_AD,         'AD'  },
    {QR_BLOG,       'BLOG'},
    {QR_GIS,        'GIS' },
    {QR_ENC,        'ENC' },
    {QR_URL,        'MARK'},    //'BOOKMARK'
    {QR_URL,        'BM'},    //'BOOKMARK'
	{BAR_CODE,		'LBAR'},	//Special
    {QR_PHONE,     'HONE'},
};

const QRCODE_CONTENT_TAGS_t qrPhonetags[] = {
    {QR_CARD_NAME		,'SUB'},
    {QR_CARD_TEL		,'TEL'},
    {QR_CARD_MOBILE		,'M'},
    {QR_ENTRY_UNKNOWN	,0	},
};

const QRCODE_CONTENT_TAGS_t qrBartags[] = {
    {QR_BAR_TYPE		,'T'},
    {QR_BAR_CODE		,'CODE'},
    {QR_ENTRY_UNKNOWN	,0	},
};

const QRCODE_CONTENT_TAGS_t qrCardtags[] = {
    {QR_CARD_NAME		,'N'},
    {QR_CARD_TITLE		,'TIL'},
    {QR_CARD_DIVISION	,'DIV'},
    {QR_CARD_COMPANY	,'COR'},
    {QR_CARD_ADDRESS	,'ADR'},
    {QR_CARD_ZIPCODE	,'ZIP'},
    {QR_CARD_MOBILE		,'M'},
    {QR_CARD_TEL		,'TEL'},
    {QR_CARD_FAX		,'FAX'},
    {QR_CARD_EMAIL		,'EM'},
    {QR_CARD_IM			,'IM'},
    {QR_ENTRY_UNKNOWN	,0	},
};
const QRCODE_CONTENT_TAGS_t qrSmstags[] = {
    {QR_SMS_RECEIVER	,'SM'},
    {QR_SMS_CONTENT		,'TXT'},
    {QR_ENTRY_UNKNOWN	,0	},
};
const QRCODE_CONTENT_TAGS_t qrMailtags[] = {
    {QR_MAIL_RECEIVER	,'TO'},
    {QR_MAIL_SUBJECT	,'SUB'},
    {QR_MAIL_CONTENT	,'TXT'},
    {QR_ENTRY_UNKNOWN	,0	},
};
const QRCODE_CONTENT_TAGS_t qrTexttags[] = {
    {QR_TEXT_SUBJECT	,'SUB'},
    {QR_TEXT_CONTENT	,'TXT'},
    {QR_ENTRY_UNKNOWN	,0	},
};
const QRCODE_CONTENT_TAGS_t qrSitetags[] = {
    {QR_SITE_TITLE		,'TITL'},
    {QR_SITE_URL		,'URL'},
    {QR_ENTRY_UNKNOWN	,0	},
};

const QRCODE_CONTENT_TAGS_t qrAdtags[] = {
    {QR_AD_SUB		    ,'SUB'},
    {QR_AD_URL	    	,'URL'},
    {QR_AD_CT	    	,'CT'},
    {QR_ENTRY_UNKNOWN	,0	},
};

const QRCODE_CONTENT_TAGS_t qrBlogtags[] = {
    {QR_BLOG_SUB		    ,'SUB'},
    {QR_BLOG_URL	    	,'URL'},
    {QR_ENTRY_UNKNOWN	,0	},
};

const QRCODE_CONTENT_TAGS_t qrGistags[] = {
    {QR_GIS_URL 		    ,'URL'},
    {QR_ENTRY_UNKNOWN	,0	},
};

const QRCODE_CONTENT_TAGS_t qrEnctags[] = {
    {QR_ENC_TXT 		    ,'TXT'},
    {QR_ENC_PWD 		    ,'PWD'},
    {QR_ENTRY_UNKNOWN	,0	},
};

const QRCODE_CONTENT_TAGS_t* Ui_CaptureWnd::qrcodeDecideEntryTagGroup(QR_t t){
    const QRCODE_CONTENT_TAGS_t* entrytags = NULL;
    switch(t){
            case QR_NAMECARD:
                entrytags = &qrCardtags[0];
                break;
            case QR_SMS:
                entrytags = qrSmstags;
                break;
            case QR_MAIL:
                entrytags = qrMailtags;
                break;
            case QR_URL:
                entrytags = qrSitetags;
                break;
            case QR_TEXT:
                entrytags = qrTexttags;
                break;
            case QR_AD:
                entrytags = qrAdtags;
                break;
            case QR_BLOG:
                entrytags = qrBlogtags;
                break;
            case QR_GIS:
                entrytags = qrGistags;
                break;
            case QR_ENC:
                entrytags = qrEnctags;
                break;
			case BAR_CODE:
				entrytags = qrBartags;
				break;
            case QR_PHONE:
                entrytags = qrPhonetags;
                break;
    }
    return entrytags;
}

void Ui_CaptureWnd::qrcodeAnaysis(const unsigned char* pcode,DWORD nsize,QRCODE_RECORD_ptr pdecoded){
    if(pcode == NULL || pdecoded == NULL || nsize == 0){
        return;
    }
    const unsigned char* p = pcode;
    if(*p == 0x1 && (*(p+1) == 0x20 || *(p+1) == 0x10)){
        p += 2;
        /////get first tag
        DWORD tag = 0;
        while(*p != ':' && p < (pcode + nsize)){	
            tag = (tag << 8)&0xffffff00 | *p;
            p++;
        }
        for(int i = 0 ; i < sizeof(qrtags)/sizeof(qrtags[0]); i++){
            if(tag == qrtags[i].tag){
                pdecoded->type = qrtags[i].type;
                break;
            }
        }
        if(pdecoded->type == QR_UNKNOWN){
            return;
        }
        const QRCODE_CONTENT_TAGS_t* 
            entrytags = qrcodeDecideEntryTagGroup(pdecoded->type);
        if(entrytags == NULL) return;
        p++;	//略过':'
        do{
            /////get entry tag
            tag = 0;
            while(*p != ':' && p < (pcode + nsize)){	
                tag = (tag << 8)&0xffffff00 | *p;
                p++;
            }
            int i = 0;
			if(pdecoded->entries[pdecoded->nEntry] == NULL){
				pdecoded->entries[pdecoded->nEntry] = new QRCODE_ENTRY_t;
			}
			while(entrytags[i].type != QR_ENTRY_UNKNOWN){
                if(tag == entrytags[i].tag){
                    pdecoded->entries[pdecoded->nEntry]->type = entrytags[i].type;
                    break;
                }
                i++;
            }
			unsigned char tmpcontent[MAX_BARCODE_DATA_LEN];
			DWORD ncontent = 0;
			p++;	//略过':'
			while(*p != ';' && p < (pcode + nsize)){	
				tmpcontent[ncontent++] = *p;
				p++;
			}
			tmpcontent[ncontent] = 0;	//字符串结束
			if(ncontent){
				::chr2wch((const char*)tmpcontent,&pdecoded->entries[pdecoded->nEntry]->content);
				pdecoded->nEntry ++;	//Entry数量增加
			}
            p++;	//略过';'
        }while(*p != ';' && p < (pcode + nsize));
    }else{	//纯文本
        pdecoded->nEntry = 1;
        pdecoded->type = QR_TXT;
        pdecoded->entries[0] = new QRCODE_ENTRY_t;
        pdecoded->entries[0]->type = QR_TXT_TEXT;
        ::chr2wch((const char*)p,&pdecoded->entries[0]->content);
    }
}
