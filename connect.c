#include"connect.h"
#include"server.h"
/*初始化一个连接池的缓冲队列*/
connectlist  *  init_conn_pool_queue(struct  serverinfo *server,int index,struct connect_pool *pool)
{
     if(!(pool->connlist=(connectlist *)mempool_alloc(server->process[index].mem_pool,sizeof(connectlist))))
	 {
        perror("init failure!\n");
	 }
	 pool->connlist->next=NULL;
}

/*添加一个连接进入到队列里*/
int   add_connect_node_to_bufflist(int   fd,int  stauts)
{
    
}

/*初始化连接池*/
struct  connect_pool *connect_pool_init(struct serverinfo *server ,int  index,struct  connect_pool *pool,int  pid)
{
	
	int   i;
    if(!(pool->conn=(connectlist **)calloc(pool->connectnum,sizeof(connectlist *))))
	{
		perror("worker connectionpool  init  error");
		return   NULL;
	}
	memset(pool->conn,0,sizeof(connectlist *)*pool->connectnum);
	printf("PID:%dworker进程连接池初始化成功!\n",pid);
	pool->slot=0;
    return  pool;
}

/*连接池的扩容*/
struct   connect_pool *  connect_pool_realloc(struct connect_pool  *pool)
{
	int a=pool->connectnum;
	 connectlist   **newconn;
	if(pool->slot>=pool->connectnum)
	{
       while(pool->slot>=pool->connectnum)
		  pool->connectnum<<1;
	   newconn=(connectlist **)realloc(pool->conn,pool->connectnum*sizeof(connectlist *));
	   if(!newconn)
		   return   NULL;
	   else
	   {
		   pool->conn=newconn;
		   memset(pool->conn+a,0,(pool->connectnum-a)*sizeof( connectlist *));
		   return   pool;
	   }
	}
}

/*从空闲中取出一个节点出来*/
connectlist * get_connection_from_free_pool(struct  connect_pool  *pool)
{
	 connectlist  *  conn;
     if(pool->slot<pool->connectnum)
	 { 
         conn=pool->conn[pool->slot];
		 printf("conn=%p\n",conn);
		 printf("pool->slot=%d\n",pool->slot);
		 return  conn;
	 }
	 else
	 {
         //不够了  我们就扩容吧;
		 printf("扩容!\n");
         pool=connect_pool_realloc(pool);
		 conn=pool->conn[pool->slot++];
		 return  conn;
	 }
}

/*新建一个连接*/
void new_create_connect(struct  connect_pool  *pool,int  fd,int  status)
{
   connectlist  *conn;
   conn=get_connection_from_free_pool(pool);
   if(conn!=NULL)
   {
	   perror("conn get  fail!");
   }
   else
   {
      if(!(conn=(connectlist *)calloc(1,sizeof(connectlist))))
	  {
		  perror("malloc  errror");
	  }

	  pool->conn[pool->slot]=conn;
	  printf("pool->conn[%d]=%p\n",pool->slot,pool->conn[pool->slot]);
      conn->socket=fd;
      conn->status=status;
	  pool->slot++;
   }
}

/*当一个连接关闭了的时候我们是否应该把连接再还回去*/
int  return_back_socket_to_pool(struct connect_pool *pool,connectlist  *c)
{
	switch(c->status)
	{
	case  SOCKET_CLOSING:
          pool->conn[pool->slot--]=c;
		  break;
	default:
		  //不是关闭状态  就不还回去;
          return  0;
	}
	return  1;
}

/*销毁连接池*/
void  destroy_worker_process_conn_pool(struct   connect_pool *pool)
{
    
	 free(pool->conn);
	 free(pool);
}

