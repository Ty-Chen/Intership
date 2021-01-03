#ifndef _KSEARCH_H_
#define _KSEARCH_H_

#include "DefineMarco.h"

class KSearch
{
public:
    KSearch();
    ~KSearch();

    bool Init(
        int   nKeyLen, char* pszKeyWord,
        char* pszFilePath
    );
    void UnInit();

    bool RunSunday();
public:
    int   m_nCntResult;

    int   m_nKeyLen;
    char* m_pszKeyWord;
    char* m_pszFilePath;
};
#endif

