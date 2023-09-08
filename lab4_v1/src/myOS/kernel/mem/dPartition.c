#include "../../include/myPrintk.h"

#define MIN_MEMEORY 0x4
#define END 0xFFFFFFFF
// dPartition 是整个动态分区内存的数据结构
typedef struct dPartition
{
	unsigned long size;
	unsigned long firstFreeStart;
} dPartition; // 共占8个字节

#define dPartition_size ((unsigned long)0x8)

void showdPartition(struct dPartition *dp)
{
	myPrintk(0x5, "dPartition(start=0x%x, size=0x%x, firstFreeStart=0x%x)\n", dp, dp->size, dp->firstFreeStart);
}

// EMB 是每一个block的数据结构，userdata可以暂时不用管。
typedef struct EMB
{
	unsigned long size;
	union
	{
		unsigned long nextStart; // if free: pointer to next block
		unsigned long userData;	 // if allocated, belongs to user
	};
} EMB; // 共占8个字节

#define EMB_size ((unsigned long)0x8)

void showEMB(struct EMB *emb)
{
	myPrintk(0x3, "EMB(start=0x%x, size=0x%x, nextStart=0x%x)\n", emb, emb->size, emb->nextStart);
}

unsigned long dPartitionInit(unsigned long start, unsigned long totalSize)
{
	// TODO
	/*功能：初始化内存。
	1. 在地址start处，首先是要有dPartition结构体表示整个数据结构(也即句柄)。
	2. 然后，一整块的EMB被分配（以后使用内存会逐渐拆分），在内存中紧紧跟在dP后面，然后dP的firstFreeStart指向EMB。
	3. 返回start首地址(也即句柄)。
	注意有两个地方的大小问题：
		第一个是由于内存肯定要有一个EMB和一个dPartition，totalSize肯定要比这两个加起来大。
		第二个注意EMB的size属性不是totalsize，因为dPartition和EMB自身都需要要占空间。

	*/
	dPartition *dPartition = start;
	dPartition->size = totalSize;

	unsigned long _EMB = start + dPartition_size;
	((EMB *)_EMB)->size = totalSize - EMB_size - dPartition_size;
	((EMB *)_EMB)->nextStart = END;

	dPartition->firstFreeStart = _EMB;
	return start;
}

void dPartitionWalkByAddr(unsigned long dp)
{
	// TODO
	/*功能：本函数遍历输出EMB 方便调试
	1. 先打印dP的信息，可调用上面的showdPartition。
	2. 然后按地址的大小遍历EMB，对于每一个EMB，可以调用上面的showEMB输出其信息

	*/
	dPartition *dPartition = dp;
	showdPartition(dPartition);
	for (unsigned long EMBptr = dp + dPartition_size; EMBptr < dp + dPartition->size; EMBptr += ((EMB *)EMBptr)->size + EMB_size)
	{
		showEMB(EMBptr);
	}
}

//=================firstfit, order: address, low-->high=====================
/**
 * return value: addr (without overhead, can directly used by user)
 **/

unsigned long dPartitionAllocFirstFit(unsigned long dp, unsigned long size)
{
	// TODO
	/*功能：分配一个空间
	1. 使用firstfit的算法分配空间，
	2. 成功分配返回首地址，不成功返回0
	3. 从空闲内存块组成的链表中拿出一块供我们来分配空间(如果提供给分配空间的内存块空间大于size，我们还将把剩余部分放回链表中)，并维护相应的空闲链表以及句柄
	注意的地方：
		1.EMB类型的数据的存在本身就占用了一定的空间。
	*/
	// 4 bytes align
	while (size % 4 != 0)
	{
		size++;
	}

	dPartition *dPartition = dp;
	unsigned long EMBptr = dPartition->firstFreeStart;
	unsigned long preEMBptr = dp;
	int flag = 0; // preEMBptr is EMB

	for (;; preEMBptr = EMBptr, EMBptr = ((EMB *)EMBptr)->nextStart)
	{
		if (EMBptr == END)
		{
			//myPrintk(0x7, "no suitable space for size %ld\n", size);
			return 0;
		}
		if (((EMB *)EMBptr)->size >= size){
			if(EMBptr == dPartition->firstFreeStart) flag = 1;
			break;
		}
	}
	unsigned long allocat_addr = (unsigned long)EMBptr;

	if (((EMB *)EMBptr)->size - size > MIN_MEMEORY)
	{
		unsigned long newEMBptr = (unsigned long)(EMBptr) + size + EMB_size;
		if (flag == 1)
		{
			dPartition->firstFreeStart = newEMBptr;
		}
		if (flag == 0)
		{
			((EMB *)preEMBptr)->nextStart = newEMBptr;
		}
		((EMB *)newEMBptr)->nextStart = ((EMB *)EMBptr)->nextStart;
		((EMB *)newEMBptr)->size = ((EMB *)EMBptr)->size - size - EMB_size;
	}
	else if (((EMB *)EMBptr)->size - size <= MIN_MEMEORY)
	{
		if (flag == 1)
		{
			dPartition->firstFreeStart = ((EMB *)EMBptr)->nextStart;
		}
		if (flag == 0)
		{
			((EMB *)preEMBptr)->nextStart = ((EMB *)EMBptr)->nextStart;
		}
	}

	((EMB *)EMBptr)->size = size;
	((EMB *)EMBptr)->nextStart = 0;
	return allocat_addr;
}

unsigned long dPartitionFreeFirstFit(unsigned long dp, unsigned long start)
{
	// TODO
	/*功能：释放一个空间
	1. 按照对应的fit的算法释放空间
	2. 注意检查要释放的start~end这个范围是否在dp有效分配范围内
		返回1 没问题
		返回0 error
	3. 需要考虑两个空闲且相邻的内存块的合并
	*/
	unsigned long end = start + ((EMB *)start)->size;
	if (end > ((dPartition *)dp)->size + dp | start < dp + dPartition_size){
		//myPrintk(0x7, "end:%d right:%d",end,((dPartition *)dp)->size + dp);
		//myPrintk(0x7, "start:%d left:%d",start,(dp + dPartition_size));
		return 0;
	}
	((EMB *)start)->nextStart = END; //

	dPartition *dPartition = dp;
	unsigned long EMBptr = dPartition->firstFreeStart;
	unsigned long followEMBptr = ((EMB *)EMBptr)->nextStart;

	if (dPartition->firstFreeStart == END)
		dPartition->firstFreeStart = start;
	else if (dPartition->firstFreeStart > start)
	{
		((EMB *)start)->nextStart = dPartition->firstFreeStart;
		dPartition->firstFreeStart = start;
		Intersect(start, ((EMB *)start)->nextStart);
	}
	else
	{
		for (;;)
		{
			if (followEMBptr == END)
				break;
			if (EMBptr < start && followEMBptr > start){
				((EMB *)start)->nextStart = followEMBptr;
				((EMB *)EMBptr)->nextStart = start;
				break;
			}
			EMBptr = ((EMB *)EMBptr)->nextStart;
			followEMBptr = ((EMB *)EMBptr)->nextStart;
		}
		Intersect(start, followEMBptr);
		Intersect(EMBptr, start);
	}
}

void Intersect(unsigned long start1, unsigned long start2)
{
	if (start2 == END)
		return;
	if (((EMB *)start1)->size + start1 + EMB_size == start2)
	{
		((EMB *)start1)->size += ((EMB *)start2)->size + EMB_size;
		((EMB *)start1)->nextStart = ((EMB *)start2)->nextStart;
	}
}

// 进行封装，此处默认firstfit分配算法，当然也可以使用其他fit，不限制。
unsigned long dPartitionAlloc(unsigned long dp, unsigned long size)
{
	return dPartitionAllocFirstFit(dp, size);
}

unsigned long dPartitionFree(unsigned long dp, unsigned long start)
{
	return dPartitionFreeFirstFit(dp, start);
}
