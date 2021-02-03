#include "KParameterAnalysis.h"

#include <stdio.h>
#include <string.h>
#include  <stdlib.h>

#include <queue>

#include "KSearch.h"

struct option
{
    const char* pszName;
    int         nVal;
};

struct option LongOptions[] =
{
    {"help",   'h'},
    {0, 0}
};

KParameterAnalysis::KParameterAnalysis()
{

}

KParameterAnalysis::~KParameterAnalysis()
{
    UnInit();
}

void KParameterAnalysis::Init()
{
    m_szFileList.clear();
}

void KParameterAnalysis::UnInit()
{
    m_szFileList.clear();
}

bool KParameterAnalysis::CommandInput(int argc, char* argv[])
{
    bool     bResult           = false;
    int      nRetCode          = 0;
    int      nArgvPos          = 0;
    int      nRetGetOption     = 0;
    int      nIllegalOptPos    = 0;
    int      nCntEffectiveArgv = 0;
    bool     bIsHelp           = false;
    bool     bIsIllegalOpt     = false;
    char*    pszEffectiveArgv[MAX_ARGV_NUM];

    KGLOG_PROCESS_ERROR(argc);
    KGLOG_PROCESS_ERROR(argv);

    while (nRetGetOption != -1)
    {
        ++nArgvPos;
        nRetGetOption = GetOption(argc, argv, LongOptions, nArgvPos);

        switch (nRetGetOption)
        {
            case 'h':
                bIsHelp = true;
                break;
            case '?':
                bIsIllegalOpt = true;
                nIllegalOptPos = nArgvPos;
                break;
            case '*':
                pszEffectiveArgv[nCntEffectiveArgv++] = argv[nArgvPos];
                break;
            default:
                break;
        }
    }

    if (bIsIllegalOpt)
    {
        printf("%s : illegal option %s\n", argv[0], argv[nIllegalOptPos]);
    }
    else if (bIsHelp || nCntEffectiveArgv == 0)
    {
        Output();
    }
    else if (nCntEffectiveArgv == 2)
    {
        nRetCode = GetFiles(pszEffectiveArgv[1]);
        KGLOG_PROCESS_ERROR(nRetCode);

        for (std::vector<std::string>::iterator itVector = m_szFileList.begin(); itVector != m_szFileList.end(); ++itVector)
        {
            printf("%s\n", itVector->c_str());
            //nRetCode = Sunday(pszEffectiveArgv[0], itVector->c_str());
            //KGLOG_PROCESS_ERROR(nRetCode);
        }
    }
    else
    {
        printf("wrong arguments\n");
    }

    bResult = true;
Exit0:
    return bResult;
}

int KParameterAnalysis::GetOption(int argc, char* argv[], struct option* pLongOptions, int nArgvPos)
{
    int nResult    = -1;

    KGLOG_PROCESS_ERROR(argc);
    KGLOG_PROCESS_ERROR(argv);
    KGLOG_PROCESS_ERROR(pLongOptions);
    KGLOG_PROCESS_ERROR(nArgvPos);

    if (nArgvPos >= argc)
    {
        goto Exit0;
    }

    if (nArgvPos == 1 && strncmp(argv[nArgvPos], "--", 2) == 0)
    {
        bool bIsEqual = false;
        for (const struct option *i = pLongOptions; i->pszName; ++i)
        {
            if (strcmp(i->pszName, argv[nArgvPos] + 2) == 0)
            {
                bIsEqual = true;
                nResult = i->nVal;
                break;
            }
        }

        if (!bIsEqual)
        {
            nResult = '?';
        }
    }
    else
    {
        nResult = '*';
    }

Exit0:
    return nResult;
}

void KParameterAnalysis::Output()
{
    printf("Usage: SearchKey [OPTION] KeyWord File\n"
        "Search for KeyWord in File\n"
        "Example: ./SearchKey KGLOG ..\\Debug\\testLog\\GeneralFile.log\n"
        "If you query directories, you must enter the suffix name of the file you want to query\n"
        "Example: ./SearchKey KGLOG ..\\Debug\\testLog\\*.log\n\n"
        "If run it under Linux,replace '\\' to '/'\n"
        "Example: ./SearchKey KGLOG \"../Debug/testLog/*.log\"\n\n"
        "Pattern selection and interpretation::\n"
        "--help\t\tdisplay this help text and exit\n"
    );
}

bool KParameterAnalysis::GetFiles(char* pszPath)
{
    bool  bResult      = false;
    int   nRetCode     = 0;
    int   nReadBuffLen = 0;
    FILE* fpFile       = NULL;
    char  szSystem[MAX_PATH_LEN + MAX_PATH_LEN];
    char  szReadBuff[MAX_PATH_LEN];
#if !WIN32
    int  nPathLen     = 0;
    int  nStarPos     = -1;
    int  nSlashPos    = -1;
    bool bIsStar      = false;
    bool bIsDirectory = false;
#endif

    KGLOG_PROCESS_ERROR(pszPath);

#if WIN32
    snprintf(szSystem, sizeof(szSystem), "dir /a-d /b /s %s > FilePath.txt", pszPath);
#else
    nPathLen = strlen(pszPath);
    KGLOG_PROCESS_ERROR(nPathLen > 0);

    for (int i = nPathLen - 1; i >= 0; --i)
    {
        if (!bIsStar && pszPath[i] == '*')
        {
            bIsStar = true;
            nStarPos = i;
        }

        if (pszPath[i] == '/')
        {
            nSlashPos = i;
            break;
        }
    }

    if (bIsStar && nSlashPos < nStarPos)
    {
        snprintf(szSystem, sizeof(szSystem), "find %.*s -type f -name \"%s\" > FilePath.txt", nPathLen - nSlashPos, pszPath, pszPath + nPathLen - nSlashPos);
    }
    else
    {
        snprintf(szSystem, sizeof(szSystem), "find %s -type f > FilePath.txt", pszPath);
    }
#endif
    szSystem[sizeof(szSystem) - 1] = '\0';

    nRetCode = system(szSystem);
    KGLOG_PROCESS_ERROR(nRetCode >= 0);

    fpFile = fopen("FilePath.txt", "r");
    KGLOG_PROCESS_ERROR(fpFile);

    while (fgets(szReadBuff, MAX_PATH_LEN, fpFile))
    {
        nReadBuffLen = strlen(szReadBuff);

        if (szReadBuff[nReadBuffLen - 1] == '\n')
        {
            szReadBuff[nReadBuffLen - 1] = '\0';
        }

        m_szFileList.push_back(szReadBuff);
    }

    bResult = true;
Exit0:
    if (fpFile)
    {
        fclose(fpFile);
        fpFile = NULL;
    }

    return bResult;
}

bool KParameterAnalysis::Sunday(char* pszKeyWord, const char* pszFilePath)
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