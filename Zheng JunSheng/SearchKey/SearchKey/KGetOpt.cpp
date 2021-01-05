#include "KGetOpt.h"

#include <stdio.h>
#include <string.h>

KGetOpt::KGetOpt()
{

}

KGetOpt::~KGetOpt()
{

}

int KGetOpt::GetOptLong(
    int argc, char* argv[], const struct option* pLongOpts
)
{
    int nResult = -1;
    int nArgv1Len = 0;
    int nIndex = 0;

    KGLOG_PROCESS_ERROR(argc < 5);
    KGLOG_PROCESS_ERROR(argv);
    KGLOG_PROCESS_ERROR(pLongOpts);

    if (argc == 1)
    {
        nResult = 'h';
    }
    else if (argc == 3 && argv[1][0] != '-')
    {
        nResult = 0;
    }
    else if (argv[1][0] == '-')
    {
        nArgv1Len = strlen(argv[1]);

        if (argv[1][1] == '-')
        {
            nIndex = 2;
        }
        else
        {
            nIndex = 1;
        }

        for (const struct option *i = pLongOpts; i->pszName; ++i)
        {
            if (strncmp(i->pszName, argv[1] + nIndex, nArgv1Len - nIndex) == 0)
            {
                nResult = i->nVal;
                break;
            }
        }
    }

    if (nResult == -1 && argv[1][0] == '-')
    {
        nResult = '?';
    }

Exit0:
    return nResult;
}