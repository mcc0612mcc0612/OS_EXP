#include "../include/task.h"
#include "../include/myPrintk.h"
#define NULL (void *)0

myTCB *idleTask;      /* idle 任务 */
myTCB *currentTask;   /* 当前任务 */
myTCB *firstFreeTask; /* 下一个空闲的 TCB */

myTCB tcbPool[TASK_NUM]; // 进程池的大小设置

rdyQueue rq;
rdyQueue pool;
struct scheduler sch;

void initTskPara(tskPara *buffer)
{
     buffer->arrTime = 0;
     buffer->exeTime = 1;
     buffer->priority = 3;
     return;
}
void setTskPara(unsigned int option, unsigned int value, tskPara *buffer)
{
     if (option == PRIORITY)
          buffer->priority = value;
     else if (option == ARRTIME)
          buffer->arrTime = value;
     else if (option == EXETIME)
          buffer->exeTime = value;
}

void setScheduler(int option){
     switch (option){
          case SCHEDULER_SJF: init_schedulerSJF();break;
          case SCHEDULER_PRIORITY: init_schedulerPriority();break;
     }
}


void rqInit(myTCB *idleTask)
{
     sch.schedulerInit_func(idleTask);
}
// tskIdleBdy进程（无需填写）
void tskIdleBdy(void)
{
     while (1)
     {
          schedule();
     }
}

// tskEmpty进程（无需填写）
void tskEmpty(void)
{
}

// 初始化栈空间（不需要填写）
void stack_init(unsigned long **stk, void (*task)(void))
{
     *(*stk)-- = (unsigned long)0x08;   // 高地址
     *(*stk)-- = (unsigned long)task;   // EIP
     *(*stk)-- = (unsigned long)0x0202; // FLAG寄存器

     *(*stk)-- = (unsigned long)0xAAAAAAAA; // EAX
     *(*stk)-- = (unsigned long)0xCCCCCCCC; // ECX
     *(*stk)-- = (unsigned long)0xDDDDDDDD; // EDX
     *(*stk)-- = (unsigned long)0xBBBBBBBB; // EBX

     *(*stk)-- = (unsigned long)0x44444444; // ESP
     *(*stk)-- = (unsigned long)0x55555555; // EBP
     *(*stk)-- = (unsigned long)0x66666666; // ESI
     *(*stk) = (unsigned long)0x77777777;   // EDI
}

//如果就绪队列为空，返回True
int IsEmptyRq(void) {//当head和tail均为NULL时，rq为空
    if (rq.head == NULL && rq.tail == NULL) {
        return 1;
    }
    return 0;
}
// 进程池中一个未在就绪队列中的TCB的开始（不需要填写）
void tskStart(myTCB *tsk)
{
     tsk->TSK_State = TSK_RDY;
     // 将一个未在就绪队列中的TCB加入到就绪队列
     sch.enqueueTsk_func(tsk);
}

// 进程池中一个在就绪队列中的TCB的结束（不需要填写）
void tskEnd(void)
{
     // 将一个在就绪队列中的TCB移除就绪队列
     sch.dequeueTsk_func(currentTask);
     // 由于TCB结束，我们将进程池中对应的TCB也删除
     destroyTsk(currentTask->TSK_ID);
     // TCB结束后，我们需要进行一次调度
     while (currentTask->info.exeTime > 0);
     schedule();
}

// 以tskBody为参数在进程池中创建一个进程，并调用tskStart函数，将其加入就绪队列（需要填写）
int createTsk(void (*tskBody)(void), tskPara para)
{ // 在进程池中创建一个进程，并把该进程加入到rqFCFS队列中
     myTCB* tsk = firstFreeTask;
     tsk->info = para;
     //setTskPara(PRIORITY, para.priority, &para);
     //setTskPara(ARRTIME, para.arrTime, &para);
     //setTskPara(EXETIME, para.exeTime, &para);
     sch.createTsk_hook(tsk,tskBody);
     // maintain the free task link
     for (int i = 1; i < TASK_NUM; i++)
     {
          if (tcbPool[i].TSK_State == TSK_NONE)
          {
               firstFreeTask = &tcbPool[i];
               break;
          }
     }
}

// 以takIndex为关键字，在进程池中寻找并销毁takIndex对应的进程（需要填写）
void destroyTsk(int takIndex)
{ // 在进程中寻找TSK_ID为takIndex的进程，并销毁该进程
     tcbPool[takIndex].stkTop = tcbPool[takIndex].stack + STACK_SIZE - 1;
     tcbPool[takIndex].task_entrance = tskEmpty;
     tcbPool[takIndex].TSK_State = TSK_NONE;
}

unsigned long **prevTSK_StackPtr;
unsigned long *nextTSK_StackPtr;

// 切换上下文（无需填写）
void context_switch(myTCB *prevTsk, myTCB *nextTsk)
{
     prevTSK_StackPtr = &(prevTsk->stkTop);
     currentTask = nextTsk;
     nextTSK_StackPtr = nextTsk->stkTop;
     CTX_SW(prevTSK_StackPtr, nextTSK_StackPtr);
}

// 调度算法（无需填写）
void schedule(void)
{
     if(!IsEmptyRq()){
          myTCB* nextTask;
          nextTask = sch.nextTsk_func();
          context_switch(currentTask, nextTask);
     }

}

// 进入多任务调度模式(无需填写)
unsigned long BspContextBase[STACK_SIZE];
unsigned long *BspContext;
void startMultitask(void)
{
     BspContext = BspContextBase + STACK_SIZE - 1;
     prevTSK_StackPtr = &BspContext;
     currentTask = &tcbPool[0];
     nextTSK_StackPtr = currentTask->stkTop;
     CTX_SW(prevTSK_StackPtr, nextTSK_StackPtr);
}

// 准备进入多任务调度模式(无需填写)
void TaskManagerInit(void)
{
     // 初始化进程池（所有的进程状态都是TSK_NONE）
     int i;
     myTCB *thisTCB;
     for (i = 0; i < TASK_NUM; i++)
     { // 对进程池tcbPool中的进程进行初始化处理
          thisTCB = &tcbPool[i];
          thisTCB->TSK_ID = i;
          thisTCB->stkTop = thisTCB->stack + STACK_SIZE - 1; // 将栈顶指针复位
          thisTCB->TSK_State = TSK_NONE;                     // 表示该进程池未分配，可用
          thisTCB->task_entrance = tskEmpty;
          thisTCB->nextrqTCB = (void *)0;
          thisTCB->nextpoolTCB = (void *)0;
     }
     // 创建idle任务
     idleTask = &tcbPool[0];
     stack_init(&(idleTask->stkTop), tskIdleBdy);
     idleTask->task_entrance = tskIdleBdy;
     idleTask->nextrqTCB = (void *)0;
     idleTask->nextpoolTCB = (void *)0;
     idleTask->TSK_State = TSK_RDY;
     rqInit(idleTask);

     firstFreeTask = &tcbPool[1];

     // 创建init任务
     createTsk(initTskBody, (tskPara){0, 0, 0});

     // 进入多任务状态
     myPrintk(0x2, "START MULTITASKING......\n");
     startMultitask();
     myPrintk(0x2, "STOP MULTITASKING......SHUT DOWN\n");
}
