#include"log.h"

/*我们的日志文件是以时间为文件名后缀名为.log*/
static  int  get_logfile_name(struct  serverinfo  *server)
{
	time_t  rtime;
	struct   tm    *temp;
	rtime=time(NULL);
	temp=localtime(&rtime);
	//这个最好搞个时间缓存;用于记录创建日志的时间;
	snprintf(server->logfilename,sizeof(server->logfilename),"%d-%d-%d.log",temp->tm_year+1900,temp->tm_mon+1,temp->tm_mday);
	return    1;
}

/*根据日志路径创建相应的日志文件;*/
int  create_log_file(struct   serverinfo  *server,char  *path,char *startpath)
{
	int   exist_flag;
	int   fd;
	if(chdir(path)<0)
	{
		//打印错误信息  然后退出;
		return   -1;
	}
	else
	{ 
		get_logfile_name(server);
		exist_flag=access(server->logfilename,F_OK);  //判断日志文件是否存在;
		if(exist_flag<0)
		{
			if((fd=open(server->logfilename,O_RDWR|O_CREAT|O_APPEND,0644))<0)
			{
				//打印错误信息;

			}
			printf("系统日志文件%s创建成功!\n",server->logfilename);
		}
		else
		{
			fd=open(server->logfilename,O_RDWR|O_APPEND,0644);
			if(fd<0)
			{
               perror("open");
			}
			printf("系统日志文件%s已经存在!\n",server->logfilename);

		}
		chdir(startpath); //返回到当前目录下;
	}
	return  fd;//返回日志文件的描述符;
}


/*日志记录*/







