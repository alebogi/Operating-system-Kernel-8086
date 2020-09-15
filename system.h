/*
 * system.h
 *
 *  Created on: Sep 7, 2020
 *      Author: OS1
 */

#ifndef SYSTEM_H_
#define SYSTEM_H_

/**
 * Class that does initialisation at begining of the program, restore at the end of program,
 * where interrupt routine is defined.
 */

#include "thread.h"
#include "pcb.h"
#include "queue.h"
#include "list.h"
#include "kerSem.h"
#include "semaphor.h"

class List;

class System{
public:
	static volatile PCB* startingPCB;
	static Thread* startingThread;

	static volatile PCB* runningPCB;
	static Thread *runningThread;

	static IdleThread* idleThread;

	static Queue* allThreadsPCBs;

	static List* allSemsInSystem;

	static volatile int timeCnt;

	static int lck;

	static void init();
	static void restore();
	static void interrupt timer();
	static void dispatch();

protected:
	friend class Thread;
	friend class PCB;
	friend class Queue;
	friend class IdleThread;
	friend class KernelSem;
	friend class List;
	friend class Semaphore;
private:

};



#endif /* SYSTEM_H_ */
