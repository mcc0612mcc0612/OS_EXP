/*int hh, mm, ss, ms;

typedef void HookUpdateFunction(void);
HookUpdateFunction *updateClock_hook = 0;

void tickUpdateClock() {
	// 100HZ -> 10ms/tick
	ms += 10;
	if (ms >= 1000) {
		ms -= 1000;
		++ss;
	}
	if (ss >= 60) {
		ss -= 60;
		++mm;
	}
	if (mm >= 60) {
		mm -= 60;
		++hh;
	}
	if (hh >= 24) {
		hh -= 24;
	}
	if (updateClock_hook) updateClock_hook();
}

void setWallClock(int HH, int MM, int SS){
	if (HH < 0 || HH > 24) hh = 0;
	else hh = HH;
	if (MM < 0 || MM > 60) mm = 0;
	else mm = MM;
	if (SS < 0 || SS > 60) ss = 0;
	else ss = SS;
	if (updateClock_hook) updateClock_hook();
}

void getWallClock(int *HH, int *MM, int *SS){
	*HH = hh;
	*MM = mm;
	*SS = ss;
}

void setClockHook(HookUpdateFunction *newhook){
	updateClock_hook = newhook;
}
*/
typedef void update_tick_hook(void);  
update_tick_hook *tick_hook = 0;
int H=0,M=0,S=0;
void setWallClock(int HH,int MM,int SS){
	H = HH;
	M = MM;
	S = SS;
	//你需要填写它
	if(tick_hook){
		tick_hook();
	}
}

void getWallClock(int *h,int *m,int *s){
	//你需要填写它
	    *h = H;
        *m = M;
        *s = S;
}

void setClockHook(update_tick_hook *newhook){
	tick_hook = newhook;
}