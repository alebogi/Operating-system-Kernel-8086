/*
 * system.cpp
 *
 *  Created on: Sep 7, 2020
 *      Author: OS1
 */


#include "system.h"
#include "idleThr.h"
#include "SCHEDULE.H"
#include "pcb.h"
#include <dos.h>
#include <iostream.h>
#include "lock.h"

//globals
unsigned int oldTimerOFF, oldTimerSEG;
unsigned int newTimerOFF, newTimerSEG;
volatile int context_switch_on_demand = 0;

unsigned int tsp;
unsigned int tss;
unsigned int tbp;

extern void tick();

volatile PCB* System::startingPCB = 0;
Thread* System::startingThread = 0;
Thread* System::runningThread = 0;
volatile PCB* System::runningPCB = 0;
IdleThread* System::idleThread = 0;
Queue* System::allThreadsPCBs = 0;
volatile int System::timeCnt = 5;

int System::lck = 0;

/**
 * initialisation of kernel.
 * Changes interrupt routine for timer from 08h to 60h, at 08h we place new routine
 * where we change context if we need to
 *
 */
void System::init(){
#ifndef BCC_BLOCK_IGNORE
	newTimerSEG = FP_SEG(System::timer);
	newTimerOFF = FP_OFF(System::timer);
	asm{
		cli
		push es
		push ax

		mov ax,0   //  ; init routine for timer
		mov es,ax

		mov ax, word ptr es:0022h //; remembers old routine at oldTimerSEG and OFF
		mov word ptr oldTimerSEG, ax
		mov ax, word ptr es:0020h
		mov word ptr oldTimerOFF, ax

		//sets new routine at entry 08h
		mov ax, word ptr newTimerSEG
		mov word ptr es:0022h, ax
		mov ax, word ptr newTimerOFF
		mov word ptr es:0020h, ax

		//sets old routine at int 60h
		mov ax, oldTimerSEG
		mov word ptr es:0182h, ax
		mov ax, oldTimerOFF
		mov word ptr es:0180h, ax

		pop ax
		pop es
	}
#endif

	allThreadsPCBs = new Queue();
	//creating thread for main
	startingThread = new Thread(maxStackSize, 1); //id==-1
	startingThread->myPCB->setState(PCB::RUNNING);
	startingPCB = startingThread->myPCB;
	runningPCB = startingThread->myPCB;
	runningThread = runningPCB->myThread;
	idleThread = new IdleThread(); //id==0
	idleThread->start();
	context_switch_on_demand = 0;

	#ifndef BCC_BLOCK_IGNORE
		asm sti
	#endif
}

/**
 * Brings back system in old condition
 * Restores old interruput routines, deallocate memory etc.
 * We dont have to return old routine at entry 60h because it was empty
 */
void System::restore(){
#ifndef BCC_BLOCK_IGNORE
	asm {
		cli
		push es
		push ax

		mov ax,0
		mov es,ax

		//returns old routine at entry 08h
		mov ax, word ptr oldTimerSEG
		mov word ptr es:0022h, ax
		mov ax, word ptr oldTimerOFF
		mov word ptr es:0020h, ax

		pop ax
		pop es
		sti
	}
#endif
	//clearing memory
	cout << "RESTORE";
	delete allThreadsPCBs;
	delete idleThread;
	delete startingThread; delete startingPCB;
	allThreadsPCBs = 0;
	idleThread = 0;
	startingThread = 0; startingPCB = 0;
}

/*
 * timers interrupt routine
 */
void interrupt System::timer(){
	unsigned int realInterrupt = 0;

	//we need to call tick() if it is a real time interrupt and lower time counter
	if (context_switch_on_demand == 0) {
		timeCnt--;
		if(timeCnt < 0) timeCnt = 0;
		tick();
		realInterrupt = 1;
	}

	//running threads execute time with quantum==0 is unlimited
	if(((timeCnt == 0) && (runningPCB->quantum != 0)) || (context_switch_on_demand == 1)){

		if (lck == 0) {//we can change context if we're not in critical section
			context_switch_on_demand=0;
			#ifndef BCC_BLOCK_IGNORE
			asm {
				// save sp, ss, bp
				mov tsp, sp
				mov tss, ss
				mov tbp, bp
			}
			#endif

			runningThread->myPCB->sp = tsp;
			runningThread->myPCB->ss = tss;
			runningThread->myPCB->bp = tbp;

			//we have to put running thread in scheduler
			//we must not put IDLE THREAD  or finished thread
			if((runningThread->myPCB->getThreadId() != idleThread->getId()) &&
					(runningThread->myPCB->getState() == PCB::RUNNING)){
				runningThread->myPCB->setState(PCB::READY);
				Scheduler::put(runningThread->myPCB);
				//lock();
				//cout << "push u SCHEDULER-" << runningThread->getId() << endl;
				//unlock();
			}else{
				if(runningThread->myPCB->getState() != PCB::FINISHED)
					runningThread->myPCB->setState(PCB::READY);
			}

			PCB* newt  = Scheduler::get();

			if(newt == 0){
				//we dont have any thread to execute so we need to run idle thread
					runningThread = idleThread;
					runningPCB = runningThread->myPCB;
			} else {
				runningPCB = newt;
				runningThread = runningPCB->myThread;
			}

			runningThread->myPCB->setState(PCB::RUNNING);

			tsp = runningThread->myPCB->sp;
			tss = runningThread->myPCB->ss;
			tbp = runningThread->myPCB->bp;

			timeCnt = runningThread->myPCB->quantum;
			#ifndef BCC_BLOCK_IGNORE
			asm {
				// restore sp,ss,bp
				mov sp, tsp
				mov ss, tss
				mov bp, tbp
			}
			#endif

		} else { //if we are in critical section-change context later
			context_switch_on_demand = 1;
		}


	}

	//old timer routine is at 60h
	//we need to execute that routine when timer interrupet really happend
	//timer interrupt really happend if we are in this IR and context_switch_on_demand==0
	#ifndef BCC_BLOCK_IGNORE
		if(realInterrupt == 1) {
			asm int 60h;
		}
	#endif


}

void System::dispatch(){

	context_switch_on_demand = 1;

#ifndef BCC_BLOCK_IGNORE
		asm int 8h
#endif
}

