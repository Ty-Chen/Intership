#ifndef _DEFINE_MARCO_H_
#define _DEFINE_MARCO_H_

#undef  WHILE_FALSE_NO_WARNING
#define WHILE_FALSE_NO_WARNING while(false)

#undef  KGLOG_PROCESS_ERROR
#define KGLOG_PROCESS_ERROR(condition) \
    do                                 \
    {                                  \
        if (!(condition))              \
        {                              \
            printf("KGLOG_PROCESS_ERROR( %s ) at line %d in %s\n", #condition, __LINE__, __FUNCTION__); \
            goto Exit0;                \
        }                              \
    } WHILE_FALSE_NO_WARNING

#define ASCII_SIZE 256
#define MAX_PATH_LEN 1000
#define MAX_BUFFER 1024 * 1024 * 50
#define MAX_LINE_LEN 1024
#define TEST_MAX_NUM 30
#define TEST_RESULT_PATH "..\\Debug\\testResult.txt"

#endif


