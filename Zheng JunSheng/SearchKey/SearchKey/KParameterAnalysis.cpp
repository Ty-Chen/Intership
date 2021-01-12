#include "KParameterAnalysis.h"

#ifdef linux
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#else
#include <direct.h>
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

        for (std::vector<std::string>::iterator itVector = m_szFileList.begin(); itVector != m_szFileList.end(); ++itVector)
        {
            printf("%s\n", itVector->c_str());
        }

        if (bIsSunday)
        {
            for (std::vector<std::string>::iterator itVector = m_szFileList.begin(); itVector != m_szFileList.end(); ++itVector)
            {
                nRetCode = Sunday(pszEffectiveArgv[0], itVector->c_str());
                KGLOG_PROCESS_ERROR(nRetCode);
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
    bool bResult = false;

#ifdef linux
    int                     nRetCode       = 0;
    int                     nFileSuffixLen = MAX_FILE_SUFFIX_LEN;
    std::string             szTempPath     = NULL;
    char                    szFileSuffix[MAX_FILE_SUFFIX_LEN + 1];
    char                    szNextPath[MAX_PATH_LEN];
    DIR                     Dir;
    struct stat             statBuff;
    struct dirent           FileInfo;
    std::queue<std::string> DirectoryQueue;

    nRetCode = stat(pszPath, &statBuff);
    KGLOG_PROCESS_ERROR(nRetCode == 0);

    if (S_ISDIR(statBuff.st_mode))
    {
        DirectoryQueue.push(pszPath);
    }
    if (S_ISREG(statBuff.st_mode))
    {
        m_szFileList.push_back(pszPath);
    }

    while (!DirectoryQueue.empty())
    {
        Dir = opendir(DirectoryQueue.front().c_str());
        if (Dir == NULL)
        {
            closedir(Dir);
            DirectoryQueue.pop();
            continue;
        }

        while (true)
        {
            FileInfo = readdir(Dir);

            if (!FileInfo)
            {
                break;
            }

            if (strcmp(FileInfo->d_name, ".") == 0 || strcmp(FileInfo->d_name, "..") == 0)
            {
                continue;
            }

            snprintf(szNextPath, sizeof(szNextPath), "%s/%s", DirectoryQueue.front().c_str(), FileInfo->d_name);
            szNextPath[sizeof(szNextPath) - 1] = '\0';

            if (FileInfo->d_type == DT_DIR)
            {
                DirectoryQueue.push(szNextPath);
            }
            else
            {
                bool bIsFileSuffix    = true;
                int  nFileDataNameLen = strlen(FileInfo.name);
                for (int i = nFileDataNameLen - 1, j = nFileSuffixLen - 1; i >= 0 && j >= 0; --i, --j)
                {
                    if (FileInfo.name[i] != szFileSuffix[j])
                    {
                        bIsFileSuffix = false;
                        break;
}
                }

                if (bIsFileSuffix)
                {
                    m_szFileList.push_back(szNextPath);
                }
            }
        }
        closedir(Dir);
        DirectoryQueue.pop();
    }
#else
    int                      nFindHandle    = 0;
    int                      nFileSuffixLen = MAX_FILE_SUFFIX_LEN;
    char                     szTempPath[MAX_PATH_LEN];
    char                     szFileSuffix[MAX_FILE_SUFFIX_LEN + 1];
    _finddata_t              FileData;
    std::queue<std::string>  DirectoryQueue;

    KGLOG_PROCESS_ERROR(pszPath);

    strcpy(szFileSuffix, "log");
    szFileSuffix[nFileSuffixLen] = '\0';

    nFindHandle = _findfirst(pszPath, &FileData);
    KGLOG_PROCESS_ERROR(nFindHandle != -1);

    if (FileData.attrib & _A_SUBDIR)
    {
        DirectoryQueue.push(pszPath);
    }
    else
    {
        m_szFileList.push_back(pszPath);
    }

    while (!DirectoryQueue.empty())
    {
        snprintf(szTempPath, sizeof(szTempPath), "%s\\*", DirectoryQueue.front().c_str());
        szTempPath[sizeof(szTempPath) - 1] = '\0';

        nFindHandle = _findfirst(szTempPath, &FileData);
        if (nFindHandle == -1)
        {
            _findclose(nFindHandle);
            DirectoryQueue.pop();
            continue;
        }

        do
        {
            if (strcmp(FileData.name, ".") == 0 || strcmp(FileData.name, "..") == 0)
            {
                continue;
            }

            snprintf(szTempPath, sizeof(szTempPath), "%s\\%s", DirectoryQueue.front().c_str(), FileData.name);
            szTempPath[sizeof(szTempPath) - 1] = '\0';

            if (FileData.attrib & _A_SUBDIR)
            {
                DirectoryQueue.push(szTempPath);
            }
            else
            {
                bool bIsFileSuffix    = true;
                int  nFileDataNameLen = strlen(FileData.name);
                for (int i = nFileDataNameLen - 1, j = nFileSuffixLen - 1; i >= 0 && j >= 0; --i, --j)
                {
                    if (FileData.name[i] != szFileSuffix[j])
                    {
                        bIsFileSuffix = false;
                        break;
                    }
                }

                if (bIsFileSuffix)
                {
                    m_szFileList.push_back(szTempPath);
                }
            }
        } while (_findnext(nFindHandle, &FileData) == 0);

        _findclose(nFindHandle);
        DirectoryQueue.pop();
    }
#endif

    bResult = true;
Exit0:
    if (!bResult)
    {
        m_szFileList.clear();
    }

    bResult = false;
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