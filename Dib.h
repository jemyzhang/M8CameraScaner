/******************************************************************************
 * 文件名：Dib.h 
 * 功能：位图操作接口
 * modified by PRTsinghua@hotmail.com
******************************************************************************/
#if !defined(AFX_DIB_H_INCLUDED_)
#define AFX_DIB_H_INCLUDED_

// include the MZFC library header file
#include <mzfc_inc.h>

class MzDib : public ImagingHelper
{
public:
	MzDib();
	virtual ~MzDib();

public:
	BYTE    *GetBits(RECT *prcRegion = NULL);
	LONG	 GetWidth();    //need
	LONG	 GetHeight();    //need
	int      GetBiBitCount();    //need

public:
	BOOL Open(PCTSTR pzFileName);    //need
    void Close();
private:
	BYTE    *lpBitmapBits;
    BOOL    bloaded;
};
#endif // !defined(AFX_DIB_H_INCLUDED_)