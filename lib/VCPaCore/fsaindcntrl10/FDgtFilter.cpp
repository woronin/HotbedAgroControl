#include "FDgtFilter.h"
#include "ArithmeticMean.h"
/*
static LArc TBL_DgtFilter[] = {
    LArc("tt","ss", "x3",	 	"y1y5y14"),		//	если ссылки истинны,
    LArc("tt","tt", "^x3",		"y15"),		//	инициализировать параметры

//	LArc("ss","ss", "--", 		"y4"),	//
    LArc("ss","ss", "^x1", 		"y2y5"),	//
    LArc("ss","s4", "x1", 		"y5"),	//
    LArc("s4","s4", "^x4x5", 	"y4"),	//
    LArc("s4","s5", "x4x5", 	"y4"),	//
    LArc("s5","s4", "--", 		"y6"),	//
    LArc("s4","tt", "^x5", 		"--"),	//
    LArc()
};
*/
/*
    LArc("0","1", "x3",	 	"y1y5y14"),		//	если ссылки истинны,
    LArc("0","0", "^x3",		"y15"),		//	инициализировать параметры
    LArc("1","1", "^x1", 		"y2y5"),	//
    LArc("1","4", "x1", 		"y5"),	//
    LArc("4","4", "^x4x5", 	"y4"),	//
    LArc("4","5", "x4x5", 	"y4"),	//
    LArc("5","4", "--", 		"y6"),	//
    LArc("4","0", "^x5", 		"--"),	//
*/
void FDgtFilter::run() {
  if (nState==0 && x3()) { y1(); y5(); y14(); nTmpState=1; } 
  if (nState==0 && !x3()) { y15(); } 
  if (nState==1 && !x1()) { y2(); y5(); } 
  if (nState==1 && x1()) { y5(); nTmpState=4; } 
  if (nState==4 && !x4() && x5()) { y4(); } 
  if (nState==4 && x4() && x5()) { y4(); nTmpState=5; } 
  if (nState==5 ) { y6(); nTmpState=4; } 
  if (nState==4 && !x5()) { nTmpState=0; } 
  
  if (nTmpState != nState) { nState = nTmpState; }
}

FDgtFilter::FDgtFilter(String strNam):
    LFsaAppl(strNam)
{
    pVarX = pVarY = nullptr;
    strNameObject = strNam;
    nN = 0;
//*
    Q_4[0] = 0.1593947439353;
    Q_4[1] = 0.3828261098786;
    Q_4[2] = 0.3828261098786;
    Q_4[3] = 0.1593947439353;
    nLength = 4;
//*/
//*
    Q_7[0] = 0.02775719763938;
    Q_7[1] = 0.1812435585682;
    Q_7[2] = 0.2075289019294;
    Q_7[3] = 0.2465725256835;
    Q_7[4] = 0.2075289019294;
    Q_7[5] = 0.1812435585682;
    Q_7[6] = 0.02775719763938;
    nLength = 7;
//*/
//*
    Q_22[0] = -0.004762461117288;
    Q_22[1] = 0.007440284356936;
    Q_22[2] = 0.01303283444271;
    Q_22[3] = 0.02220628062942;
    Q_22[4] = 0.03405562188429;
    Q_22[5] = 0.04772703516517;
    Q_22[6] = 0.06213296107266;
    Q_22[7] = 0.07596611045978;
    Q_22[8] = 0.08789373573557;
    Q_22[9] = 0.09665024706019;
    Q_22[10] = 0.10129200197;
    Q_22[11] = 0.10129200197;
    Q_22[12] = 0.09665024706019;
    Q_22[13] = 0.08789373573557;
    Q_22[14] = 0.07596611045978;
    Q_22[15] = 0.06213296107266;
    Q_22[16] = 0.04772703516517;
    Q_22[17] = 0.03405562188429;
    Q_22[18] = 0.02220628062942;
    Q_22[19] = 0.01303283444271;
    Q_22[20] = 0.007440284356936;
    Q_22[21] = -0.004762461117288;
    nLength = 22;
//*/
    a[0]	= 1;
    a[1]	= -2.80824697537014;
    a[2]	= 2.63445942814709;
    a[3]	= -0.825374836556498;
    b[0]	= 0.00398311415499999;
    b[1]	= -0.00356430604477575;
    b[2]	= -0.00356430604477575;
    b[3]	= 0.003983114155;
    nCountLength = 0;
    nTypeFilter = 0;
    bTimeMsec = false;
    nN = 50;
/*    
    if (bTimeMsec&&nTypeFilter==5) {
        int nT = atoi(pVarPrmProc->strParam10.c_str());
        int nD = int(pTAppCore->pCVarSetting->dDeltaTime);
        nN = nT/nD;
    }
    else {
        if (nTypeFilter==5)
            nN = atoi(pVarPrmProc->strParam10.c_str());
        else
            nN = atoi(pVarPrmProc->strParam2.c_str());
    }
*/          
    nCLK = 0;  // задержка в тактах
}

FDgtFilter::~FDgtFilter(void)
{
}
/*
bool FDgtFilter::FCreationOfLinksForVariables() {
    // входы (r-контакты)
        nTypeFilter = atoi(pVarPrmProc->strParam4.c_str());
        bTimeMsec = atoi(pVarPrmProc->strParam9.c_str());
        if (bTimeMsec&&nTypeFilter==5) {
            int nT = atoi(pVarPrmProc->strParam10.c_str());
            int nD = int(pTAppCore->pCVarSetting->dDeltaTime);
            nN = nT/nD;
        }
        else {
            if (nTypeFilter==5)
                nN = atoi(pVarPrmProc->strParam10.c_str());
            else
                nN = atoi(pVarPrmProc->strParam2.c_str());
        }

    // выходы (w-контакты)
        pVarY = pTAppCore->GetAddressVar(pVarPrmProc->strParam3.c_str(), this);
        CreateLocVar("y", CLocVar::vtDouble, "выход блока");
        CreateLocVar("x", CLocVar::vtDouble, "вход блока");
        pCVarResource = CreateLocVar("dR", CLocVar::vtDouble, "ресурс");
        pCVarResource->SetDataSrc(this, 0.0); dR = 0;
        pCVarDelayCLK = CreateLocVar("nDelayCLK", CLocVar::vtInteger, "задержка в тактах");
        nCLK = int(pCVarDelayCLK->GetDataSrc());

    return true;
}
*/
// ПРЕДИКАТЫ
// рестарт?
int FDgtFilter::x1() {
    return nLength-2<=nCountLength;
}
// время дискретизации истекло
int FDgtFilter::x2() { return false; }
// ссылки входов/выходов установлены?
int FDgtFilter::x3() {
    return pVarX&&pVarY;
}
// изменилось значение интервала таймера?
int FDgtFilter::x4() { return nCLK; }
// изменилось значение интервала таймера?
int FDgtFilter::x5() { return nSavTypeFilter == nTypeFilter; }
// ДЕЙСТВИЯ
// сбросить флаг синхронизации (данные приняты!)
void FDgtFilter::y1() {
    double d = *pVarX;
    *pVarY = d;
    if (nTypeFilter==1)
        nLength = 4;
    if (nTypeFilter==2)
        nLength = 7;
    if (nTypeFilter==3)
        nLength = 22;
    if (nTypeFilter==4)
        nLength = 4;
    else
        nLength = 0;
    nSavTypeFilter = nTypeFilter;
//    pCVarResource->SetDataSrc(this, 0.0); dR = 0;
}
// сброить импульс: задний фронт импульса сигнала времени
void FDgtFilter::y2() {
    nCountLength++;
}
//
void FDgtFilter::y3() { }
//
void FDgtFilter::y4() {
    d = *pVarX;
//    nCLK = int(pCVarDelayCLK->GetDataSrc());
    if (nTypeFilter == 0) {
        dArray.Put(d, nN, true);
        d = dArray.Get(nN, true);
//        Serial.printf("Put=%.2f;Get=%.2f\n", *pVarX, d);
    }
    else if (nTypeFilter==1) {
        nLength = 4;
        d=filter4(d);
    }
    else if (nTypeFilter==2) {
        nLength = 7;
        d=filter7(d);
    }
    else if (nTypeFilter==3) {
        nLength = 22;
        d=filter22(d);
    }
    else if (nTypeFilter==4) {
        nLength = 4;
        d=filterCh(d);
    }
    else if (nTypeFilter==5) {
        dR += d;
        dArray.Put(d, nN, false);
        d = dArray.Get(nN, false);
        dR -= d;
//        pCVarResource->SetDataSrc(this,dR);
//		pCVarResource->UpdateVariable();
    }
    if (nCLK==0) {
        if (pVarY) *pVarY = d;
    }
}
// запомнить отсечку времени такта
void FDgtFilter::y5() {
    double d = *pVarX;	// задание
    NCof nc(d,d);
    aArray4T_1.Add(nc, nLength);
//    nCLK = int(pCVarDelayCLK->GetDataSrc());
}
// запомнить отсечку времени такта
void FDgtFilter::y6() {
    if (pVarY) *pVarY = d;
}
// запомнить отсечку времени такта
void FDgtFilter::y8() {
    double d = *pVarX;	// задание
    d=filter4(d);
    *pVarY = d;	// задание
}
// число циклов = 1; число стартовых циклов = 0; сброс флага "рестарт"
void FDgtFilter::y14() {  }
// инициализация ссылок на внешние связи
void FDgtFilter::y15() { }

void NArrayCof::Add(NCof &c, int nL)
{
    for (int i=1; i<nL; i++) {
        aCof[nL-i] = aCof[nL-(i+1)];
    }
    aCof[0] = c;
    if (nFull<6)
        nFull++;
}

double FDgtFilter::filter4(double d)
{
    NCof nc(d,d);
    aArray4T_1.Add(nc, nLength);
    d=0;
    for (int i=0; i<nLength; i++) {
        d += aArray4T_1.aCof[i].dY * Q_4[i];
    }
    return d;	// задание
}

double FDgtFilter::filter7(double d)
{
    NCof nc(d,d);
    aArray4T_1.Add(nc, nLength);
    d=0;
    for (int i=0; i<nLength; i++) {
        d += aArray4T_1.aCof[i].dY * Q_7[i];
    }
    return d;	// задание
}

double FDgtFilter::filter22(double d)
{
    NCof nc(d,d);
    aArray4T_1.Add(nc, nLength);
    d=0;
    for (int i=0; i<nLength; i++) {
        d += aArray4T_1.aCof[i].dY * Q_22[i];
    }
    return d;	// задание
}

double FDgtFilter::filterCh(double d)
{
    double yi=d;
//    double fi=0;
    double f1=0, f2=0, f3=0, f4=0, f5=0, f6=0, f7 =0;
//f(i)=b(0)y(i)+b(1)y(i-1)+b(2)y(i-2)+b(3)y(i-3)-a(1)f(i-1)-a(2)f(i-2)-a(3)f(i-3)
/*
    aArray4T_1.Add(NCof(1,5), nLength);
    aArray4T_1.Add(NCof(2,6), nLength);
    aArray4T_1.Add(NCof(3,7), nLength);
    aArray4T_1.Add(NCof(4,0), nLength);

    NCof c0, c1, c2, c3, c4;
    c0 = aArray4T_1.aCof[0];
    c1 = aArray4T_1.aCof[1];
    c2 = aArray4T_1.aCof[2];
    c3 = aArray4T_1.aCof[3];
*/
    NCof nc(0,yi);
    aArray4T_1.Add(nc, nLength);
    f1 = b[0] * (aArray4T_1.aCof[0].dY); //i=0
    f2 = b[1] * aArray4T_1.aCof[1].dY;	// i-1
    f3 = b[2] * aArray4T_1.aCof[2].dY;	// i-2
    f4 = b[3] * aArray4T_1.aCof[3].dY;	// i-3
    f5 = a[1] * aArray4T_1.aCof[1].dF;
    f6 = a[2] * aArray4T_1.aCof[2].dF;
    f7 = a[3] * aArray4T_1.aCof[3].dF;
    aArray4T_1.aCof[0].dF =
        f1 + f2	+ f3 + f4 - f5 - f6	- f7;
/*
    aArray4T_1.Add(NCof(fi,yi), nLength);
    f1 = b[0] * (aArray4T_1.aCof[3].dE);
    f2 = b[1] * aArray4T_1.aCof[0].dE;
    f3 = b[2] * aArray4T_1.aCof[1].dE;
    f4 = b[3] * aArray4T_1.aCof[0].dE;
    f5 = a[1] * aArray4T_1.aCof[2].dK;
    f6 = a[2] * aArray4T_1.aCof[1].dK;
    f7 = a[3] * aArray4T_1.aCof[0].dK;
    aArray4T_1.aCof[3].dK =
        f1 + f2	+ f3 + f4 - f5 - f6	- f7;
*/
/*
    aArray4T_1.aCof[3].dK =
        b[0]*aArray4T_1.aCof[3].dE
        + b[1]*aArray4T_1.aCof[0].dE
        + b[2]*aArray4T_1.aCof[1].dE
        + b[3]*aArray4T_1.aCof[0].dE
        - a[1]*aArray4T_1.aCof[2].dK
        - a[2]*aArray4T_1.aCof[1].dK
        - a[3]*aArray4T_1.aCof[0].dK;
*/
    return aArray4T_1.aCof[0].dF;
}
