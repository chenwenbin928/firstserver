#include"msgprocess.h"
#include"signalex.h"
static  sig_atomic_t  sigint=0;
static  sig_atomic_t  sigquit=0;
static  sig_atomic_t  comrestart=0;

/*worker进程登记其他worker进程套接字的函数接口*/
int   worker_process_socket_register(struct  serverinfo   *server,int  index, int  fd)
{
	int  i,j;
	for(i=index+1;i<server->processnum;i++)
	{
		for(j=0;j<2;j++)
		{
			if(server->process[i].channel[j]==0)
			{
			   server->process[i].channel[j]=fd;
			   if(i==server->processnum-1&&j)
			   {
				  server->process[index].recvflag=1;
			   }
			   return   1;
			}
			else
			   continue;
		}
	}
	return  1;
}

/*发送文件描述函数*/
int   send_socketfd_to_workerprocess(struct  serverinfo  *server, int  index,struct  command *smsg,size_t   smsglen)
{
     struct  msghdr    msg;
	 struct  iovec    iov[1];
	 size_t  n;
     union
	 {
        struct    cmsghdr   cm;
        char      data[CMSG_SPACE(sizeof(int))];
	 }cmsg;
     msg.msg_name=NULL;
	 msg.msg_namelen=0;

	 iov[0].iov_base=(char *)smsg;
	 iov[0].iov_len=smsglen;
      
	 msg.msg_control=(void *)&cmsg;
	 msg.msg_controllen=sizeof(cmsg);

	 cmsg.cm.cmsg_len=CMSG_LEN(sizeof(int));
     cmsg.cm.cmsg_level=SOL_SOCKET;
	 cmsg.cm.cmsg_type=SCM_RIGHTS;

	 msg.msg_flags=0;
	 memcpy(CMSG_DATA(&cmsg.cm),&smsg->fd,sizeof(int));
     msg.msg_iov=iov;
	 msg.msg_iovlen=1;
	 n=sendmsg(server->process[index].channel[1],&msg,0);
     if(n==-1)
	 {
		 if(errno==EAGAIN)
		 {
             return  2;
		 }
		 return  -1;
	 }
	 return    1;
}
  





/*发送消息*/
int    sendmessage(struct   serverinfo *server,int  index,struct  message *msg,size_t  msglen)
{
    switch(msg->type)
	{
	  case  SEND_DATA_MESSAGE:
		                
			break;
	  default:
			perror("unknown  message  type!\n");
			return  0;
	}
	return  1;
}

/*接收文件描述符函数*/
int   recv_socketfd_from_masterprocess(struct  serverinfo * server,int   index, struct  command  *rmsg,size_t  rmsglen)
{
	struct  msghdr  msg;
	struct  iovec  iov[1];
	size_t      n;
	union
	{
		struct  cmsghdr   cm;
		char    data[CMSG_SPACE(sizeof(int))];
	}cmsg;
    msg.msg_name=NULL;
	msg.msg_namelen=0;
	iov[0].iov_base=(void *)rmsg;
	iov[0].iov_len=rmsglen;
	msg.msg_iov=iov;
	msg.msg_iovlen=1;
	msg.msg_control=(void *)&cmsg;
	msg.msg_controllen=sizeof(cmsg);
	n=recvmsg(server->process[index].channel[0],&msg,0);
	if(n==-1)
	{
        if(errno==EAGAIN)
		{
			return   2;
		}
		return   -1;
	}
    memcpy(&rmsg->fd,CMSG_DATA(&cmsg.cm),sizeof(int));
	if(rmsg->type==SEND_FILE_DES)
         worker_process_socket_register(server,index,rmsg->fd);
	return   1;
}

/*接收消息*/
int    recvmessage(struct   serverinfo *server,int index,struct  message  *msg,size_t  msglen)
{
     switch(msg->type)
	 {
		   
	   case SEND_DATA_MESSAGE:
			break;
	   default:
			perror("unknown  message  type!\n");
			return  0;
	 }
     return  1;   
}

int   worker_process_exit_handler(struct  serverinfo  * server,int  index)
{
	 free(server->process[index].head);
	 exit(0);  
}

//worker进程接收到信号或者命令之后的处理;
int   worker_process_recv_comm_signal_handler(struct serverinfo  *server,int  index)
{

	if(server->process[index].recvflag==0)
	{
	    worker_process_recv_socket(server,index);
		return   1;
	}
	printf("server->process[%d].recvflag=%d\n",index,server->process[index].recvflag);
	struct  command   comm;
	memset(&comm,0,sizeof(comm));
	int  ret;
	ret=read(server->process[index].channel[0],&comm,sizeof(comm));
	if(ret<0)
	{
		perror("recv");
		return  -1;
	}
	switch(comm.msgcode)
	{
	case  SIGINT:
		  sigint=1;
		  printf("接收到了信号%d\n",SIGINT);
		  break;
	case  SIGQUIT:
		  sigquit=1;
		  printf("接收到了安全退出的信号!\n");
          worker_process_exit_handler(server,index);
		  break;
	case  COMM_RESTART:
		  comrestart=1;
		  printf("接收到了重新启动的命令!\n");
		  break;
	default:
		  printf("unknown  msgcode  can be known!PID:%d\n",getpid());
		  return -1;
	}
    return   1;
}



