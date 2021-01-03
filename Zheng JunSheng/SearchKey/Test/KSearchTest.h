#ifndef _KSEARCHTEST_H_
#define _KSEARCHTEST_H_

#include "DefineMarco.h"

class KSearchTest
{
public:
    KSearchTest();
    ~KSearchTest();

    bool Run(char* pszTestKeyWordPath, char* pszTestTxtPath);

private:
    bool GetFile(char* pszFilePath, char*& pszText, int& nTextLen);
    bool GetTestInfo(
        char* pszTestInfoPath, char szTestInfo[][MAX_PATH_LEN],
        int& nCntTest
    );
    bool GetTestFileType(
        char szTestFilePath[][MAX_PATH_LEN], int nCntTest,
        char szTestType[][MAX_PATH_LEN]
    );
};
#endif;
