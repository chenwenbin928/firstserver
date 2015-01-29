#include"file_mutex.h"

/*打开一个文件用于文件锁*/
int   server_open_file_mutex(struct  file_mutex_t *file )
{

    file->fd=open("/dev/null",O_RDWR,0644);
	if(file->fd<0)
	{
		perror("open file_mutex error!\n");
		return  -1;
	}
    snprintf(file->filename,sizeof("/dev/null"),"%s","/dev/null");
    printf("主进程打开锁文件%s成功!\n",file->filename);

	return  1;
}

/*加锁*/
int   worker_process_lock_set(struct file_mutex_t  *file)
{
	struct   flock   fl;
	fl.l_type=F_WRLCK; //独占锁;
	fl.l_whence=SEEK_SET;
    fl.l_start=0;
	fl.l_len=0;
	fl.l_pid=getpid();
	if(fcntl(file->fd,F_GETLK,&fl)<0)
	{
		perror("worker  process set  filelock  successfully!\n");
		return  -1;
	}
	printf("进程PID:%d获取锁成功!\n",getpid());
	return   1;
}



/*解锁*/
int   worker_process_unlock_set(struct file_mutex_t  *file)
{
   struct    flock  fl;
   fl.l_type=F_UNLCK;
   fl.l_whence=SEEK_SET;
   fl.l_start=0;
   fl.l_len=0;
   fl.l_pid=getpid();
   if(fcntl(file->fd,F_SETLK,&fl)<0)
   {
	   perror("worker  process  realease  filelock  failure!\n");
	   return  -1;
   }
   printf("进程PID:%d 释放锁成功!\n",getpid());
   return   1;
}
