#pragma once
#ifndef KMACRO_H_
#define KMACRO_H_

#undef  WHILE_FALSE_NO_WARNING
#define WHILE_FALSE_NO_WARNING while(false)
#undef  KGLOG_PROCESS_ERROR
#define KGLOG_PROCESS_ERROR(condition) \
    do                                 \
    {                                  \
        if(!(condition))               \
        {                              \
            printf("KGLOG_PROCESS_ERROR(%s) at line %d  in %s\n", #condition, __LINE__, __FILE__); \
            goto Exit0;                \
        }                              \
    } WHILE_FALSE_NO_WARNING

#define ASCLL  256
#define PATTERN_SIZE 300
#define BUFFER_SIZE (1024 * 1024 * 1)
#define MAX_FILE_SIZE (1024 * 1024 * 200)
#define MAX_LINE 1024
#define MATCH_RESULT  "Match_Result_Statistics.txt"
#define THREAD_NUM 8


#endif // !MacroDefinition
