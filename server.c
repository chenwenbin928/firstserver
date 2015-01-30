#include"server.h"
#include"msgprocess.h"
#include"connect.h"
#include"signalex.h"
#include"event.h"

sig_atomic_t      sigint=0;  //这种数据类型是原子性;
sig_atomic_t      sigchild=0;
sig_atomic_t      sigquit=0;
sig_atomic_t      comrestart=0;
sig_atomic_t      comdisplay=0;

//解析命令行参数需要改变的一些标志位;
int     do_help=0;
int     do_version=0;
int     err_flag=0;
int     server_debug=0;

static   struct   option    longoption[]=
{
	{"debug",no_argument,NULL,'d'},
	{"record",optional_argument,NULL,'r'},
	{"dohelp",no_argument,NULL,'h'},
	{"do_verion",no_argument,NULL,'v'},
	{0,0,0,0}
};


//系统帮助函数;
int  server_help(void)
{
	printf("---------------help-----------------\n");
	printf("-h     获取帮助                     \n");
	printf("-v     当前版本号                   \n");
	printf("-r     修改日志存放路径             \n");
	printf("-d     打印调试信息                 \n");
	printf("------------------------------------\n");
	exit(0);
}
//显示当前的版本号;
int   server_version_show(void)
{
	printf("系统版本:0.0.1\n");
	exit(0);
}

/*解析命令行参数*/
int  master_process_para_args(struct  serverinfo *server,int  argc,char **argv,char *startpath)
{
	char  c;
	while((c=getopt_long(argc,argv,"dhvr:",longoption,NULL))!=-1)
	{
		switch(c)
		{
		case 'd':
			server_debug=1;
			break;
		case 'r':
			if(optarg)
			{
				memset(server->log_path,0,sizeof(server->log_path));
				memcpy(server->log_path,optarg,strlen(optarg));
				server->logchangeflag=1;
				printf("当前日志路径被修改为:%s\n",server->log_path);
			}
			break;
		case 'h':
			do_help=1;
			break;
		case 'v':
			do_version=1;
			break;
		case '?':
		default:
			break;
		case ':':
			err_flag=1;
			break;
		}
	}
	if(do_help)
	{
		//显示帮助函数;
		server_help();
	}
	if(do_version)
	{
		//显示当前的版本号;
		server_version_show();
	}
	if(err_flag)
	{
		//解析出现错误的时候;

	}
	if(server_debug)
	{
		//进入调试模式;
	}
	if(server->logchangeflag)
	{
		//创建相应的日志文件;
		server->logfd=create_log_file(server,server->log_path,startpath);
	}
	return    1;
}
/*绑定相应的进程到CPU核心上 因为本地是只有一个CPU核心就不绑定了!*/
int  set_cpu_affinity(pid_t  pid,unsigned  long   mask)
{
	unsigned  int  len=sizeof(mask);
	if(sched_setaffinity(pid,len,&mask)<0)
	{
		perror("sched_setaffinity");
		return  -1;
	}
	printf("%d绑定成功!\n",pid);
}

/*信号触发处理函数*/
void   signal_happen_handler(int  sig)
{
	switch(sig)
	{
	case   SIGINT:  
		sigint=1;
		printf("接收到%d信号!\n",SIGINT);
		break;
	case   SIGQUIT: //这个信号是安全退出;
		sigquit=1;
		printf("接收到%d信号!\n",SIGQUIT);
		break;
	case   SIGCHLD: 
		printf("接收到%d信号!\n",SIGCHLD);
		sigchild=1;
		break;
	case   COMM_RESTART:
		printf("接收到%d命令!\n",COMM_RESTART);
		comrestart=1;
		break;
	case   COMM_DISPLAY_SERVER:
		printf("接收到%d命令!\n",COMM_DISPLAY_SERVER);
		comdisplay=1;
		break;
	default:
		signal(sig,SIG_IGN);
		break;
	}
}
/*
 *服务器配置文件里的要素数组;
 */
char  *server_ini_info[]=
{
	"SERVER_NAME",
	"WORK_PATH",
	"SERVER_IP",
	"SERVER_PORT",
	"LOG_PATH",
	"PROCESS_NUM",
};

/*
 *服务器程序后台化;
 */
int    server_daemon()
{
	int   i;
	pid_t   pid;
	pid=fork();
	if(pid<0)
	{
		perror("fork");
		return  -1;
	}
	else if(pid>0)
	{
		exit(0);
	}
	if(setsid()<0)
	{
		perror("setsid error");
		return -1;
	}
	pid=fork();
	if(pid<0)
	{
		perror("fork error");
		return  -1;
	}
	else if(pid>0)
	{
		exit(0);
	}
	chdir("/");
	for(i=0;i<3;i++)
	{
		close(i);
	}
	umask(0);
	return   1;
}

/*
 *匹配配置文件的的配置项;
 */
int    switch_server_ini(struct   serverinfo  * server,char  * buf)
{
	int  i,j,len;
	char *temp;
	for(i=0;temp=server_ini_info[i];i++)
	{
		len=strlen(temp);
		if(memcmp(temp,buf,len)==0)
		{
			j=i;
			buf=buf+len+1;
			break;
		}
	}
	switch(j)
	{
	case 0:
		memcpy(server->servername,buf,strlen(buf)-1); //减一是不要'\n'符;
		printf("服务器名称:%s\n",server->servername);
		break;
	case 1:
		memcpy(server->main_path,buf,strlen(buf)-1);
		printf("工作路径:%s\n",server->main_path);
		break;
	case 2:
		memcpy(server->serverip,buf,strlen(buf)-1);
		printf("服务器ip:%s\n",server->serverip);
		break;
	case 3:
		server->serverport=atoi(buf);
		printf("服务器端口号:%d\n",server->serverport);
		break;
	case 4:
		if(!server->logchangeflag)
	           memcpy(server->log_path,buf,strlen(buf)-1);
		printf("服务器日志路径:%s\n",server->log_path);
		break;
	case 5:
		server->processnum=atoi(buf);
		printf("服务器子进程数量:%d\n",server->processnum);
		break;
	default:
		perror("unkown   server_ini_info!");
		return  -1;
	}
}

/*
 *扫描本地目录;
 */
char *  scan_local_dir(char *path)
{
	int   ret;
	int   flag=0;
	DIR   *dir;
	struct dirent  *dt;
	dir=opendir(path);
	if(dir==NULL)
	{
		perror("opendir");
		return   NULL;
	}
	while((dt=readdir(dir))!=NULL)
	{
		ret=nameswitch(dt->d_name,"server.ini");
		if(ret==1)
		{
			printf("找到了配置文件!\n");
			sprintf(path+strlen(path),"/%s",dt->d_name);
			closedir(dir);
			return path;
		}
	}
	if(flag==0)
	{
		printf("没有找到系统配置文件!\n");
		return  NULL;
	}
}
/*读取配置文件*/
int   read_server_ini(struct  serverinfo  *server,char *path)
{
	FILE  *fd;
	char  buf[BUFSIZE];
	chdir("/");
	fd=fopen(path,"a+");
	if(fd==NULL)
	{
		perror("open error!\n");
		return  -1;
	}
	while(fgets(buf,BUFSIZE,fd)!=NULL)
	{
		switch_server_ini(server,buf);
	}
	fclose(fd);
}
/*
 *添加事件到epoll;
 */
int  add_epoll_event(struct  serverinfo  *server,int  sockfd,int  index,int  type)
{
	struct   epoll_event   event;
	memset(&event,0,sizeof(event));
	event.data.fd=sockfd;
	event.events=EPOLLIN;
	switch(type)
	{
	case  MASTER_PROCESS:
		if(epoll_ctl(server->epfd,EPOLL_CTL_ADD,sockfd,&event)<0)
		{
			perror("epoll_ctl");
			return  -1;
		}
		break;
	case  WORKER_PROCESS:
		if(epoll_ctl(server->process[index].epfd,EPOLL_CTL_ADD,sockfd,&event)<0)
		{
			perror("epoll_ctl");
			return  -1;
		}
		break;
	}
	return   1;
}

/*初始化服务器*/

struct  serverinfo  *server_init(struct  serverinfo *server,char *path,int argc,char **argv,char *startpath)
{
	int   opt=1;
	int    i;
	if(!(server=(struct  serverinfo *)calloc(1,sizeof(struct serverinfo))))
	{
		perror("server_init  error");
		free(server);
		return  NULL;
	}
	memset(server,0,sizeof(struct  serverinfo));
	//搞个读取配置文件函数接口;	
	master_process_para_args(server,argc,argv,startpath);
	read_server_ini(server,path);

	//下面就是各种初始化操作;
	if(server->logchangeflag==0)
	{
		server->logfd=create_log_file(server,server->log_path,startpath);
	}
	server->listenfd=socket(AF_INET,SOCK_STREAM,0);
	if(server->listenfd<0)
	{
		perror("server process create   socket   error!\n");
		return  NULL;
	}
	server->serveraddr.sin_family=AF_INET;
	server->serveraddr.sin_port=htons(server->serverport);
	if(inet_aton(server->serverip,&server->serveraddr.sin_addr)<0)
	{
		perror("inet_aton");
		return NULL;
	}
	if(setsockopt(server->listenfd,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt))<0)
	{
		perror("setsockopt");
		return NULL;
	}
	if(!(server->process=(struct   processinfo *)calloc(server->processnum,sizeof(struct  processinfo))))
	{
		perror("calloc  error");
		free(server->process);
		return  NULL;
	}
	server->currentprocessnum=0;
	server->pid=getpid();
	if(bind(server->listenfd,(struct sockaddr *)&server->serveraddr,sizeof(server->serveraddr))<0)
	{
		perror("bind");
		return  NULL;
	}
	if(listen(server->listenfd,BACKLOG)<0)
	{
		perror("listen");
		return   NULL;
	}
	return   server;
}

/*
 *设置成为非阻塞;
 */
int  setsocketnonblocking(int   socket)
{
	int  flag;
	if((flag=fcntl(socket,F_GETFL,0))<0)
	{
		perror("fcntl");
		return  -1;
	}
	flag|=O_NONBLOCK;
	if(fcntl(socket,F_SETFL,flag)<0)
	{
		perror("fcntl");
		return  -1;
	}
	return  1;
}
/*
 *设置执行exec后关闭fd
 */
int  setsocketexecclose(int  socket)
{
	int  flag;
	if((flag=fcntl(socket,F_GETFD,0))<0)
	{
		perror("fcntl");
		return  -1;
	}
	flag|=FD_CLOEXEC;
	if(fcntl(socket,F_SETFD,flag)<0)
	{
		perror("fcntl");
		return  -1;
	}
	return  1;
}
/*
 *在当前进程表中查找空闲位置;
 */
int  find_worker_process_idle(struct  serverinfo  * server)
{
	int  i;
	for(i=0;i<server->processnum;i++)
	{
		if(server->process[i].channel[0]==0||server->process[i].channel[1]==0)
		{
			return   i;
		}
		else
			continue;
	} 
}
/*
 *master进程把套接字主动告知给前面生成的worker进程;
 *
 */
int   send_socket_to_other_worker_process(struct  serverinfo  * server,int  index,int  processnum)
{
	int  i,j;
	struct   command   msg;
	memset(&msg,0,sizeof(msg));
	for(i=0;i<processnum;i++)
	{   
		if(i==index)
			break;
		else
		{
			for(j=0;j<2;j++)
			{
				msg.type=SEND_FILE_DES;
				msg.fd=server->process[index].channel[j];
				send_socketfd_to_workerprocess(server,i,&msg,sizeof(msg));
			}
		}
	}
	return  1;
}

/*
 *worker进程一直等待所有进程表信息都填充完成后退出;
 */
int   worker_process_recv_socket(struct  serverinfo  *server,int index)
{
	struct   command  msg;
	memset(&msg,0,sizeof(msg));
	recv_socketfd_from_masterprocess(server,index,&msg,sizeof(msg));
	return  1;
}

/*
 *worker进程循环处理函数接口;
 */
void  worker_process_cycle_handler(struct callback_arg  *data)
{ 
    int   tempindex=data->index;
	int   timeout=data->timeout;
	struct   epoll_event   event[EPOLL_EVENT_MAXSiZE];
	int  nfds,i;
	while(1)
	{
		nfds=epoll_wait(server->process[tempindex].epfd,event,EPOLL_EVENT_MAXSiZE,timeout);
		if(nfds==-1)
		{
			if(errno!=EINTR)
			{
				continue;
			}
			//调用信号处理函数;

		}
		else
		{
			for(i=0;i<nfds;i++)
			{
				if(event[i].data.fd==server->process[tempindex].channel[0])
				{
					//process发过来的,那就是命令或者信号变成守护进程时;
					worker_process_recv_comm_signal_handler(server,tempindex);
				}
				else if(event[i].data.fd==server->listenfd)
				{
					 //接收连接;
                     worker_process_handler(server,tempindex);      
				}
				else
				{    
					//这边是处理数据传输之类的任务;

				}
			}

		}
	}

}

/*
 *初始化worker进程接口;
 */
int   init_worker_process(struct serverinfo  *server,int index)
{
    struct  callback_arg  arg;
	arg.index=index;
	arg.timeout=EPOLL_TIME_OUT;
	server->process[index].pid=getpid();
	//要不要把worker进程绑定到CPU上;
	server->process[index].slot=index;
	server->process[index].mem_pool=mempool_init(server->process[index].mem_pool,PAGE_SIZE);
	server->process[index].epfd=epoll_create(BACKLOG);
	if(server->process[index].epfd<0)
	{
		perror("epoll_create");
		return  -1;
	}
	add_epoll_event(server,server->process[index].channel[0],index,WORKER_PROCESS);
	add_epoll_event(server,server->listenfd,index,WORKER_PROCESS);	
	queue_init(server->process[index].head,index);
	sigemptyset(&server->process[index].set);
	if(sigprocmask(SIG_SETMASK,&server->process[index].set,NULL)<0)
	{
		perror("sigprocmask");
		return  -1;
	}
	if(!(server->process[index].pool=(struct connect_pool*)calloc(1,sizeof(struct  connect_pool))))
	{
		perror("worker process create connectionpool error\n");
		free(server->process[index].pool);
		return  -1;
	}
	server->process[index].pool->slot=0;
	server->process[index].pool->connectnum=MAX_CONNECT_POOL;

	server->process[index].pool=connect_pool_init(server,index,server->process[index].pool,getpid());
	//调用这个worker进程的回调函数-----目的是循环处理连接任务。
	//把我的套接字主动告知给前面的生成的worker进程;接收完成后继续;
	if(index==server->processnum-1)
       server->process[index].recvflag=1;
	else
       server->process[index].recvflag=0;
	worker_process_cycle_handler((struct callback_arg *)&arg);
	return  1;
}

/*
 *创建各个子进程函数;
 */
int   create_worker_process(struct  serverinfo  *server)
{
	pid_t  pid;
	int    index;
	int    i;
	for(i=0;i<server->processnum;i++)
	{
		//查找空闲位置用于填充worker进程的信息;
		index=find_worker_process_idle(server);
		if(socketpair(AF_LOCAL,SOCK_STREAM,0,server->process[index].channel)<0)
		{
			perror("socketpair");
			return  -1;
		}
		setsocketnonblocking(server->process[index].channel[0]);
		setsocketexecclose(server->process[index].channel[0]);
		setsocketnonblocking(server->process[index].channel[1]);
		setsocketexecclose(server->process[index].channel[1]);
		pid=fork();
		switch(pid)
		{
		case -1:
			perror("fork error");
			break;
		case  0:
			//worker进程的初始化函数接口;
			init_worker_process(server,index);
			break;
		default:
			server->process[index].pid=pid;
			server->process[index].slot=index;
			//把我的套接字主动告知给前面的生成的worker进程;
			++server->currentprocessnum;
			send_socket_to_other_worker_process(server,index,server->currentprocessnum);
			break;
		}
	}
}
/*把子进程中的channel[1]端加入到进程表中;*/
int  register_master_process_epfd(struct  serverinfo   *server,int  type)
{
	int  i;
	for(i=0;i<server->processnum;i++)
	{
		add_epoll_event(server,server->process[i].channel[1],i,MASTER_PROCESS);
	}
}

int command_and_signo_send(struct  serverinfo *server,struct  command   *comm,size_t size)
{
	int  i;
	for(i=0;i<server->processnum;i++)
	{
		write(server->process[i].channel[1],comm,size);
	}
}

/*master进程接收到我们发送过去的信号或者命令都转发给我们的worker进程*/
int  send_signo_command_to_workerprocess(struct serverinfo  *server,int msgcode)
{
	struct   command   comm;
	size_t   size=sizeof(comm);
	memset(&comm,0,size);
	switch(msgcode)
	{
	case  SIGINT:
		comm.type=SEND_SIG_NO;
		comm.msgcode=msgcode;
		comm.name="SIGINT";
		break;
	case  SIGQUIT:
		comm.type=SEND_SIG_NO;
		comm.msgcode=msgcode;
		comm.name="SIGQUIT";
		break;
	case  COMM_RESTART:
		comm.type=SERVER_COMM;
		comm.msgcode=msgcode;
		comm.name="RESTART";
		break;
	case  COMM_DISPLAY_SERVER:
		//返回服务器当前的信息;
		break;
	default:
		perror("master 主进程收到了未知指令!\n");
		return  -1;
	}
    command_and_signo_send(server,&comm,size);
	return   1;
}
/*
 *系统退出处理;
 *
 */
int  server_exit_handler(struct  serverinfo  * server)
{
	int  i;
	int    status;
	int    ret;
	for(i=0;i<server->processnum;i++)
	{
		ret=waitpid(-1,&status,WNOHANG);
		if(ret<0)
			return  -1;
		else
	    {
			continue;
		}
		  printf("<<<\n");
	}
	//最后释放本地master进程资源;
	printf("it's  over!\n");
	close(server->epfd);
	close(server->listenfd);
	close(server->file.fd);
	close(server->logfd);
	free(server->process);
	free(server);
	exit(0);
}


/*
 *主函数;
 */
int  main(int  argc,char **argv)
{
	int  ret;
	char  current_work_path[BUFSIZE];
	char  *server_ini_file=NULL;
	getcwd(current_work_path,BUFSIZE);
	server_ini_file=scan_local_dir(current_work_path);
	if(server_ini_file==NULL)
	{
		perror("没有找到相应的配置文件!系统错误!\n");
		return  0;
	}
	server=server_init(server,server_ini_file,argc,argv,current_work_path);
	signal_set_init(&server->act);
    server_open_file_mutex(&server->file);	
	add_signal_to_master_process_set(&server->set);
	//初始化子进程函数;	
#ifndef  _DAEMONE_
	server_daemon();
#endif
	create_worker_process(server);
	//这边开始创建master进程的epfd;
	server->epfd=epoll_create(EPOLL_EVENT_MAXSiZE);
	if(server->epfd<0)
	{
		perror("epoll_create");
		return  -1;
	}
	//把子进程的channel[1]端加入到epfd中;
	register_master_process_epfd(server,MASTER_PROCESS);
	while(1)
	{

		//让master主进程挂起;
		sigsuspend(&server->set); 
		if(sigquit)
		{
			ret=send_signo_command_to_workerprocess(server,SIGQUIT);
			server_exit_handler(server);
			sigquit=0;
		}
		if(sigint)
		{
			ret=send_signo_command_to_workerprocess(server,SIGINT);
			sigint=0;
		}	
		if(comdisplay)
		{
			send_signo_command_to_workerprocess(server,COMM_DISPLAY_SERVER);
			comdisplay=0;
		}
		if(comrestart)
		{
			send_signo_command_to_workerprocess(server,COMM_RESTART);
			comrestart=0;
		}
	}
	return  1;
}







