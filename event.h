#ifndef  _EVENT_H__
#define  _EVENT_H__

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"server.h"

#define   EVENT_TIMEOUT    0x01  //超时事件;
#define   EVENT_READ       0x02  //读事件;
#define   EVENT_WRITE      0x03  //写事件;



struct   event_t
{
	int   event_type;  //事件类型;


	//对应的连接索引号;
	//在链表中的位置;
	//在小根堆中位置;
};






















int worker_process_handler(struct  serverinfo  *server,int  index);

#endif


