#include "../myOS/userInterface.h"   //interface from kernel
#include "shell.h"
#include "memTestCase.h"

void myTsk0(void);
void myTsk1(void);
void myTsk2(void);
void startShell(void);
void initShell(void);
void memTestCaseInit(void);
int createTsk(void (*tskBody)(void),tskPara para);
void tskEnd(void);

void wallClock_hook_main(void){
	int _h, _m, _s;
	char hhmmss[]="hh:mm:ss\0\0\0\0";

	getWallClock(&_h,&_m,&_s);
	sprintf(hhmmss,"%02d:%02d:%02d",_h,_m,_s);
	put_chars(hhmmss,0x7E,24,72);
}

void doSomeTestBefore(void){		
	setWallClock(18,59,59);		//set time 18:59:59
    	setWallClockHook(&wallClock_hook_main);
     setTickHook(sch.tick_hook);
}

// init task 入口
void myMain(void) {
     clear_screen();

     //myPrintf(0x2,"debug_myMain_begin......\n");

     doSomeTestBefore();
     
     myPrintf(0x07, "********************************\n");
	myPrintf(0x07, "*         INIT   INIT !        *\n");
	myPrintf(0x07, "********************************\n");

     createTsk(myTsk0, (tskPara){ 3, 13, 2});
     createTsk(myTsk1,(tskPara) { 2, 11, 7 });
     createTsk(myTsk2,(tskPara) { 1, 13,12 });

     //#error "TODO: 初始化 shell 并创建 shell task"
     
     initShell();
     memTestCaseInit();
     createTsk(startShell,(tskPara){ 0, 0, 0 });

     //myPrintf(0x2,"debug_myMain_end......\n");

     tskEnd();
}
