#ifndef _KSUNDAY_H_
#define _KSUNDAY_H_

#include "DefineMarco.h"

class KSunday
{
public:
    KSunday();
    ~KSunday();

    bool Init(
        char* pszKeyWord, const int nKeyLen,
        char* pszResultPath
    );
    void UnInit();

    bool Run(char* pszText, int nTextLen);

public:
    int   m_nCntResult;

private:
    char* m_pszKeyWord;
    int   m_nKeyLen;

    char* m_pszResultPath;

    int m_nSunday[ASCII_SIZE];
};
#endif
