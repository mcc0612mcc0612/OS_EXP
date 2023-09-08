#include "../../include/myPrintk.h"
#include "../../include/mem.h"
unsigned long pMemStart;  // 可用的内存的起始地址
unsigned long pMemSize;  // 可用的大小
unsigned long pMemHandler;
unsigned long pMemHandler_kernel;

void memTest(unsigned long start, unsigned long grainSize){
	// TODO
	/*功能：检测算法
		这一个函数对应实验讲解ppt中的第一大功能-内存检测。
		本函数的功能是检测从start开始有多大的内存可用，具体算法参照ppt检测算法的文字描述
	注意点三个：
	1、覆盖写入和读出就是往指针指向的位置写和读，不要想复杂。
	  (至于为什么这种检测内存的方法可行大家要自己想一下)
	2、开始的地址要大于1M，需要做一个if判断。
	3、grainsize不能太小，也要做一个if判断
	*/
	if(start < 0x100000)
	{
		myPrintk(0x4,"raise warning: memory access below 1M, change boundary to 1M!\n");
		start = 0x100000;
	}
	if(grainSize <= 0x400)  // 0x400 = 1K 最小为1K
	{
		myPrintk(0x4,"raise warning: memory detect. footstep is too small, footstep: %x\n, changed to 1K",grainSize);
		grainSize = 0x400;
	}
	unsigned long pMemStart =start;
	for(;;){
		unsigned short* ptr = (unsigned short*) start;
		unsigned short tempptr = *ptr;
		*ptr = 0xAA55;
		if(*ptr != 0xAA55)    break;
		*ptr = 0x55AA;
        if(*ptr != 0x55AA)   break;
		*ptr = tempptr;

		ptr = (unsigned short*)(start + grainSize) - 1;
		tempptr = *ptr;
		*ptr = 0xAA55;
		if(*ptr != 0xAA55)    break;
		*ptr = 0x55AA;
        if(*ptr != 0x55AA)   break;
		*ptr = tempptr;

		start += grainSize;
	}
	pMemSize = start - pMemStart;
	myPrintk(0x7,"MemStart: %x  \n", pMemStart);
	myPrintk(0x7,"MemSize:  %x  \n", pMemSize);
	
}

extern unsigned long _end;
void pMemInit(void){
	unsigned long _end_addr = (unsigned long) &_end;
	memTest(0x100000,0x1000);
	myPrintk(0x7,"_end:  %x  \n", _end_addr);
	if (pMemStart <= _end_addr) {
		pMemSize -= _end_addr - pMemStart;
		pMemStart = _end_addr;
	}
	
	// 此处选择不同的内存管理算法
	pMemHandler = dPartitionInit(pMemStart, 0x1000000-pMemStart);	
	pMemHandler_kernel = dPartitionInit(0x1000000, pMemSize + pMemStart -0x1000000);
}
