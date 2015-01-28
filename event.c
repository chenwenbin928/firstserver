#include"event.h"
#include"file_mutex.h"
/*
 *客户端的一些连接信息显示;
 */
void  display_client_connect_info(int  fd,struct  sockaddr_in  *clientaddr)
{
	printf("------------------------客户连接信息--------------------------\n");
    printf("接收来自用户IP:%s端口号:%d的连接%d\n",inet_ntoa(clientaddr->sin_addr),ntohs(clientaddr->sin_port),fd);
	printf("--------------------------------------------------------------\n");
}
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
   //释放锁;
   fd=accept(server->listenfd,(struct sockaddr *)&clientaddr,&clielen);
   if(fd<0)
   {
	   perror("accept error!");
   }
   //正常情形下我应该从连接池里拿出一个连接出来给我这个新的连接;
   worker_process_unlock_set(&server->file);
   display_client_connect_info(fd,&clientaddr);


   return   1;
}
