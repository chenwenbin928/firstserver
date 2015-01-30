#include"connect.h"
#include"server.h"
#include"mempool.h"
/*初始化一个连接池的空闲节点链表*/
connectlist  *  init_conn_pool_queue(struct  serverinfo *server,int index,struct connect_pool *pool)
{
	if(!(pool->freeconnhead=(connectlist *)mempool_alloc(server->process[index].mem_pool,sizeof(connectlist))))
	{
		perror("init failure!\n");
		return  NULL;
	}
	pool->freeconnhead->next=pool->freeconnhead;
	return   pool->freeconnhead;
}

/*添加一个空闲连接点进入到链表*/
int   add_connect_node_to_bufflist(struct  connect_pool * pool,connectlist  * conn)
{
	connectlist  * tail=pool->freeconnhead;
	switch(conn->status)
	{
	case   SOCKET_CLOSING://判断是不是连接关闭状态;
		//空闲节点的信息全部清空;
		pool->conn[conn->index]=NULL;
		memset(conn,0,sizeof(connectlist));
		tail->next=conn;
		conn->next=pool->freeconnhead;
		tail=conn; 
		pool->connnum++;
		printf("空闲节点插入空闲链表成功!\n");
		break;
	default:
		printf("the  connection node  stauts  is  not   socket_closing!\n");
		return  -1;
	}

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
	pool->freeconnhead=init_conn_pool_queue(server,index,pool);
	pool->slot=0;
	pool->maxflag=0;
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
		{
			pool->connectnum=pool->connectnum*2;
		}
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
connectlist * get_connection_from_free_pool(struct  connect_pool  *pool,connectlist  *conn)
{
	int     i;
	for(i=0;i<pool->connectnum;i++)
	{
		if(!pool->conn[i])
		{
			if(conn)
			{
				//这个是一个空闲节点;
				pool->conn[i]=conn;
				conn->index=i;
				return  conn;
			}
			else
			{
                conn=pool->conn[i];
				pool->slot=i;
				return  conn;
			}
		}
	}
	if(pool->slot>=pool->connectnum)
	{
		//不够了  我们就扩容吧;
		pool=connect_pool_realloc(pool);
		conn=pool->conn[pool->slot++];
		return  conn;
	}
}
/*从空闲节点链表中取出一个空闲节点出来存放连接节点信息*/
connectlist  *  find_free_node_from_connectfreelist(struct  connect_pool *pool)
{
	connectlist  *  p;
	connectlist  *  q;
	p=pool->freeconnhead;
	q=p->next;
	if(q!=p)
	{
		p->next=q->next;
		pool->connnum--;
		return  q;
	}
	return  NULL;
}

/*新建一个连接*/
void new_create_connect(struct  serverinfo  *server,int index,struct  connect_pool  *pool,int  fd,int  status,struct   sockaddr_in *client_addr)
{
	int    startup_time=0;
	connectlist  *conn;
	connectlist  *temp=pool->freeconnhead;
	if(temp->next!=pool->freeconnhead)
	{
		conn=find_free_node_from_connectfreelist(pool);
		//如果找到空闲节点,conn!=NULL;
		//否则的话,conn==NULL;
	}
	conn=get_connection_from_free_pool(pool,conn);
	if(conn!=NULL)
	{
        conn->socket=fd;
		conn->status=status;
		//索引号已经有了;
	}
	else
	{
		if(!(conn=(connectlist *)mempool_alloc(server->process[index].mem_pool,sizeof(connectlist))))
		{
			perror("malloc  errror");
		}
		pool->conn[pool->slot]=conn;
		//这个地方要把连接节点的信息给初始化下;
		conn->socket=fd;        //连接描述字;
		conn->index=pool->slot; //如果这个连接失效的话,我就直接根据索引号找到它在我那个数组表中的位置;
		conn->status=status;
		pool->slot++;
	}
	struct timeval  timeout={60,0};
	gettimeofday(&conn->starttv,NULL);
	setsockopt(conn->socket,SOL_SOCKET,SO_SNDTIMEO,(char*)&timeout,sizeof(int));
	setsockopt(conn->socket,SOL_SOCKET,SO_RCVTIMEO,(char*)&timeout,sizeof(int));
	setsocketnonblocking(conn->socket);
	startup_time=conn->starttv.tv_sec*1000+conn->starttv.tv_usec/1000;
	printf("连接建立时间:%dms\n",startup_time);
    memcpy(conn->client_addr,inet_ntoa(client_addr->sin_addr),sizeof(inet_ntoa(client_addr->sin_addr)));
    conn->port=ntohs(client_addr->sin_port);
}

/*销毁连接池*/
void  destroy_worker_process_conn_pool(struct   connect_pool *pool)
{

	free(pool->conn);
	free(pool);
}

