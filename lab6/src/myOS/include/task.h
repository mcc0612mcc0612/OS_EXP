#ifndef __TASK_H__
#define __TASK_H__

#ifndef USER_TASK_NUM
#include "../../userApp/userApp.h"
#endif
#define NULL (void *)0

#define TASK_NUM (2 + USER_TASK_NUM)   // at least: 0-idle, 1-init

#define initTskBody myMain         // connect initTask with myMain

#define STACK_SIZE 512            // definition of STACK_SIZE

//option for setTskPara()/getTskPara
#define PRIORITY 1
#define EXETIME 2
#define ARRTIME 3
#define SCHED_POLICY 4

#define TSK_RDY 0    // 表示当前进程已经进入就绪队列中
#define TSK_WAIT -1  // 表示当前进程还未进入就绪队列中
#define TSK_RUNING 1 // 表示当前进程正在运行
#define TSK_NONE 2   // 表示进程池中的TCB为空未进行分配

extern struct scheduler sch;

void initTskBody(void);

void CTX_SW(void*prev_stkTop, void*next_stkTop);

typedef struct tskPara{
     unsigned int priority;
     unsigned int exeTime;
     unsigned int arrTime;
     unsigned int schedPolicy;
} tskPara;

//#error "TODO: 为 myTCB 增补合适的字段"
typedef struct myTCB {
     unsigned long *stkTop;        /* 栈顶指针 */
     unsigned long stack[STACK_SIZE];      /* 开辟了一个大小为STACK_SIZE的栈空间 */  
     unsigned long TSK_State;   /* 进程状态 */
     unsigned long TSK_ID;      /* 进程ID */ 
     void (*task_entrance)(void);  /*进程的入口地址*/
     struct myTCB * nextrqTCB;           /*下一个rqTCB*/
     struct myTCB * nextpoolTCB;           /*下一个poolTCB*/
     tskPara info;
} myTCB;

#define SCHEDULER_FCFS 0
#define SCHEDULER_SJF 1
#define SCHEDULER_PRIORITY0 2
#define SCHEDULER_RR 3
#define SCHEDULER_PRIORITY 4
#define SCHEDULER_MQ 5
#define SCHEDULER_FMQ 6

struct scheduler {
    unsigned int type;
    int preemptiveOrNot;
    myTCB* (*nextTsk_func)(void);
    void (*enqueueTsk_func)(myTCB *tsk);
    void (*dequeueTsk_func)(myTCB *tsk);
    void (*schedulerInit_func)(myTCB* tcb);
    void (*createTsk_hook)(myTCB* task,void (*taskBody)(void)); //if set, will be call in createTsk (before tskStart)
    void (*tick_hook)(void); //if set, tick_hook will be called every tick
};

typedef struct rdyQueue
{
     myTCB *head;
     myTCB *tail;
     myTCB *idleTsk;
} rdyQueue;

extern rdyQueue rq;
extern rdyQueue pool;

void initTskPara(tskPara *buffer);
void setTskPara(unsigned int option, unsigned int value, tskPara *buffer);
void setScheduler(int option);

void stack_init(unsigned long **stk, void (*task)(void));
void tskStart(myTCB *tsk);
//SJF
void init_schedulerSJF();
void init_schedulerPriority();

extern myTCB* idleTask;            /* idle 任务 */
extern myTCB* currentTask;         /* 当前任务 */
extern myTCB* firstFreeTask;       /* 下一个空闲的 TCB */
extern myTCB tcbPool[TASK_NUM];    //进程池的大小设置

extern struct scheduler sch;

void TaskManagerInit(void);

#endif
