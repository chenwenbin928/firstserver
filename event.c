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
   worker_process_lock_set(&server->file); 
   //释放锁;
   fd=accept(server->listenfd,(struct sockaddr *)&clientaddr,&clielen);
   if(fd<0)
   {
	   perror("accept error!");
   }
   //正常情形下我应该从连接池里拿出一个连接出来给我这个新的连接;
   worker_process_unlock_set(&server->file);
   new_create_connect(server,index,server->process[index].pool,fd,WAIT_LISTENING,&clientaddr);
   return   1;
}
