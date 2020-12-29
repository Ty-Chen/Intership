#ifndef _KBM_H_
#define _KBM_H_

#include "DefineMarco.h"

class KBM
{
public:
    KBM();
    ~KBM();

    bool Init(
        char* pszKeyWord, const int nKeyLen,
        char* pszText, const int nTextLen,
        char* pszResultPath
    );
    void UnInit();

    bool Run();
    bool RunSingleChar();
    bool RunNewText(char* pszText, int nTextLen);

private:
    bool PrepareGoodSuffix(
        int* pnGoodSuffix, const int nGoodSuffixSize,
        int* pnSuffix, const int nSuffixSize
    );

    bool PrepareBadChar(
        int nBadChar[], const int nBadCharSize
    );

public:
    int   m_nCntResult;

private:
    char* m_pszKeyWord;
    int   m_nKeyLen;

    int*  m_pnSuffix;
    int*  m_pnGoodSuffix;
    int   m_nBadChar[ASCII_SIZE];

    char* m_pszText;
    int   m_nTextLen;

    char* m_pszResultPath;
};
#endif
