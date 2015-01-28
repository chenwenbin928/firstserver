#ifndef _CONNECT_H__
#define _CONNECT_H__
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include"server.h"
#define  MAX_CONNECT_POOL   1024

enum status
{
	WAIT_LISTENING,
	DATA_READING,
	DATA_WRITING,
	SOCKET_CLOSING
};
//状态机机制;

typedef   struct  connection
{
	int    socket;   //连接套接字;
	int    status;   //状态;
	struct  timeval  tv; //超时时间;
	char   client_addr[50];
	int    port;
	struct  timeval  timeouttv;   //超时时间;
	struct  timeval  starttv; //连接建立时间;
	
	struct    connection *  next;
}connectlist;

struct   connect_pool
{
	connectlist    **conn;     //连接池的元素节点;
	int        slot;      //当前连接池的下标索引;
	int       connectnum; //连接池的容量;
    connectlist  * connlist;//当前的一个连接节点;
	connectlist  * bufflist;//存放缓冲的队列; 
	int         buffnum; //当前缓冲节点数;
	int         connnum; //当前连接节点数;
};


struct  connect_pool *connect_pool_init(struct serverinfo  *server,int index,struct  connect_pool *pool,int  pid);
struct   connect_pool * connect_pool_realloc(struct connect_pool  *pool);
connectlist *  get_connection_from_free_pool(struct  connect_pool  *pool);
//connectlist *  new_create_connect(struct connect_pool  *pool,int  fd,int  status);
//
//
void (*callback)(struct  connect_pool *,int,int);

int  return_back_socket_to_pool(struct connect_pool *pool,connectlist  *c);
void  destroy_worker_process_conn_pool(struct   connect_pool *pool);



#endif

