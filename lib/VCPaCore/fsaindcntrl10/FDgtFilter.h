#ifndef FDGTFILTER_H
#define FDGTFILTER_H


#include "LFsaAppl.h"
#include "ArithmeticMean.h"

class NCof
{
public:
    NCof(double df=0, double dy=0) {dF=df; dY=dy;}
    double dF;
    double dY;
    NCof(NCof &c) {dF=c.dF; dY=c.dY; }
};

class NArrayCof
{
public:
    NArrayCof() { nFull = 0; bNorm=true; }
    NCof	aCof[22];
    void Add(NCof &c, int nL);
    bool bFullStack() {
        if (nFull>=4)
            return true;
        else
            return false;
    }
    int nFull;
    bool bNorm;
};

class FDelayCLK;
class FDgtFilter :
    public LFsaAppl
{
public:
    FDgtFilter(String strNam);
    virtual ~FDgtFilter(void);
    void run();
// входы
    float       *pVarX;			// >1 вход сигнал
    int			nN;				// >1 вход глубина/частота
    int			nTypeFilter;	// тип фильтра: 0 - усредняющий, 1 - цифровой
    bool		bTimeMsec;		// true = время задержки в мсек; false - в дискретных шагах
// выходы
    float       *pVarY;			// >1 выходной сигнал

    double Q_4[4];
    double Q_7[7];
    double Q_22[22];
    double a[4];
    double b[4];

protected:
    int x1(); int x2(); int x3(); int x4(); int x5();
    void y1(); void y2(); void y3(); void y4();  void y5();  void y6();  void y8(); void y14(); void y15();
    String	strNameObject;

    CArrayFilter dArray;
    NArrayCof aArray4T_1;
    int		nLength;
    int		nCountLength;
    double filter4(double d);
    double filter7(double d);
    double filter22(double d);
    double filterCh(double d);
    int	nSavTypeFilter;
    double dR;
    int nCLK;
    double d;

};

#endif // FDGTFILTER_H
