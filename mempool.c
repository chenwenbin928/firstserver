#include"mempool.h"

/*
 *初始化内存池结构;
 */

unsigned int  count;
mempool  *  mempool_init(mempool * pool,size_t  size)
{
	size_t    esize=size?size:PAGE_SIZE;
	if(!(pool=(mempool *)malloc(esize)))
	{
		perror("pool init  error!\n");
		return   NULL;
	}

	pool->epsize=esize-sizeof(mempool);
	pool->data.last=(unsigned char *)pool+sizeof(mempool);
	pool->data.end=(unsigned char *)pool+size;
	pool->data.next=NULL;
	pool->data.flag=0;
	pool->large=NULL;
	pool->current=pool;
	printf("---------------------内存池--------------------------\n");
	printf("sizeof(mempool)=%d\n",                sizeof(mempool));
	printf("起始地址:%p\n",                       pool->data.last);
	printf("结束地址:%p\n",                       pool->data.end);
	printf("可用区域大小:%d\n",                   pool->epsize);
	printf("当前内存块:%p\n",                     pool->current);
	printf("-----------------------------------------------------\n");
	return   pool;
}

/*重新申请一块小内存*/
void  * mempool_block_alloc(mempool  *pool,size_t size)
{
	size_t  esize;
	void    *m;
	mempool *p,*temp,*q;
	esize=(size_t)pool->data.end-(size_t)pool;
	size+=ALIGN_DEFAULT-ALIGN_DEFAULT%esize;
	if(!(m=malloc(esize)))
	{
		perror("malloc  block  error!\n");
		return    NULL;
	}
	p=(mempool *)m;
	p->epsize=esize;
	p->large=NULL;
	p->data.next=NULL;
    m+=sizeof(struct  mempool_data);
	p->data.end=m+p->epsize;
	p->data.last=m+size;
	//插入链表;
	temp=pool->current;
	for(q=temp;q->data.next!=NULL;q=q->data.next)
	{

	}
	q->data.next=p;
	pool->current=temp?temp:p;
	printf("申请了新的内存块!%p\n",p);
	return  m;
}

/*重新申请一块大内存区域*/
void  * mempool_large_block_alloc(mempool *pool,size_t size)
{
	void  *m;
	struct   mempool_large_t   *p;
	size+=ALIGN_DEFAULT-(ALIGN_DEFAULT%size);
	m=(void *)malloc(size);
	if(!m)
	{
		perror("large  malloc");
		return  NULL;
	}
	for(p=pool->large;p;p=p->next)
	{
		if(p->alloc==NULL)
		{
			p->alloc=m;
			return   m;
		}

	}
	//插入链表;
	if(!(p=(struct  mempool_large_t *)mempool_alloc(pool,sizeof(struct  mempool_large_t))))
	{
		perror("malloc  errorr \n");
		return  NULL;
	}
	p->alloc=m;
	p->next=pool->large;
	pool->large=p;
	return  m;
}

/*申请一块内存*/
void  * mempool_alloc(mempool *pool,size_t  size)
{
	mempool *p=pool->current;
	void  *ptr;
	if(size<=p->epsize)
	{
		size+=ALIGN_DEFAULT-(size%ALIGN_DEFAULT);
		printf("size=%d count=%d\n",size,count++);
		do
		{
			ptr=p->data.last;
			if(size<=(size_t)p->data.end-(size_t)p->data.last)
			{
				p->data.last=ptr+size;
				return  ptr;
			}
			p=p->data.next;
		}while(p);
		return    mempool_block_alloc(pool,size);
	}
	return    mempool_large_block_alloc(pool,size);
}

int  main(int argc,char **argv)
{
	mempool  *pool;
	int     i;
	pool=mempool_init(pool,50);
	for(i=0;i<100;i++)
	{
		void  *m;
		m=mempool_alloc(pool,4096);
		printf("m=%p\n",(unsigned char *)m);

	}

}

