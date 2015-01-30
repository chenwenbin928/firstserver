#include"event.h"
#include"file_mutex.h"
#include"connect.h"
/*
 *处理新的连接;
 */
int worker_process_handler(struct  serverinfo  *server,int  index)
{
	struct   sockaddr_in   clientaddr;
	int    fd;
	connectlist   * conn;  
	socklen_t   clielen;
	clielen=sizeof(clientaddr);
	memset(&clientaddr,0,sizeof(struct  sockaddr_in));
	//获取锁的情况下;调用accept接收连接;
	if(!server->process[index].pool->maxflag)
	{
		worker_process_lock_set(&server->file); 
		//释放锁;
		fd=accept(server->listenfd,(struct sockaddr *)&clientaddr,&clielen);
		if(fd<0)
		{

			perror("accept error!");
			worker_process_unlock_set(&server->file);
			//添加个网络连接上限标志位!;
			server->process[index].pool->maxflag=1;
			printf("PID:%d当前连接总数%d\n",getpid(),server->process[index].pool->slot);
			return  -1;
		}
		//正常情形下我应该从连接池里拿出一个连接出来给我这个新的连接;
		worker_process_unlock_set(&server->file);
		new_create_connect(server,index,server->process[index].pool,fd,WAIT_LISTENING,&clientaddr);
        add_epoll_event(server,fd,index,WORKER_PROCESS); //加入到当前进程的epoll中;
		//发送一个数据告诉客户端连接建立成功,你那边建立一个定时器,在规定时间内如果没有数据过来,定时器超时
		//就发送一个信息过来我把连接给关闭;
	}
	return   1;
}
