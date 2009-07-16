#ifndef _UI_MAIN_H
#define _UI_MAIN_H

// include the MZFC library header file
#include <mzfc_inc.h>
// Main window derived from CMzWndEx
#include "ui_capture.h"

class Ui_MainWnd : public CMzWndEx {
    MZ_DECLARE_DYNAMIC(Ui_MainWnd);
public:
	Ui_MainWnd();
	~Ui_MainWnd();
public:
    UiToolbar_Text m_Toolbar;
    Ui_CaptureWnd *m_pCapture;
protected:
    // Initialization of the window (dialog)
    virtual BOOL OnInitDialog();

    // override the MZFC window messages handler
    LRESULT MzDefWndProc(UINT message, WPARAM wParam, LPARAM lParam);

    // override the MZFC command handler
    virtual void OnMzCommand(WPARAM wParam, LPARAM lParam);
};


#endif /*_UI_MAIN_H*/