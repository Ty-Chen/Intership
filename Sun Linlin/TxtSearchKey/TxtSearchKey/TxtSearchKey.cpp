///////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
//
// FileName : TxtSearchKey.h
// Creator  : sunlinlin
// Date     : 2020-12-5 15:21:50
// Comment  : Text search and display
//
///////////////////////////////////////////////////////////////

#include "KHelp.h"

int main(int argc, char *argv[])
{
    int        nResult     = -1;
    bool       bRetCode    = false;
    KHelp*     pHelp       = NULL;

    pHelp = new KHelp();
    KGLOG_PROCESS_ERROR(pHelp);

    bRetCode = pHelp->Init();
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = pHelp->TxtSearchKeyOption(argc, argv);
    KGLOG_PROCESS_ERROR(bRetCode);

    nResult = 0;
Exit0:
    if (pHelp)
    {
        delete pHelp;
        pHelp = NULL;
    }
    return nResult;
}
