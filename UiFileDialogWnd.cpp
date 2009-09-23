#include "UiFileDialogWnd.h"

//#include <InitGuid.h>
#include <IMzUnknown.h>
#include <IMzUnknown_IID.h>
#include <IFileBrowser.h>
//#include <IFileBrowser_GUID.h>

#include "mz_commonfunc.h"
using namespace MZ_CommonFunc;

#define MZ_IDC_TOOLBAR_MAIN 101
#define MZ_IDC_BUTTON_FOLDER 102

MZ_IMPLEMENT_DYNAMIC(UiFileDialogWnd)

BOOL UiFileDialogWnd::OnInitDialog(){
    // Must all the Init of parent class first!

    if (!CMzWndEx::OnInitDialog()) {
        return FALSE;
    }
    int y = 0;
    m_DialogTitle.SetPos(0,y,GetWidth(),MZM_HEIGHT_CAPTION);
    AddUiWin(&m_DialogTitle);

	y += MZM_HEIGHT_CAPTION;
	m_FileName.SetPos(0,y,GetWidth(),MZM_HEIGHT_SINGLELINE_EDIT);
	m_FileName.SetTextSize(20);
	m_FileName.SetTip(L"请输入文件名");
	if(_filename){
		m_FileName.SetText(_filename);
	}
	m_FileName.SetRightInvalid(60);
	AddUiWin(&m_FileName);

	y += MZM_HEIGHT_SINGLELINE_EDIT;
	m_FolderPath.SetPos(0,y,GetWidth(),MZM_HEIGHT_BUTTONEX);
	m_FolderPath.SetText(L"文件夹:");
	m_FolderPath.SetTextSize(25);
	m_FolderPath.SetTextMaxLen2(0);
	m_FolderPath.SetTextSize2(20);
	m_FolderPath.SetButtonType(MZC_BUTTON_LINE_BOTTOM);
	m_FolderPath.SetID(MZ_IDC_BUTTON_FOLDER);
	AddUiWin(&m_FolderPath);

    m_Toolbar.SetPos(0, GetHeight() - MZM_HEIGHT_TEXT_TOOLBAR, GetWidth(), MZM_HEIGHT_TEXT_TOOLBAR);
    m_Toolbar.SetID(MZ_IDC_TOOLBAR_MAIN);
    m_Toolbar.SetButton(0, true, true, L"取消");
    m_Toolbar.SetButton(2, true, true, L"保存");
    AddUiWin(&m_Toolbar);

	m_FileName.SetFocus(true);
    return TRUE;
}

void UiFileDialogWnd::OnMzCommand(WPARAM wParam, LPARAM lParam) {
    UINT_PTR id = LOWORD(wParam);
    switch (id) {
		case MZ_IDC_BUTTON_FOLDER:
			{
				IFileBrowser *pFile = NULL;                      
				CoInitializeEx(NULL, COINIT_MULTITHREADED );
				IMzSave *pSave = NULL; 
				if ( SUCCEEDED( CoCreateInstance( CLSID_FileBrowser, NULL,CLSCTX_INPROC_SERVER ,IID_MZ_FileBrowser,(void **)&pFile ) ) )
				{     
					if( SUCCEEDED( pFile->QueryInterface( IID_MZ_Save, (void**)&pSave ) ) )
					{
						pFile->SetParentWnd( m_hWnd );
						pFile->SetOpenDocumentType(DOCUMENT_SAVE_FILES);
						if(m_FolderPath.GetText2().IsEmpty()){
							pFile->SetOpenDirectoryPath( L"\\Disk" );
						}else{
							pFile->SetOpenDirectoryPath( m_FolderPath.GetText2().C_Str() );
						}
						pFile->SetTitle(L"请选择文件夹");
						if( pSave->Invoke() ) 
						{
							m_FolderPath.SetText2(pFile->GetSaveFileDirectory());
							m_FolderPath.Invalidate();
							m_FolderPath.Update();
						}
						pSave->Release();
					}     
					pFile->Release();
				}
				CoUninitialize();
			}
        case MZ_IDC_TOOLBAR_MAIN:
            {
                int nIndex = lParam;
                if(nIndex == 0){	//返回
                    EndModal(ID_CANCEL);
                    return;
                }
				if(nIndex == 2){	//保存
					getfileInfo();
					if(checkFileName()){
						if(checkFileExist()){
							if(::MzMessageBoxEx(m_hWnd,L"文件已经存在，是否要覆盖？",L"问题",MZ_YESNO,false) == 1){
								EndModal(ID_OK);
							}
						}else{
							EndModal(ID_OK);
						}
					}
                    return;					
				}
            }
    }
    CMzWndEx::OnMzCommand(wParam,lParam);
}



void UiFileDialogWnd::SetTitle(LPCTSTR text){
	if(text){
		m_DialogTitle.SetText(text);
	}
}
void UiFileDialogWnd::SetInitFileName(LPCTSTR text){
	if(text){
		C::newstrcpy(&_filename,text);
		m_FileName.SetText(text);
	}
}
void UiFileDialogWnd::SetInitFileSuffix(LPCTSTR text){
	if(text){
		C::newstrcpy(&_filesuffix,text);
		m_FileName.SetTip2(text,true);
	}
}
void UiFileDialogWnd::SetInitFolder(LPCTSTR text){
	if(text){
		m_FolderPath.SetText2(text);
	}
}

//将控件中的数据取出
void UiFileDialogWnd::getfileInfo(){
	//文件名
	if(!m_FileName.GetText().IsEmpty()){
		C::newstrcpy(&_filename,m_FileName.GetText().C_Str());
	}else{
		delete [] _filename;
		_filename = NULL;
	}

	//文件名
	if(!m_FolderPath.GetText2().IsEmpty()){
		C::newstrcpy(&_folder,m_FolderPath.GetText2().C_Str());
	}else{
		delete [] _folder;
		_folder = NULL;
	}

}
/*
文件名中不允许使用下列字符：< > / \ | : " * ?
*/
bool UiFileDialogWnd::checkFileName(){
	bool bRet = false;
	if(_folder == NULL || lstrlen(_folder) == 0) return bRet;

	int szfilename = lstrlen(_filename);
	if(_filename == NULL || szfilename == 0) return bRet;

	wchar_t forbiddenchars[] = {'<','>','/','\\','|',':','\"','*','?'};
	int szconst = sizeof(forbiddenchars)/sizeof(forbiddenchars[0]);
	wchar_t* pouter;
	wchar_t* pinner;
	int szouter,szinner;
	if(szfilename > szconst){
		pouter = _filename; szouter = szfilename;
		pinner = forbiddenchars; szinner = szconst;
	}else{
		pinner = _filename; szinner = szfilename;
		pouter = forbiddenchars; szouter = szconst;
	}
	for(int i = 0; i < szouter; i++){
		for(int j = 0; j < szinner; j++){
			if(pinner[j] == pouter[i]){
				return bRet;
			}
		}
	}
	bRet = true;
	return bRet;
}

/*
检查文件是否已经存在，如果存在则弹出确认对话框
*/
bool UiFileDialogWnd::checkFileExist(){
	return File::FileExists(GetFullFileName());
}