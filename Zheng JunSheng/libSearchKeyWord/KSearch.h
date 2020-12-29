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
        char* pszFilePath, char* pszResultPath
    );
    void UnInit();

    bool RunBigFile();
public:
    int   m_nCntResult;

    int   m_nKeyLen;
    char* m_pszKeyWord;
    char* m_pszFilePath;
    char* m_pszResultPath;
};
#endif

