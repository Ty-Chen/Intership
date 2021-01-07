#include "KOperation.h"

#include<time.h>

#ifdef WIN32
#include<io.h>
#include<fcntl.h>
#else
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#endif

#include<string.h>

#include<iostream>
#include<vector>
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

bool KOperation::CommandInput(int argc, char* argv[])
{
    bool bResult      = false;
    bool bRetCode     = false;
    bool bIsHelp      = false;
    int  nRestultChar = 0;
    int  nOptind      = 1;
    char* pszPattern  = NULL;
    char* pszTestPath = NULL;

    nRestultChar = Getopt(argc, argv, "hp:f:", &nOptind);

    while(nRestultChar != -1)
    {
        switch(nRestultChar)
        {
            case 'h':
                bIsHelp = true;
                break;
            case 'p':
                pszPattern  = m_pszOptarg;
                break;
            case 'f':
                pszTestPath = m_pszOptarg;
                break;
            case '?':
                printf("Command cannot be executed\n");
                goto Exit0;
                break;
            default:
                break;
        }
        nRestultChar = Getopt(argc, argv, "hp:f:", &nOptind);
    }

    if (bIsHelp && !pszTestPath && !pszPattern)
    {
        printf(
            "usage: Search  KeyWord LogPath\n"
            "sample: Search KGLOG  E:/TestText/Log\n"
            "\nexplain:\n"
            "Two parameters, the first for the query keyword, the second for the query text path\n"
        );
    }
    else if(pszTestPath && pszPattern && !bIsHelp)
    {
        KGLOG_PROCESS_ERROR(strlen(pszPattern) <= PATTERN_SIZE);

#ifdef WIN32
		_setmode(fileno(stdout), O_BINARY);
#endif

        bRetCode = Output((unsigned char*)pszPattern, pszTestPath);
        KGLOG_PROCESS_ERROR(bRetCode);
    }
	else
	{
		printf("Command cannot be executed\n");
	}

    bResult = true;
Exit0:
    return bResult;
}

int KOperation:: Getopt(int argc, char* argv[], const char* pszOpstrintg, int* pnOptind)
{
    bool bResult          = false;
    int  nOptind          = 0;
    char cRetChar         = 0;
    char* pParameterStart = NULL;

    KGLOG_PROCESS_ERROR(argv);
    KGLOG_PROCESS_ERROR(pnOptind);
    KGLOG_PROCESS_ERROR(pszOpstrintg);

	nOptind = *pnOptind;

    if (nOptind >= argc || argv[nOptind][0] != '-' || argv[nOptind][1] == '\0')
    {
        goto Exit0;
    }

    else if (strcmp(argv[nOptind], "--") == 0)
    {
        nOptind++;
        *pnOptind = nOptind;
        goto Exit0;
    }

    cRetChar = argv[nOptind][1];

    pParameterStart = (char*)strchr(pszOpstrintg, cRetChar);

    if (pParameterStart == NULL || cRetChar ==':')
    {
        nOptind++;
        *pnOptind = nOptind;

        m_pszOptarg = NULL;

        bResult  = true;
        cRetChar = '?';
        goto Exit0;
    }
    if (*++pParameterStart == ':')
    {
        if (argv[nOptind][2] != '\0')
        {
            m_pszOptarg = &argv[nOptind][2];
            *pnOptind += 1;
        }
        else if(++nOptind >= argc)
        {
            *pnOptind = nOptind;
            cRetChar = '?';
        }
        else
        {
            m_pszOptarg = argv[nOptind];

            nOptind++;
            *pnOptind = nOptind;
        }
    }
    else
    {
        if(argv[nOptind][2] != '\0')
        {
            cRetChar = '?';
        }
        nOptind++;
        *pnOptind = nOptind;
        m_pszOptarg = NULL;
    }

    bResult = true;
Exit0:
    if (!bResult)
    {
        return -1;
    }

    return (int)cRetChar;
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
#ifdef WIN32
    bRetCode = GetWindowsFiles(szSourcePath);
    KGLOG_PROCESS_ERROR(bRetCode);
#else
	bRetCode = GetLinuxFiles(szSourcePath);
	KGLOG_PROCESS_ERROR(bRetCode);
#endif

    nFileCount = (int)m_szFileList.size();
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
#ifdef WIN32
bool KOperation::GetWindowsFiles(std::string szPath)
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

        szFullPath = szTempPath;
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
            if (FileInfo.attrib &  _A_SUBDIR )
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

#else
bool KOperation::GetLinuxFiles(std::string szPath)
{
	bool                    bResult         = false;
	int                     nRetCode        = 0;
	std::string             szFullPath      = "";
	std::string             szTempPath      = "";
	std::queue<std::string> szTempFileList;
	DIR                     *Dirstream      = NULL;
	struct dirent           *FileInfo       = NULL;
	struct stat              Stat;

	stat(szPath.c_str(), &Stat);

	if (!S_ISDIR(Stat.st_mode))
	{
		m_szFileList.push_back(szPath);
		bResult = true;
		goto Exit0;
	}

	szTempFileList.push(szPath);

	while (!szTempFileList.empty())
	{
		szTempPath = szTempFileList.front();
		szTempFileList.pop();

		szFullPath = szTempPath;

		Dirstream = opendir(szFullPath.c_str());
		if (Dirstream == NULL)
		{
			nRetCode = closedir(Dirstream);
			KGLOG_PROCESS_ERROR(nRetCode == 0);
			continue;
		}

		while (true)
		{
			FileInfo = readdir(Dirstream);

			if (!FileInfo)
			{
				break;
			}

			szFullPath = szTempPath;
			szFullPath += '/';
			szFullPath += FileInfo->d_name;

			if (strcmp(FileInfo->d_name, ".") == 0 || strcmp(FileInfo->d_name, "..") == 0)    ///current dir OR parrent dir
			{
				continue;
			}
			else if (FileInfo->d_type == DT_REG)    //常规文件
			{
				m_szFileList.push_back(szFullPath);
			}
			else if (FileInfo->d_type == DT_DIR)    //文件夹
			{
				szTempFileList.push(szFullPath);
			}
		}
		nRetCode = closedir(Dirstream);
		KGLOG_PROCESS_ERROR(nRetCode == 0);
	}

	bResult = true;
Exit0:
	return bResult;
}
#endif
