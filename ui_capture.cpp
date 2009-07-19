#include "ui_main.h"
#include "ui_ProgressBar.h"
#include "resource.h"
#include "mz_commonfunc.h"
using namespace MZ_CommonFunc;

#include <InitGuid.h>
#include <ICamera_GUID.h>


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
    int  count, i;
    wchar_t ch[200];
    uiRefreshProgressBar(L"解码完成",2,3);
    if( pBar->dwTotalCount<=0 )
    {
        MzAutoMsgBoxEx(m_hWnd,L"No barcodes were found",2000);
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

    //RotateScreen(1);    //竖屏
    Ui_ResultWnd dlg;
    dlg.setDecodeType(m_bartype);
    dlg.setQRCodeRecord(&qrcode);

    RECT rcWork = MzGetWorkArea();
    dlg.Create(rcWork.left, rcWork.top, RECT_WIDTH(rcWork), RECT_HEIGHT(rcWork), m_hWnd, 0, WS_POPUP);
    dlg.DoModal();
    //RotateScreen(0);    //横屏

}

bool Ui_CaptureWnd::PtApiDecoder(BarCodeType_t type){
    bool bRet = false;
    InitBarCodeInfo();
    PTTOTALBARCODEINFO CodeInfo;
    POINT pos = this->GetWindowPos();
    //int width = GetWidth();
    //int height = GetHeight();

    m_para.dwStartX = pos.x + m_rcCamera.left;//pos.x;
    m_para.dwStartY = pos.y + m_rcCamera.top;//pos.y;
    m_para.dwEndX = pos.x + m_rcCamera.right;//pos.x + width;
    m_para.dwEndY = pos.y + m_rcCamera.bottom;//pos.y + height;
    m_para.dwMaxCount = 0;
    if(type == T_QR_CODE){ //QR
        PtQRDecodeInit(&CodeInfo);
        if(  PtQRDecode ( &m_image, &m_para, &CodeInfo ) != PT_QRDECODE_SUCCESS ){
            MzAutoMsgBoxEx(m_hWnd,L"An error occured while recognition",2000);
        }else{
            ShowBarCodeInfo( &CodeInfo );
        }
        PtQRDecodeFree( &CodeInfo );//release the mem allocated while decoding
    }else if(type == T_DATAMATRIX_CODE){	//DM
        PtDMDecodeInit(&CodeInfo);
        if(  PtDMDecode ( &m_image, &m_para, &CodeInfo ) != PT_QRDECODE_SUCCESS ){
            MzAutoMsgBoxEx(m_hWnd,L"An error occured while recognition",2000);
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
            MzAutoMsgBoxEx(m_hWnd,L"An error occured while recognition",2000);
        }else{
            ShowBarCodeInfo( &CodeInfo );
        }
        PtQRDecodeFree( &CodeInfo );//release the mem allocated while decoding
    }else if(type == 1){	//DM
        PtDMDecodeInit(&CodeInfo);
        if(  PtDMDecodeFromFile ( FileName, &m_para, &CodeInfo ) != PT_QRDECODE_SUCCESS ){
            MzAutoMsgBoxEx(m_hWnd,L"An error occured while recognition",2000);
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
            MzAutoMsgBoxEx(m_hWnd,L"An error occured while recognition",2000);
        }else{
            ShowBarCodeInfo( &CodeInfo );
        }
        PtQRDecodeFree( &CodeInfo );//release the mem allocated while decoding
    }else if(type == 1){	//DM
        PtDMDecodeInit(&CodeInfo);
        if(  PtDMDecodeFromBitmap ( hBitmap, &m_para, &CodeInfo ) != PT_QRDECODE_SUCCESS ){
            MzAutoMsgBoxEx(m_hWnd,L"An error occured while recognition",2000);
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
    InitPtApi();
}

Ui_CaptureWnd::~Ui_CaptureWnd(){
    if(isInitialized){
        m_pDevice->UnInitDevice();
        m_pDevice->Release();
        m_pDevice = NULL;
    }
    PtFreeImage(&m_image);
}

void Ui_CaptureWnd::PaintWin(HDC hdc, RECT* prcUpdate){
    RotateScreen(0);    //保持横屏
    HBRUSH myBrush = CreateSolidBrush(RGB(16,0,16));
#if 0
    RECT rc = {0,0,720,480};
    FillRect(hdc,&rc,myBrush);
#else
    FillRect(hdc,&m_rcCamera,myBrush);
#endif
    CMzWndEx::PaintWin(hdc,prcUpdate);
}

BOOL Ui_CaptureWnd::OnInitDialog() {
    // Must all the Init of parent class first!

    if (!CMzWndEx::OnInitDialog()) {
        return FALSE;
    }

    m_OptionQR.SetPos(5,GetHeight() - MZM_HEIGHT_TEXT_TOOLBAR_w720 - MZM_HEIGHT_SINGLELINE_EDIT,GetWidth()/3,MZM_HEIGHT_SINGLELINE_EDIT);
    m_OptionQR.SetStatus(true);
    m_OptionQR.SetText(L"QR CODE");
    m_OptionQR.SetID(MZ_IDC_OPTION_QR);
    AddUiWin(&m_OptionQR);

    m_OptionDM.SetPos(5,GetHeight() - MZM_HEIGHT_TEXT_TOOLBAR_w720 - MZM_HEIGHT_SINGLELINE_EDIT*2,GetWidth()/3,MZM_HEIGHT_SINGLELINE_EDIT);
    m_OptionDM.SetStatus(false);
    m_OptionDM.SetText(L"DM CODE");
    m_OptionDM.SetID(MZ_IDC_OPTION_DM);
    AddUiWin(&m_OptionDM);

    m_OptionBAR.SetPos(5,GetHeight() - MZM_HEIGHT_TEXT_TOOLBAR_w720 - MZM_HEIGHT_SINGLELINE_EDIT*3,GetWidth()/3,MZM_HEIGHT_SINGLELINE_EDIT);
    m_OptionBAR.SetStatus(false);
    m_OptionBAR.SetText(L"BAR CODE");
    m_OptionBAR.SetID(MZ_IDC_OPTION_BAR);
    AddUiWin(&m_OptionBAR);

    m_Toolbar.SetPos(0, GetHeight() - MZM_HEIGHT_TEXT_TOOLBAR_w720, GetWidth(), MZM_HEIGHT_TEXT_TOOLBAR_w720);
    m_Toolbar.SetID(MZ_IDC_TOOLBAR_MAIN);
    m_Toolbar.SetTextBarType(TEXT_TOOLBAR_TYPE_720);
    m_Toolbar.SetButton(0, true, true, L"对焦");
    m_Toolbar.SetButton(1, true, true, L"解码");
    m_Toolbar.SetButton(2, true, true, L"关闭");
    AddUiWin(&m_Toolbar);
    adjustCameraPos();

    initUiProgressBar(L"启动摄像头",m_hWnd);
    uiRefreshProgressBar(L"准备摄像头",0,3);
    // Then init the controls & other things in the window
    if(	SUCCEEDED(CoCreateInstance(CLSID_CameraDevice, NULL,CLSCTX_INPROC_SERVER ,IID_MZ_CameraInterface,(void **)&m_pDevice))){
        uiRefreshProgressBar(L"初始化摄像头设备",1,3);
        if( SUCCEEDED( m_pDevice->InitDevice(m_hWnd/*m_wnd*/)) )
        {
            uiRefreshProgressBar(L"设置摄像头参数",2,3);
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
            m_pDevice->SetPhotoName(L"/cap.jpg");
            isInitialized = true;
            m_pDevice->StartPreview();
            //m_pDevice->SetPreviewAreaAlphaValue(5);
        }
    }
    uiRefreshProgressBar(NULL,3,3);

    return TRUE;
}


LRESULT Ui_CaptureWnd::MzDefWndProc(UINT message, WPARAM wParam, LPARAM lParam) {
    return CMzWndEx::MzDefWndProc(message, wParam, lParam);
}

bool Ui_CaptureWnd::StartDecode(){
    if(isInitialized){
        while(!m_pDevice->IsAFEnd());
        m_pDevice->PausePreview();

        initUiProgressBar(L"启动解码",m_hWnd);
        uiRefreshProgressBar(L"获取图像",0,3);
        m_pDevice->TakePhoto();
        ImagingHelper m_Image;

        DateTime::waitms(1000);
        int retry = 3;
        int bdecode = 1;
        while(retry--){
            if(m_bartype == T_BAR_CODE){
                POINT pos = this->GetWindowPos();

                DECODEPARA_t param;
                param.dwScanRegion.left = pos.x + m_rcCamera.left;
                param.dwScanRegion.top = pos.y + m_rcCamera.top;
                param.dwScanRegion.right = pos.x + m_rcCamera.right;
                param.dwScanRegion.bottom = pos.y + m_rcCamera.bottom;
                param.dwThrehold = 160;
                if(decoder.DecodeFromFile(L"/cap.jpg",&param)){
                    bdecode = 0;
                }
                if(bdecode == 0){//解码完成
                    break;
                }
            }else{
                int frames=PtGetImageFrames( L"/cap.jpg" );
                if(frames > 0){
                    int ret = PtLoadImage(L"/cap.jpg",&m_image,frames - 1);
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
        if(bdecode){
            wchar_t errmsg[128];
            wsprintf(errmsg,L"图像获取失败:%d",bdecode);
            MzAutoMsgBoxEx(m_hWnd,errmsg,2000);
        }
        PtFreeImage(&m_image);
        m_pDevice->StartPreview();
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
                    adjustCameraPos();
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
                    adjustCameraPos();
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
                    adjustCameraPos();
                }
                break;
            }
        case MZ_IDC_TOOLBAR_MAIN:
            {
                int nIndex = lParam;
                if(nIndex == 0){
                    m_pDevice->StartAF();
                }
                if(nIndex == 1){
                    StartDecode();
                }
                if(nIndex == 2){
                    //this->Show(false);
                    EndModal(ID_OK);
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

const QRCODE_NAMES_t qrCodeNames[] = {
    {
        QR_NAMECARD,
            L"名片",
            10,
        {
            L"姓名",L"职务",L"部门",L"公司",L"地址",
                L"邮编",L"移动电话",L"固定电话",L"传真",L"电子邮件"
        }
    },
    {
        QR_SMS,
            L"短信",
            2,
        {
            L"收信人",L"短信内容"
        }
    },
    {
        QR_MAIL,
            L"电子邮件",
            3,
        {
            L"收件人",L"主题",L"正文"
        }
    },
    {
        QR_URL,
            L"网址",
            2,
        {
            L"站点名称",L"链接地址"
        }
    },
    {
        QR_TEXT,
            L"文本",
            2,
        {
            L"标题",L"内容"
        }
    },
    {
        QR_TXT,
            L"普通文本",
            1,
        {
            L"内容"
        }
    },
    {
        QR_AD,
            L"链接或电话",
            3,
        {
            L"类型",
            L"WAP链接",
            L"呼叫电话"
        }
    },
    {
        QR_BLOG,
            L"博客",
            2,
        {
            L"名称",
            L"链接地址"
        }
    },
    {
        QR_GIS,
            L"地图",
            1,
        {
            L"链接地址"
        }
    },
    {
        QR_ENC,
            L"加密文字",
            2,
        {
            L"文本",
            L"密码"
        }
    },
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
        p++;	//略过':'
        do{
            /////get entry tag
            tag = 0;
            while(*p != ':' && p < (pcode + nsize)){	
                tag = (tag << 8)&0xffffff00 | *p;
                p++;
            }
            const QRCODE_CONTENT_TAGS_t* 
                entrytags = qrcodeDecideEntryTagGroup(pdecoded->type);
            if(entrytags == NULL) return;
            int i = 0;
            while(entrytags[i].tag != QR_ENTRY_UNKNOWN){
                if(tag == entrytags[i].tag){
                    if(pdecoded->entries[pdecoded->nEntry] == NULL){
                        pdecoded->entries[pdecoded->nEntry] = new QRCODE_ENTRY_t;
                    }
                    pdecoded->entries[pdecoded->nEntry]->type = entrytags[i].type;
                    break;
                }
                i++;
            }
            /////get entry content
            if(pdecoded->entries[pdecoded->nEntry]->type != QR_ENTRY_UNKNOWN){
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


//////////////////////////////////////////////
MZ_IMPLEMENT_DYNAMIC(Ui_ResultWnd)
Ui_ResultWnd::~Ui_ResultWnd(){
    if(m_pMultiLineEdit) delete [] m_pMultiLineEdit;
    if(m_pEntryTitles) delete [] m_pEntryTitles;
}

BOOL Ui_ResultWnd::OnInitDialog() {
    // Must all the Init of parent class first!

    if (!CMzWndEx::OnInitDialog()) {
        return FALSE;
    }
    int y = 0;
    m_Title.SetPos(0,y,GetWidth(),MZM_HEIGHT_CAPTION);
    AddUiWin(&m_Title);

    y+=MZM_HEIGHT_CAPTION;
    m_ScrollWin.SetPos(0,y,GetWidth(),GetHeight() - y - MZM_HEIGHT_TEXT_TOOLBAR_w720);
    m_ScrollWin.EnableScrollBarV(true);
    AddUiWin(&m_ScrollWin);

    m_Toolbar.SetPos(0, GetHeight() - MZM_HEIGHT_TEXT_TOOLBAR_w720, GetWidth(), MZM_HEIGHT_TEXT_TOOLBAR_w720);
    m_Toolbar.SetID(MZ_IDC_TOOLBAR_MAIN);
    m_Toolbar.SetTextBarType(TEXT_TOOLBAR_TYPE_720);
    m_Toolbar.SetButton(1, true, true, L"OK");
    AddUiWin(&m_Toolbar);

    setupUi();
    return TRUE;
}
void Ui_ResultWnd::setupUi(){
    if(m_type == T_QR_CODE){
        m_Title.SetText(L"QR Code");
    }else if(m_type == T_DATAMATRIX_CODE){
        m_Title.SetText(L"DM Code");
    }else if(m_type == T_BAR_CODE){
        m_Title.SetText(L"BAR Code");
    }
    m_Title.Invalidate();
    m_Title.Update();
    if(m_pqrrecord == NULL) return;
    if(m_pqrrecord->nEntry == 0) return;
    int nameidx = 0;
    for(int i = 0; i < sizeof(qrCodeNames)/sizeof(qrCodeNames[0]); i++){
        if(m_pqrrecord->type == qrCodeNames[i].type){
            nameidx = i;
            break;
        }
    }
    CMzString stitle;
    if(m_type == T_QR_CODE){
        stitle = L"QR Code[";
    }else if(m_type == T_DATAMATRIX_CODE){
        stitle = L"DM Code[";
    }else if(m_type == T_BAR_CODE){
        stitle = L"BAR Code[";
    }
    stitle = stitle + qrCodeNames[nameidx].tname;
    stitle = stitle + L"]";
    m_Title.SetText(stitle.C_Str());

    m_pEntryTitles = new UiStatic[m_pqrrecord->nEntry];
    m_pMultiLineEdit = new UiMultiLineEdit[m_pqrrecord->nEntry];
    int y = 0;
    for(int i = 0; i < m_pqrrecord->nEntry; i++){
        m_pMultiLineEdit[i].SetText(
            m_pqrrecord->entries[i]->content);
        m_pMultiLineEdit[i].SetEditBgType(UI_EDIT_BGTYPE_ROUND_RECT);
        m_pMultiLineEdit[i].SetReadOnly(true);
        m_pMultiLineEdit[i].SetLineSpace(2);
        int lineWidth = GetWidth()*5/6 - 20;
        int lines = m_pMultiLineEdit[i].GetTextLen() * m_pMultiLineEdit[i].GetTextSize()/lineWidth + 1;
        int lineHeight = m_pMultiLineEdit[i].getLineHeight() * (lines == 1 ? 2 : lines);
        m_pMultiLineEdit[i].SetPos(
            GetWidth() - lineWidth - 20,y,
            lineWidth,lineHeight);
        m_ScrollWin.AddChild(&m_pMultiLineEdit[i]);

        m_pEntryTitles[i].SetPos(0,y,GetWidth() - lineWidth - 25,lineHeight);
        m_pEntryTitles[i].SetText(
            qrCodeNames[nameidx].enames[m_pqrrecord->entries[i]->type]);
        m_pEntryTitles[i].SetDrawTextFormat(DT_RIGHT | DT_VCENTER);
        m_ScrollWin.AddChild(&m_pEntryTitles[i]);

        y+=lineHeight + 5;
    }
}

void Ui_ResultWnd::OnMzCommand(WPARAM wParam, LPARAM lParam) {
    UINT_PTR id = LOWORD(wParam);
    switch (id) {
        case MZ_IDC_TOOLBAR_MAIN:
            {
                int nIndex = lParam;
                if(nIndex == 1){	//确定
                    EndModal(ID_OK);
                    return;
                }
            }
    }
    CMzWndEx::OnMzCommand(wParam,lParam);
}
