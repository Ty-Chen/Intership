#ifndef _KSUNDAY_H_
#define _KSUNDAY_H_

#include "DefineMarco.h"

class KSunday
{
public:
    KSunday();
    ~KSunday();

    bool Init(
        unsigned char* pszKeyWord, const int nKeyLen
    );
    void UnInit();

    bool Run(unsigned char* pszText, int nTextLen);

public:
    int m_nCntResult;

private:
    int            m_nKeyLen;
    unsigned char* m_pszKeyWord;

    int m_nSunday[ASCII_SIZE];

    char m_pszResult[MAX_BUFFER + 1];
};
#endif
