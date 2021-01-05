#include "KParameterAnalysis.h"

#include <stdio.h>
#include <string>

#include "KGetOpt.h"
#include "KSearch.h"

const struct option LongOptions[] =
{
    {"h",      'h'},
    {"help",   'h'},
    {"sunday", 's'},
    {0, 0}
};

KParameterAnalysis::KParameterAnalysis()
{

}

KParameterAnalysis::~KParameterAnalysis()
{

}

bool KParameterAnalysis::GetOption(int argc, char* argv[])
{
    bool     bResult  = false;
    int      nOpt     = 0;
    KGetOpt* pGetOpt  = NULL;

    KGLOG_PROCESS_ERROR(argc);
    KGLOG_PROCESS_ERROR(argv);

    pGetOpt = new KGetOpt();
    KGLOG_PROCESS_ERROR(pGetOpt);

    nOpt = pGetOpt->GetOptLong(argc, argv, LongOptions);
    switch (nOpt)
    {
        case 'h':
            OutHelp();
            break;
        case 's':
            Sunday(argv[2], argv[3]);
            break;
        case 0:
            Sunday(argv[1], argv[2]);
            break;
        case '?':
            printf("%s : illegal option %s\n", argv[0], argv[1]);
            break;
        case -1:
            printf("wrong arguments\n");
            break;
        default:
            printf("getopt returned character code %c", nOpt);
    }

    if (nOpt != -1)
    {
        bResult = true;
    }
Exit0:
    if (pGetOpt)
    {
        delete pGetOpt;
        pGetOpt = NULL;
    }

    return bResult;
}

void KParameterAnalysis::OutHelp()
{
    printf("Usage: SearchKey [OPTION] KeyWord File\n"
        "Search for KeyWord in File\n"
        "Example: ./SearchKey KGLOG ..\\Debug\\testLog\\NormalFile.log\n\n"
        "Pattern selection and interpretation::\n"
        "-h, --help\t\tdisplay this help text and exit\n"
        "    --sunday\t\tsearch by sunday algorithm\n"
    );
}

bool KParameterAnalysis::Sunday(char* pszKeyWord, char* pszFilePath)
{
    bool     bResult  = false;
    int      nRetCode = 0;
    int      nKeyLen  = 0;
    KSearch* pSearch  = NULL;
    char     szKeyWord[MAX_PATH_LEN];
    char     szFilePath[MAX_PATH_LEN];

    KGLOG_PROCESS_ERROR(pszKeyWord);
    KGLOG_PROCESS_ERROR(pszFilePath);

    strncpy(szKeyWord, pszKeyWord, sizeof(szKeyWord));
    szKeyWord[sizeof(szKeyWord) - 1] = '\0';

    nKeyLen = strlen(szKeyWord);

    strncpy(szFilePath, pszFilePath, sizeof(szFilePath));
    szFilePath[sizeof(szFilePath) - 1] = '\0';

    pSearch = new KSearch();
    KGLOG_PROCESS_ERROR(pSearch);

    nRetCode = pSearch->Init(
        nKeyLen, szKeyWord, szFilePath
    );
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = pSearch->RunSunday();
    KGLOG_PROCESS_ERROR(nRetCode);

    bResult = true;
Exit0:
    if (pSearch)
    {
        delete pSearch;
        pSearch = NULL;
    }

    return bResult;
}