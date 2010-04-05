#ifndef _UI_MAIN_H
#define _UI_MAIN_H

// include the MZFC library header file
#include <mzfc_inc.h>
// Main window derived from CMzWndEx
#include "ui_capture.h"
#include "ui_VideoSurface.h"
#include "UiImage.h"

class Ui_MainWnd : public CMzWndEx {
    MZ_DECLARE_DYNAMIC(Ui_MainWnd);
public:
	Ui_MainWnd();
	~Ui_MainWnd();
   	void PaintWin(HDC hdc, RECT* prcUpdate = NULL);

public:
    UiToolBarPro m_Toolbar;
    //Ui_CaptureWnd *m_pCapture;
	ui_VideoSurface *m_pCapture;
	Ui_CaptureWnd *m_pDecode;
	UiButton m_ButtonCapture;
	UiButton m_ButtonLoadFile;
	UiEdit m_TextAbout;
	UiImage	m_Logo;
protected:
    // Initialization of the window (dialog)
    virtual BOOL OnInitDialog();

    // override the MZFC window messages handler
    LRESULT MzDefWndProc(UINT message, WPARAM wParam, LPARAM lParam);

    // override the MZFC command handler
    virtual void OnMzCommand(WPARAM wParam, LPARAM lParam);
	bool OnShellMessage(UINT message, WPARAM wParam, LPARAM lParam);
protected:
	bool GetImageFile();
private:
	wchar_t* m_ImageFile;

};


#endif /*_UI_MAIN_H*/