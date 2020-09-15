/*
 * kerSem.cpp
 *
 *  Created on: Sep 14, 2020
 *      Author: OS1
 */

#include "kerSem.h"
#include "lock.h"
#include "system.h"
#include "list.h"
#include "queue.h"
#include "SCHEDULE.H"



KernelSem::KernelSem(int init, Semaphore* s){
	mySem = s;
	value = init;
	threadsWaitingTimeSlice = new Queue();
	threadsWaitingValue = new Queue();


	//add to global list of sempahores
	if(System::allSemsInSystem == 0){
			System::allSemsInSystem = new List();
	}
	System::allSemsInSystem->push(this);

}


KernelSem:: ~KernelSem(){
	lock();
	delete threadsWaitingTimeSlice;
	threadsWaitingTimeSlice = 0;
	delete threadsWaitingValue;
	threadsWaitingValue = 0;
	mySem = 0;
	delete mySem;
	unlock();
}

/**
 * Block thread for maxTimeToWait*55ms (if arg>0) or while val<0 (if arg==0)
 * Returns 0 if thread is woke beacuse of tick, 1 in other cases
 */
int KernelSem::wait(Time maxTimeToWait){
	if(maxTimeToWait > 0){ //block thread for maxTimeToWait*55ms
		int ret = 0;
		ret = waitWithTime(maxTimeToWait); //ret 0 if timeSlice == 0
		if (ret == 0){
			return 0;
		}else{
			//its signal(n), n>0 that woke thread
			return 1;
		}
	}else if (maxTimeToWait == 0){ //standard wait
		lock();
		value = value - 1;
		if(value < 0){
			//block thread
			System::runningThread->myPCB->setState(PCB::BLOCKED);
			threadsWaitingValue->push(System::runningThread->myPCB);
			unlock();
			System::dispatch();
		}else{
			unlock();
		}
		return 1;
	}

	return maxTimeToWait; //if maxTimeToWait < 0, not gonna happend?
}

int KernelSem::waitWithTime(Time maxTimeToWait){
	lock();
	System::runningThread->myPCB->setWaitingQuantum(maxTimeToWait);
	System::runningThread->myPCB->setState(PCB::BLOCKED);
	threadsWaitingTimeSlice->push(System::runningThread->myPCB);
	unlock();

	System::dispatch();

	//returns how much time has left, to see if it is signal that woke this thread or tick
	return System::runningThread->myPCB->getWaitingQuantum();
}

/**
 * If n>0, we shoud deblock n threads. If number of blocked threads<n, then unblock as many threads
 * as there is.
 * If n==0, it acts like standard signal
 * If n<0, nothing
 */
int KernelSem::signal(int n){
	PCB* thrToDeblock;
	int numOfUnblockedThreads = 0;
	if(n > 0){ //try to wake n threads
		lock();
		value = value + n;
		//first try to wake n threads from threadsWaitingForValue queue
		for(int i = 0; (i < n) && (threadsWaitingValue->size() > 0); i++){
			thrToDeblock = threadsWaitingValue->pop(); //fifo order
			thrToDeblock->setState(PCB::READY);
			Scheduler::put(thrToDeblock);
			numOfUnblockedThreads++;
		}
		//if we dont have n threads in threadsWaitingForValue queue, look for threads in threadsWaitingForTimeSlice queue
		if(numOfUnblockedThreads < n){
			for(int i = numOfUnblockedThreads; (i < n) && (threadsWaitingTimeSlice->size() > 0); i++){
				thrToDeblock = threadsWaitingTimeSlice->pop(); //fifo order
				thrToDeblock->setState(PCB::READY);
				Scheduler::put(thrToDeblock);
				numOfUnblockedThreads++;
			}
		}
		unlock();
		return numOfUnblockedThreads;
	}else if (n == 0){ //standard signal
		lock();
		value = value + 1;
		if(value <=0){
			//unblock thread
			thrToDeblock = threadsWaitingValue->pop();//fifo order
			thrToDeblock->setState(PCB::READY);
			Scheduler::put(thrToDeblock);
			unlock();
		}else{
			unlock();
		}
		return 0;
	}

	return n; //if n<0 -> ignore
}


/**
 * This method is called when real interrupts occur.
 * Increment timer and see if some threads can be deblocked.
 * Do this for EVERY SEMAPHORE IN SYSTEM!!!
 */
void KernelSem::signalTick(){
	lock();

	PCB* tryToDeblock;
	KernelSem* tempSem;
	int sizeOfQueuePCBs;
	int sizeOfListSems = System::allSemsInSystem->size();
	//for every semaphore in system
	for(int i = 0; i < sizeOfListSems; i++){
		tempSem = System::allSemsInSystem->pop();

		/* Pop element from queue, decrement waiting quantum, if quantum is now 0 --> deblock thread
		 * if quantum is still > 0 --> push thread back to queue.
		 * We do that for every thread, so FIFO order stays same.
		 * */
		sizeOfQueuePCBs = tempSem->threadsWaitingTimeSlice->size();
		for(int j = 0; j < sizeOfQueuePCBs; j++){
			tryToDeblock = tempSem->threadsWaitingTimeSlice->pop();
			tryToDeblock->decrementWaitingQuantum();
			if(tryToDeblock->getWaitingQuantum() == 0){
				//deblock
				tryToDeblock->setState(PCB::READY);
				Scheduler::put(tryToDeblock);
			}else{
				//return to queue
				tempSem->threadsWaitingTimeSlice->push(tryToDeblock);
			}
		}


		System::allSemsInSystem->push(tempSem);
	}

	unlock();
}

int KernelSem::val() const{
	return value;
}


