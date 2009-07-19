/******************************************************************************  
* �ļ�����Dib.cpp
* ���ܣ�λͼ�����ӿ�  
* modified by PRTsinghua@hotmail.com  
******************************************************************************/

#include "Dib.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/******************************************************************************  
* ���캯��/��������  
******************************************************************************/

MzDib::MzDib()
{
    bloaded = false;
    lpBitmapBits = NULL;
}

MzDib::~MzDib()
{
    if(lpBitmapBits){
        delete []lpBitmapBits;
        lpBitmapBits = NULL;
    }
    Close();
}


/******************************************************************************  
* ���ͼ����  
******************************************************************************/
LONG MzDib::GetWidth()
{
    return GetImageWidth();
}


/******************************************************************************  
* ���ͼ��߶�  
******************************************************************************/
LONG MzDib::GetHeight()
{
    return GetImageHeight();  
}


/******************************************************************************  
* �ر�ͼ��  
******************************************************************************/
void MzDib::Close()
{
    if(bloaded)
    {
        UnloadImage();
    }
}


/******************************************************************************  
* ��ͼ��  
******************************************************************************/
BOOL MzDib::Open(PCTSTR pzFileName)
{
    bloaded = LoadImageW(pzFileName,true,false,false);
    return bloaded;
}


/******************************************************************************  
* ����λͼͼ��  
******************************************************************************/

/******************************************************************************  
* ���λͼ����  
******************************************************************************/
BYTE * MzDib::GetBits(RECT *prcRegion)
{
    RECT rc;
    if(prcRegion == NULL){
        rc.left = 0; rc.right = GetImageWidth();
        rc.top = 0; rc.bottom = GetImageHeight();
    }else{
        rc.left = prcRegion->left; rc.right = prcRegion->right;
        rc.top = prcRegion->top; rc.bottom = prcRegion->bottom;
    }
    if(lpBitmapBits){
        delete lpBitmapBits;
        lpBitmapBits = NULL;
    }
    HDC hScrDC = GetDC();//CreateCompatibleDC(NULL);
//    HBITMAP hBitmap = CreateBitmap();
//    SelectObject(hScrDC, hBitmap);
    HDC hMemDC = CreateCompatibleDC(hScrDC);

	BITMAPINFO RGB24BitsBITMAPINFO;
	ZeroMemory(&RGB24BitsBITMAPINFO, sizeof(BITMAPINFO));
	RGB24BitsBITMAPINFO.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	RGB24BitsBITMAPINFO.bmiHeader.biWidth = RECT_WIDTH(rc);
	RGB24BitsBITMAPINFO.bmiHeader.biHeight = RECT_HEIGHT(rc);
	RGB24BitsBITMAPINFO.bmiHeader.biPlanes = 1;
	RGB24BitsBITMAPINFO.bmiHeader.biBitCount = 24;

	//use the function CreateDIBSection and SelectObject 
	//in order to get the bimap pointer : lpBitmapBits
	HBITMAP directBmp = CreateDIBSection(hMemDC, (BITMAPINFO*)&RGB24BitsBITMAPINFO,
		DIB_RGB_COLORS, (void **)&lpBitmapBits, NULL, 0);
	HGDIOBJ previousObject = SelectObject(hMemDC, directBmp);

	// copy the screen dc to the memory dc
    BitBlt(hMemDC, 0, 0, RECT_WIDTH(rc), RECT_HEIGHT(rc), hScrDC, rc.left,rc.top, SRCCOPY);

    return lpBitmapBits;
}


/******************************************************************************  
* λͼ���ݴ�С  
******************************************************************************/
int MzDib::GetBiBitCount()
{
    return 24;
}