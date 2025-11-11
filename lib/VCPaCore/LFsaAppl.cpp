#include "LFsaAppl.h"
LFsaAppl::LFsaAppl(String StrNam) 
{
    StrName = StrNam;
    nState = nTmpState = 0;
}

void LFsaAppl::run() {
}

String LFsaAppl::FGetState() { return String(nState); }
