///////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
//
// FileName : SearchText.cpp
// Creator :  chenlingyuan
// Date : 2020-12-23 16:21:00
// Comment : Test Match Result.
//
///////////////////////////////////////////////////////////////
#include<Windows.h>
#include<stdio.h>
#include<stdlib.h>
#include<time.h>

#include"KMacro.h"
#include"KBlockMatch.h"
#include"KInstruction.h"
#include"KMatchWords.h"

int main(int argc, char *argv[])
{
    bool          bRetCode     = false;
    KInstruction* pInstruction = NULL;

    pInstruction = new KInstruction();

    bRetCode = pInstruction->Init(argc, argv);
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = pInstruction->ShowInstruction();
    KGLOG_PROCESS_ERROR(bRetCode);

Exit0:
    if (pInstruction)
    {
        delete pInstruction;
        pInstruction = NULL;
    }

    return 0;
}
