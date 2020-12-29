#ifndef _KBM_H
#define _KBM_H

#include "MacroDefinition.h"

class KBm
{
public:
    KBm();
    ~KBm();

    bool Init(char* pszPattern, int nPatternLen);
    void UnInit();

    int  GetMatchTime();

    bool Search(char* pszPattern, int nPatternLen, char* pszText, int nTextLen);

private:
    bool PrepareBadChar();
    bool PrepareSuffix();
    bool PrepareGoodSuffix();
    bool BoyerMoore(char* pszPattern, int nPatternLen, char* pszText, int nTextLen);

private:
    char* m_pszPattern;
    int   m_nPatternLen;
    int   m_nSumLine;

    int   m_nMatchTime;

    int   m_nGoodSuffix[MAX_PATTERN];
    int   m_nSuff[MAX_PATTERN];
    int   m_nBadChar[MAX_CHAR];
};
#endif // !_KBM_H
