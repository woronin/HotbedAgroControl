#include "ArithmeticMean.h"

CArithmeticMean::CArithmeticMean(void):
    CArrayFilter()
{
}

CArithmeticMean::~CArithmeticMean(void)
{
}

void CArrayFilter::Put(double &d, int n, bool bType)
{
    double dTmp;
    if (bType) {
        if (n!=0) {
            if (n==nLength) {
                nSumLeft +=d;
                nSumRight -= *pnCurN;
                *pnCurN = d;
                pnCurN++;
                if (pnCurN>pnEnd) {
                    pnCurN=pnBegin;
                    dTmp = nSumRight;
                    nSumRight = nSumLeft;
                    nSumLeft = dTmp;
                }
            }
            else {
                pnBegin = &aFilter[0];
                pnCurN = pnBegin;
                pnEnd = pnCurN+n;
                nLength = n;
                int i=0;
                nSumLeft = nSumRight = 0;
                while (pnCurN<=pnEnd) {
                    *pnCurN = d;
                    pnCurN++;
                    i++;
                    nSumRight +=d;
                }
                pnCurN = pnBegin;
            }
        }
        else {
            nSumLeft =d;
            nSumRight = 0;
        }
    }
    else {
        if (n!=0) {
            if (n==nLength) {
                *pnCurN = d;
                pnCurN++;
                if (pnCurN>pnEnd) {
                    pnCurN=pnBegin;
                }
            }
            else {
                pnBegin = &aFilter[0];
                pnCurN = pnBegin;
                pnEnd = pnCurN+n;
                nLength = n;
                while (pnCurN<=pnEnd) {
                    *pnCurN = d;
                    pnCurN++;
                }
                pnCurN = pnBegin;
            }
        }
        else {
            nSumLeft =d;
            nSumRight = 0;
        }
    }
}

double CArrayFilter::Get(int n, bool bType)
{
    double d=0;
    if (bType) {
        if (n!=0) {
            if (n==nLength) {
                d = (nSumLeft+nSumRight)/(nLength+1);
            }
        }
        else {
            d = nSumLeft;
        }
    }
    else {
        if (n!=0) {
//			d = aFilter[n-1];
            d = *pnCurN;
        }
        else {
            d = nSumLeft;
        }
    }
    return d;
}
//*/
