#include"queue.h"
#include"server.h"
#include"mempool.h"
/*比较用户名是否相同*/
int  nameswitch(char *elm1,char *elm2)
{
	int   ret;
	int  len1,len2;
	if(elm1==NULL||elm2==NULL)
	{
		perror("elm  is  null!\n");
		return  -1;
	}
	len1=strlen(elm1);
	len2=strlen(elm2);
	if(len1!=len2)
	{
		return  2;
	}
	else
	{
		ret=strncmp(elm1,elm2,len1);
		if(ret==0)
			return  1;
		else
			return  2;
	}
}
/*初始化双向队列*/
struct   userinfo  *  queue_init(struct   userinfo  * head,int  index)
{
	if(!(head=(struct  userinfo *)mempool_alloc(server->process[index].mem_pool,sizeof(struct  userinfo))))
	{
		perror("malloc error");
		return  NULL;
	}
	head->next=head;
	head->prev=head;
	return  head;
}
/*插入节点到链表操作*/
int   queue_insert_node(struct  userinfo  *head, struct  userinfo  *elm)
{
	if(elm==NULL)
	{
		perror("elm is null!\n");
		return  -1;
	}
	elm->next=head->next;
	head->next=elm;
	elm->prev=head;
	if(head->next==head)
	{
       head->prev=elm;
	}
	return    1;
}

/*删除链表节点操作*/
int  queue_remove_node(struct  userinfo  *head,struct  userinfo  *elm)
{
   int   ret;
   int   flag=0;
   struct   userinfo  *  temp;
   struct   userinfo  *  tempprev;
   if(elm==NULL)
   {
	   perror("elm  is null!\n");
	   return  -1;
   }
   else
   {
	  tempprev=head->prev;
	  temp=head->next;
	  while(temp!=head)
	  {
		  //比较用户名是否相同;
          ret=nameswitch(temp->name,elm->name);
		  if(ret==1)
		  {
			 flag=1;
             tempprev->next=temp->next;
			 temp->next->prev=tempprev;
			 printf("删除成功!\n");
			 free(temp);
			 return  1;
		  }
		  tempprev=temp;
		  temp=temp->next;
	  }
	  if(flag==0)
	  {
		  perror("elm  is  not found!\n");
		  return  2;
	  }
   }
}

/*
 *遍历链表;
 */
void  display_queue_info(struct  userinfo  * head)
{
    struct   userinfo  *temp;
	temp=head->next;
	while(temp!=head)
	{
		printf("-----------------用户信息---------------\n");
		printf("用户名:%s\n", temp->name);
		printf("用户密码:%s\n",temp->passwd);
		printf("用户年龄:%d\n",temp->year);
		printf("客户端的ip地址:%s\n",temp->ipaddr);
		printf("----------------------------------------\n");
		temp=temp->next;
	}
}

