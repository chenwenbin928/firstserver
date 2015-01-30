#ifndef  _EVENT_H__
#define  _EVENT_H__

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"server.h"

#define   EVENT_TIMEOUT    0x01  //超时事件;
#define   EVENT_READ       0x02  //读事件;
#define   EVENT_WRITE      0x03  //写事件;























int worker_process_handler(struct  serverinfo  *server,int  index);

#endif


