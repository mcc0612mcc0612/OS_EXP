#include "../../include/mem.h"

// 这里实现之前内存管理函数的封装，只对用户提供malloc/free接口

unsigned long malloc(unsigned long size){
    // 调用实现的dPartition或者是ePartition的alloc
    return dPartitionAlloc(pMemHandler,size);
    //return eFPartitionAlloc(pMemHandler);
}

unsigned long free(unsigned long start){
    // 调用实现的dPartition或者是ePartition的free
    return dPartitionFree(pMemHandler,start);
    //return eFPartitionFree(pMemHandler,start);
}

unsigned long kmalloc(unsigned long size){
    return dPartitionAlloc(pMemHandler_kernel,size);
}

unsigned long kfree(unsigned long start){
    return dPartitionFree(pMemHandler_kernel,start);
}
