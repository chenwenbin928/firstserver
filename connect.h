#ifndef _CONNECT_H__
#define _CONNECT_H__
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
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
}connectlist;

struct   connect_pool
{
	connectlist    **conn;     //连接池的元素节点;
	int        slot;      //当前连接池的下标索引;
	int       connectnum; //连接池的容量;

};


struct  connect_pool *connect_pool_init(struct  connect_pool *pool,int  pid);
struct   connect_pool * connect_pool_realloc(struct connect_pool  *pool);
connectlist *  get_connection_from_free_pool(struct  connect_pool  *pool);
connectlist *   new_create_connect(struct connect_pool  *pool,int  fd,int  status);
int  return_back_socket_to_pool(struct connect_pool *pool,connectlist  *c);
void  destroy_worker_process_conn_pool(struct   connect_pool *pool);



#endif

