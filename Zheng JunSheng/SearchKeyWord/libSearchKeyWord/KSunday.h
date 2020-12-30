#ifndef _KSUNDAY_H_
#define _KSUNDAY_H_

#include "DefineMarco.h"

class KSunday
{
public:
    KSunday();
    ~KSunday();

    bool Init(
        unsigned char* pszKeyWord, const int nKeyLen,
        char* pszResultPath
    );
    void UnInit();

    bool Run(unsigned char* pszText, int nTextLen);

public:
    int   m_nCntResult;

private:
    int            m_nKeyLen;
    unsigned char* m_pszKeyWord;

    char* m_pszResultPath;

    int m_nSunday[ASCII_SIZE];
};
#endif
