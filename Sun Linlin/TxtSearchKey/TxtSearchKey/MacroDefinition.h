#ifndef _KGLOG_H
#define _KGLOG_H

#include <stdio.h>

#include <iostream>
#include <algorithm>

#define MAX_CHAR        256
#define MAX_PATTERN     280
#define MAX_PATHLEN     2000
#define MAX_FILE        100
#define MAX_BUFFER_SIZE 1024 * 1024 * 56
#define MAX_TEST_NUM    50
#define KEYWORD_PATH    ".\\One_KeyWord.txt"
#define FILEPATH        ".\\One_FilePath.txt"
#define RESULTPATH      ".\\Match_Result\\"
#define HELPPATH        ".\\help.txt"

#undef  WHILE_FALSE_NO_WARNING
#define WHILE_FALSE_NO_WARNING while(false)
#undef  KGLOG_PROCESS_ERROR
#define KGLOG_PROCESS_ERROR(Condition) \
    do  \
    {   \
        if (!(Condition))       \
        {                       \
            printf(             \
                "KGLOG_PROCESS_ERROR(%s) at line %d ", #Condition, __LINE__ \
            );                  \
            goto Exit0;         \
        }                       \
    } WHILE_FALSE_NO_WARNING

#endif // !_KGLOG_H
