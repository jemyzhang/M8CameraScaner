#ifndef _UI_CAPTURE_H
#define _UI_CAPTURE_H

// include the MZFC library header file
#include <mzfc_inc.h>
#include <ICameraApp.h> 
#include <ICameraDeviceInterface.h>
// Main window derived from CMzWndEx

class Ui_CaptureWnd : public CMzWndEx {
    MZ_DECLARE_DYNAMIC(Ui_CaptureWnd);
public:
	Ui_CaptureWnd();
	~Ui_CaptureWnd();

    bool StartDecode();
public:
	ICameraDeviceInterface *m_pDevice;
protected:
    // Initialization of the window (dialog)
    virtual BOOL OnInitDialog();

    // override the MZFC window messages handler
    LRESULT MzDefWndProc(UINT message, WPARAM wParam, LPARAM lParam);

    // override the MZFC command handler
    virtual void OnMzCommand(WPARAM wParam, LPARAM lParam);
    void decode(HBITMAP hBitmap,const char *filename = 0);
	bool DataMatrixDecode(unsigned char* px,int w, int h);
private:
	bool isInitialized;
    bool isPaused;
};


#endif /*_UI_CAPTURE_H*/