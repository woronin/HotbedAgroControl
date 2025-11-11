#ifndef FDGTFILTERBAT_H
#define FDGTFILTERBAT_H


#include "LFsaAppl.h"
#include <string>
using namespace std;
class CArrayFilt {
public:
    CArrayFilt() {
        nSumLeft = 0;
        nSumRight = 0;
        nLength = 0;
        pnBegin = &aFilter[0];
        pnEnd = &aFilter[501];
        pnCurN = pnBegin;
        int i=0;
        while (pnCurN<=pnEnd) {
            *pnCurN = 0; pnCurN++;
            i++;
        }
        pnCurN = pnBegin;
    }
    double aFilter[501];
    void Put(double &d, int n, bool bType);
    double Get(int n, bool bType);
    int nLength;				// ����� �������
    double *pnBegin;			// ��������� �� ������
    double *pnEnd;				// ��������� �� �����
    double *pnCurN;				// ��������� �� ������� �������
    double nSumLeft;			// ����� ����� ���������
    double nSumRight;			// ����� ����� ���������
};

class NCofF
{
public:
    NCofF(double df=0, double dy=0) {dF=df; dY=dy;}
    double dF;
    double dY;
    NCofF(NCofF &c) {dF=c.dF; dY=c.dY; }
};

class NArrayCofF
{
public:
    NArrayCofF() { nFull = 0; bNorm=true; }
    NCofF	aCof[22];
    void Add(NCofF &c, int nL);
    bool bFullStack() {
        if (nFull>=4)
            return true;
        else
            return false;
    }
    int nFull;
    bool bNorm;
};


class FDgtFilterBat :
    public LFsaAppl
{
public:
    FDgtFilterBat(String strNam);
    virtual ~FDgtFilterBat(void);
    void run();
// входы
    float	*pVarX;				// >1 вход сигнал
    int		nTypeFilter;		// тип фильтра: 0 - усредняющий, 1 - цифровой
    double dPrecision;			// точность фильтра
// выходы
    float	*pVarY;				// >1 выходной сигнал

    double a6_35[4];
    double b6_35[4];
    double a6_10[4];
    double b6_10[4];
    double a2_35[4];
    double b2_35[4];
    double a2_10[4];
    double b2_10[4];

protected:
    int x1(); int x2(); int x3(); int x4();
    void y1(); void y2(); void y3(); void y4();  void y5();
    String	strNameObject;
    CArrayFilt dArray;
    NArrayCofF aArray4T_1;
    int		nLength;
    int		nCountLength;
    double *pQ[];
    double filter6_35(double d);
    double filter6_10(double d);
    double filter2_35(double d);
    double filter2_10(double d);
};

#endif // FDGTFILTERBAT_H
