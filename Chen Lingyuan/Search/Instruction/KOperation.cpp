#include "KOperation.h"

#include<time.h>
#include<io.h>
#include<fcntl.h>

#include<iostream>
#include<vector>
#include<string>
#include<queue>

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
#define EPR                 fprintf(stderr,
#define ERR(str, chr)       if(opterr){EPR "%s%c\n", str, chr);}
int    opterr = 1;//当opterr=0时,getopt不向stderr输出错误信息
int    optind = 1;//下一次调用getopt时，从optind存储的位置处重新开始检查选项
int    optopt;//当命令行选项字符不包括在optstring中
char    *optarg;//选项的参数指针

int getopt (int argc, char *const argv[], const char *opts)//optstring
{
    static int sp = 1;
    register int c;
    register char *cp;

    if (sp == 1)
        if (optind >= argc || argv[optind][0] != '-' || argv[optind][1] == '\0')
            return -1;
        else if (strcmp(argv[optind], "--") == 0) {
            optind++;
            return -1;
        }
    optopt = c = argv[optind][sp];
    if (c == ':' || (cp=strchr(opts, c)) == 0) {
        ERR (": illegal option -- ", c);
        if (argv[optind][++sp] == '\0') {
            optind++;
            sp = 1;
        }
        return '?';
    }
    if (*++cp == ':') {
        if (argv[optind][sp+1] != '\0')
            optarg = &argv[optind++][sp+1];
        else if (++optind >= argc) {
            ERR (": option requires an argument -- ", c);
            sp = 1;
            return '?';
        } else
            optarg = argv[optind++];
        sp = 1;
    } else {
        if (argv[optind][++sp] == '\0') {
            sp = 1;
            optind++;
        }
        optarg = 0;
    }
    return c;
}

bool KOperation::CommandInput(int argc, char* argv[])
{
    bool bResult  = false;
    bool bRetCode = false;
    char szPattern[PATTERN_SIZE];
    char szTestPath[MAX_LINE];

    if (argc == 2)
    {
        if (strcmp(argv[1], "help") == 0)
        {
            printf("usage: Search  KeyWord LogPath\n");
            printf("sample: Search KGLOG  E:/TestText/Log\n");
            printf("\nexplain:\n");
            printf("Two parameters, the first for the query keyword, the second for the query text path\n");
        }
    }

    else if (argc == 3)
    {
        KGLOG_PROCESS_ERROR(argv);

        KGLOG_PROCESS_ERROR(argv[1][0] != '\0');
        KGLOG_PROCESS_ERROR(argv[2][0] != '\0');

        KGLOG_PROCESS_ERROR(strlen(argv[1]) <= PATTERN_SIZE);

        strncpy(szPattern, argv[1], sizeof(szPattern));
        szPattern[sizeof(szPattern) - 1] = '\0';

        strncpy(szTestPath, argv[2], sizeof(szTestPath));
        szTestPath[sizeof(szTestPath) - 1] = '\0';

        _setmode(fileno(stdout), O_BINARY);

        bRetCode = Output((unsigned char*)szPattern, szTestPath);
        KGLOG_PROCESS_ERROR(bRetCode);
    }

    bResult = true;
Exit0:
    return bResult;
}

bool KOperation::Output(unsigned char* pszPattern, char* pszSourcePath)
{
    bool         bResult             = false;
    bool         bRetCode            = false;
    int          nFileCount          = 0;
    std::string  szSourcePath        = "";
    KBlockMatch* pBlockMatch         = NULL;

    KGLOG_PROCESS_ERROR(pszPattern);
    KGLOG_PROCESS_ERROR(pszSourcePath);

    szSourcePath.append(pszSourcePath);

    pBlockMatch = new KBlockMatch();
    KGLOG_PROCESS_ERROR(pBlockMatch);

    bRetCode = pBlockMatch->Init();
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = GetFiles(szSourcePath);
    KGLOG_PROCESS_ERROR(bRetCode);

    nFileCount = m_szFileList.size();
    if (nFileCount == 0)
    {
        printf("File does Exist\n");
    }

    KGLOG_PROCESS_ERROR(nFileCount > 0);

    for (int i = 0; i < nFileCount; i++)
    {
        int   nMatchCount = 0;

        nMatchCount = pBlockMatch->MatchWords(
            pszPattern, m_szFileList[i]
            );
        KGLOG_PROCESS_ERROR(nMatchCount >= 0);
    }

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

bool KOperation::GetFiles(std::string szPath)
{
    bool                    bResult        = false;
    int                     nRetCode       = 0;
    long                    hFile          = 0;;
    std::string             szFullPath     = "";
    std::string             szTempPath     = "";
    std::queue<std::string> szTempFileList;
    FILE*                   fpFile         = NULL;

    fpFile = fopen(szPath.c_str(), "rb");
    if (fpFile)
    {
        m_szFileList.push_back(szPath);

        fclose(fpFile);
        fpFile = NULL;

        bResult = true;
        goto Exit0;
    }

    szTempFileList.push(szPath);

    while (!szTempFileList.empty())
    {
        struct _finddata_t FileInfo;

        szTempPath = szTempFileList.front();
        szTempFileList.pop();

        szFullPath = szPath;
        szFullPath += "/*";

        hFile = _findfirst(szFullPath.c_str(), &FileInfo);

        if (hFile == -1)
        {
            nRetCode = _findclose(hFile);
            KGLOG_PROCESS_ERROR(nRetCode == 0);
            continue;
        }

        do
        {
            szFullPath = szTempPath;
            szFullPath += '/';
            szFullPath += FileInfo.name;
            if (FileInfo.attrib ==  _A_SUBDIR )
            {
                if (strcmp(FileInfo.name, ".") != 0 && strcmp(FileInfo.name, "..") != 0)
                {
                    szTempFileList.push(szFullPath);
                }
            }
            else
            {
                m_szFileList.push_back(szFullPath);
            }
        } while (_findnext(hFile,&FileInfo) == 0);

        nRetCode = _findclose(hFile);
        KGLOG_PROCESS_ERROR(nRetCode == 0);
    }

    bResult = true;
Exit0:
    return bResult;
}
