#include"event.h"
#include"file_mutex.h"
/*
 *处理新的连接;
 */
int worker_process_handler(struct  serverinfo  *server,int  index)
{
   struct   sockaddr_in   clientaddr;
   int    fd;
   socklen_t   clielen;
   clielen=sizeof(clientaddr);
   memset(&clientaddr,0,sizeof(struct  sockaddr_in));
   //获取锁的情况下;调用accept接收连接;
   worker_process_lock_set(&server->file); 
   printf("-----------------------------------\n");
   //释放锁;
   fd=accept(server->listenfd,(struct sockaddr *)&clientaddr,&clielen);
   if(fd<0)
   {
	   perror("accept error!");
   }
   printf("fd=======%d\n",fd);
   printf("-----------------------------------\n");
   worker_process_unlock_set(&server->file);
   return   1;
}
