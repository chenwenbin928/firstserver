#ifndef  _MSG_PROCESS_H__
#define  _MSG_PROCESS_H__
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include"queue.h"
#include"server.h"
#define  MESSAGE_MAX_SIZE   512
#define  EPOLL_EVENT_MAXSiZE   1024



//消息类型定义

#define   SEND_FILE_DES     0x01   //传递文件描述符;
#define   SEND_SIG_NO       0x02   //发送信号;
#define   SEND_DATA_MESSAGE 0x03   //发送数据信息; 这个是用户和用户之间传递;
#define   SERVER_COMM       0x05   //系统命令;     这个是主进程和子进程之间的传递

#define  COMM_RESTART         0x10  //重启;
#define  COMM_DISPLAY_SERVER  0x12   //显示服务情况;



struct   command
{
	int   type;   //消息类型;
	int   msgcode;
	char  *name;
	int   fd; //接收套接字;
};

struct    message
{ 
	int   type;      //消息类型定义;
	int   slot;      //发送方的索引号;
	int   senddrt;   //发送方向;
    pid_t srcpid;    //发送消息的pid号;
	pid_t dstpid;    //接收消息的pid号;
	char  name[50];  //发送者姓名;
	struct  timeval  tv; //时间;
	size_t  len;      //本次消息长度;
	int   overflag;   //消息的结束标志;
	char  msgbuf[MESSAGE_MAX_SIZE];//发送的数据;
};



int   worker_process_socket_register(struct  serverinfo   *server,int  index, int  fd);
int   send_socketfd_to_workerprocess(struct  serverinfo  *server, int  index,struct  command *smsg,size_t   smsglen);
int   sendmessage(struct   serverinfo *server,int  index,struct  message *msg,size_t  msglen);
int   recv_socketfd_from_masterprocess(struct  serverinfo * server,int   index, struct  command  *rmsg,size_t  rmsglen);
int   recvmessage(struct   serverinfo *server,int index,struct  message  *msg, size_t  msglen);
int   worker_process_recv_comm_signal_handler(struct serverinfo  *server,int  index);
int   worker_process_handler_comm_siganl(struct  serverinfo *server,int  index);
int    command_and_signo_send(struct  serverinfo *server,struct  command   *comm,size_t size);
#endif

