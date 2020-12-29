#include "KHelp.h"

KHelp::KHelp()
{
    m_pszHelpText = NULL;
}

KHelp::~KHelp()
{
    UnInit();
}

bool KHelp::Init()
{
    bool  bResult  = false;
    int   nRetCode = 0;
    int   nHelpLen = 0;
    int   nHelpPos = 0;
    FILE* fpFile   = NULL;

    fpFile = fopen(HELPPATH, "rb");
    KGLOG_PROCESS_ERROR(fpFile);

    nHelpPos = fseek(fpFile, 0L, SEEK_END);
    KGLOG_PROCESS_ERROR(nHelpPos == 0);

    nHelpLen = ftell(fpFile);
    KGLOG_PROCESS_ERROR(nHelpLen >= 0);

    nHelpPos = fseek(fpFile, 0L, SEEK_SET);
    KGLOG_PROCESS_ERROR(nHelpPos == 0);

    m_pszHelpText = new char[nHelpLen + 1];
    KGLOG_PROCESS_ERROR(m_pszHelpText);

    nRetCode = fread(m_pszHelpText, 1, nHelpLen, fpFile);
    KGLOG_PROCESS_ERROR(nRetCode == nHelpLen);

    m_pszHelpText[nHelpLen + 1] = '\0';

    bResult = true;
Exit0:
    if (fpFile)
    {
        fclose(fpFile);
        fpFile = NULL;
    }
    if (!bResult)
    {
        if (m_pszHelpText)
        {
            delete[] m_pszHelpText;
            m_pszHelpText = NULL;
        }
    }
    return bResult;
}

void KHelp::UnInit()
{
    if (m_pszHelpText)
    {
        delete[] m_pszHelpText;
        m_pszHelpText = NULL;
    }
}

bool KHelp::TxtSearchKeyOption(int argc, char* argv[])
{
    bool       bResult    = false;
    bool       bRetCode   = false;
    int        nPrintType = 1;
    KSearch*   pSearch    = NULL;

    if (argc < 4 && argc > 6)
    {
        //std::cout << "用法 :" << "[grep \"KeyWord\" \"FilePath\"]" << std::endl;
        //std::cout << "[time grep \"KeyWord\" \"FilePath\"]" << std::endl;
        //std::cout << "[time grep -rc \"KeyWord\" \"FilePath\"]" << std::endl;
        //std::cout << "[time grep -r \"KeyWord\" \"FilePath\" > \"ResultPath\"]" << std::endl;

        bRetCode = PrintHelp(nPrintType);
    }
    else
    {
        KGLOG_PROCESS_ERROR(argv[1]);
        KGLOG_PROCESS_ERROR(argv[2]);
        KGLOG_PROCESS_ERROR(argv[3]);
        if (argc == 4)
        {

        }
        pSearch = new KSearch();
        KGLOG_PROCESS_ERROR(pSearch);

        bRetCode = pSearch->Init(argv[2]);
        KGLOG_PROCESS_ERROR(bRetCode);

        bRetCode = pSearch->SearchBm(argv[3]);
        KGLOG_PROCESS_ERROR(bRetCode);


        bResult = true;
    }

Exit0:
    if (pSearch)
    {
        delete pSearch;
        pSearch = NULL;
    }
    return bResult;
}

bool KHelp::PrintHelp(int nPrintType)
{
    bool  bResult  = false;

    if (nPrintType == 1)
    {
        std::cout << m_pszHelpText << std::endl;
    }
    else {
        std::cout << "无该命令请重新输入" << std::endl;
    }
    return bResult;
}
