#include"memmanger.h"

unsigned  int  power_largest;
/*内存池的一个初始化操作只是初始化了每个slabclass的一个slab*/
static  int  mempool_init(size_t  limit,int    factor)
{
   size_t  size=sizeof(struct  data_buff)+64;
   int   i=0;
   while(++i<MAX_MEMPOOL_SIZE&&size<=limit/factor)
   {
	   if(size%ALIGN_MEM_DEFAULT)
		   size+=ALIGN_MEM_DEFAULT-(size%ALIGN_MEM_DEFAULT);  //字节对齐;
       mempool[i].size=size;
	   mempool[i].perslab=limit/mempool[i].size;
	   size*=factor;
	   printf("slabclass  ID:%d  size:%u  perslab:%u\n",i,mempool[i].size,mempool[i].perslab);
   }
   power_largest=i;
   mempool[power_largest].size=size;
   mempool[power_largest].perslab=1;
   printf("slabclass  ID:%d  size:%u  perslab:%u\n",i,mempool[i].size,mempool[i].perslab);
}

//给定一个size在slabclass这个数组中查找到相应的id号;
static   int   grow_list_id(size_t  size)
{
    int  res=1;
	if(res<1&&res>power_largest)
		return  0;
	while(size>mempool[res].size)
		if(mempool[res++].size>DEFAULT_MEM_SIZE)
			return   0;
	return  res;
}
//扩容处理;
static  int   grow_slab_list(unsigned signed  int  id)
{
   
    
}

//申请一块内存;
static  void * do_slab_alloc(size_t size, unsigned  int  id)
{
	//根据id号去找相应的slabclass;
    struct  mem_pool *p;
	struct  data_buff  *data;
	p=&mempool[id];
	if(!(p->slots!=NULL))
	{
		//重新分配内存;
	}
	else
	{
        //说明有空闲内存;

	}
}





















