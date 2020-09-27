/*
 * pcb.cpp
 *
 *  Created on: Sep 7, 2020
 *      Author: OS1
 */


#include "pcb.h"
#include <iostream.h>
#include "system.h"
#include "dos.h"
#include "lock.h"
#include "SCHEDULE.H"

int PCB::idCnt = -1; //main:-1, idle:0, others: >0

PCB::PCB(Thread* thread, StackSize sz, Time timeSlice){
	lock();

	threadId = idCnt++;
	myThread = thread;
	stackSizeInBytes = sz;
	if(stackSizeInBytes <= 0){
		stackSizeInBytes = 4096;
	}else if(stackSizeInBytes > maxStackSize){
		stackSizeInBytes = maxStackSize;
	}
	realStackSize = stackSizeInBytes / sizeof(unsigned);
	quantum = timeSlice;
	ss = sp = bp = 0; //it will be initialised in create context
	state = NEW;
	threadsWaitingForMe = new Queue();

	createContext();

	//add this PCB to global queue of all threads
	System::allThreadsPCBs->push(this);

	unlock();
}

/**
 * Initialise stack
 * We want to execute RUN method of thread when thread is in RUNNING state.
 * Thats why we push wrapper of run method on stack at CS, IP place.
 */
void PCB::createContext(){
	unsigned PSW, tempCS, tempIP, tempSP, tempSS, tempBP;
	myStack = new unsigned[realStackSize];

	//stack should look like this:
	/*          <- myStack + size
	 * 	PSW   -1
	 * 	CS    -2  => addr of wrapper, segment
	 * 	IP    -3  => addr of wrapper, offset
	 * 	ax,bx,cx,dx,es,ds,si,di
	 * 	BP    -12 <- ss:sp
	 * 	....
	 * 	....
	 * 	       <- myStack
	 * */

	//we have to set I bit to allow interrupts
	PSW = 0x200; // I bit is on 9th position: 0010 0000 0000
	myStack[realStackSize - 1] = PSW;//push PSW on stack

	#ifndef BCC_BLOCK_IGNORE
		tempCS = FP_SEG(PCB::wrapper);
		tempIP = FP_OFF(PCB::wrapper);
		myStack[realStackSize - 2] = tempCS; //push CS
		myStack[realStackSize - 3] = tempIP; //push IP

		//push ax, bx, cx, dx, es, ds, si, di -> 8 regs

		tempSP = FP_OFF(myStack + realStackSize - 12);
		tempSS = FP_SEG(myStack + realStackSize - 12);
		tempBP = tempSP;

		this->sp = tempSP;
		this->ss = tempSS;
		this->bp = tempBP;
	#endif

}

void PCB::wrapper(){
	System::runningThread->run();

	//thread is over
	lock();
	//wake up all threads that are waiting for it to finish
	PCB* cur;
	while(!(System::runningPCB->threadsWaitingForMe->isEmpty())){
		cur = System::runningThread->myPCB->threadsWaitingForMe->pop();
		cur->setState(PCB::READY);
		Scheduler::put(cur);
	}

	System::runningThread->myPCB->setState(PCB::FINISHED);
	((PCB*)System::runningPCB)->setState(PCB::FINISHED);
//cout << "thr " << System::runningThread->getId() << " je finished. " << endl;
	unlock();

	System::dispatch();
}

ID PCB::getThreadId(){
	return threadId;
}

PCB::States PCB::getState(){
	return state;
}

Thread* PCB::getMyThread(){
	return myThread;
}

Time PCB::getQuantum(){
	return quantum;
}

Time PCB::getWaitingQuantum(){
	return waitingQuantum;
}

void PCB::setState(States s){
	state = s;
}

void PCB::setWaitingQuantum(Time maxTimeToWait){
	lock();
	waitingQuantum = maxTimeToWait;
	unlock();
}

void PCB::decrementWaitingQuantum(){
	lock();
	waitingQuantum = waitingQuantum - 1;
	unlock();
}

/**
 * We remove this PCB from global queue of all threads and deallocate memmory.
 * delete stack, threadsWitingForMe
 */
PCB::~PCB(){
	lock();
	//if(threadId > 0)
	//	 System::allThreadsPCBs->popThreadsPCBbyId(threadId);
	delete myStack; myStack = 0;
	delete threadsWaitingForMe; threadsWaitingForMe = 0;
	unlock();
}



