/*
 * pcb.h
 *
 *  Created on: Sep 7, 2020
 *      Author: OS1
 */

#ifndef PCB_H_
#define PCB_H_

#include "queue.h"
#include "thread.h"

class PCB{
public:
	static enum States {NEW, RUNNING, READY, BLOCKED, FINISHED};

	PCB(Thread* thread, StackSize sz, Time timeSlice);
	void createContext();
	static void wrapper();

	//geters
	ID getThreadId();
	States getState();
	Time getPassedTime();
	Thread* getMyThread();

	//seters
	void setState(States s);

	~PCB();

protected:
	friend class Thread;
	friend class Queue;
	friend class System;
private:
	unsigned sp; //stack pointer reg
	unsigned ss; //stack segment reg
	unsigned bp; //base pointer reg
	StackSize stackSizeInBytes;
	StackSize realStackSize; //size of stack array
	unsigned int *myStack;

	States state;
	ID threadId;
	Thread *myThread;
	Time quantum; //time slice

	Queue *threadsWaitingForMe; // threads that called waitToComplete over this thread

	static ID idCnt;
};



#endif /* PCB_H_ */
