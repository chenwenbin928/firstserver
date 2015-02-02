#ifndef _CONNECT_H__
#define _CONNECT_H__
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<sys/time.h>
#include"server.h"

#define  MAX_CONNECT_POOL   1024

#define  FIRST_MALLOC       0x01
#define  FREE_FIND          0x02

enum status
{
	WAIT_LISTENING,
	DATA_READING,
	DATA_WRITING,
	SOCKET_CLOSING
};
//状态机机制;
/*
 *一个连接的定义;
 *对于连接超时的问题,原先是打算提交事件到二叉树来解决   
 *但是对于频繁的网络连接,我只能呵呵了
 *于是乎  我就打算把网络连接事件交给客户端
 *
 */
typedef   struct  connection
{
	int     socket;   //连接套接字;
	int     status;   //状态;
	char    client_addr[50];//ip地址;
	int     port;       //端口号;
    struct  timeval     starttv; //连接建立时间;
	struct  connection  *next; //这个指针域是在节点释放的时候 我才会去用;
	//下面定义对应的事件吧;回调函数实现;event.h结构体中
}connectlist;

/*
 *连接池定义;
 *这个连接池主要就是一个索引表,相应的指针节点指向一个连接节点,如果连接节点失效,节点我不释放,清空节点后
 *加入到连接池中的空闲节点链表中去;
 *所以我也改了一部分东西;
 */
struct   connect_pool
{
	connectlist    **conn;     //连接池的索引表; 可以用哈希表代替吧;
	int       nodenum;      //当前连接池的有效节点个数;
	int       connectnum; //连接池的最大容量;
	//pthread_mutex_t   mutex;
    connectlist  * freeconnhead;//当前的空闲节点链表;		
	int         connnum; //当前空闲节点数目;
	int         maxflag; //网络连接上限标志位;为1的话  我们就不允许接收连接了！
};


struct  connect_pool *connect_pool_init(struct serverinfo  *server,int index,struct  connect_pool *pool,int  pid);//连接池初始化;
struct   connect_pool * connect_pool_realloc(struct connect_pool  *pool,int  fd); //连接池扩容;
connectlist *  get_connection_from_free_pool(struct  connect_pool  *pool,int  fd);//从空闲索引表中取出一个指针表示我们实际上的在线连接。
void (*callback)(struct  serverinfo * ,int  ,struct  connect_pool *,int,int,struct sockaddr_in *); //回调函数;
void  destroy_worker_process_conn_pool(struct   connect_pool *pool); //销毁连接池;
int   add_connect_node_to_bufflist(struct  connect_pool * pool,connectlist  * conn);//把失效的连接节点送入到空闲链表中;
connectlist  *  find_free_node_from_connectfreelist(struct  connect_pool *pool);//在空闲节点链表中找到出一个空闲节点来初始化新的连接;

#endif

