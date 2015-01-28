/*
 *用文件锁来控制进程间的惊群;
 *
 */
#ifndef  _FILE_MUTEX_H__
#define  _FILE_MUTEX_H__
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<fcntl.h>
#include<unistd.h>

/*
 *文件锁;
 */
struct  file_mutex_t
{
	int     fd;
	char    filename[50];
};



int   server_open_file_mutex(struct  file_mutex_t *file);
int   worker_process_lock_set(struct  file_mutex_t  *file);
int   worker_process_unlock_set(struct   file_mutex_t  *file);

#endif

