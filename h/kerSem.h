/*
 * kerSem.h
 *
 *  Created on: Sep 14, 2020
 *      Author: OS1
 */

#ifndef KERSEM_H_
#define KERSEM_H_

#include "semaphor.h"
#include "system.h"


class KernelSem{
public:
	KernelSem(int init=1, Semaphore* s);
	virtual ~KernelSem();

	int wait(Time maxTimeToWait);
	int waitWithTime(Time maxTimeToWait);
	int signal(int n=0);
	static void signalTick();

	int val() const; //Returns the current value of the semaphore
protected:
	friend class Queue;
	friend class System;
	friend class Thread;
	friend class PCB;
private:
	Semaphore* mySem;
	int value; //current value of the semaphore
	Queue* threadsWaitingTimeSlice; //queue of all threads that are blocked on mySem with time slice
	Queue* threadsWaitingValue;//queue of all threads that are blocked on mySem without time slice (value is negative)

};



#endif /* KERSEM_H_ */
