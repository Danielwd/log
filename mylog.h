/*
 * logc.h
 *
 *  Created on: Jul 10, 2014
 *      Author: zjp35
 */
 
#ifndef MYLOG_H_
#define MYLOG_H_
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <stdarg.h>
#include <unistd.h>
 
#define MAXLEN (2048)
#define MAXLINE (1000)    //日志最大存储数量
#define MAXCHAR (128)
#define MAXFILEPATH (512)
#define MAXFILENAME (50)
#define PRE_STR "[%s:%s(%d)] ===> "
#define VAL_STR ,__FILE__,__FUNCTION__,__LINE__

#define LOG_INFO(format,...)   Log_INFO(PRE_STR format VAL_STR ,##__VA_ARGS__)
#define LOG_ERROR(format,...)  Log_ERROR(PRE_STR format VAL_STR ,##__VA_ARGS__)
#define LOG_WARN(format,...)   Log_WARN(PRE_STR format VAL_STR ,##__VA_ARGS__)
#define LOG_DEBUG(format,...)  Log_DEBUG(PRE_STR format VAL_STR ,##__VA_ARGS__)
#define LOG_TRACE(format,...)  Log_TRACE(PRE_STR format VAL_STR ,##__VA_ARGS__)



typedef enum{
    ERROR_1=-1,
    ERROR_2=-2,
    ERROR_3=-3
}ERROR0;
 
 
typedef enum{
    NONE=0,
	TRACE=1,
	DEBUG=2,
    INFO=3,
    WARN=4,
    ERROR=5,
    ALL=255
}LOGLEVEL;
 
typedef struct log{
    char logtime[20];
    char filepath[MAXFILEPATH];
    FILE *logfile;
}LOG;
 
typedef struct logseting{
    char filepath[MAXFILEPATH];
    unsigned int maxfilelen;
    unsigned char loglevel;
}LOGSET;
 
int Log_INFO(char *fromat,...);
int Log_TRACE(char *fromat,...);
int Log_DEBUG(char *fromat,...);
int Log_WARN(char *fromat,...);
int Log_ERROR(char *fromat,...);
#endif 
