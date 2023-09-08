
#include "../include/task.h"

void InitRqPriority(myTCB* idleTask) {//对rq进行初始化处理
    rq.idleTsk = idleTask;
}    //idleTask不入队

int IsEmptyPoolPriority(void) {//当head和tail均为NULL时，rq为空
    if (pool.head == NULL && pool.tail == NULL) {
        return 1;
    }
    return 0;
}

void tskEnqueuePoolPriority(myTCB *tsk)
{ // 将tsk入队rqFCFS
     if (pool.head == NULL)
     {
          pool.head = tsk;
          pool.tail = tsk;
     }
    else
    {
        pool.tail->nextpoolTCB = tsk;
        pool.tail = tsk;
    }
}

void tskEnqueueRqPriority(myTCB *tsk)
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
        if(tsk->info.priority > rq.tail->info.priority){
            rq.tail->nextrqTCB = tsk;
            rq.tail = tsk;
        }
        else{
            for(;start;start = start->nextrqTCB){
                if(tsk->info.priority < start->info.priority){
                    if(pre == NULL){
                        rq.head = tsk;
                        tsk->nextrqTCB = start;
                        schedule();
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

void tskDequeueRqPriority()
{ // rqFCFS出队
    if (rq.head == rq.tail) {
        rq.head = NULL;
        rq.tail = NULL;
    }
    else{
        rq.head = rq.head->nextrqTCB;
    }
}
int tskDequeuePoolPrioriy(myTCB* tsk){
 // rqFCFS出队
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


myTCB *nextTskRqPriority(void)
{ // 获取下一个Tsk
    if (!rq.head) {
        return &tcbPool[1];
    }
    else {
        return rq.head;
    }     
}

// 以tskBody为参数在进程池中创建一个进程，并调用tskStart函数，将其加入就绪队列（需要填写）
void createTskPriority(myTCB* task,void (*tskBody)(void))
{ 
    task->task_entrance = tskBody;
    // 在进程池中创建一个进程，并把该进程加入到rq队列中
    stack_init(&(firstFreeTask->stkTop), tskBody);
    task->task_entrance = tskBody;
    task->TSK_State = TSK_WAIT;
    if(task->info.arrTime == 0)
        tskStart(task);
    else
        tskEnqueuePoolPriority(task);
}

myTCB *nextTskPoolPriority(void){
   if (!pool.head) {
        return &tcbPool[1];
    }
    else {
        return pool.head;
    }
}



void tickHookPriority(){
    myTCB* task = pool.head;
    while (task) {
        if (task->TSK_State == TSK_WAIT) {
            task->info.arrTime--;
            if (task->info.arrTime == 0) {
                task->TSK_State = TSK_RDY;
                tskDequeuePoolPrioriy(task);
                tskEnqueueRqPriority(task);
            }
        }
        task = task->nextpoolTCB;
    }
    if (currentTask->info.exeTime) {
        currentTask->info.exeTime--;
    }
}




void init_schedulerPriority(){
    sch.createTsk_hook = createTskPriority;
    sch.dequeueTsk_func = tskDequeueRqPriority;
    sch.enqueueTsk_func = tskEnqueueRqPriority;
    sch.preemptiveOrNot = 1;
    sch.nextTsk_func = nextTskRqPriority;
    sch.schedulerInit_func = InitRqPriority;
    sch.tick_hook = tickHookPriority;
}