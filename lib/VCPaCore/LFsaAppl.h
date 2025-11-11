#ifndef LFSAAPPL_H
#define LFSAAPPL_H
#include <Arduino.h>
class LFsaAppl {
public:
    void virtual run();
    LFsaAppl(String StrNam);
    virtual ~LFsaAppl() {};    
    int nState{0};
    virtual String FGetState();
protected:    
    int nTmpState{0};
    String StrName;
};
#endif // LFSAAPPL_H
