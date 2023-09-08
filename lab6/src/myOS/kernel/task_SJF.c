
#include "../include/task.h"

void InitRqSJF(myTCB* idleTask) {//对rq进行初始化处理
    rq.idleTsk = idleTask;
}    //idleTask不入队

void tskEnqueuePoolSJF(myTCB *tsk)
{ // 将tsk入队rqFCFS
     if (pool.head == NULL)
     {
          pool.head = tsk;
          pool.tail = tsk;
     }
    else
    {
        myTCB *start = pool.head;
        myTCB *pre = NULL;
        if(tsk->info.exeTime > pool.tail->info.exeTime){
            pool.tail->nextpoolTCB = tsk;
            pool.tail = tsk;
        }
        else{
            for(;start;start = start->nextpoolTCB){
                if(tsk->info.exeTime < start->info.exeTime){
                    if(pre == NULL){
                        pool.head = tsk;
                        tsk->nextpoolTCB = start;
                    }
                    else{
                        pre->nextpoolTCB = tsk;
                        tsk->nextpoolTCB = start;
                    }
                }
            }
        }
    }
}

void tskEnqueueRqSJF(myTCB *tsk)
{ // 将tsk入队rqFCFS
     if (rq.head == NULL)
     {
          rq.head = tsk;
          rq.tail = tsk;
     }
    else
    {
        myTCB *start = rq.head;
        myTCB *pre = NULL;
        if(tsk->info.exeTime > rq.tail->info.exeTime){
            rq.tail->nextrqTCB = tsk;
            rq.tail = tsk;
        }
        else{
            for(;start;start = start->nextrqTCB){
                if(tsk->info.exeTime < start->info.exeTime){
                    if(pre == NULL){
                        rq.head = tsk;
                        tsk->nextrqTCB = start;
                    }
                    else{
                        pre->nextrqTCB = tsk;
                        tsk->nextrqTCB = start;
                    }
                    break;
                }
            }
        }
    }
}


// 将就绪队列中的TCB移除（需要填写）
void tskDequeuePoolSJF(myTCB *tsk)
{ // rqFCFS出队
    myTCB *pre_start = NULL;
    myTCB *start = pool.head;
    for (; start;)
    {
        if (start == tsk)
        {
            if (pool.tail == tsk)
                if (pre_start == NULL)
                {
                    pool.head = start->nextpoolTCB;
                    pool.tail = pre_start;
                }
                else
                {
                    pre_start->nextpoolTCB = start->nextpoolTCB;
                    pool.tail = pre_start;
                }
                else
                {
                    if (pre_start == NULL)
                    {
                        pool.head = start->nextpoolTCB;
                    }
                    else
                    {
                        pre_start->nextpoolTCB = start->nextpoolTCB;
                    }
                }
        }
        pre_start = start;
        start = start->nextpoolTCB;
    }
}

void tskDequeueRqSJF()
{ // rqFCFS出队
    if (rq.head == rq.tail) {
        rq.head = NULL;
        rq.tail = NULL;
    }
    else{
        rq.head = rq.head->nextrqTCB;
    }
}


myTCB *nextTskRqSJF(void)
{ // 获取下一个Tsk
    if (!rq.head) {
        return &tcbPool[1];
    }
    else {
        return rq.head;
    }     
}


// 以tskBody为参数在进程池中创建一个进程，并调用tskStart函数，将其加入就绪队列（需要填写）
void createTskSJF(myTCB* task,void (*tskBody)(void))
{ 
    task->task_entrance = tskBody;
    // 在进程池中创建一个进程，并把该进程加入到rq队列中
    stack_init(&(firstFreeTask->stkTop), tskBody);
    task->task_entrance = tskBody;
    task->TSK_State = TSK_WAIT;
    if(task->info.arrTime == 0)
        tskStart(task);
    else
        tskEnqueuePoolSJF(task);
}

myTCB *nextTskPoolSJF(void){
   if (!pool.head) {
        return &tcbPool[1];
    }
    else {
        return pool.head;
    }
}

void tickHookSJF(){
    myTCB* task = pool.head;
    while (task) {
        if (task->TSK_State == TSK_WAIT) {
            task->info.arrTime--;
            if (task->info.arrTime == 0) {
                task->TSK_State = TSK_RDY;
                tskDequeuePoolSJF(task);
                tskEnqueueRqSJF(task);
            }
        }
        task = task->nextpoolTCB;
    }
    if (currentTask->info.exeTime) {
        currentTask->info.exeTime--;
    }
}




void init_schedulerSJF(){
    sch.createTsk_hook = createTskSJF;
    sch.dequeueTsk_func = tskDequeueRqSJF;
    sch.enqueueTsk_func = tskEnqueueRqSJF;
    sch.preemptiveOrNot = 1;
    sch.nextTsk_func = nextTskRqSJF;
    sch.schedulerInit_func = InitRqSJF;
    sch.tick_hook = tickHookSJF;
}