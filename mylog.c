/*
 * logc.c
 *
 */
#include "mylog.h"
#define MAXLEVELNUM (3)

LOGSET logsetting;
LOG loging;

void del_first_line();
const static char LogLevelText[6][10]={"TRACE","DEBUG","INFO","WARN","ERROR","NONE"};

static char * getdate(char *date);
static pthread_mutex_t log_lock = PTHREAD_MUTEX_INITIALIZER;

static unsigned char getcode(char *path){
	unsigned char code=255;
	if(strcmp("TRACE",path)==0)
		code=1;
	else if(strcmp("DEBUG",path)==0)
		code=2;
	else if(strcmp("INFO",path)==0)
		code=3;
	else if(strcmp("WARN",path)==0)
		code=4;
	else if(strcmp("ERROR",path)==0)
		code=5;
	else if(strcmp("NONE",path)==0)
		code=0;
	return code;
}

static unsigned char ReadConfig(char *path){
	char value[512]={0x0};
	char data[50]={0x0};

	FILE *fpath=fopen(path,"r");
	if(fpath==NULL)
		return -1;
	fscanf(fpath,"path=%s\n",value);
	//getdate(data);
	strcat(data,"radar");
	strcat(data,".log");
	strcat(value,"/");
	strcat(value,data);
	if(strcmp(value,logsetting.filepath)!=0)
		memcpy(logsetting.filepath,value,strlen(value));
	memset(value,0,sizeof(value));

	fscanf(fpath,"level=%s\n",value);
	logsetting.loglevel=getcode(value);
	fclose(fpath);
	return 0;
}
/*
 *日志设置信息
 * */
static LOGSET *getlogset(){
	char path[512]={0x0};
	getcwd(path,sizeof(path));
	//  getcwd(path,sizeof(path));
	strcat(path,"/log.conf");
	if(access(path,F_OK)==0)
	{
		if(ReadConfig(path)!=0)
		{
			logsetting.loglevel=INFO;
			logsetting.maxfilelen=4096;
		}
	}
	else
	{
		logsetting.loglevel=255;
		logsetting.maxfilelen=4096;
	}
	return &logsetting;
}

/*
 *获取日期
 * */
static char * getdate(char *date){
	time_t timer=time(NULL);
	strftime(date,11,"%Y-%m-%d",localtime(&timer));
	return date;
}

/*
 *获取时间
 * */
static void settime(){
	time_t timer=time(NULL);
	strftime(loging.logtime,20,"%Y-%m-%d %H:%M:%S",localtime(&timer));
}

/*
 *不定参打印
 * */
static void PrintfLog(char * format,va_list args)
{
	char str[512];
	//int d;
	//char c,*s;
#if 0
	while(*format)
	{
		switch(*format){
		case 's':{
					 printf("2222\n");
					 s = va_arg(args, char *);
					 fprintf(loging.logfile,"%s",s);
					 break;}
		case 'd':{
					 d = va_arg(args, int);
					 fprintf(loging.logfile,"%d",d);
					 break;}
		case 'c':{
					 c = (char)va_arg(args, int);
					 fprintf(loging.logfile,"%c",c);
					 break;}
		default:{
					printf("11111\n");
					if(*format!='%'&&*format!='\n')
					{
						fprintf(loging.logfile,"%c",*format);
					}
					break;
				}
		}
		format++;
	}
#endif
	vsnprintf(str,512,format,args);
	strcat(str,"\n");
	fprintf(loging.logfile,"%s",str);
	//fprintf(loging.logfile,"%s","]\n");
}

static int initlog(unsigned char loglevel)
{
	char strdate[30]={0x0};
	LOGSET *logsetting;
	//获取日志配置信息
	if((logsetting=getlogset())==NULL)
	{
		perror("Get Log Set Fail!");
		return -1;
	}
	if((loglevel&(logsetting->loglevel))!=loglevel)
		return -1;

	memset(&loging,0,sizeof(LOG));
	//获取日志时间
	settime();
	if(strlen(logsetting->filepath)==0)
	{
		char *path=getenv("HOME");
		memcpy(logsetting->filepath,path,strlen(path));

		//getdate(strdate);
		strcat(strdate,"radar");
		strcat(strdate,".log");
		strcat(logsetting->filepath,"/");
		strcat(logsetting->filepath,strdate);
	}
	memcpy(loging.filepath,logsetting->filepath,MAXFILEPATH);
	strcpy(loging.filepath,"/opt/ARS/radar.log");
	//strcpy(loging.filepath,"./radar.log");
	//打开日志文件
	if(loging.logfile==NULL)
		loging.logfile=fopen(loging.filepath,"a+");
	if(loging.logfile==NULL)
	{
		perror("Open Log File Fail!");
		return -1;
	}
	//写入日志级别，日志时间
	fprintf(loging.logfile,"[%s] [%s]:",LogLevelText[loglevel-1],loging.logtime);
	return 0;
}

/**
 * @brief 获取日志文件行数
 * @retval [description] 
 */
int get_line()
{
	fseek(loging.logfile,0,SEEK_SET);
	char buf[MAXCHAR]={0};
	int line=0;
	while(fgets(buf,MAXCHAR,loging.logfile)!=NULL)
	{
		if(buf[strlen(buf)-1]=='\n')//若这次读取读到了'\n'，则证明该行结束
		{
			line++;//行数+1
		}

	}
	fseek(loging.logfile,0,SEEK_END);
	return line;

}
#if 1
/**
 * @brief	删除日志文件第一行
 * @retval [description] 
 */
int delete_first_line()
{
	FILE *fin,*ftmp;
	int c;
	fclose(loging.logfile);
	fin=fopen(loging.filepath,"r");
	ftmp=fopen("t.tmp","w");
	while (1) 
	{
		c=fgetc(fin);
		if (EOF==c) break;
		if ('\n'==c) break;
	}
	if (EOF!=c)	
		while (1) 
		{
			c=fgetc(fin);
			if (EOF==c) break;
			fputc(c,ftmp);
		}

	fclose(ftmp);
	fclose(fin);
	remove(loging.filepath);
	rename("t.tmp",loging.filepath);
	if((loging.logfile = fopen(loging.filepath,"a+"))==NULL)//打开文件，之后判断是否打开成功
	{
		perror("cannot open file");
		exit(0);
	}
	fseek(loging.logfile,0,SEEK_END);
}
#endif
#if 0
/**
 * @brief	删除日志文件第一行
 * @retval [description] 
 */
int delete_first_line()
{
	system("sed -i '1d' radar.log");
}
#endif
/**
 * @brief  info 日志写入 
 * @retval [description] 
 */
int Log_INFO(char *format,...)
{
	pthread_mutex_lock(&log_lock);//上锁
	int line=0;
	va_list args;
	if(initlog(INFO)!=0)   //初始化日志
		return -1;

	line=get_line();
	while(line>=MAXLINE)
	{
		//delete_first_line();
		del_first_line();
		line=get_line();
	}

	va_start(args,format);  //日志文件打印
	PrintfLog(format,args);
	va_end(args);
	fflush(loging.logfile); //刷新缓存区
	if(loging.logfile!=NULL)  //关闭日志
		fclose(loging.logfile);
	loging.logfile=NULL;
	pthread_mutex_unlock(&log_lock); //解锁
	return 0;
}

/**
 * @brief ERROR 日志写入
 * @retval [description] 
 */
int Log_ERROR(char *format,...)
{
	pthread_mutex_lock(&log_lock);//上锁
	int line = 0;
	va_list args;
	if(initlog(ERROR)!=0)   //初始化日志
		return -1;

	line=get_line();
	while(line>=MAXLINE)
	{
		//delete_first_line();
		del_first_line();
		line=get_line();
	}

	va_start(args,format);  //日志文件打印
	PrintfLog(format,args);
	va_end(args);
	fflush(loging.logfile); //刷新缓存区
	if(loging.logfile!=NULL)  //关闭日志
		fclose(loging.logfile);
	loging.logfile=NULL;
	pthread_mutex_unlock(&log_lock); //解锁
	return 0;
}
/**
 * @brief WARN 日志写入
 * @retval [description] 
 */
int Log_WARN(char *format,...)
{
	pthread_mutex_lock(&log_lock);//上锁
	int line = 0;
	va_list args;
	if(initlog(WARN)!=0)   //初始化日志
		return -1;

	line=get_line();
	while(line>=MAXLINE)
	{
		//delete_first_line();
		del_first_line();
		line=get_line();
	}

	va_start(args,format);  //日志文件打印
	PrintfLog(format,args);
	va_end(args);
	fflush(loging.logfile); //刷新缓存区
	if(loging.logfile!=NULL)  //关闭日志
		fclose(loging.logfile);
	loging.logfile=NULL;
	pthread_mutex_unlock(&log_lock); //解锁
	return 0;
}
/**
 * @brief DEBUG 日志写入
 * @retval [description] 
 */
int Log_DEBUG(char *format,...)
{
	pthread_mutex_lock(&log_lock);//上锁
	int line = 0;
	va_list args;
	if(initlog(DEBUG)!=0)   //初始化日志
		return -1;

	line=get_line();
	while(line>=MAXLINE)
	{
		//delete_first_line();
		del_first_line();
		line=get_line();
	}

	va_start(args,format);  //日志文件打印
	PrintfLog(format,args);
	va_end(args);
	fflush(loging.logfile); //刷新缓存区
	if(loging.logfile!=NULL)  //关闭日志
		fclose(loging.logfile);
	loging.logfile=NULL;
	pthread_mutex_unlock(&log_lock); //解锁
	return 0;
}
/**
 * @brief TRACE 日志写入
 * @retval [description] 
 */
int Log_TRACE(char *format,...)
{
	pthread_mutex_lock(&log_lock);//上锁
	int line = 0;
	va_list args;
	if(initlog(TRACE)!=0)   //初始化日志
		return -1;

	line=get_line();
	while(line>=MAXLINE)
	{
		//delete_first_line();

		del_first_line();
		line=get_line();
	}

	va_start(args,format);  //日志文件打印
	PrintfLog(format,args);
	va_end(args);
	fflush(loging.logfile); //刷新缓存区
	if(loging.logfile!=NULL)  //关闭日志
		fclose(loging.logfile);
	loging.logfile=NULL;
	 pthread_mutex_unlock(&log_lock); //解锁
	return 0;
}
void del_first_line()
{
	fclose(loging.logfile);
	//fseek(loging.logfile,0,SEEK_SET);
	//usleep(125);
	system("sed -i '1d' radar.log");
	if((loging.logfile = fopen(loging.filepath,"a+"))==NULL)//打开文件，之后判断是否打开成功
	{
		perror("cannot open file");
		exit(0);
	}
	fseek(loging.logfile,0,SEEK_END);
}

