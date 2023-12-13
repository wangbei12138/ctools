/**
 * @file rcst_log.h
 * Coding: utf-8
 * Author   : liushuo
 * time     : 2023/12/13 11:00
*/
#ifndef __RCST_LOG_H__
#define __RCST_LOG_H__

#include <string.h>

/*通用字符串存储大小定义*/
#define STR_COMM_SIZE 128
#define STR_MAX_SIZE 1024

#define MAX_LOG_FILE_NUM        (3)
#define NUMBER(type) sizeof(type)/sizeof(type[0])
#define __FILENAME__ (strrchr(__FILE__, '/') ? (strrchr(__FILE__, '/') + 1) : __FILE__)

/*日志类型*/
enum
{
    LOG_DEBUG = 0,/*调试日志*/
    LOG_ERROR,/*错误日志*/
    LOG_WARNING,/*告警日志*/
    LOG_ACTION,/*运行日志*/
    LOG_SYSTEM,/*系统日志*/
    BUTTOM
};

/*将日志输出到终端*/
#define PRINT_LOG_TO_TERM (0)
/*将日志输出到文件中*/
#define PRINT_LOG_TO_FILE (1)

/*调试日志宏定义*/
#define DEBUG_PRINT 1
#define LOG_PRINT(fmt, ...) do{\
    if(DEBUG_PRINT)\
    {\
        printf(fmt"  [line:%d] [%s]\n", ##__VA_ARGS__, __LINE__, __FUNCTION__);\
    }\
} while(0);

/*错误日志打印(在日志打印模块还未启动时使用)*/
#define LOG_ERR(fmt, ...) do{\
    printf("[ERROR]  "fmt"  [line:%d] [%s]\n", ##__VA_ARGS__, __LINE__, __FUNCTION__);\
} while(0);

/*存储日志标记. 0-不存储日志, 1-存储日志*/
extern unsigned long g_print_log_place_flag;

/*是否打印调试日志标记,0-不打印调试日志,1-打印调试日志*/
extern unsigned long g_print_debug_log_flag;

unsigned long rcst_log_print(unsigned char type, unsigned char *log_info);


/*日志打印宏定义*/
#define LOG_INFO(type, fmt, ...) do{\
    if(PRINT_LOG_TO_TERM == g_print_log_place_flag) \
    { \
        if((0 == g_print_debug_log_flag) && (LOG_DEBUG == type)) \
        {\
            break;\
        }\
        unsigned char log_info[STR_MAX_SIZE] = {0}; \
        snprintf((char *)log_info, sizeof(log_info) - 1, "  [%s] [line:%d] [%s]"fmt, __FILENAME__, __LINE__, __FUNCTION__, ##__VA_ARGS__); \
        rcst_log_print(type, log_info); \
    } \
    else \
    { \
        unsigned char log_info[STR_MAX_SIZE] = {0}; \
        snprintf((char *)log_info, sizeof(log_info) - 1, "  [%s] [line:%d] [%s]"fmt, __FILENAME__, __LINE__, __FUNCTION__, ##__VA_ARGS__); \
        rcst_log_print(type, log_info); \
    } \
} while(0)

/*是否打印调试日志标记,0-不打印调试日志,1-打印调试日志*/
void rcst_log_set_print_debug_flag(unsigned long flag);
/*存储日志标记. 0-不存储日志, 1-存储日志*/
void rcst_log_set_print_place_flag(unsigned long flag);

int rcst_log_init(const unsigned char *log_file_name, unsigned long file_size);
void rcst_log_destroy(void);

#endif // __RCST_LOG_H__