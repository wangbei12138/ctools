/** 
 * @file rcst_log.c
 * Coding: utf-8
 * Author   : liushuo
 * time     : 2023/10/11 17:00
*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <ctype.h>
#include <pthread.h>

#include <rcst_log.h>

/*存储日志的文件名*/
static unsigned char g_log_file_name[MAX_LOG_FILE_NUM][STR_COMM_SIZE] = {{0}};

/*指明是g_log_file_name中的哪个文件*/
static unsigned char g_log_file_no = 0;

/*输出日志位置标记,0-输出到终端,1-输出到日志文件*/
unsigned long g_print_log_place_flag = 0;
/*是否打印调试日志标记,0-不打印调试日志,1-打印调试日志*/
unsigned long g_print_debug_log_flag = 0;

/*日志文件大小*/
static unsigned long g_log_file_size = 0;

/*日志文件句柄*/
static FILE* pfile = NULL;

/*日志存储互斥锁*/
static pthread_mutex_t g_save_log_mutex_lock;

/*日志模块初始化标记*/
static unsigned long g_log_init_flag = 0;

void rcst_log_set_print_place_flag(unsigned long flag)
{
    g_print_log_place_flag = flag;
}

void rcst_log_set_print_debug_flag(unsigned long flag)
{
    g_print_debug_log_flag = flag;
}

/*****************************************************************
** 函数名: get_file_size
** 输　入: char *path
** 输　出:
** 功能描述:获取指令文件大小
** 返回值: long
****************************************************************/
static long get_file_size(const char *path)
{
    long filesize = -1;
    struct stat statbuff;
    
    if (stat(path, &statbuff) < 0) {
        return filesize;
    } else{
        filesize = statbuff.st_size;
    }

    return filesize;
}

/*****************************************************************
** 函数名: unsigned long rcst_log_print_logtime
** 输　入:  unsigned long buf_len 存储时间的空间长度
** 输　出:unsigned char *time  存储时间
** 功能描述:日志输出
** 返回值:unsigned long
****************************************************************/
unsigned long rcst_log_print_logtime(unsigned char *time, unsigned long buf_len)
{
    struct tm* time_sec;
    struct timeval time_msec;

    if (NULL == time) {
        return -1;
    }

    gettimeofday(&time_msec, NULL);
    time_sec = localtime(&time_msec.tv_sec);
    snprintf((char *)time, buf_len - 1, "%04d-%02d-%02d %02d:%02d:%02d %03ldms",
            time_sec->tm_year + 1900, time_sec->tm_mon + 1, time_sec->tm_mday, time_sec->tm_hour, 
            time_sec->tm_min, time_sec->tm_sec, time_msec.tv_usec / 1000);
    
    return 0;
}

/*****************************************************************
** 函数名:  unsigned char *rcst_log_type_to_str
** 输　入:    unsigned char type  日志类型
            unsigned long buf_len 存储日志类型字符串空间的长度
** 输　出:unsigned char *type_string 根据日志类型将其转换成相应的字符串
** 功能描述:根据日志类型转换成相应的字符串
** 返回值:unsigned long
****************************************************************/
unsigned long rcst_log_type_to_str(unsigned char type, unsigned char *type_string, unsigned long buf_len)
{
    if (NULL == type_string) {
        return -1;
    }
    /*防止发生越界*/
    buf_len -= 1;

    switch(type)
    {
        case LOG_DEBUG:
        {
            strncpy((char *)type_string, "DEBUG", buf_len);
            break;
        }
        case LOG_ERROR:
        {
            strncpy((char *)type_string, "ERROR", buf_len);
            break;
        }
        case LOG_WARNING:
        {
            strncpy((char *)type_string, "WARNING", buf_len);
            break;
        }
        case LOG_ACTION:
        {
            strncpy((char *)type_string, "ACTION", buf_len);
            break;
        }
        case LOG_SYSTEM:
        {
            strncpy((char *)type_string, "SYSTEM", buf_len);
            break;
        }
        default:
        {
            strncpy((char *)type_string, "UNKNOWN", buf_len);
            break;
        }
    }

    return 0;
}

/*****************************************************************
** 函数名: unsigned long rcst_log_open_log_file
** 输　入:  void
** 输　出:void
** 功能描述:打开日志文件
** 返回值:unsigned long
****************************************************************/
unsigned long rcst_log_open_log_file(void)
{
    char *path = (char*)g_log_file_name[g_log_file_no];
    char *flag = NULL;
    int len = 0;
    
    /*判断文件是否已经打开*/
    if (NULL != pfile) {
        return 0;
    }
    /*判断文件名是否有定义*/
    if (NULL == path) {
        LOG_PRINT("[ERROR] file name is NULL.");
        return -1;
    }

    /*判断文件是否存在*/
    if (!access(path, 0)) {
        /*获取文件大小*/
        if (0 > (len = get_file_size(path)))
        {
            LOG_PRINT("[ERROR] get file size failed!");
            return -1;
        }
    }
    flag = (len > 0 && len < g_log_file_size) ? "a" : "w";

    /*打开文件*/
    pfile = fopen(path, flag);
    if (NULL == pfile) {
        LOG_PRINT("[ERROR] open file failed!");
        return -1;
    }

    LOG_PRINT("[DEBUG] open file name = %s", path);
    return 0;
}
 
/*****************************************************************
** 函数名: rcst_log_print
** 输　入:  unsigned char *uc_log_info  需要打印或者存储的日志信息
            unsigned char type 日志类型
** 输　出:void
** 功能描述:日志输出
** 返回值:unsigned long
****************************************************************/
unsigned long rcst_log_print(unsigned char type, unsigned char *log_info)
{
    unsigned long result = 0;
    unsigned long file_len = 0;
    unsigned char time[STR_COMM_SIZE] = {0};
    unsigned char log_type_str[STR_COMM_SIZE] = {0};
    unsigned char uc_log_info[STR_MAX_SIZE] = {0};

    if (NULL == log_info) {
        return -1;
    }

    /*将日志类型转换成字符串*/
    result = rcst_log_type_to_str(type, log_type_str, sizeof(log_type_str));
    /*获取生成日志的时间*/
    result += rcst_log_print_logtime(time, sizeof(time));
    if (0 != result) {
        return -1;
    }

    snprintf((char *)uc_log_info, sizeof(uc_log_info) - 1, "[%s] [%s] %s", time, log_type_str, log_info);
    /*判断是否打印调试日志*/
    if (PRINT_LOG_TO_TERM == g_print_log_place_flag) {
        printf("%s", uc_log_info);
        return 0;
    }
    /*加锁保护文件操作*/
    pthread_mutex_lock(&g_save_log_mutex_lock);
    /*打开日志文件*/
    (void)rcst_log_open_log_file();
    if (NULL != pfile) {
        fputs((char *)uc_log_info, pfile);
        /* 新加的，保证打开文件时能够写入 */
        fflush(pfile);
        file_len = ftell(pfile);

        if (file_len >= g_log_file_size)
        {
            fclose(pfile);
            pfile = NULL;
            g_log_file_no = (g_log_file_no + 1) % MAX_LOG_FILE_NUM;
        }
    }
    pthread_mutex_unlock(&g_save_log_mutex_lock);
    return 0;
}

/*****************************************************************
** 函数名: rcst_log_init
** 输　入:  const unsigned char* log_file_name  用来保存日志的文件名
			int file_size 存储日志的文件大小
** 输　出:void
** 功能描述:日志打印初始化
** 返回值: int
****************************************************************/
int rcst_log_init(const unsigned char* log_file_name, unsigned long file_size)
{
    unsigned int i = 0;
    /*判断参数的合法性*/
    if ((NULL == log_file_name) || !(file_size > 0)) {
        return -1;
    }
    /*判断是否将日志输出到日志文件*/
    if ((PRINT_LOG_TO_FILE != g_print_log_place_flag) || (0 != g_log_init_flag)) {
        printf("g_print_log_place_flag = %lu g_log_init_flag = %lu\n",g_print_log_place_flag, g_log_init_flag);
        LOG_PRINT("print log to termination!!");
        return 0;
    }

    /*记录日志模块已经被初始化(防止改模块被重复初始化)*/
    g_log_init_flag = 1;

    /*生成存储日志的文件名*/
    for (i = 0; i < NUMBER(g_log_file_name); i++) {
        snprintf((char *)g_log_file_name[i], sizeof(g_log_file_name[i]) - 1, "%s_%02u", log_file_name, i);
        LOG_PRINT("Log File: %s", g_log_file_name[i]);
        printf("Log File: %s\n", g_log_file_name[i]);
    }
    /*设置日志文件大小*/
    g_log_file_size = file_size;
    LOG_PRINT("g_log_file_size:%lu\n", g_log_file_size);
    pthread_mutex_init(&g_save_log_mutex_lock, NULL);

    return 0;
}

/*****************************************************************
** 函数名: rcst_log_destroy
** 输　入:  void
** 输　出:void
** 功能描述:日志打印资源释放
** 返回值:unsigned long
****************************************************************/
void rcst_log_destroy(void)
{
    if (pfile != NULL) {
        fclose(pfile);
        pfile = NULL;
    }
    pthread_mutex_destroy(&g_save_log_mutex_lock);
    return;
}