/*服务器端的内存管理策略
 *
 *
 *
 */
#ifndef  _MEMMANGER_H__
#define  _MEMMANGER_H__
#include<stdio.h>
#include<stdlib.h>
#include<string.h>


#define    MAX_MEMPOOL_SIZE        201
#define    ALIGN_MEM_DEFAULT       8
#define    DEFAULT_MEM_SIZE        1024*1024


struct   data_buff
{ 
	struct  data_buff  *next;     //双向链表;
	struct  data_buff  *prev;     //双向链表;
	void  *data;                  //数据存储区;
};



struct   mem_pool
{

	void  ** slab_list;       //slabclass管理slab_list的大小;
    unsigned  int   list_slab;//当前slab_list表的尺寸大小;   
	unsigned  int   size;     //每个chunk的大小;
	unsigned  int   perslab;  //每个slab中可以容纳多少个chunk;
    void    *slots;           //空闲链表;
	unsigned  int   sl_curr;  //空闲链表的个数;
    unsigned  int   slabs;    //已经创建了多少个slab;
};





struct   mem_pool   mempool[MAX_MEMPOOL_SIZE];


#endif    
