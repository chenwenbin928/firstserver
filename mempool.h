#ifndef  _MEM_POOL_H__
#define  _MEM_POOL_H__

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define    PAGE_SIZE    4096   
#define    ALIGN_DEFAULT   8


typedef   struct  mempool_t   mempool;  

/*
 *内存池的小内存块结构体定义;
 */
struct  mempool_data
{
  unsigned  char  *last;
  unsigned  char  *end;
  mempool         *next;
  int             failedflag;
  size_t          memID;      //memID号;

};

/*内存池的大内存块的结构体定义*/
struct  mempool_large_t   
{
	struct  mempool_large_t   *next;
	void   *alloc;
	size_t    memID;    //memID号;
};

/*内存池的结构*/
struct  mempool_t
{
  struct  mempool_data  data;  //内存池的数据区;
  size_t    epsize;            //第一个小内存块的有效长度;
  struct    mempool_large_t    *large; //大内存区域;
  mempool  *   current;//当前的内存块;

};

void  * mempool_alloc(mempool *pool,size_t  size);
mempool  *  mempool_init(mempool * pool,size_t  size);
void  * mempool_large_block_alloc(mempool *pool,size_t size);


#endif

