#include "FDgtFilterBat.h"
//#include <cmath>
/*
static LArc TBL_DgtFilterBat[] = {
    LArc("tt","ss", "x3",	 	"y1y5y14"),		//	если ссылки истинны,
    LArc("tt","tt", "^x3",		"y15"),		//	инициализировать параметры

    LArc("ss","ss", "^x1", 		"y2y5"),	//
    LArc("ss","s4", "x1", 		"y5"),	//
    LArc("s4","s4", "--", 		"y4"),	//

    LArc(0,1, "x3",	 	"y1y5y14"),		//	если ссылки истинны,
    LArc(0,0, "^x3",		"y15"),		//	инициализировать параметры

    LArc(1,1, "^x1", 		"y2y5"),	//
    LArc(1,4, "x1", 		"y5"),	//
    LArc(4,4, "--", 		"y4"),	//

    LArc()
};
*/
void FDgtFilterBat::run() {
  if (nState==0 && x3()) { y1(); y5(); nTmpState=1; } 
  if (nState==0 && !x3()) { } 
  if (nState==1 && !x1()) { y2(); y5(); } 
  if (nState==1 && x1()) { y5(); nTmpState=4; } 
  if (nState==4) { y4(); } 
  
  if (nTmpState != nState) { nState = nTmpState; }
}

FDgtFilterBat::FDgtFilterBat(String strNam):
    LFsaAppl(strNam)
{
    pVarX = pVarY = nullptr;
    strNameObject = strNam;
    dPrecision = 0;

    a6_35[0]	= 1;
    a6_35[1]	= -2.73629493116515;
    a6_35[2]	= 2.50622421122369;
    a6_35[3]	= -0.767907452390706;
    b6_35[0]	= 0.000252728458479901;
    b6_35[1]	= 0.000758185375439702;
    b6_35[2]	= 0.000758185375439702;
    b6_35[3]	= 0.000252728458479901;

    a6_10[0]	= 1;
    a6_10[1]	= -2.25008508172639;
    a6_10[2]	= 1.75640138178595;
    a6_10[3]	= -0.468312111171712;
    b6_10[0]	= 0.00475052361098088;
    b6_10[1]	= 0.0142515708329427;
    b6_10[2]	= 0.0142515708329427;
    b6_10[3]	= 0.00475052361098088;


    a2_35[0]	= 1;
    a2_35[1]	= -2.91204248342765;
    a2_35[2]	= 2.8279107470461;
    a2_35[3]	= -0.91578680426307;
    b2_35[0]	= 1.01824194213562e-005;
    b2_35[1]	= 3.05472582640687e-005;
    b2_35[2]	= 3.05472582640687e-005;
    b2_35[3]	= 1.01824194213562e-005;

    a2_10[0]	= 1;
    a2_10[1]	= -2.74883580921468;
    a2_10[2]	= 2.52823121914256;
    a2_10[3]	= -0.77763856023808;
    b2_10[0]	= 0.000219606211225409;
    b2_10[1]	= 0.000658818633676228;
    b2_10[2]	= 0.000658818633676228;
    b2_10[3]	= 0.000219606211225409;

    nCountLength = 0;
    dPrecision = 0.0;
    nTypeFilter = 2;
}

FDgtFilterBat::~FDgtFilterBat(void)
{
}
// ПРЕДИКАТЫ
// рестарт?
int FDgtFilterBat::x1() {
    return nLength-2<=nCountLength;
}
// время дискретизации истекло
int FDgtFilterBat::x2() { return false; }
// ссылки входов/выходов установлены?
int FDgtFilterBat::x3() { return pVarX&&pVarY; }
// изменилось значение интервала таймера?
int FDgtFilterBat::x4() { return false; }
// ДЕЙСТВИЯ
// сбросить флаг синхронизации (данные приняты!)
void FDgtFilterBat::y1() {
    double d = *pVarX;
    if (nTypeFilter==0)
        nLength = 4;
    if (nTypeFilter==1)
        nLength = 4;
    if (nTypeFilter==2)
        nLength = 4;
    if (nTypeFilter==3)
        nLength = 4;
    else
        nLength = 4;
}
// сброить импульс: задний фронт импульса сигнала времени
void FDgtFilterBat::y2() { nCountLength++; }
//
void FDgtFilterBat::y3() { }
//
void FDgtFilterBat::y4() {
    double d = *pVarX;
    if (nTypeFilter == 0) { nLength = 4; d=filter6_35(d); }
    else if (nTypeFilter==1) { nLength = 4; d=filter6_10(d); }
    else if (nTypeFilter==2) { nLength = 4; d=filter2_35(d); }
    else if (nTypeFilter==3) { nLength = 4; d=filter2_10(d); }
    else { nLength = 4; }
    double dres = d;
    double dPrec;
    if (dPrecision<1&&dPrecision>0) {
        dPrec = 1/dPrecision;
        d +=(0.5/dPrec);
        modf(d*dPrec, &dres);
        dres /=dPrec;
    }
    else if (dPrecision>1) {
        dPrec = dPrecision;
        d /= dPrecision;
        d +=0.5;
        modf(d, &dres);
        dres *=dPrec;
    }
    else if (int(dPrecision)==1) {
        modf(d, &dres);
    }
        *pVarY = dres;
}
// запомнить отсечку времени такта
void FDgtFilterBat::y5() {
    double d = *pVarX;	// задание
    NCofF nc(d,d);
    aArray4T_1.Add(nc, nLength);
}
double FDgtFilterBat::filter6_35(double d)
{
    double yi=d;
//    double fi=0;
    double f1=0, f2=0, f3=0, f4=0, f5=0, f6=0, f7 =0;
    NCofF nc(0,yi);
    aArray4T_1.Add(nc, nLength);
    f1 = b6_35[0] * (aArray4T_1.aCof[0].dY);// i=0  b(0)*y(i)
    f2 = b6_35[1] * aArray4T_1.aCof[1].dY;	// i-1	b(1)*y(i-1)
    f3 = b6_35[2] * aArray4T_1.aCof[2].dY;	// i-2	b(2)*y(i-2)
    f4 = b6_35[3] * aArray4T_1.aCof[3].dY;	// i-3	b(0)*y(i-3)
    f5 = a6_35[1] * aArray4T_1.aCof[1].dF;	// i=0  a(1)*f(i-1)
    f6 = a6_35[2] * aArray4T_1.aCof[2].dF;	// i=1  a(2)*f(i-2)
    f7 = a6_35[3] * aArray4T_1.aCof[3].dF;	// i=2  a(3)*f(i-3)
    aArray4T_1.aCof[0].dF =
        f1 + f2	+ f3 + f4 - f5 - f6	- f7;	//	f(i) = ...
    return aArray4T_1.aCof[0].dF;
}

double FDgtFilterBat::filter6_10(double d)
{
    double yi=d;
//	double fi=0;
    double f1=0, f2=0, f3=0, f4=0, f5=0, f6=0, f7 =0;
    NCofF nc(0,yi);
    aArray4T_1.Add(nc, nLength);
    f1 = b6_10[0] * (aArray4T_1.aCof[0].dY); //i=0
    f2 = b6_10[1] * aArray4T_1.aCof[1].dY;	// i-1
    f3 = b6_10[2] * aArray4T_1.aCof[2].dY;	// i-2
    f4 = b6_10[3] * aArray4T_1.aCof[3].dY;	// i-3
    f5 = a6_10[1] * aArray4T_1.aCof[1].dF;
    f6 = a6_10[2] * aArray4T_1.aCof[2].dF;
    f7 = a6_10[3] * aArray4T_1.aCof[3].dF;
    aArray4T_1.aCof[0].dF =
        f1 + f2	+ f3 + f4 - f5 - f6	- f7;
    return aArray4T_1.aCof[0].dF;
}

double FDgtFilterBat::filter2_35(double d)
{
    double yi=d;
//    double fi=0;
    double f1=0, f2=0, f3=0, f4=0, f5=0, f6=0, f7 =0;
    NCofF nc(0,yi);
    aArray4T_1.Add(nc, nLength);
    f1 = b2_35[0] * (aArray4T_1.aCof[0].dY); //i=0
    f2 = b2_35[1] * aArray4T_1.aCof[1].dY;	// i-1
    f3 = b2_35[2] * aArray4T_1.aCof[2].dY;	// i-2
    f4 = b2_35[3] * aArray4T_1.aCof[3].dY;	// i-3
    f5 = a2_35[1] * aArray4T_1.aCof[1].dF;
    f6 = a2_35[2] * aArray4T_1.aCof[2].dF;
    f7 = a2_35[3] * aArray4T_1.aCof[3].dF;
    aArray4T_1.aCof[0].dF =
        f1 + f2	+ f3 + f4 - f5 - f6	- f7;

    Serial.printf("Bat:Put=%.2f;Get=%.2f\n", d, aArray4T_1.aCof[0].dF);

    return aArray4T_1.aCof[0].dF;
}

double FDgtFilterBat::filter2_10(double d)
{
    double yi=d;
//    double fi=0;
    double f1=0, f2=0, f3=0, f4=0, f5=0, f6=0, f7 =0;
    NCofF nc(0,yi);
    aArray4T_1.Add(nc, nLength);
    f1 = b2_10[0] * (aArray4T_1.aCof[0].dY); //i=0
    f2 = b2_10[1] * aArray4T_1.aCof[1].dY;	// i-1
    f3 = b2_10[2] * aArray4T_1.aCof[2].dY;	// i-2
    f4 = b2_10[3] * aArray4T_1.aCof[3].dY;	// i-3
    f5 = a2_10[1] * aArray4T_1.aCof[1].dF;
    f6 = a2_10[2] * aArray4T_1.aCof[2].dF;
    f7 = a2_10[3] * aArray4T_1.aCof[3].dF;
    aArray4T_1.aCof[0].dF =
        f1 + f2	+ f3 + f4 - f5 - f6	- f7;
    return aArray4T_1.aCof[0].dF;
}

void CArrayFilt::Put(double &d, int n, bool bType)
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

double CArrayFilt::Get(int n, bool bType)
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

void NArrayCofF::Add(NCofF &c, int nL)
{
    for (int i=1; i<nL; i++) {
        aCof[nL-i] = aCof[nL-(i+1)];
    }
    aCof[0] = c;
    if (nFull<6)
        nFull++;
}

