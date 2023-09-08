/*include "wallClock.h"
int system_ticks = 0;

void tick(void) {
	++system_ticks;
	tickUpdateClock();
	return;
}
*/

#include "wallClock.h"
int system_ticks = 0;
int HH,MM,SS,ms = 0;
#define HH_TO_MS 60*60*1000
#define MM_TO_MS 60*1000
#define SS_TO_MS 1000
#define TIMEMAX 24*HH_TO_MS
void tick(void){
	system_ticks++;
	//你需要填写它
	ms += 10;
	if (ms >= 1000) {
		ms -= 1000;
		++SS;
	}
	if (SS >= 60) {
		SS -= 60;
		++MM;
	}
	if (MM>= 60) {
		MM -= 60;
		++HH;
	}
	if (HH >= 24) {
		HH -= 24;
	}
	setWallClock(HH,MM,SS);
	return;
}