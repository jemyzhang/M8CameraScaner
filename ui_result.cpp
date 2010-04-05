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
            L"��Ƭ",
            11,
        {
            L"����",L"ְ��",L"����",L"��˾",L"��ַ",
                L"�ʱ�",L"�ƶ��绰",L"�̶��绰",L"����",L"�����ʼ�",L"IM"
        }
    },
    {
        QR_SMS,
            L"����",
            2,
        {
            L"������",L"��������"
        }
    },
    {
        QR_MAIL,
            L"�����ʼ�",
            3,
        {
            L"�ռ���",L"����",L"����"
        }
    },
    {
        QR_URL,
            L"��ַ",
            2,
        {
            L"վ������",L"���ӵ�ַ"
        }
    },
    {
        QR_TEXT,
            L"�ı�",
            2,
        {
            L"����",L"����"
        }
    },
    {
        QR_TXT,
            L"��ͨ�ı�",
            1,
        {
            L"����"
        }
    },
    {
        QR_AD,
            L"���ӻ�绰",
            3,
        {
            L"����",
            L"WAP����",
            L"���е绰"
        }
    },
    {
        QR_BLOG,
            L"����",
            2,
        {
            L"����",
            L"���ӵ�ַ"
        }
    },
    {
        QR_GIS,
            L"��ͼ",
            1,
        {
            L"���ӵ�ַ"
        }
    },
    {
        QR_ENC,
            L"��������",
            2,
        {
            L"�ı�",
            L"����"
        }
    },
    {
        BAR_CODE,
            L"������",
            2,
        {
            L"����",
            L"����"
        }
    },
};

const QRActionName_t qrActionName[] = {
	{	CmdSaveText,		L"���浽�ı�"	},
	{	CmdClipBoard,		L"���Ƶ�������"	},
	{	CmdDialNumber,		L"����绰"		},
	{	CmdSendSms,			L"���Ͷ���"		},
	{	CmdSaveContact,		L"���浽��ϵ��"	},
	{	CmdOpenSite,		L"������"		},
	{	CmdSendMail,		L"�����ʼ�"		},
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
    m_Toolbar.SetButton(TOOLBARPRO_LEFT_TEXTBUTTON, true, true, L"����");
    m_Toolbar.SetMiddleButton(true, true, L"����",NULL,NULL,NULL);
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
        m_Title.SetText(L"������");
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
        stitle = L"������[";
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
			m_pEntryTitles[i].SetText(L"δ֪");
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
                if(nIndex == TOOLBARPRO_LEFT_TEXTBUTTON){	//����
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
			dlg.SetTitle(L"��ѡ��Ҫ������ļ�");
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
					MzMessageAutoBoxV2(m_hWnd,L"�ļ���ʧ�ܡ�");
					return;
				}
				for(int i = 0; i < pr->nEntry; i++){
					fwprintf(fp,L"%s:%s\n",this->m_pEntryTitles[i].GetText().C_Str(),this->m_pMultiLineEdit[i].GetText().C_Str());
				}
				fclose(fp);
				CMzString msg = L"�������ļ�";
				msg = msg + filename;
				MzMessageAutoBoxV2(m_hWnd, msg.C_Str());
			}
		}
			break;
		case CmdSaveContact:
			if(SaveContact(pr)){
				MzMessageAutoBoxV2(m_hWnd, L"��ϵ�˱���ɹ�");
			}else{
				MzMessageAutoBoxV2(m_hWnd, L"��ϵ�˱���ʧ��");
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
					MzMessageAutoBoxV2(m_hWnd, L"������ϵ�˵绰Ϊ��");
					return;
				}
				if(smscontent == NULL){
					MzMessageAutoBoxV2(m_hWnd, L"���󣬶�������Ϊ��");
					return;
				}	
				PROCESS_INFORMATION pi;
				CMzString s = L"-n ";
				s = s + phonenumber;
				s = s + L"��";
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
			MzMessageAutoBoxV2(m_hWnd, L"������δ���");
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
			MzMessageAutoBoxV2(m_hWnd, L"������δ���");
			break;
	}
}

#define DEFAULT_CONTACT_DB  L"\\Documents and Settings\\default.vol"
typedef enum 
{
    Firstname = 0x3a06001f,//��
    WorkTel  = 0x3a08001f,//�����绰
    HomeTel  = 0x3a09001f,//סլ�绰
    Lastname  = 0x3a11001f,//��
    Company   = 0x3a16001f,//��˾/��λ
    JobTitle = 0x3a17001f,//ְ��
    Department = 0x3a18001f,//����
    OfficeLoc = 0x3a19001f,//�칫��
    MobileTel=  0x3a1c001f,//�ƶ��绰 
    RadioTel =  0x3a1d001f,//���ߵ绰
    CarTel   = 0x3a1e001f,//���ص绰
    WorkFax  = 0x3a24001f,//��������
    HomeFax  = 0x3a25001f,//סլ����
    HomeTel2 = 0x3a2f001f ,//סլ�绰2
    Birthday  = 0x40010040,//����
    Assistant = 0x4002001f,//����
    Anniversary = 0x40030040, //�������
    AssistantTel = 0x4004001f,//����绰
    Children = 0x4006001f,//��Ů
    WorkTel2 = 0x4007001f,//�����绰2
    HomePage = 0x4008001f,//��ҳ
    Pager = 0x4009001f,//Ѱ����
    Spouse = 0x400a001f,//��ż
    Name = 0x4013001f,//����
    Title = 0x4023001f,//�ƺ�(����ϵ��)
    HomeAddr = 0x4040001f,//��ͥ��ַ
    HomeCity = 0x4041001f,//��ͥ���ڳ���
    HomeState = 0x4042001f,//��ͥ���ڵ�ʡ/��
    HomeZip = 0x4043001f,//��ͥ�ʱ�
    HomeCountry = 0x4044001f,//��ͥ���ڹ���
    WorkAddr = 0x4045001f,//������ַ
    WorkCity = 0x4046001f,//�����ĳ���
    WorkState = 0x4047001f, //������ʡ/��
    WorkZip = 0x4048001f,//�����ص��ʱ�
    WorkCountry = 0x4049001f,//�����صĹ���
    OtherAddr = 0x404a001f,//������ַ
    OtherCity = 0x404b001f,//��������
    OtherState = 0x404c001f,//����ʡ/��
    OtherZip = 0x404d001f,//�����ʱ�
    OtherCountry = 0x404e001f,//�����Ĺ���
    Email = 0x4083001f,//��������
    Email2nd = 0x4093001f,//��������2
    Email3rd = 0x40a3001f//��������3
}Contacts_field; //Contacts Database���ֶ�,ϵͳ���岻�ɸ���

#pragma comment(lib,"cellcore.lib")
bool Ui_ResultWnd::SaveContact(QRCODE_RECORD_ptr pr){
	/////////////////
	//�������ݿ�
	/////////////////
	CEGUID ceguid;
	TCHAR szVolName[128];
	CREATE_INVALIDGUID(&ceguid);
	while(::CeEnumDBVolumes(&ceguid,szVolName,MAX_PATH))
	{
		if(wcsnicmp(DEFAULT_CONTACT_DB,szVolName,10)==0)
		{
			/////����ǰ���ݿ��λ��SystemHeap��
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

	//����ϵ�����ݿ�,��ע���¼�
	HANDLE hdDB;
	hdDB = ::CeOpenDatabaseEx(&ceguid,&ceoid,L"Contacts Database",0,0,pRequest);

	if(hdDB == INVALID_HANDLE_VALUE)
	{
		MzMessageAutoBoxV2(m_hWnd, L"��ϵ�����ݿ��ʧ��");
		return false;
	}

	///////////////////////
	// �����ݿ���д����
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

	//д���ļ���
	CEOID CEoid = CeWriteRecordProps(hdDB,0,cnt,pProps);
	DWORD ERR = GetLastError();
	if(0 == CEoid){
		MzMessageAutoBoxV2(m_hWnd, L"д���ݿ����ʧ��!");
		delete []pProps;
		return false;
	}

	//�ر����ݿ�
	if(! CloseHandle(hdDB))
	{
		MzMessageAutoBoxV2(m_hWnd, L"�ر����ݿ�ʧ��");
		return false;
	}
	 CeFlushDBVol(&ceguid); 
	if(!CeUnmountDBVol(&ceguid)){
		MzMessageAutoBoxV2(m_hWnd, L"���ݿ�ˢ��ʧ��,����m8��ɿ����¼���ϵ�ˣ�δ������⣩");
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

        //��ַ��ʽע����ں���ǰ��"+86"
        smsaDestination.smsatAddressType = SMSAT_INTERNATIONAL;
        _tcsncpy(smsaDestination.ptsAddress, lpNumber,SMS_MAX_ADDRESS_LENGTH);

        tpsd.dwMessageOptions  = PS_MESSAGE_OPTION_NONE;
        //tpsd.dwMessageOptions =  PS_MESSAGE_OPTION_STATUSREPORT;//��ʾ��Ҫ״̬����
        tpsd.psMessageClass  = PS_MESSAGE_CLASS1;
        //PS_MESSAGE_CLASS0��ʾ�����ڱ����պ�������ʾ�Ҳ��洢���ռ���(��Ϊ����)
        //PS_MESSAGE_CLASS1��ʾһ�������������պ�洢���ռ��䲢����һ��ȷ�ϻض������ģ����ͷ��յ�һ���ѱ����յ�״̬���档

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