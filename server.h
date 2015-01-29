#ifndef  _SERVER_H__
#define  _SERVER_H__

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<fcntl.h>
#include<unistd.h>
#include<netinet/in.h>
#include<netinet/ip.h>
#include<sys/stat.h>
#include<sys/epoll.h>
#include<dirent.h>
#include"queue.h"
#include<signal.h>
#include<arpa/inet.h>
#include<string.h>
#include<sched.h>
#include<getopt.h>
#include<time.h>
#include"mempool.h"
#include"file_mutex.h"

#define     BUFSIZE   200
#define     BACKLOG   2048
#define     MASTER_PROCESS  0x01
#define     WORKER_PROCESS  0x02
#define    _DAEMONE_        0x03


/*
 *进程表;
 */
struct   processinfo
{
	int     epfd;
	pid_t   pid;//进程pid号;
	int     slot; //当前进程在进程表中的索引号;
	int   channel[2];
	int     recvflag;
	//存放用户信息的链表(在线用户)
	struct   userinfo  *head;
	//注册用户信息;可以用数据库来搞吧;
	//业务处理函数
	sigset_t   set;
	void  *data;  //传入回调函数的参数;
    void (*worker_process_handler)(void *data);
	struct  connect_pool   *pool;
	mempool    * mem_pool;
};
/*
 *服务器要素;
 */
struct   serverinfo
{
   char  servername[100];
   int   listenfd;
   int   epfd;
   pid_t   pid;
   char  main_path[100];
   char  serverip[20];
   struct sockaddr_in  serveraddr;
   short  serverport;
   int    currentprocessnum;   //当前进程数目;
   //进程表;
   struct processinfo  *process;
   int    logfd;  //日志文件;
   int    logchangeflag;
   char   log_path[100];
   int    processnum;
   struct  file_mutex_t  file;
   char   logfilename[50];  
   sigset_t   set; //信号集;
   struct   sigaction  act;
 
};

struct   serverinfo   *server;

int    server_daemon();
int    switch_server_ini(struct   serverinfo  * server,char  * buf);
char * scan_local_dir(char *path);
int    read_server_ini(struct  serverinfo  *server,char *path);
int    add_epoll_event(struct  serverinfo  *server,int  sockfd,int  index,int  type);
struct serverinfo  *server_init(struct  serverinfo *server,char *path,int argc,char **argv,char *startpath);
int    find_worker_process_idle(struct  serverinfo  * server);
int    setsocketnonblocking(int   socket);
int    setsocketexecclose(int  socket);
void   worker_process_cycle_handler(void  *data);
int    create_worker_process(struct  serverinfo  *server);
int    master_process_para_args(struct   serverinfo  *server,int  argc,char *argv[],char *startpath);


#endif

