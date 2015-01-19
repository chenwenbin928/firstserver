#ifndef  _QUEUE_H_
#define  _QUEUE_H_
#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<netinet/ip.h>
#include<netinet/in.h>
#include<string.h>
/*
 *每个用户的用户要素;
 */
struct  userinfo
{
   char  name[50];
   char  passwd[20];
   int   year;
   char  ipaddr[20];
   struct   sockaddr_in  serveraddr;   //ip地址;
   short    serverport;                //ip端口;
   int      fd;                        
   struct   userinfo  *next;
   struct   userinfo  *prev;
};


int  nameswitch(char *elm1,char *elm2);
struct   userinfo  *  queue_init(struct   userinfo  * head);
int   queue_insert_node(struct  userinfo  *head, struct  userinfo  *elm);
int  queue_remove_node(struct  userinfo  *head,struct  userinfo  *elm);
void  display_queue_info(struct  userinfo  * head);



#endif

