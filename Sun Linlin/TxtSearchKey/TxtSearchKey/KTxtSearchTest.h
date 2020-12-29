#ifndef _KTXTSEARCHTEST_H
#define _KTXTSEARCHTEST_H

#include "KSearch.h"

class KTxtSearchTest
{
public:
    KTxtSearchTest();
    ~KTxtSearchTest();

    bool Init(const char* pszPatternPath, const char* pszPathFile, int nTestNum);
    void UnInit();

    bool TestRun();

private:
    bool TxtFilePathInput(const char* pszFilePath);
    bool PatternInput(const char* pszPatternPath);

private:
    char*   m_pszPattern;
    int     m_nPatternPathLen;

    int     m_nTestNum;

    char    m_szFilePath[MAX_FILE][MAX_PATHLEN];
    int     m_nFilePathNum;
};

#endif // !_KTXTSEARCHTEST_H
