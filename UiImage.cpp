#include "UiImage.h"

void UiImage::PaintWin(HDC hdcDst, RECT* prcWin, RECT* prcUpdate){
    UiWin::PaintWin(hdcDst,prcWin,prcUpdate);
    if(pimg){
        pimg->Draw(hdcDst,prcWin,true,true);
    }
}
