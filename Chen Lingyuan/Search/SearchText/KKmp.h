#pragma once
#ifndef KKMP_H_
#define KKMP_H_

#include"KMacro.h"

class KKmp
{
public:
    KKmp();
    ~KKmp();

    bool Init( const char* pcszPattern,int nPatternLen);

    int KmpCount(
        const char* pcszText, int nTextLength,
        const char* pcszPattern, int nPatternLen,
        int* pnMatchArray, int nMatchArrayLen
    );


private:
    bool GetNext(
        const char* pcszPattern,int nPatternLen,
        int* pnNext, int nNextLen
    );

private:
    int m_nNext[PATTERN_SIZE];
    int m_nNextLen;
};
#endif // !KKMP_H
