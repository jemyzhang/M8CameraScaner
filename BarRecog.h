/****************************************************************************** 
 * �ļ�����BarRecog.h 
 * ���ܣ�ʵ�ֶ��������ʶ�� 
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
    //���캯��/�������� 
	MzBarDecoder(); 
	virtual ~MzBarDecoder(); 
private: 
	int JudgNum(int w1, int w2, int w3, int w4, double mx); 
	CMzString strCodeNumber; 
	CMzString strCodeStyle; 
	BOOL PreProcess(void); 
	BOOL Recognize(); 
 
	//��Ա���� 
	BYTE** ImageArray;				//ԭʼ��ͼ���������飭�Ҷ� 
	RGBQUAD** m_tOriPixelArray;		//ԭʼλͼ���� 
 
	long ImageHeight;				//ԭͼ��ĸ߶� 
	long ImageWidth;				//ԭͼ��Ŀ�� 
 
	int arPixelH[1000];				//Ϊ��ˮƽ����ֱ��ͼͳ���� 
	int arPixelV[1000];				//Ϊ�˴�ֱ����ֱ��ͼͳ���� 
	int arWidth[200];				//���������У�������ߵĺ�����ʼ 
	int arDifference[1000];			//��� 
	bool arMark[1000];				//��� 
	int ImageLeft;					//ͼ������ߵ����� 
	int ImageRight;					//ͼ�����ұߵ����� 
	int ImageTop;					//�����붥�������� 
	int ImageBottom;				//�������²������� 
 
	MzDib m_dib; 
	 
	//���� 
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