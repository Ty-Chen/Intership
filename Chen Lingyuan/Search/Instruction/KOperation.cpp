#include "KOperation.h"

#include<time.h>
#include<io.h>

#include<iostream>
#include<sstream>
#include<vector>
#include<string>

KOperation::KOperation()
{

}

KOperation::~KOperation()
{

}

void KOperation::Init()
{
    m_szFileList.clear();
}

void KOperation::CommandInput()
{
    bool bRetCode         = false;
    std::string szCommand = "";

    while (true)
    {
        std::getline(std::cin, szCommand);

        if (szCommand == "End")
        {
            break;
        }

        bRetCode = CommandParse(szCommand);
        if (!bRetCode)
        {
            printf("Command cannot be resolved\n");
        }

        printf("\n");
    }
}

bool KOperation::CommandParse(std::string szCommand)
{
    bool bResult  = false;
    bool bRetCode = false;

    bRetCode = JudgeIsCompare(szCommand);
    if (bRetCode)
    {
        bResult = true;
        goto Exit0;
    }

    bRetCode = JudgeIsSearch(szCommand);
    if (!bRetCode)
    {
        goto Exit0;
    }

    bResult = true;
Exit0:
    return bResult;
}

bool KOperation::JudgeIsCompare(std::string szCommand)
{
    bool                      bResult      = false;
    bool                      bRetCode     = false;
    std::string               szTempResult = "";
    std::stringstream         sStream;
    std::vector <std::string> szCommandResult;

    sStream = std::stringstream(szCommand);
    while (sStream >> szTempResult)
    {
        szCommandResult.push_back(szTempResult);
    }

    if (szCommandResult.size() != 3)
    {
        goto Exit0;
    }

    if (szCommandResult[0] == "compare")
    {
       bRetCode = CompareFile(szCommandResult[1], szCommandResult[2]);
       KGLOG_PROCESS_ERROR(bRetCode);
    }
    else
    {
        goto Exit0;
    }

    bResult = true;
Exit0:
    return bResult;
}

bool KOperation::JudgeIsSearch(std::string szCommand)
{
    bool                      bResult             = false;
    bool                      bRetCode            = false;
    int                       nCommandLen         = 0;
    int                       nBeforeIndex        = -1;
    int                       nAfterIndex         = -1;
    std::string               szTempResult        = "";
    std::string               szFrontPartCommand  = "";
    std::string               szLatterPartCommand = "";
    std::string               szPattern           = "";
    std::stringstream         sStream;
    std::vector <std::string> szCommandResult;

    nCommandLen = szCommand.length();

    for (int i = 0; i < nCommandLen; i++)
    {
        if (szCommand[i] == '\'')
        {
            nBeforeIndex = i;
            break;
        }
    }

    for (int i = nCommandLen - 1; i >= 0; i--)
    {
        if (szCommand[i] == '\'')
        {
            nAfterIndex = i;
            break;
        }
    }

    if (nBeforeIndex >= nAfterIndex)
    {
        goto Exit0;
    }

    for (int i = nBeforeIndex + 1; i <= nAfterIndex - 1; i++)
    {
        szPattern += szCommand[i];
    }

    szFrontPartCommand = szCommand.substr(0, nBeforeIndex);
    szLatterPartCommand = szCommand.substr(nAfterIndex + 1, nCommandLen - nAfterIndex - 1);

    sStream = std::stringstream(szFrontPartCommand);
    while (sStream >> szTempResult)
    {
        szCommandResult.push_back(szTempResult);
    }

    sStream = std::stringstream(szLatterPartCommand);
    while (sStream >> szTempResult)
    {
        szCommandResult.push_back(szTempResult);
    }

    if (szCommandResult.size() == 5)
    {
        if (szCommandResult[0] == "time" && szCommandResult[1] == "search" && szCommandResult[3] == ">")
        {
            bRetCode = OutputToDoucment(
            (unsigned char*)szPattern.c_str(),
            (char*)szCommandResult[2].c_str(),
            (char*)szCommandResult[4].c_str()
            );
            KGLOG_PROCESS_ERROR(bRetCode);
        }
    }

    else if(szCommandResult.size() == 3)
    {
        if (szCommandResult[0]=="time" && szCommandResult[1] == "search")
        {
             bRetCode = OutputMatchResult((unsigned char*)szPattern.c_str(), szCommandResult[2]);
             KGLOG_PROCESS_ERROR(bRetCode);
        }
    }

    else
    {
        goto Exit0;
    }

    bResult = true;
Exit0:
    return bResult;
}

bool KOperation::CompareFile(std::string szFirstFilePath, std::string szSecondFilePath)
{
    bool  bResult              = false;
    bool  bMatch               = true;
    int   nRetCode             = 0;
    char* pszFirst             = NULL;
    char* pszSecond            = NULL;
    FILE* fpFirstFile          = NULL;
    FILE* fpSecondFile         = NULL;

    fpFirstFile = fopen(szFirstFilePath.c_str(), "rb");
    if (!fpFirstFile)
    {
        printf("%s does not Exist\n", szFirstFilePath.c_str());
        bResult = true;
        goto Exit0;
    }

    fpSecondFile = fopen(szSecondFilePath.c_str(), "rb");
    if (!fpSecondFile)
    {
        printf("%s does not Exits\n", szSecondFilePath.c_str());
        bResult = true;
        goto Exit0;
    }

    pszFirst = new char[BUFFER_SIZE];
    KGLOG_PROCESS_ERROR(pszFirst);
    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        pszFirst[i] = '\0';
    }

    pszSecond = new char[BUFFER_SIZE];
    KGLOG_PROCESS_ERROR(pszSecond);
    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        pszSecond[i] = '\0';
    }

    while (!feof(fpFirstFile) || !feof(fpSecondFile))
    {
        nRetCode = fread(pszFirst, 1, BUFFER_SIZE, fpFirstFile);
        KGLOG_PROCESS_ERROR(nRetCode >= 0);

        nRetCode = fread(pszSecond, 1, BUFFER_SIZE, fpSecondFile);
        KGLOG_PROCESS_ERROR(nRetCode >= 0);

        for (int i = 0; i < BUFFER_SIZE; i++)
        {
            if (pszFirst[i] != pszSecond[i])
            {
                printf("%d %d %d\n",i,

                    pszFirst[i], pszSecond[i]);
                bMatch = false;
            }
        }
    }

    if (bMatch)
    {
        printf("Successful Match\n");
    }
    else
    {
        printf("Failed Match\n");
    }

    bResult = true;
Exit0:

    if (fpFirstFile)
    {
        fclose(fpFirstFile);
        fpFirstFile = NULL;
    }

    if (fpSecondFile)
    {
        fclose(fpSecondFile);
        fpSecondFile = NULL;
    }

    if (pszFirst)
    {
        delete []pszFirst;
        pszFirst = NULL;
    }

    if (pszSecond)
    {
        delete []pszSecond;
        pszSecond = NULL;
    }

    return bResult;
}

bool KOperation::OutputMatchResult(unsigned char* pszPattern, std::string szSourcePath)
{
    bool    bResult          = false;
    bool    bRetCode         = false;
    int     nFileCount       = 0;
    int     nMatchCount      = 0;
    clock_t nStart           = 0;
    clock_t nEnd             = 0;
    KBlockMatch* pBlockMatch = NULL;
    FILE* fpSourcePath       = NULL;

    KGLOG_PROCESS_ERROR(pszPattern);

    pBlockMatch = new KBlockMatch();
    KGLOG_PROCESS_ERROR(pBlockMatch);

    bRetCode = pBlockMatch->Init();
    KGLOG_PROCESS_ERROR(bRetCode);

    nStart = clock();

     fpSourcePath = fopen(szSourcePath.c_str(), "rb");
    if (fpSourcePath)
    {
        std::string szTestPath = "";
         nMatchCount = pBlockMatch->MatchWords(
            pszPattern, fpSourcePath, (char*)szTestPath.c_str(),
            NULL, true
            );
        KGLOG_PROCESS_ERROR(nMatchCount >= 0);
    }
    else
    {
        GetFiles(szSourcePath);

        nFileCount = m_szFileList.size();
        if (nFileCount == 0)
        {
            printf("The SourceFile does not Exist\n");
            bResult = true;
            goto Exit0;
        }

        for (int i = 0; i < nFileCount; i++)
        {
            FILE* fpTestFile  = NULL;

            fpTestFile = fopen(m_szFileList[i].c_str(), "rb");
            KGLOG_PROCESS_ERROR(fpTestFile);

            m_szFileList[i] += ':';

            nMatchCount = pBlockMatch->MatchWords(
                pszPattern, fpTestFile, (char*)m_szFileList[i].c_str(),
                NULL, true
                );
            KGLOG_PROCESS_ERROR(nMatchCount >= 0);

            if (fpTestFile)
            {
                fclose(fpTestFile);
                fpTestFile = NULL;
            }
        }
    }

    nEnd = clock();

    printf("Time: %d ms\n", nEnd - nStart);

    bResult = true;
Exit0:
    m_szFileList.clear();

    if (pBlockMatch)
    {
        delete pBlockMatch;
        pBlockMatch = NULL;
    }
    return bResult;
}

bool KOperation::OutputToDoucment(unsigned char* pszPattern, std::string szSourcePath, std::string szTargetPath)
{
    bool  bResult             = false;
    bool  bRetCode            = false;
    int   nFileCount          = 0;
    int   nSumMatchCount      = 0;
    clock_t nStart            = 0;
    clock_t nEnd              = 0;
    FILE* fpTargetFile        = NULL;
    FILE* fpSourcePath        = NULL;
    KBlockMatch* pBlockMatch  = NULL;

    KGLOG_PROCESS_ERROR(pszPattern);

    pBlockMatch = new KBlockMatch();
    KGLOG_PROCESS_ERROR(pBlockMatch);

    bRetCode = pBlockMatch->Init();
    KGLOG_PROCESS_ERROR(bRetCode);

    fpTargetFile = fopen(szTargetPath.c_str(), "wb");
    if (!fpTargetFile)
    {
        printf("The TargetFile does not Exist\n");
        bResult = true;
        goto Exit0;
    }

    nStart = clock();

    fpSourcePath = fopen(szSourcePath.c_str(), "rb");
    if (fpSourcePath)
    {
        std::string pszTestPath = "";

        nSumMatchCount = pBlockMatch->MatchWords(
            pszPattern, fpSourcePath, (char*)pszTestPath.c_str(),
            fpTargetFile, false
            );
        KGLOG_PROCESS_ERROR(nSumMatchCount >= 0);
    }
    else
    {
        GetFiles(szSourcePath);

        nFileCount = m_szFileList.size();
        if (nFileCount == 0)
        {
            printf("The SourceFile does not Exist\n");
            bResult = true;
            goto Exit0;
        }

        for (int i = 0; i < nFileCount; i++)
        {
            FILE* fpTestFile  = NULL;
            int   nMatchCount = 0;

            fpTestFile = fopen(m_szFileList[i].c_str(), "rb");
            KGLOG_PROCESS_ERROR(fpTestFile);

            m_szFileList[i] += ':';

            nMatchCount = pBlockMatch->MatchWords(
                pszPattern, fpTestFile, (char*)m_szFileList[i].c_str(),
                fpTargetFile, false
                );
            KGLOG_PROCESS_ERROR(nMatchCount >= 0);
            nSumMatchCount += nMatchCount;

            if (fpTestFile)
            {
                fclose(fpTestFile);
                fpTestFile = NULL;
            }
        }
    }

    nEnd = clock();

    printf("Match : %d \n", nSumMatchCount);
    printf("Time : %d ms\n", nEnd - nStart);

    bResult = true;
Exit0:
    m_szFileList.clear();

    if (fpSourcePath)
    {
        fclose(fpSourcePath);
        fpSourcePath = NULL;
    }

    if (fpTargetFile)
    {
        fclose(fpTargetFile);
        fpTargetFile = NULL;
    }

    if (pBlockMatch)
    {
        delete pBlockMatch;
        pBlockMatch = NULL;
    }
    return bResult;
}

void KOperation::GetFiles(std::string szPath)
{
    long               hFile      = 0;
    struct _finddata_t FileInfo;
    std::string        szFullPath = "";

    szFullPath = szPath;
    szFullPath += "/*";

    hFile = _findfirst(szFullPath.c_str(), &FileInfo);
    if (hFile != -1)
    {
        do
        {
            szFullPath = szPath;
            szFullPath += "/";
            szFullPath += FileInfo.name;

            if ((FileInfo.attrib ==  _A_SUBDIR))
            {
                if (strcmp(FileInfo.name, ".") != 0 && strcmp(FileInfo.name, "..") != 0)
                {
                    GetFiles(szFullPath);
                }
            }
            else
            {
                m_szFileList.push_back(szFullPath);
            }
        } while (_findnext(hFile, &FileInfo) == 0);

        _findclose(hFile);
    }

}
