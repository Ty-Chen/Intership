#include "KParameterAnalysis.h"

#ifdef linux
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#else
#include<fcntl.h>
#include <io.h>
#endif

#include <stdio.h>
#include <string.h>

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
    int      nOptind           = 0;
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
        ++nOptind;
        nRetGetOption = GetOption(argc, argv, LongOptions, nOptind);

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
                nIllegalOptPos = nOptind;
                break;
            case 1:
            case 2:
            case 3:
                pszEffectiveArgv[nCntEffectiveArgv++] = argv[nRetGetOption];
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

#ifdef WIN32
        _setmode(_fileno(stdout), O_BINARY);
#endif

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

int KParameterAnalysis::GetOption(int argc, char* argv[], struct option* pLongOptions, int nOptind)
{
    int nResult = -1;

    KGLOG_PROCESS_ERROR(argc);
    KGLOG_PROCESS_ERROR(argv);
    KGLOG_PROCESS_ERROR(pLongOptions);
    KGLOG_PROCESS_ERROR(nOptind);

    if (nOptind >= argc)
    {
        goto Exit0;
    }

    nResult = nOptind;

    if (nOptind == 1 && strncmp(argv[nOptind], "--", 2) == 0)
    {
        bool bIsEqual = false;
        for (const struct option *i = pLongOptions; i->pszName; ++i)
        {
            if (strcmp(i->pszName, argv[nOptind] + 2) == 0)
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

Exit0:
    return nResult;
}

void KParameterAnalysis::Output()
{
    printf("Usage: SearchKey OPTION KeyWord File\n"
        "Search for KeyWord in File\n"
        "Example: ./SearchKey --sunday KGLOG ..\\Debug\\testLog\\GeneralFile.log\n\n"
        "Pattern selection and interpretation::\n"
        "--help\t\tdisplay this help text and exit\n"
        "--sunday\tsearch by sunday algorithm\n"
    );
}

bool KParameterAnalysis::GetFiles(char* pszPath)
{
    bool bResult       = false;
    int  nRetCode      = 0;
    int  nReadBuffLen  = 0;
    FILE* fpFile       = NULL;
    char zFinishChar;
    char szSystem[2 * MAX_PATH_LEN];
    char szReadBuff[MAX_PATH_LEN + 1];
    char szTempPath[MAX_TEMP_PATH_LEN];
    char szFileSuffix[MAX_FILE_SUFFIX_LEN + 1];

#if WIN32
    int         nFindHandle = 0;
    _finddata_t FileData;
#endif

    KGLOG_PROCESS_ERROR(pszPath);

    snprintf(szFileSuffix, sizeof(szFileSuffix), "%s", ".log");
    szFileSuffix[sizeof(szFileSuffix) - 1] = '\0';

    snprintf(szTempPath, sizeof(szTempPath), "%s", "FilePath.txt");
    szTempPath[sizeof(szTempPath) - 1] = '\0';

#if WIN32
    nFindHandle = _findfirst(pszPath, &FileData);
    _findclose(nFindHandle);

    if (FileData.attrib & _A_SUBDIR)
    {
        snprintf(szSystem, sizeof(szSystem), "dir /b /s %s\\*%s > %s", pszPath, szFileSuffix, szTempPath);
        szSystem[sizeof(szSystem) - 1] = '\0';
    }
    else
    {
        snprintf(szSystem, sizeof(szSystem), "dir /b /s %s > %s", pszPath, szTempPath);
        szSystem[sizeof(szSystem) - 1] = '\0';
    }

    zFinishChar = '\r';
#else
    snprintf(szSystem, sizeof(szSystem), "find %s -type f -name *%s > %s", pszPath, szFileSuffix, szTempPath);
    szSystem[sizeof(szSystem) - 1] = '\0';

    zFinishChar = '\n';
#endif

    nRetCode = system(szSystem);
    KGLOG_PROCESS_ERROR(nRetCode != -1);

    fpFile = fopen(szTempPath, "rb");
    KGLOG_PROCESS_ERROR(fpFile);

    while (fgets(szReadBuff, MAX_PATH_LEN + 1, fpFile))
    {
        nReadBuffLen = strlen(szReadBuff);

        for (int i = nReadBuffLen - 1; i >= 0; --i)
        {
            if (szReadBuff[i] == zFinishChar)
            {
                szReadBuff[i] = '\0';
                break;
            }
        }

        m_szFileList.push_back(szReadBuff);
    }

    bResult = true;
Exit0:
    if (!bResult)
    {
        m_szFileList.clear();
    }

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