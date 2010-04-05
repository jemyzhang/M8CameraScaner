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
	bool InitCameraDevice();	//打开摄像头设备
	bool ReleaseCameraDevice();	//	关闭摄像头设备

	void adjustCameraPos();		//调整摄像头位置

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
	wchar_t* m_ImageFile;	//保存截图路径
	bool isInitialized;
    bool isAFStart;
	RECT m_rcCamera;
	BarCodeType_t m_type;
	bool reqInitCamera;
	UINT fadeinStep;
	UINT autoEndTime;	//自动退出相机
	Ui_CaptureWnd *m_pDecode;
	int orignalPhotoSize;	//原始照片大小
};
