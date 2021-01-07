#include "KParameterAnalysis.h"

#include <stdio.h>
#ifdef linux
#include <unistd.h>
#include <dirent.h>
#endif
#ifdef WIN32

#endif
#include <direct.h>
#include <io.h>

#include <string>
#include <vector>
#include <queue>

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

bool KParameterAnalysis::GetFiles(char* pszPath)
{
    bool               bResult          = false;
    int                nFindHandle      = 0;
    int                nFileSuffixLen   = 0;
    char*              pszDirectoryPath = NULL;
    char*              pszFileSuffix    = NULL;
    char*              pszIndex         = NULL;
    _finddata_t        FileData;
    std::queue<char*>  DirectoryQueue;
    std::vector<char*> FilesVecotr;
    
    KGLOG_PROCESS_ERROR(pszPath);

    pszIndex = new char[MAX_PATH_LEN];
    KGLOG_PROCESS_ERROR(pszIndex);

    pszDirectoryPath = new char[MAX_PATH_LEN];
    KGLOG_PROCESS_ERROR(pszDirectoryPath);

    pszFileSuffix = new char[MAX_PATH_LEN];
    KGLOG_PROCESS_ERROR(pszFileSuffix);

    strcpy(pszFileSuffix, "log");
    nFileSuffixLen = strlen(pszFileSuffix);
    pszFileSuffix[nFileSuffixLen] = '\0';

    nFindHandle = _findfirst(pszPath, &FileData);
    KGLOG_PROCESS_ERROR(nFindHandle != -1);

    if (FileData.attrib == _A_ARCH)
    {
        FilesVecotr.push_back(pszPath);
    }
    else if (FileData.attrib == _A_SUBDIR)
    {
        DirectoryQueue.push(pszPath);
    }

    while (!DirectoryQueue.empty())
    {
        strcpy(pszIndex, DirectoryQueue.front());
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

            char* pszTempPath = NULL;
            pszTempPath = new char[MAX_PATH_LEN];
            KGLOG_PROCESS_ERROR(pszTempPath);

            sprintf(pszTempPath, "%s\\%s", pszIndex, FileData.name);
            pszTempPath[strlen(pszTempPath)] = '\0';

            if (FileData.attrib == _A_ARCH)
            {
                bool bIsFileSuffix    = true;
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
                    FilesVecotr.push_back(pszTempPath);
                }
            }
            else if (FileData.attrib == _A_SUBDIR)
            {
                DirectoryQueue.push(pszTempPath);
            }
        }

        _findclose(nFindHandle);
    }

    bResult = true;
Exit0:
    if (!bResult)
    {
        for (std::vector<char*>::iterator itVector = FilesVecotr.begin(); itVector != FilesVecotr.end(); ++itVector)
        {
            delete []*itVector;
            *itVector = NULL;
        }
    }

    if (pszFileSuffix)
    {
        delete []pszFileSuffix;
        pszFileSuffix = NULL;
    }

    if (pszIndex)
    {
        delete []pszIndex;
        pszIndex = NULL;
    }

    return bResult;
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