/******************************************************************************  
* 文件名：BarRecog.cpp  
* 功能：实现对条形码的识别  
* modified by PRTsinghua@hotmail.com  
******************************************************************************/

#include "BarRecog.h"


/******************************************************************************  
* 构造函数  
******************************************************************************/
MzBarDecoder::MzBarDecoder()
{
    //置相应初始变量为空
    ImageArray = NULL;
    m_tOriPixelArray = NULL;

    ImageWidth  = 0;
    ImageHeight = 0;
}


/******************************************************************************  
* 析构函数  
******************************************************************************/
MzBarDecoder::~MzBarDecoder()
{
	int i;
	//检测是否为空，不为空则要释放内存
	if(m_tOriPixelArray!=NULL)
	{
		for(i=0; i<ImageHeight; i++)
			if(m_tOriPixelArray[i]!=NULL) 
				delete m_tOriPixelArray[i];
		delete m_tOriPixelArray;
	}
	
	if(ImageArray!=NULL)
	{
		for(i=0; i<ImageHeight; i++)
			if(ImageArray[i]!=NULL) 
				delete ImageArray[i];
		delete ImageArray;
	}
}

BOOL MzBarDecoder::DecodeFromFile(PCTSTR FileName,DECODEPARA_ptr pPara){
    BOOL bRet = false;
    if(FileName == NULL) return bRet;

    DECODEPARA_t param;
    if(pPara == NULL) {
        param.dwThrehold = 128;
        param.dwScanRegion.left = 0; param.dwScanRegion.right = m_dib.GetImageWidth();
        param.dwScanRegion.top = 0; param.dwScanRegion.bottom = m_dib.GetImageHeight();
    }else{
        param.dwThrehold = pPara->dwThrehold;
        param.dwScanRegion = pPara->dwScanRegion;
    }
    if(LoadImage(FileName)){
        GrayImage(&param.dwScanRegion);
        BinaryImage(param.dwThrehold);
        if(PreProcess()){
            bRet = Recognize();
        }
    }
    return bRet;
}
/******************************************************************************
* 功能：装载图像 参数：FilePathName－图像文件路径名构造函数
*/
BOOL MzBarDecoder::LoadImage(PCTSTR FilePathName) {
    int i;
    //检测是否为空，不为空则要释放内存
    if(m_tOriPixelArray!=NULL){
        for(i=0; i<ImageHeight; i++)
            if(m_tOriPixelArray[i]!=NULL)
                delete m_tOriPixelArray[i];
		delete m_tOriPixelArray;
    }
    if(ImageArray!=NULL) {
        for(i=0; i<ImageHeight; i++)
            if(ImageArray[i]!=NULL)
                delete ImageArray[i];
		delete ImageArray;
    }
    return m_dib.Open(FilePathName);
}

void MzBarDecoder::GrayImage(RECT *prcRegion){
    int i,j;
    RECT rc;
    if(prcRegion == NULL){
        rc.left = 0; rc.right = m_dib.GetImageWidth();
        rc.top = 0; rc.bottom = m_dib.GetImageHeight();
        ImageWidth = m_dib.GetWidth();
        ImageHeight = m_dib.GetHeight();
    }else{
        rc.left = prcRegion->left; rc.right = prcRegion->right;
        rc.top = prcRegion->top; rc.bottom = prcRegion->bottom;
        ImageWidth = RECT_WIDTH(rc);
        ImageHeight = RECT_HEIGHT(rc);
    }

    BYTE    *colorTable;   
    colorTable = m_dib.GetBits(&rc);   

    int byteBitCount  = m_dib.GetBiBitCount()/8;

    m_tOriPixelArray  = new RGBQUAD*[ImageHeight];

    ImageArray = new BYTE*[ImageHeight];
    for(int l = 0 ; l < ImageHeight; l++) {
        m_tOriPixelArray[l] = new RGBQUAD[ImageWidth];
        ImageArray[l] = new BYTE[ImageWidth];
    }
    int count=0;
    for(i = ImageHeight-1; i >= 0; i--)
    {
        for(j = 0; j < ImageWidth; j++)
        {
            m_tOriPixelArray[i][j].rgbBlue = colorTable[count++];
            m_tOriPixelArray[i][j].rgbGreen = colorTable[count++]; 
            m_tOriPixelArray[i][j].rgbRed = colorTable[count++]; 
            m_tOriPixelArray[i][j].rgbReserved=0;
            //灰度化
            ImageArray[i][j] = (BYTE)((
                (unsigned int)m_tOriPixelArray[i][j].rgbBlue*11 + 
                (unsigned int)m_tOriPixelArray[i][j].rgbGreen*59 +
                (unsigned int)m_tOriPixelArray[i][j].rgbRed*30
                ) / 100);
            count += byteBitCount-3;
        }
		count += (4-(ImageWidth*byteBitCount)%4)%4;
    }
}

/*******************************************************************************
*功能：二值化图像
********************************************************************************/
void MzBarDecoder::BinaryImage(BYTE threhold)
{
	//未装载图像时不进行任何操作
	if(ImageArray == NULL)
		return;

	int i, j;
	for(i=0; i<ImageHeight; i++)
	{
		for(j=0; j<ImageWidth; j++)
		{
			if(ImageArray[i][j] >= threhold)
				ImageArray[i][j] = (BYTE)0;
			else 
				ImageArray[i][j] = (BYTE)1;
		}
	}
}


/******************************************************************************  
* 功能：对条形码图像进行预处理，得出宽度序列  
******************************************************************************/
BOOL MzBarDecoder::PreProcess()
{
    int i, j;
    int tempMax;
    int tempArray[1000];

    //进行水平方向和垂直方向上的直方图统计   
    for(i=0; i<ImageHeight; i++)
        arPixelV[i]=0;
    for(i=0; i<ImageWidth; i++)
        arPixelH[i]=0;
	for(i=0; i<ImageHeight; i++)
	{
		for(j=0; j<ImageWidth; j++)
		{
			if(ImageArray[i][j] == 1)
			{
				arPixelV[i] += 1;
				arPixelH[j] += 1;
			}
		}
	}

	//寻找包含条形码的区域，
	//线寻找水平方向上黑象素最大的行
	
	tempMax = 0;
	for(i=0; i<ImageHeight; i++)
	{
		if(arPixelV[i]>tempMax)
			tempMax = arPixelV[i];
		arMark[i] = false;
	}
	
	for(i=0; i<ImageHeight-1; i++)
	{
		//计算差分
		arDifference[i] = arPixelV[i+1] - arPixelV[i];

		//如果该行像素足够多且变化不大，标记为true
		if( (abs(arDifference[i])<20) && (arPixelV[i]>(0.75*tempMax)) )
			arMark[i] = true;
	}
	
	//确定包含条码的行
	int iLengthThrehold = 40;
	int iCount;
	for(i=0; i<ImageHeight-iLengthThrehold; i++)
	{
		iCount = 0;
		for(j=0; j<iLengthThrehold; j++)
		{
			if(arMark[i+j] == true)
				iCount++;
		}
		if(iCount >= 37)
		{
			ImageTop = i+10;		//确定顶部
			break;
		}
	}

	for(i=ImageHeight-1; i>=iLengthThrehold-1; i--)
	{
		iCount = 0;
		for(j=0; j<iLengthThrehold; j++)
		{
			if(arMark[i-j] == true)
				iCount++;
		}
		if(iCount >= 37)	//iLengthThrehold-3
		{
			ImageBottom = i-10;		//确定底部
			break;
		}
	}
	
	//寻找左边缘,为了保证鲁棒性，在已经确定的上下边界内全局搜索
	for(i=ImageTop; i<=ImageBottom; i++)
	{
		for(j=20; j<ImageWidth; j++)
		{
			if( (ImageArray[i][j-1]==0) && (ImageArray[i][j]==1) )
			{
				arLeftEdge[i] = j;
				break;
			}
		}
	}
	
	//为消除噪声的干扰，下面确定准确的左边界
	tempMax = 0;
	int iMax = 0;
	for(i=ImageTop; i<=ImageBottom; i++)
	{
		if(arLeftEdge[i] > tempMax)
		{
			tempMax = arLeftEdge[i];
			iMax = i;
		}
	}
	
	//倾斜度不能大于1/10
	iCount = 0;
	for(i=ImageTop; i<=ImageBottom; i++)
	{
		if( abs(tempMax-arLeftEdge[i]) < abs(i-iMax)/6+1 )
		{
			iCount++;
		}
	}
	
    double rate = (double)iCount/(ImageBottom-ImageTop);
	if( ((double)iCount/(ImageBottom-ImageTop))<0.6 )
		return false;
	
	//调整起点
	for(i=iMax; i>ImageTop; i--)
	{
		if( abs(arLeftEdge[i]-arLeftEdge[i-1])>=2 )
		{
			if(ImageArray[i-1][arLeftEdge[i]]-ImageArray[i-1][arLeftEdge[i]-1] == 1)
				arLeftEdge[i-1] = arLeftEdge[i];
			else if(ImageArray[i-1][arLeftEdge[i]-1]-ImageArray[i-1][arLeftEdge[i]-2] == 1)
				arLeftEdge[i-1] = arLeftEdge[i]-1;
			else if(ImageArray[i-1][arLeftEdge[i]+1]-ImageArray[i-1][arLeftEdge[i]] == 1)
				arLeftEdge[i-1] = arLeftEdge[i]+1;
			else
				arLeftEdge[i-1] = arLeftEdge[i];
		}
	}
	
	for(i=iMax; i<ImageBottom; i++)
	{
		if(i == ImageBottom)
			break;
	
		if( abs(arLeftEdge[i]-arLeftEdge[i+1])>=2 )
		{
			if(ImageArray[i+1][arLeftEdge[i]]-ImageArray[i+1][arLeftEdge[i]-1] == 1)
				arLeftEdge[i+1] = arLeftEdge[i];
			else if(ImageArray[i+1][arLeftEdge[i]-1]-ImageArray[i+1][arLeftEdge[i]-2] == 1)
				arLeftEdge[i+1] = arLeftEdge[i]-1;
			else if(ImageArray[i+1][arLeftEdge[i]+1]-ImageArray[i+1][arLeftEdge[i]] == 1)
				arLeftEdge[i+1] = arLeftEdge[i]+1;
			else
				arLeftEdge[i+1] = arLeftEdge[i];
		}
	}
	
	int n;
	//搜索出所有的宽度
	for(n=0; n<29; n++)
	{
		//搜索条的右边缘
		for(i=ImageTop; i<=ImageBottom; i++)
		{
			for(j = arLeftEdge[i]+1; j<ImageWidth; j++)
			{
				if( (ImageArray[i][j-1]==1) && (ImageArray[i][j]==0) )
				{
					arLeftEdge1[i] = j;
					break;
				}
			}
			arDelta[i] = arLeftEdge1[i] - arLeftEdge[i];
		}
		
		//假定条和空的宽度最多为11
		//排序，可以认为最中间的5个宽度是平均宽度
		for(i=ImageTop; i<ImageBottom; i++)
			tempArray[i] = arDelta[i];
		
		for(i=ImageTop; i<ImageBottom; i++)
		{
			for(j=ImageBottom; j>i; j--)
			{
				int tempSwap;
				if(tempArray[j] < tempArray[j-1])
				{
					tempSwap = tempArray[j];
					tempArray[j] = tempArray[j-1];
					tempArray[j-1] = tempSwap;
				}
			}
		}
		
		if(tempArray[ImageTop+(ImageBottom-ImageTop)/2+2]-tempArray[ImageTop+(ImageBottom-ImageTop)/2-2]>1)
			return false;
		else
			arWidth[2*n] = tempArray[ImageTop+(ImageBottom-ImageTop)/2];
		
		//调整下一列边缘
		for(i=ImageTop; i<=ImageBottom; i++)
		{
			if(abs(arDelta[i] - arWidth[2*n])>2)
				arLeftEdge1[i] = arLeftEdge[i] + arWidth[2*n];
			arLeftEdge[i] = arLeftEdge1[i];
		}
		
		//搜索空的右边缘
		for(i=ImageTop; i<=ImageBottom; i++)
		{
			for(j = arLeftEdge[i]+1; j<ImageWidth; j++)
			{
				if( (ImageArray[i][j-1]==0) && (ImageArray[i][j]==1) )
				{
					arLeftEdge1[i] = j;
					break;
				}
			}
			arDelta[i] = arLeftEdge1[i] - arLeftEdge[i];
		}
		
		//假定条和空的宽度最多为11
		//排序，可以认为最中间的5个宽度是平均宽度
		for(i=ImageTop; i<ImageBottom; i++)
			tempArray[i] = arDelta[i];
		
		for(i=ImageTop; i<ImageBottom; i++)
		{
			for(j=ImageBottom; j>i; j--)
			{
				int tempSwap;
				if(tempArray[j] < tempArray[j-1])
				{
					tempSwap = tempArray[j];
					tempArray[j] = tempArray[j-1];
					tempArray[j-1] = tempSwap;
				}
			}
		}
		
		if(tempArray[ImageTop+(ImageBottom-ImageTop)/2+2]-tempArray[ImageTop+(ImageBottom-ImageTop)/2-2]>1)
			return false;
		else
			arWidth[2*n+1] = tempArray[ImageTop+(ImageBottom-ImageTop)/2];
		
		//调整下一列边缘
		for(i=ImageTop; i<=ImageBottom; i++)
		{
			if(abs(arDelta[i] - arWidth[2*n+1])>2)
				arLeftEdge1[i] = arLeftEdge[i] + arWidth[2*n+1];
			arLeftEdge[i] = arLeftEdge1[i];
		}
	}
	
	//搜索最后一个条的右边缘
	for(i=ImageTop; i<=ImageBottom; i++)
	{
		for(j = arLeftEdge[i]+1; j<ImageWidth; j++)
		{
			if( (ImageArray[i][j-1]==1) && (ImageArray[i][j]==0) )
			{
				arLeftEdge1[i] = j;
				break;
			}
		}
		arDelta[i] = arLeftEdge1[i] - arLeftEdge[i];
	}
	
	//假定条和空的宽度最多为11
	//排序，可以认为最中间的5个宽度是平均宽度
	for(i=ImageTop; i<ImageBottom; i++)
		tempArray[i] = arDelta[i];
	
	for(i=ImageTop; i<ImageBottom; i++)
	{
		for(j=ImageBottom; j>i; j--)
		{
			int tempSwap;
			if(tempArray[j] < tempArray[j-1])
			{
				tempSwap = tempArray[j];
				tempArray[j] = tempArray[j-1];
				tempArray[j-1] = tempSwap;
			}
		}
	}
	
	if(tempArray[ImageTop+(ImageBottom-ImageTop)/2+2]-tempArray[ImageTop+(ImageBottom-ImageTop)/2-2]>1)
		return false;
	else
		arWidth[2*n] = tempArray[ImageTop+(ImageBottom-ImageTop)/2];
	
	//调整下一列边缘
	for(i=ImageTop; i<=ImageBottom; i++)
	{
		if(abs(arDelta[i] - arWidth[2*n+1])>2)
			arLeftEdge1[i] = arLeftEdge[i] + tempArray[ImageTop+(ImageBottom-ImageTop)/2];
		arLeftEdge[i] = arLeftEdge1[i];
	}

	return true;
}



/******************************************************************************  
* 识别  
******************************************************************************/
BOOL MzBarDecoder::Recognize()
{
	//总共有7×12＋3×2＋5＝ 95个单位宽度
	//有4×12＋3×2＋5＝59个宽度，
	int i;
	int result[12];
	double mx = 0.0;	//平均宽度

	for(i=0; i<59; i++)
		mx += (double)arWidth[i];
	mx /= 95.0;

	//起始条文
	for(i=0; i<3; i++)
	{
		double dTemp = (double)arWidth[i]/mx;
		if( dTemp<0.6 || dTemp>1.4 )
			break;
	}
	//起始码不符合要求
	//if(i<3)
	//	return false;
	
	//识别前6个
	for(i=0; i<6; i++)
	{
		result[i] = JudgNum(arWidth[i*4+3], arWidth[i*4+4], arWidth[i*4+5], arWidth[i*4+6], mx);
	}
	//识别后6个
	for(i=6; i<12; i++)
	{
		result[i] = JudgNum(arWidth[i*4+8], arWidth[i*4+9], arWidth[i*4+10], arWidth[i*4+11], mx);
	}

    //判断码制
    if( result[0]==7 && result[1]==7 )
    {
        strCodeStyle = L"ISSN";
    }
    else if( result[0]==7 && result[1]==8 )
    {
        strCodeStyle = L"ISBN";
    }
    else
        strCodeStyle = L"Unknown!";

    //判断是否全部识别出来
    for(i=0; i<12; i++)
        if(result[i] == -1)
            return false;

    CMzString strTemp;
    RETAILMSG(1,(L"%s:",strCodeStyle.C_Str()));
    //strCodeNumber.Format("");
    for(i=0; i<12; i++)
    {
        RETAILMSG(1,(L"%d,",result[i]));
        //strTemp.Format("%d", result[i]);
        //strCodeNumber += strTemp;
    }
    RETAILMSG(1,(L"\n"));
    return true;
}


/******************************************************************************  
* 根据宽度数组判断数字  
******************************************************************************/
int MzBarDecoder::JudgNum(int w1, int w2, int w3, int w4, double mx)
{
	double a1, a2, a3;
	int ia1, ia2, ia3;
	a1 = (double)(w1+w2)/mx;
	a2 = (double)(w2+w3)/mx;
	a3 = (double)(w3+w4)/mx;
	ia1 = (int)(a1+0.5);
	ia2 = (int)(a2+0.5);
	ia3 = (int)(a3+0.5);
		
	//判断该码值
	if( (ia1==5 && ia2==3 && ia3==2) || (ia1==2 && ia2==3 && ia3==5) )
		return 0;
	
	if( (ia1==4 && ia2==4 && ia3==3) || (ia1==3 && ia2==4 && ia3==4) )
	{
		if(ia1 == 4)
		{
			double dw2 = (double)w2/mx;
			if(dw2 < 2.4)
				return 1;
			else if(dw2 > 2.6)
				return 7;
			else return -1;
		}
	
		if(ia1 == 3)
		{
			double dw3 = (double)w3/mx;
			if(dw3 < 2.4)
				return 1;
			else if(dw3 > 2.6)
				return 7;
			else return -1;
		}
	}
	
	if( (ia1==3 && ia2==3 && ia3==4) || (ia1==4 && ia2==3 && ia3==3) )
	{
		if(ia1 == 3)
		{
			double dw4 = (double)w4/mx;
			if(dw4 < 2.4)
				return 2;
			else if(dw4 > 2.6)
				return 8;
			else return -1;
		}
	
		if(ia1 == 4)
		{
			double dw1 = (double)w1/mx;
			if(dw1 < 2.4)
				return 2;
			else if(dw1 > 2.6)
				return 8;
			else return -1;
		}
	}
	
	if( (ia1==5 && ia2==5 && ia3==2) || (ia1==2 && ia2==5 && ia3==5) )
		return 3;
	
	if( (ia1==2 && ia2==4 && ia3==5) || (ia1==5 && ia2==4 && ia3==2) )
		return 4;
	
	if( (ia1==3 && ia2==5 && ia3==4) || (ia1==4 && ia2==5 && ia3==3) )
		return 5;
	
	if( (ia1==2 && ia2==2 && ia3==5) || (ia1==5 && ia2==2 && ia3==2) )
		return 6;
	
	if( (ia1==4 && ia2==2 && ia3==3) || (ia1==3 && ia2==2 && ia3==4) )
		return 9;

    return false;
}