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
		memset(conn,0,sizeof(connectlist));
    	printf("空闲节点插入空闲链表成功!\n");
		conn->next=tail->next;
		tail->next=conn;
		break;
	default:
		printf("the  connection node  stauts  is  not   socket_closing!\n");
		return  -1;
	}
    return  1;
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
	pool->nodenum=0;
	pool->maxflag=0;
	return  pool;
}

/*连接池的扩容*/
struct   connect_pool *  connect_pool_realloc(struct connect_pool  *pool,int  fd)
{
	int a=pool->connectnum;
	connectlist   **newconn;
	if(fd>=pool->connectnum)
	{
		while(fd>=pool->connectnum)
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
connectlist * get_connection_from_free_pool(struct  connect_pool  *pool,int  fd)
{
	connectlist  *  conn;
	if(fd>=pool->connectnum)
	{
		connect_pool_realloc(pool,fd);
		conn=pool->conn[fd];
	}
	else
	{
		conn=pool->conn[fd];
	}
	return  conn;
}
/*从空闲节点链表中取出一个空闲节点出来存放连接节点信息*/
connectlist  *  find_free_node_from_connectfreelist(struct  connect_pool *pool)
{
	connectlist  *p;
	connectlist  *q;
	p=pool->freeconnhead;
	q=p->next;
	while(q!=p)
	{
	   p->next=q->next;
	   return   q;
	}

}

/*新建一个连接*/
void new_create_connect(struct  serverinfo  *server,int index,struct  connect_pool  *pool,int  fd,int  status,struct   sockaddr_in *client_addr)
{
	int    startup_time=0;
	connectlist  *conn=NULL;
	if(pool->freeconnhead->next!=NULL)
	{
		conn=find_free_node_from_connectfreelist(pool);
		pool->conn[fd]=conn;
		printf("获取到空闲节点!conn=%p\n",conn);
	}
	else
	{
		conn=get_connection_from_free_pool(server->process[index].pool,fd);
		if(!(conn=(connectlist *)mempool_alloc(server->process[index].mem_pool,sizeof(connectlist))))
		{
			perror("malloc  errror");
		}
		pool->conn[fd]=conn; //我原先用的是索引号 发现在释放的时候 不是很方便;
		//这个地方要把连接节点的信息给初始化下;
	}
	conn->socket=fd;        //连接描述字;
	conn->status=status;
	pool->nodenum++;
	struct timeval  timeout={30,0};
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

