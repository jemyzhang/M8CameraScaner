#pragma once
// include the MZFC library header file
#include <mzfc_inc.h>
#include <ICameraApp.h> 
#include <ICameraDeviceInterface.h>

#include "ui_capture.h"

class ui_VideoSurface :
	public CMzWndEx
{
public:
	ui_VideoSurface(void);
	~ui_VideoSurface(void);

	wchar_t* getCapturePath(){
		return m_ImageFile;
	}
    RECT getCameraRegion(){
        return m_rcCamera;
    }
    BarCodeType_t getDecodeType(){
        return m_type;
    }
public:
	void PaintWin(HDC hdc, RECT* prcUpdate = NULL);
	void OnTimer(UINT nIDEvent);
	virtual int DoModal(){
		reqInitCamera = true;
		return CMzWndEx::DoModal();
	}
protected:
	bool InitCameraDevice();	//������ͷ�豸
	bool ReleaseCameraDevice();	//	�ر�����ͷ�豸

	void adjustCameraPos();		//��������ͷλ��

	void resetAutoOff(){
		autoEndTime = 60;
	}
    void stopAutoOff(){
        autoEndTime = 0;
    }

	// Initialization of the window (dialog)
    virtual BOOL OnInitDialog();
    // override the MZFC window messages handler
    LRESULT MzDefWndProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void OnMzCommand(WPARAM wParam, LPARAM lParam);
	bool OnShellMessage(UINT message, WPARAM wParam, LPARAM lParam);
private:
	UiButton_Image m_ButtonExit;
	UiButton_Image m_ButtonCapture;
	UiButton_Image m_ButtonChangeShape;
private:
	ICameraDeviceInterface *m_pDevice;
	wchar_t* m_ImageFile;	//�����ͼ·��
	bool isInitialized;
    bool isAFStart;
	RECT m_rcCamera;
	BarCodeType_t m_type;
	bool reqInitCamera;
	UINT fadeinStep;
	UINT autoEndTime;	//�Զ��˳����
	Ui_CaptureWnd *m_pDecode;
	int orignalPhotoSize;	//ԭʼ��Ƭ��С
};
