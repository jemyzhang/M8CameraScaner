#include "ui_result.h"
#include "resource.h"
#include "mz_commonfunc.h"
using namespace MZ_CommonFunc;

#include "UiFileDialogWnd.h"

//////////////////////////////////////////////
#define MZ_IDC_SCROLLBAR	102
#define MZ_IDC_TOOLBAR_MAIN 101
#define MZ_IDC_MENU_ACTION 110

const QRCODE_NAMES_t qrCodeNames[] = {
    {
        QR_NAMECARD,
            L"名片",
            11,
        {
            L"姓名",L"职务",L"部门",L"公司",L"地址",
                L"邮编",L"移动电话",L"固定电话",L"传真",L"电子邮件",L"IM"
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
    {
        BAR_CODE,
            L"条形码",
            2,
        {
            L"类型",
            L"序列"
        }
    },
};

const QRActionName_t qrActionName[] = {
	{	CmdSaveText,		L"保存到文本"	},
	{	CmdClipBoard,		L"复制到剪贴板"	},
	{	CmdDialNumber,		L"拨打电话"		},
	{	CmdSendSms,			L"发送短信"		},
	{	CmdSaveContact,		L"保存到联系人"	},
	{	CmdOpenSite,		L"打开链接"		},
	{	CmdSendMail,		L"发送邮件"		},
};

const QRActionMap_t qrActionMap[] = {
	{QR_NAMECARD,CmdSaveText | CmdSaveContact | CmdDialNumber | CmdSendSms | CmdClipBoard},
	{QR_TXT, CmdSaveText | CmdClipBoard},
	{QR_SMS, CmdSaveText | CmdClipBoard | CmdSendSms},
	{QR_MAIL, CmdSaveText | CmdClipBoard | CmdSendMail},
	{QR_URL, CmdSaveText | CmdClipBoard | CmdOpenSite},
	{QR_TEXT, CmdSaveText | CmdClipBoard},
	{QR_PHONE,CmdSaveText | CmdDialNumber | CmdSendSms | CmdClipBoard},
};

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
    m_Title.SetPos(0,y,GetWidth(),MZM_HEIGHT_HEADINGBAR);
    AddUiWin(&m_Title);

    y+=MZM_HEIGHT_HEADINGBAR + 5;
    m_ScrollWin.SetPos(0,y,GetWidth(),GetHeight() - y - MZM_HEIGHT_TOOLBARPRO);
	m_ScrollWin.SetID(MZ_IDC_SCROLLBAR);
    m_ScrollWin.EnableScrollBarV(true);
    AddUiWin(&m_ScrollWin);

    m_Toolbar.SetPos(0, GetHeight() - MZM_HEIGHT_TOOLBARPRO, GetWidth(), MZM_HEIGHT_TOOLBARPRO);
    m_Toolbar.SetID(MZ_IDC_TOOLBAR_MAIN);
    m_Toolbar.SetButton(TOOLBARPRO_LEFT_TEXTBUTTON, true, true, L"返回");
    m_Toolbar.SetMiddleButton(true, true, L"操作",NULL,NULL,NULL);
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
        m_Title.SetText(L"条形码");
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
        stitle = L"条形码[";
    }
    stitle = stitle + qrCodeNames[nameidx].tname;
    stitle = stitle + L"]";
    m_Title.SetText(stitle.C_Str());

    m_pEntryTitles = new UiStatic[m_pqrrecord->nEntry];
    m_pMultiLineEdit = new UiEdit[m_pqrrecord->nEntry];
    int y = 0;
    for(int i = 0; i < m_pqrrecord->nEntry; i++){
        m_pMultiLineEdit[i].SetEditBgType(UI_EDIT_BGTYPE_FILL_WHITE_AND_TOPSHADOW);
        m_pMultiLineEdit[i].SetTextSize(20);
        m_pMultiLineEdit[i].SetReadOnly(true);
        m_pMultiLineEdit[i].SetLineSpace(2);
        int lineWidth = GetWidth()*3/4 - 20;
        m_pMultiLineEdit[i].SetPos(
            GetWidth() - lineWidth - 20,y,
            lineWidth,200);
        m_pMultiLineEdit[i].SetText(
            m_pqrrecord->entries[i]->content);
		int lineHeight = m_pMultiLineEdit[i].CalcContentHeight() +  m_pMultiLineEdit[i].GetTopInvalid() +  m_pMultiLineEdit[i].GetBottomInvalid () +
			(m_pMultiLineEdit[i].GetRowCount() + 0) * m_pMultiLineEdit[i].GetLingSpace();
        m_pMultiLineEdit[i].SetPos(
            GetWidth() - lineWidth - 20,y,
            lineWidth,lineHeight);
        m_ScrollWin.AddChild(&m_pMultiLineEdit[i]);

        m_pEntryTitles[i].SetPos(0,y,GetWidth() - lineWidth - 25,lineHeight);
		if(m_pqrrecord->entries[i]->type == QR_ENTRY_UNKNOWN){
			m_pEntryTitles[i].SetText(L"未知");
		}else{
			m_pEntryTitles[i].SetText(
				qrCodeNames[nameidx].enames[m_pqrrecord->entries[i]->type]);
		}
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
                if(nIndex == TOOLBARPRO_LEFT_TEXTBUTTON){	//返回
                    if (m_GridMenu.IsContinue()){
                        m_GridMenu.EndGridMenu();
                    }
                    EndModal(ID_OK);
                    return;
                }
				if(nIndex == TOOLBARPRO_MIDDLE_TEXTBUTTON){
                    if (m_GridMenu.IsContinue()){
                        m_GridMenu.EndGridMenu();
                    }else{
                        popupMenu(m_pqrrecord);
                    }
                    return;
				}
            }
    }
    ProcGridMenu(id);
    CMzWndEx::OnMzCommand(wParam,lParam);
}

LRESULT Ui_ResultWnd::MzDefWndProc(UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case MZ_WM_WND_ACTIVATE:
            {
                if(LOWORD(wParam)==WA_INACTIVE){
                    if (m_GridMenu.IsContinue()){
                        m_GridMenu.EndGridMenu();
                    }
                }
                break;
            }
    }
    return CMzWndEx::MzDefWndProc(message, wParam, lParam);
}

void Ui_ResultWnd::popupMenu(QRCODE_RECORD_ptr pr){
	if(pr == NULL) return;
	uint actioncode = CmdSaveText | CmdClipBoard;
	for(int i = 0; i < sizeof(qrActionMap)/sizeof(qrActionMap[0]); i++){
		if(qrActionMap[i].type == pr->type){
			actioncode = qrActionMap[i].actioncode ;
		}
	}

    ImagingHelper* m_NewImg = 
        m_imgContainer.LoadImage(GetMzResV2ModuleHandle(), MZRESV2_IDR_PNG_CONTACT, true);
    ImagingHelper* m_NewImgPrs = 
        m_imgContainer.LoadImage(GetMzResV2ModuleHandle(), MZRESV2_IDR_PNG_CONTACT_PRESSED, true);

    ImagingHelper* m_TxtImg = 
        m_imgContainer.LoadImage(GetMzResV2ModuleHandle(), MZRESV2_IDR_PNG_SAVE, true);
    ImagingHelper* m_TxtImgPrs = 
        m_imgContainer.LoadImage(GetMzResV2ModuleHandle(), MZRESV2_IDR_PNG_SAVE_PRESSED, true);

    ImagingHelper* m_DialImg = 
        m_imgContainer.LoadImage(GetMzResV2ModuleHandle(), MZRESV2_IDR_PNG_PHONE, true);
    ImagingHelper* m_DialImgPrs = 
        m_imgContainer.LoadImage(GetMzResV2ModuleHandle(), MZRESV2_IDR_PNG_PHONE_PRESSED, true);

    ImagingHelper* m_SmsImg = 
        m_imgContainer.LoadImage(GetMzResV2ModuleHandle(), MZRESV2_IDR_PNG_REPLY_NORMAL, true);
    ImagingHelper* m_SmsImgPrs = 
        m_imgContainer.LoadImage(GetMzResV2ModuleHandle(), MZRESV2_IDR_PNG_REPLY_PRESSED, true);

    ImagingHelper* m_SendImg = 
        m_imgContainer.LoadImage(GetMzResV2ModuleHandle(), MZRESV2_IDR_PNG_SEND, true);
    ImagingHelper* m_SendImgPrs = 
        m_imgContainer.LoadImage(GetMzResV2ModuleHandle(), MZRESV2_IDR_PNG_SEND_PRESSED, true);

    ImagingHelper* m_SiteImg = 
        m_imgContainer.LoadImage(GetMzResV2ModuleHandle(), MZRESV2_IDR_PNG_JUMPTO, true);
    ImagingHelper* m_SiteImgPrs = 
        m_imgContainer.LoadImage(GetMzResV2ModuleHandle(), MZRESV2_IDR_PNG_JUMPTO_PRESSED, true);

    ImagingHelper* m_ClipImg = 
        m_imgContainer.LoadImage(GetMzResV2ModuleHandle(), MZRESV2_IDR_PNG_MOVE, true);
    ImagingHelper* m_ClipImgPrs = 
        m_imgContainer.LoadImage(GetMzResV2ModuleHandle(), MZRESV2_IDR_PNG_MOVE_PRESSED, true);

    struct QRActionImage {
        ResultActionCmd_t type;
        ImagingHelper* img;
        ImagingHelper* presimg;
    }actionImage[] = {
        {CmdSaveContact,m_NewImg,m_NewImgPrs},
        {CmdSaveText,m_TxtImg,m_TxtImgPrs},
        {CmdDialNumber,m_DialImg,m_DialImgPrs},
        {CmdSendSms,m_SmsImg,m_SmsImgPrs},
        {CmdSendMail,m_SendImg,m_SendImgPrs},
        {CmdOpenSite,m_SiteImg,m_SiteImgPrs},
        {CmdClipBoard,m_ClipImg,m_ClipImgPrs},
    };
    if(m_GridMenu.IsContinue()){
        m_GridMenu.EndGridMenu();
    }
    m_GridMenu.RemoveAllMenuItem();
	for(int i = 0 ; i < sizeof(qrActionName)/sizeof(qrActionName[0]); i++){
		if(actioncode & qrActionName[i].type){
            for(int j = 0; j < sizeof(actionImage)/sizeof(actionImage[0]); j++){
                if(actionImage[j].type == qrActionName[i].type){
                    m_GridMenu.AppendMenuItem(MZ_IDC_MENU_ACTION + qrActionName[i].type, 
                        qrActionName[i].name, actionImage[j].img, actionImage[j].presimg);
                }
            }
		}
	}
    m_GridMenu.TrackGridMenuDialog(m_hWnd, MZM_HEIGHT_TOOLBARPRO);
}

void Ui_ResultWnd::ProcGridMenu(UINT_PTR id){
    UINT_PTR nID = id - MZ_IDC_MENU_ACTION;
    QRCODE_RECORD_ptr pr = m_pqrrecord;
	switch(nID){
		case CmdNone:
			break;
		case CmdSaveText:
		{
			UiFileDialogWnd dlg;
			dlg.SetTitle(L"请选择要保存的文件");
			dlg.SetInitFileSuffix(L".txt");
			dlg.SetInitFolder(L"\\Disk");
			CMzString filename = this->m_Title.GetText();
			filename = filename + L"_";
			filename = filename + DateTime::NowtoStr();
			dlg.SetInitFileName(filename.C_Str());
			RECT rcWork = MzGetWorkArea();
			dlg.Create(rcWork.left, rcWork.top, RECT_WIDTH(rcWork), RECT_HEIGHT(rcWork), m_hWnd, 0, WS_POPUP);
			int nRet = dlg.DoModal();
			if(nRet == ID_OK){
				FILE* fp;
				filename = dlg.GetFullFileName();
				fp = _wfopen(filename,L"wt");
				if(fp == NULL){
					MzMessageAutoBoxV2(m_hWnd,L"文件打开失败。");
					return;
				}
				for(int i = 0; i < pr->nEntry; i++){
					fwprintf(fp,L"%s:%s\n",this->m_pEntryTitles[i].GetText().C_Str(),this->m_pMultiLineEdit[i].GetText().C_Str());
				}
				fclose(fp);
				CMzString msg = L"保存至文件";
				msg = msg + filename;
				MzMessageAutoBoxV2(m_hWnd, msg.C_Str());
			}
		}
			break;
		case CmdSaveContact:
			if(SaveContact(pr)){
				MzMessageAutoBoxV2(m_hWnd, L"联系人保存成功");
			}else{
				MzMessageAutoBoxV2(m_hWnd, L"联系人保存失败");
			}
			break;
		case CmdDialNumber:
			for(int ie = 0; ie < pr->nEntry; ie++){
				if( (pr->entries[ie]->type == QR_CARD_MOBILE || pr->entries[ie]->type == QR_CARD_TEL) && pr->entries[ie]->content != NULL){
					PROCESS_INFORMATION pi;
					CMzString s = L"-n ";
					s = s + pr->entries[ie]->content;
					CreateProcess(L"\\Windows\\callui.exe", //\\Windows\\DialManager.exe
						s.C_Str() , NULL, NULL, FALSE, 0, NULL, NULL, NULL, &pi);
				}
			}
			break;
		case CmdSendSms:
			if(pr->type == QR_SMS){
				wchar_t* phonenumber = NULL;
				wchar_t* smscontent = NULL;
				for(int ie = 0; ie < pr->nEntry; ie++){
					if(pr->entries[ie]->type == QR_SMS_RECEIVER && pr->entries[ie]->content != NULL){
						phonenumber = pr->entries[ie]->content;
					}
					if(pr->entries[ie]->type == QR_SMS_CONTENT && pr->entries[ie]->content != NULL){
						smscontent = pr->entries[ie]->content;
					}
				}
				if(phonenumber == NULL){
					MzMessageAutoBoxV2(m_hWnd, L"错误，联系人电话为空");
					return;
				}
				if(smscontent == NULL){
					MzMessageAutoBoxV2(m_hWnd, L"错误，短信内容为空");
					return;
				}	
				PROCESS_INFORMATION pi;
				CMzString s = L"-n ";
				s = s + phonenumber;
				s = s + L"▓";
				s = s + smscontent;
				CreateProcess(L"\\windows\\smsui.exe", 
					s.C_Str() , NULL, NULL, FALSE, 0, NULL, NULL, NULL, &pi);
			}else{
				for(int ie = 0; ie < pr->nEntry; ie++){
					if(pr->entries[ie]->type == QR_CARD_MOBILE && pr->entries[ie]->content != NULL){
						PROCESS_INFORMATION pi;
						CMzString s = L"-n ";
						s = s + pr->entries[ie]->content;
						CreateProcess(L"\\windows\\smsui.exe", 
							s.C_Str() , NULL, NULL, FALSE, 0, NULL, NULL, NULL, &pi);
					}
				}
			}
			break;
		case CmdSendMail:
			MzMessageAutoBoxV2(m_hWnd, L"功能尚未完成");
			break;
		case CmdOpenSite:
			for(int ie = 0; ie < pr->nEntry; ie++){
				if(pr->entries[ie]->type == QR_SITE_URL && pr->entries[ie]->content != NULL){
					PROCESS_INFORMATION pi;
					CreateProcess(L"\\Program Files\\WebBrowser\\WebBrowser.exe", 
						pr->entries[ie]->content , NULL, NULL, FALSE, 0, NULL, NULL, NULL, &pi);
				}
			}
			break;
		case CmdClipBoard:
			MzMessageAutoBoxV2(m_hWnd, L"功能尚未完成");
			break;
	}
}

#define DEFAULT_CONTACT_DB  L"\\Documents and Settings\\default.vol"
typedef enum 
{
    Firstname = 0x3a06001f,//姓
    WorkTel  = 0x3a08001f,//工作电话
    HomeTel  = 0x3a09001f,//住宅电话
    Lastname  = 0x3a11001f,//名
    Company   = 0x3a16001f,//公司/单位
    JobTitle = 0x3a17001f,//职务
    Department = 0x3a18001f,//部门
    OfficeLoc = 0x3a19001f,//办公室
    MobileTel=  0x3a1c001f,//移动电话 
    RadioTel =  0x3a1d001f,//无线电话
    CarTel   = 0x3a1e001f,//车载电话
    WorkFax  = 0x3a24001f,//工作传真
    HomeFax  = 0x3a25001f,//住宅传真
    HomeTel2 = 0x3a2f001f ,//住宅电话2
    Birthday  = 0x40010040,//生日
    Assistant = 0x4002001f,//助理
    Anniversary = 0x40030040, //周年纪念
    AssistantTel = 0x4004001f,//助理电话
    Children = 0x4006001f,//子女
    WorkTel2 = 0x4007001f,//工作电话2
    HomePage = 0x4008001f,//主页
    Pager = 0x4009001f,//寻呼机
    Spouse = 0x400a001f,//配偶
    Name = 0x4013001f,//姓名
    Title = 0x4023001f,//称呼(对联系人)
    HomeAddr = 0x4040001f,//家庭地址
    HomeCity = 0x4041001f,//家庭所在城市
    HomeState = 0x4042001f,//家庭所在的省/州
    HomeZip = 0x4043001f,//家庭邮编
    HomeCountry = 0x4044001f,//家庭所在国家
    WorkAddr = 0x4045001f,//工作地址
    WorkCity = 0x4046001f,//工作的城市
    WorkState = 0x4047001f, //工作的省/州
    WorkZip = 0x4048001f,//工作地的邮编
    WorkCountry = 0x4049001f,//工作地的国家
    OtherAddr = 0x404a001f,//其他地址
    OtherCity = 0x404b001f,//其他城市
    OtherState = 0x404c001f,//其他省/州
    OtherZip = 0x404d001f,//其他邮编
    OtherCountry = 0x404e001f,//其他的国家
    Email = 0x4083001f,//电子邮箱
    Email2nd = 0x4093001f,//电子邮箱2
    Email3rd = 0x40a3001f//电子邮箱3
}Contacts_field; //Contacts Database的字段,系统定义不可更改

#pragma comment(lib,"cellcore.lib")
bool Ui_ResultWnd::SaveContact(QRCODE_RECORD_ptr pr){
	/////////////////
	//　打开数据库
	/////////////////
	CEGUID ceguid;
	TCHAR szVolName[128];
	CREATE_INVALIDGUID(&ceguid);
	while(::CeEnumDBVolumes(&ceguid,szVolName,MAX_PATH))
	{
		if(wcsnicmp(DEFAULT_CONTACT_DB,szVolName,10)==0)
		{
			/////将当前数据库卷定位到SystemHeap上
			break;
		}
	}

	CEOID ceoid;
	ceoid = 0;

	CENOTIFYREQUEST *pRequest = (CENOTIFYREQUEST *) LocalAlloc(LPTR,sizeof(CENOTIFYREQUEST));
	pRequest->dwSize = sizeof(CENOTIFYREQUEST);
	pRequest->hwnd = NULL;
	pRequest->hHeap = NULL;
	pRequest->dwFlags = CEDB_EXNOTIFICATION;

	//打开联系人数据库,并注册事件
	HANDLE hdDB;
	hdDB = ::CeOpenDatabaseEx(&ceguid,&ceoid,L"Contacts Database",0,0,pRequest);

	if(hdDB == INVALID_HANDLE_VALUE)
	{
		MzMessageAutoBoxV2(m_hWnd, L"联系人数据库打开失败");
		return false;
	}

	///////////////////////
	// 向数据库中写数据
	///////////////////////
	CEPROPVAL *pProps;
	pProps = new CEPROPVAL[pr->nEntry];
	memset(pProps,0,LocalSize(pProps));

	CEPROPVAL *pp = pProps;
	WORD cnt = 0;
	for(int i = 0; i < pr->nEntry; i++){
		bool bvalid = true;
		if(pr->entries[i]->content != NULL){
			switch(pr->entries[i]->type){
			case QR_CARD_MOBILE:
				pp->propid = MobileTel;
				break;
			case QR_CARD_NAME:
				pp->propid = Name;
				break;
			case QR_CARD_TITLE:
				pp->propid = Title;
				break;
			case QR_CARD_DIVISION:
				pp->propid = Department;
				break;
			case QR_CARD_COMPANY:
				pp->propid = Company;
				break;
			case QR_CARD_ADDRESS:
				pp->propid = HomeAddr;
				break;
			case QR_CARD_ZIPCODE:
				pp->propid = HomeZip;
				break;
			case QR_CARD_TEL:
				pp->propid = HomeTel;
				break;
			case QR_CARD_FAX:
				pp->propid = WorkFax;
				break;
			case QR_CARD_EMAIL:
				pp->propid = Email;
				break;
			default:
				bvalid = false;
				break;
			}
			if(bvalid){
				pp->val.lpwstr = pr->entries[i]->content;
				pp++;
				cnt++;
			}
		}
	}

	//写到文件中
	CEOID CEoid = CeWriteRecordProps(hdDB,0,cnt,pProps);
	DWORD ERR = GetLastError();
	if(0 == CEoid){
		MzMessageAutoBoxV2(m_hWnd, L"写数据库操作失败!");
		delete []pProps;
		return false;
	}

	//关闭数据库
	if(! CloseHandle(hdDB))
	{
		MzMessageAutoBoxV2(m_hWnd, L"关闭数据库失败");
		return false;
	}
	 CeFlushDBVol(&ceguid); 
	if(!CeUnmountDBVol(&ceguid)){
		MzMessageAutoBoxV2(m_hWnd, L"数据库刷新失败,重启m8后可看到新加联系人（未解决问题）");
	}
	return true;
}

#if 0
#include <sms.h>
#pragma comment(lib,"sms.lib")

bool Ui_ResultWnd::SendSMS(LPCTSTR lpNumber,LPCTSTR lpszMessage)
{

        HRESULT hRes;
        SMS_HANDLE   smsHandle=NULL;
        SMS_ADDRESS   smsaDestination;
        SMS_MESSAGE_ID   smsmidMessageID=0;
        TEXT_PROVIDER_SPECIFIC_DATA   tpsd;

        hRes=SmsOpen(SMS_MSGTYPE_TEXT,SMS_MODE_SEND,&smsHandle,NULL);
        if   (FAILED(hRes))
        {
                return   false;
        }

        //地址方式注意国内号码前加"+86"
        smsaDestination.smsatAddressType = SMSAT_INTERNATIONAL;
        _tcsncpy(smsaDestination.ptsAddress, lpNumber,SMS_MAX_ADDRESS_LENGTH);

        tpsd.dwMessageOptions  = PS_MESSAGE_OPTION_NONE;
        //tpsd.dwMessageOptions =  PS_MESSAGE_OPTION_STATUSREPORT;//表示需要状态报告
        tpsd.psMessageClass  = PS_MESSAGE_CLASS1;
        //PS_MESSAGE_CLASS0表示短信在被接收后立即显示且不存储在收件箱(称为闪信)
        //PS_MESSAGE_CLASS1表示一般的情况，被接收后存储到收件箱并发送一个确认回短信中心，发送方收到一个已被接收的状态报告。

        ZeroMemory(tpsd.pbHeaderData, sizeof(tpsd.pbHeaderData));         
        tpsd.dwHeaderDataSize = 0;
        tpsd.fMessageContainsEMSHeaders = FALSE;
        tpsd.dwProtocolID = SMS_MSGPROTOCOL_UNKNOWN;
        tpsd.psReplaceOption = PSRO_NONE;


        hRes= SmsSendMessage(smsHandle,
                NULL,   
                &smsaDestination,   
                NULL,
                (PBYTE)lpszMessage,   
                _tcslen(lpszMessage) *  sizeof(TCHAR),   
                (PBYTE)&tpsd,
                sizeof(TEXT_PROVIDER_SPECIFIC_DATA),   
                SMSDE_OPTIMAL,   
                SMS_OPTION_DELIVERY_NONE,
                &smsmidMessageID);

        SmsClose(smsHandle);

        if   (SUCCEEDED(hRes))
        {
                return true;
        }
        else
        {
                return false;
        }
} 
#endif