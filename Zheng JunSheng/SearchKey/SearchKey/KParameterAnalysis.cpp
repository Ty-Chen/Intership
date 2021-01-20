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
    {"sunday", 's'},
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
    bool     bIsSunday         = false;
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
            case 's':
                bIsSunday = true;
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

        if (bIsSunday)
        {
            for (std::vector<std::string>::iterator itVector = m_szFileList.begin(); itVector != m_szFileList.end(); ++itVector)
            {
                printf("%s\n", itVector->c_str());
                //nRetCode = Sunday(pszEffectiveArgv[0], itVector->c_str());
                //KGLOG_PROCESS_ERROR(nRetCode);
            }
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
#ifdef WIN32
    printf("Usage: SearchKey OPTION KeyWord File\n"
        "Search for KeyWord in File\n"
        "Example: ./SearchKey --sunday KGLOG ..\\Debug\\testLog\\GeneralFile.log\n\n"
        "Pattern selection and interpretation::\n"
        "--help\t\tdisplay this help text and exit\n"
        "--sunday\tsearch by sunday algorithm\n"
    );
#else
    printf("Usage: SearchKey OPTION KeyWord File\n"
        "Search for KeyWord in File\n"
        "Example: ./SearchKey --sunday KGLOG ../Debug/testLog/GeneralFile.log\n\n"
        "Pattern selection and interpretation::\n"
        "--help\t\tdisplay this help text and exit\n"
        "--sunday\tsearch by sunday algorithm\n"
    );
#endif
}

bool KParameterAnalysis::GetFiles(char* pszPath)
{
    bool  bResult      = false;
    int   nRetCode     = 0;
    int   nPathLen     = 0;
    int   nSlashPos    = 0;
    int   nPointPos    = 0;
    int   nReadBuffLen = 0;
    bool  bIsPoint     = false;
    FILE* fpFile       = NULL;
    char  szSystem[2 * MAX_PATH_LEN];
    char  szReadBuff[MAX_PATH_LEN + 1];
    char  szTempPath[MAX_PATH_LEN];

    KGLOG_PROCESS_ERROR(pszPath);

    nPathLen = strlen(pszPath);
    KGLOG_PROCESS_ERROR(nPathLen > 0);

    for (int i = nPathLen - 1; i >= 0; --i)
    {
        if (pszPath[i] == '.' && !bIsPoint)
        {
            bIsPoint = true;
            nPointPos = i;
        }

        if (pszPath[i] == '\\' || pszPath[i] == '/')
        {
            nSlashPos = i;
            break;
        }
    }

    if (!bIsPoint || (bIsPoint && nPointPos < nSlashPos))
    {
        if (pszPath[nSlashPos] == '\\')
        {
            if (nSlashPos == nPathLen - 1)
            {
                snprintf(szTempPath, sizeof(szTempPath), "%s*.log", pszPath);
            }
            else
            {
                snprintf(szTempPath, sizeof(szTempPath), "%s\\*.log", pszPath);
            }
        }
        else
        {
            snprintf(szTempPath, sizeof(szTempPath), "%s", pszPath);
        }
    }
    else if (bIsPoint && nPointPos > nSlashPos)
    {
        if (pszPath[nSlashPos] == '/' && strcmp(pszPath + nSlashPos + 1, "*.log") == 0)
        {
            pszPath[nSlashPos + 1] = '\0';
        }
        snprintf(szTempPath, sizeof(szTempPath), "%s", pszPath);
       
    }
    szTempPath[sizeof(szTempPath) - 1] = '\0';

#if WIN32
    snprintf(szSystem, sizeof(szSystem), "dir /b /s %s > FilePath.txt", szTempPath);
    szSystem[sizeof(szSystem) - 1] = '\0';
#else
    snprintf(szSystem, sizeof(szSystem), "find %s -type f -name *.log > FilePath.txt", szTempPath);
    szSystem[sizeof(szSystem) - 1] = '\0';
#endif

    nRetCode = system(szSystem);
    KGLOG_PROCESS_ERROR(nRetCode >= 0);

    fpFile = fopen("FilePath.txt", "r");
    KGLOG_PROCESS_ERROR(fpFile);

    while (fgets(szReadBuff, MAX_PATH_LEN + 1, fpFile))
    {
        nReadBuffLen = strlen(szReadBuff);

        if (szReadBuff[nReadBuffLen - 2] == '\r')
        {
            szReadBuff[nReadBuffLen - 2] = '\0';
        }

        if (szReadBuff[nReadBuffLen - 1] == '\n')
        {
            szReadBuff[nReadBuffLen - 1] = '\0';
        }
        else
        {
            continue;
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