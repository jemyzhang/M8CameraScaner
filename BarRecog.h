/****************************************************************************** 
 * 文件名：BarRecog.h 
 * 功能：实现对条形码的识别 
 * modified by PRTsinghua@hotmail.com 
******************************************************************************/ 
 
#ifndef AFX_BARRECOG_H 
#define AFX_BARRECOG_H 

#include "Dib.h" 

typedef struct tagDECODEPARA
{
    RECT        dwScanRegion;
    BYTE        dwThrehold;
} DECODEPARA_t, *DECODEPARA_ptr;

class MzBarDecoder   
{ 
public:
    //构造函数/析构函数 
	MzBarDecoder(); 
	virtual ~MzBarDecoder(); 
private: 
	int JudgNum(int w1, int w2, int w3, int w4, double mx); 
	CMzString strCodeNumber; 
	CMzString strCodeStyle; 
	BOOL PreProcess(void); 
	BOOL Recognize(); 
 
	//成员变量 
	BYTE** ImageArray;				//原始的图像数据数组－灰度 
	RGBQUAD** m_tOriPixelArray;		//原始位图数据 
 
	long ImageHeight;				//原图像的高度 
	long ImageWidth;				//原图像的宽度 
 
	int arPixelH[1000];				//为了水平方向直方图统计用 
	int arPixelV[1000];				//为了垂直方向直方图统计用 
	int arWidth[200];				//保存宽度序列：从最左边的黑条开始 
	int arDifference[1000];			//差分 
	bool arMark[1000];				//标记 
	int ImageLeft;					//图像最左边的座标 
	int ImageRight;					//图像最右边的座标 
	int ImageTop;					//条形码顶部的座标 
	int ImageBottom;				//条形码下部的座标 
 
	MzDib m_dib; 
	 
	//操作 
	BOOL LoadImage(PCTSTR FilePathName); 
	void BinaryImage(BYTE threhold = 128);
    void GrayImage(RECT *prcRegion);
public:
    //Decode From File
    BOOL DecodeFromFile(PCTSTR FileName,DECODEPARA_ptr pPara = NULL);
 
protected:	 
	int arDelta[1000]; 
	int arLeftEdge1[1000]; 
	int arLeftEdge2[1000]; 
	int arLeftEdge[1000]; 
}; 
 
#endif // !defined(AFX_BARRECOG_H) 