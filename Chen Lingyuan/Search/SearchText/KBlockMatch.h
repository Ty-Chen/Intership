#pragma once
#ifndef KBLOCKMATCH_H_
#define KBLOCKMATCH_H_

#include<windows.h>
#include<psapi.h>
#include<stdio.h>

#include<vector>

#include"KMacro.h"
#include"KSunday.h"

struct  KPART_OF_FILE
{
    int*  pnMatchArray;
    int   nMatchArrayLen;
    int   nTextLen;
    char* pText;
};

class KBlockMatch
{
public:
    KBlockMatch();
    ~KBlockMatch();

    bool Init();
    void UnInit();

    int MatchWords(unsigned char* pszPattern, char* pszTestPath);

    long long TraversBlock(char* pszTestPath,FILE* fpResultFile, bool IsOutputConsole);

private:


     bool ReadFileBlock(FILE* fpFile);

     bool  CollectDate(
         int* pnMatchArray, int nMatchArrayLen,
         unsigned char* pszText, int nTextLen,
         FILE* fpResult, char* pszTestPath, bool IsOutputConsole
     );

     bool IsReadComplete();

private:
    unsigned char*  m_pszReadText[THREAD_NUM];
    int             m_nReadTextLen[THREAD_NUM];

    unsigned char*  m_pszPerformText[THREAD_NUM];
    int             m_nPerformTextLen[THREAD_NUM];

    unsigned char*  m_pszOutputText;

    int*            m_pnTempMatch[THREAD_NUM];
    int             m_nTempMatchLen[THREAD_NUM];

    std::vector<KPART_OF_FILE> m_FileBlock;
};

#endif
