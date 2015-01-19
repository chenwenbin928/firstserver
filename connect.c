#include"connect.h"
#include"server.h"

//枚举类型;
enum status
{
	WAIT_LISTENING,
	DATA_READING,
	DATA_WRITING,
	SOCKET_CLOSING
};

/*初始化连接池*/
struct  connect_pool *connect_pool_init(struct  connect_pool *pool,int  pid)
{
	
	int   i;
    if(!(pool->conn=(connectlist **)malloc(pool->connectnum*sizeof(connectlist *))))
	{
		perror("malloc   error");
		return   NULL;
	}
	for(i=0;i<pool->connectnum;i++)
	{
		pool->conn[i]=NULL;
	}
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
	else
		return   NULL;
    
}

/*从空闲中取出一个节点出来*/
connectlist *  get_connection_from_free_pool(struct  connect_pool  *pool)
{
	 connectlist  *  conn;
     if(pool->slot<pool->connectnum)
	 { 
         conn=pool->conn[pool->slot++];
		 return  conn;
	 }
	 else
	 {
         //不够了  我们就扩容吧;
         pool=connect_pool_realloc(pool);
		 conn=pool->conn[pool->slot++];
		 return  conn;
	 }
}

/*新建一个连接*/
connectlist *   new_create_connect(struct  connect_pool  *pool,int  fd,int  status)
{
   connectlist  *conn;
   conn=get_connection_from_free_pool(pool);
   if(!conn)
	   return  NULL;
   else
   {
      conn->socket=fd;
      conn->status=status;
      return   conn;
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

