﻿#pragma once
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
};

class KBlockMatch
{
public:
    KBlockMatch();
    ~KBlockMatch();

    bool Init();
    void UnInit();

    int MatchWords(unsigned char* pszPattern, char* pszResultPath, char* pszTestPath);

    long long TraversBlock(char* pszTestPath,char* pszResultPath);

private:
     bool ReadFileBlock(FILE* fpFile);

     bool  CollectDate(
         int* pnMatchArray, int nMatchArrayLen,
         unsigned char* pszText, int nTextLen,
         FILE* fpResult
     );

private:
    unsigned char*  m_pszReadText[THREAD_NUM];
    int             m_nReadTextLen[THREAD_NUM];

    unsigned char*  m_pszPerformText[THREAD_NUM];
    int             m_nPerformTextLen[THREAD_NUM];

    unsigned char*  m_pszOutputText;

    int*            m_pnTempMatch[THREAD_NUM];
    int             m_nTempMatchLen[THREAD_NUM];

    char            m_szBlockLine[MAX_LINE];
    int             m_nBlockLineLen;

    std::vector<KPART_OF_FILE> m_FileBlock;
};

#endif
