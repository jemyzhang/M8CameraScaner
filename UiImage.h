#pragma once
// include the MZFC library header file
#include <mzfc_inc.h>

class UiImage : public UiWin
{
public:
	UiImage(void){
		_reqUpdate = true;
		pimg = 0;
	}
	~UiImage(void){
	}
	void setupImage(ImagingHelper* img) {
		pimg = img;
	}
	virtual void PaintWin(HDC hdcDst, RECT* prcWin, RECT* prcUpdate);
	virtual void Update() {
		_reqUpdate = true;
		UiWin::Update();
	}
private:
	bool _reqUpdate;
	ImagingHelper *pimg;
};
