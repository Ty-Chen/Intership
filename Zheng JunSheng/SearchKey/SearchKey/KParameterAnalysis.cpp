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
    std::vector <std::string>().swap(m_szFileList);
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
            case 2:
            case 3:
                pszEffectiveArgv[nCntEffectiveArgv++] = argv[nRetGetOption];
                break;
            default:
                break;
        }

        ++nOptind;
        nRetGetOption = GetOption(argc, argv, LongOptions, nOptind);
    }

    if (bIsIllegalOpt)
    {
        printf("%s : illegal option %s\n", argv[0], argv[nIllegalOptPos]);
    }
    else if (bIsHelp || nCntEffectiveArgv == 0)
    {
        OutHelp();
    }
    else if(nCntEffectiveArgv == 2)
    {

#ifdef linux
        nRetCode = GetLinuxFile(pszEffectiveArgv[1]);
        KGLOG_PROCESS_ERROR(nRetCode);
#else
        nRetCode = GetWindowsFiles(pszEffectiveArgv[1]);
        KGLOG_PROCESS_ERROR(nRetCode);
#endif 

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

    if (nOptind == 1)
    {
        if (strncmp(argv[nOptind], "--", 2) == 0)
        {
            int nArgvLen = 0;
            for (const struct option *i = pLongOptions; i->pszName; ++i)
            {
                nArgvLen = strlen(i->pszName);
                if (strncmp(i->pszName, argv[nOptind] + 2, nArgvLen - 2) == 0)
                {
                    nResult = i->nVal;
                    break;
                }
            }
        }

        if (nResult == -1)
        {
            nResult = '?';
        }
    }
    else
    {
        nResult = nOptind;
    }

Exit0:
    return nResult;
}

void KParameterAnalysis::OutHelp()
{
    printf("Usage: SearchKey OPTION KeyWord File\n"
        "Search for KeyWord in File\n"
        "Example: ./SearchKey --sunday KGLOG ..\\Debug\\testLog\\NormalFile.log\n\n"
        "Pattern selection and interpretation::\n"
        "--help\t\tdisplay this help text and exit\n"
        "--sunday\tsearch by sunday algorithm\n"
    );
}

#ifdef linux
bool KParameterAnalysis::GetLinuxFile(char* pszPath)
{
    bool                    bResult     = false;
    int                     nRetCode    = 0;
    char*                   pszNextPath = NULL;
    std::string             szTempPath  = NULL;
    struct dirent*          pFileInfo   = NULL;
    DIR*                    pDir;
    struct stat             statBuff;
    std::queue<std::string> DirectoryQueue;

    pFileInfo = new struct dirent;
    KGLOG_PROCESS_ERROR(pFileInfo);

    nRetCode = stat(pszPath, &statBuff);
    KGLOG_PROCESS_ERROR(nRetCode == 0);

    pszNextPath = new char[MAX_PATH_LEN];
    KGLOG_PROCESS_ERROR(pszNextPath);

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
        szTempPath = DirectoryQueue.front();
        DirectoryQueue.pop();

        pDir = opendir(szTempPath.c_str());
        if (pDir == NULL)
        {
            closedir(pDir);
            break;
        }

        while (true)
        {
            pFileInfo = readdir(pDir);

            if (!pFileInfo)
            {
                break;
            }

            sprintf(pszNextPath, "%s/%s", szTempPath.c_str(), pFileInfo->d_name);
            pszNextPath[strlen(pszNextPath)] = '\0';

            if (strcmp(pFileInfo->d_name, ".") == 0 || strcmp(pFileInfo->d_name, "..") == 0)
            {
                continue;
            }

            if (pFileInfo->d_type == DT_REG)
            {
                m_szFileList.push_back(pszNextPath);
            }
            else if (pFileInfo->d_type == DT_DIR)
            {
                DirectoryQueue.push(pszNextPath);
            }
        }
        closedir(pDir);
    }

    bResult = true;
Exit0:
    if (pszNextPath)
    {
        delete []pszNextPath;
        pszNextPath = NULL;
    }

    if (pFileInfo)
    {
        delete pFileInfo;
        pFileInfo = NULL;
    }

    return bResult;
}
#else
bool KParameterAnalysis::GetWindowsFiles(char* pszPath)
{
    bool                     bResult          = false;
    int                      nFindHandle      = 0;
    int                      nFileSuffixLen   = 0;
    char*                    pszIndex         = NULL;
    char*                    pszTempPath      = NULL;
    char*                    pszFileSuffix    = NULL;
    char*                    pszDirectoryPath = NULL;
    std::string              szTempPath       = "";
    _finddata_t              FileData;
    std::queue<std::string>  DirectoryQueue;

    KGLOG_PROCESS_ERROR(pszPath);

    pszTempPath = new char[MAX_PATH_LEN];
    KGLOG_PROCESS_ERROR(pszTempPath);

    pszIndex = new char[MAX_PATH_LEN];
    KGLOG_PROCESS_ERROR(pszIndex);

    pszDirectoryPath = new char[MAX_PATH_LEN];
    KGLOG_PROCESS_ERROR(pszDirectoryPath);

    pszFileSuffix = new char[MAX_FILE_SUFFIX_LEN];
    KGLOG_PROCESS_ERROR(pszFileSuffix);

    strcpy(pszFileSuffix, "log");
    nFileSuffixLen = strlen(pszFileSuffix);
    pszFileSuffix[nFileSuffixLen] = '\0';

    nFindHandle = _findfirst(pszPath, &FileData);
    KGLOG_PROCESS_ERROR(nFindHandle != -1);

    if (FileData.attrib == _A_ARCH)
    {
        m_szFileList.push_back(pszPath);
    }
    else if (FileData.attrib == _A_SUBDIR)
    {
        DirectoryQueue.push(pszPath);
    }

    while (!DirectoryQueue.empty())
    {
        strcpy(pszIndex, DirectoryQueue.front().c_str());
        pszIndex[strlen(pszIndex)] = '\0';
        DirectoryQueue.pop();

        sprintf(pszDirectoryPath, "%s\\*", pszIndex);
        pszDirectoryPath[strlen(pszDirectoryPath)] = '\0';

        nFindHandle = _findfirst(pszDirectoryPath, &FileData);
        while (_findnext(nFindHandle, &FileData) == 0)
        {
            if (strcmp(FileData.name, ".") == 0 || strcmp(FileData.name, "..") == 0)
            {
                continue;
            }

            sprintf(pszTempPath, "%s\\%s", pszIndex, FileData.name);
            pszTempPath[strlen(pszTempPath)] = '\0';

            szTempPath = pszTempPath;

            if (FileData.attrib == _A_ARCH)
            {
                bool bIsFileSuffix = true;
                int  nFileDataNameLen = strlen(FileData.name);
                for (int i = nFileDataNameLen - nFileSuffixLen, j = 0; i < nFileDataNameLen; ++i, ++j)
                {
                    if (FileData.name[i] != pszFileSuffix[j])
                    {
                        bIsFileSuffix = false;
                        break;
                    }
                }

                if (bIsFileSuffix)
                {
                    m_szFileList.push_back(pszTempPath);
                }
            }
            else if (FileData.attrib == _A_SUBDIR)
            {
                DirectoryQueue.push(szTempPath);
            }
        }
        _findclose(nFindHandle);
    }

    bResult = true;
Exit0:
    if (!bResult)
    {
        std::vector <std::string>().swap(m_szFileList);
    }

    if (pszFileSuffix)
    {
        delete []pszFileSuffix;
        pszFileSuffix = NULL;
    }

    if (pszDirectoryPath)
    {
        delete []pszDirectoryPath;
        pszDirectoryPath = NULL;
    }

    if (pszIndex)
    {
        delete []pszIndex;
        pszIndex = NULL;
    }

    if (pszTempPath)
    {
        delete []pszTempPath;
        pszTempPath = NULL;
    }

    return bResult;
}
#endif



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