#ifndef  __LOG_H__
#define  __LOG_H__
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"server.h"
int  create_log_file(struct   serverinfo  *server,char  *path,char *startpath);
static  int  get_logfile_name(struct  serverinfo  *server);
#endif

