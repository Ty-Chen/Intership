#include "KInstruction.h"

KInstruction::KInstruction()
{

}

KInstruction::~KInstruction()
{

}

bool KInstruction::Init(int argc, char* argv[])
{
    bool bResult = false;

    KGLOG_PROCESS_ERROR(argv);

    m_nArgNum = argc;
    m_pszArgv = argv;

    bResult = true;
Exit0:
    return bResult;
}

bool KInstruction::ShowInstruction()
{
    bool bResult      = false;
    bool bRetCode     = false;
    char cInstruction;
    bool bEndProcess  = false;

    printf("选项:\n");

    printf("T           测试模式串匹配\n");
    printf("H           参数说明\n");
    printf("E           退出程序\n");

    while (!bEndProcess)
    {
        scanf("%c", &cInstruction);
        getchar();

        switch (cInstruction)
        {
        case 'T' :
        {
            bRetCode = MatchWordsTest();
            KGLOG_PROCESS_ERROR(bRetCode);
            break;
        }

        case 'H' :
        {
            bRetCode = ParameterDescription();
            KGLOG_PROCESS_ERROR(bRetCode);
            break;
        }

        case 'E' :
        {
            bEndProcess = true;
            break;
        }

        default:
        {
            printf("无法识别的选项\n");
        }

        }
    }

    bResult = true;
Exit0:
    return bResult;
}

bool KInstruction::MatchWordsTest()
{
    bool         bResult          = false;
    bool         bRetCode         = false;
    KMatchTest*  pMatchTest       = NULL;
    char         szTestDirectory[MAX_LINE];
    char         szContrastDirectory[MAX_LINE];
    char         szKeyWordsDirectory[MAX_LINE];
    char         szMatchResultDirectory[MAX_LINE];

    KGLOG_PROCESS_ERROR(m_nArgNum == 5);

    KGLOG_PROCESS_ERROR(m_pszArgv[1][0] != '\0');
    strncpy(szKeyWordsDirectory, m_pszArgv[1], sizeof(szKeyWordsDirectory));
    szKeyWordsDirectory[sizeof(szKeyWordsDirectory) - 1] = '\0';

    KGLOG_PROCESS_ERROR(m_pszArgv[2][0] != '\0');
    strncpy(szTestDirectory, m_pszArgv[2], sizeof(szTestDirectory));
    szTestDirectory[sizeof(szTestDirectory) - 1] = '\0';

    KGLOG_PROCESS_ERROR(m_pszArgv[3][0] != '\0');
    strncpy(szMatchResultDirectory, m_pszArgv[3], sizeof(szMatchResultDirectory));
    szMatchResultDirectory[sizeof(szMatchResultDirectory) - 1] = '\0';

     KGLOG_PROCESS_ERROR(m_pszArgv[4][0] != '\0');
    strncpy(szContrastDirectory, m_pszArgv[4], sizeof(szContrastDirectory));
    szContrastDirectory[sizeof(szContrastDirectory) - 1] = '\0';

    pMatchTest = new KMatchTest();
    KGLOG_PROCESS_ERROR(pMatchTest);

    bRetCode = pMatchTest->Init(
        szKeyWordsDirectory, szTestDirectory,
        szMatchResultDirectory, szContrastDirectory
    );
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = pMatchTest->Match();
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    if (pMatchTest)
    {
        delete pMatchTest;
        pMatchTest = NULL;
    }
    return bResult;
}

bool KInstruction::ParameterDescription()
{
    bool bResult = false;

    KGLOG_PROCESS_ERROR(m_nArgNum == 5);

    printf("参数数量: %d\n", m_nArgNum - 1);

    printf("参数一 : %s ,          说明:关键字列表\n", m_pszArgv[1]);
    printf("参数二 : %s ,          说明:测试文档列表\n", m_pszArgv[2]);
    printf("参数三 : %s ,          说明:测试结果文档列表\n", m_pszArgv[3]);
    printf("参数四 : %s ,          说明:对照文档列表\n", m_pszArgv[4]);

    bResult = true;
Exit0:
    return bResult;
}
