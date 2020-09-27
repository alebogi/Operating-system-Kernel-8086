/*
 * thread.cpp
 *
 *  Created on: Sep 7, 2020
 *      Author: OS1
 */

#include "thread.h"
#include "system.h"
#include "SCHEDULE.H"
#include "lock.h"
#include "idleThr.h"

Thread::Thread (StackSize stackSize, Time timeSlice){
	lock();
	myPCB = new PCB(this, stackSize, timeSlice);
	unlock();
}

void Thread::start(){
	lock();
	if(myPCB->getState() == PCB::NEW){
		myPCB->setState(PCB::READY);
		Scheduler::put(myPCB);
	}
	unlock();
}

/*
 * Running thread that called waitToComplete becomes blocked.
 * We have to add running thread into queue of threads waiting for it to finish
 */
void Thread::waitToComplete(){
	//lock();
	//cout << "usli u wait to complete za thread " << this->getId() << endl;
	//unlock();

	lock();
	//If thread is allready over - do nothing
	//cout <<"state--------------  " << this->myPCB->getState() << endl;
	if(this->myPCB->getState() == PCB::FINISHED){
		unlock();
		return;
	}

	//if thread called this method over idle thread, we ignore it
	if(this->getId() == System::idleThread->getId()){
		unlock();
		return;
	}

	//if thread called this method over main (starting) thread, we ignore it
	if(this->getId() == System::startingThread->getId()){
		unlock();
		return;
	}

	//if thread called this method over itself, we ignore it
	if(this->getId() == System::runningThread->getId()){
		unlock();
		return;
	}


	System::runningThread->myPCB->setState(PCB::BLOCKED);
	((PCB*)(System::runningPCB))->setState(PCB::BLOCKED);
	myPCB->threadsWaitingForMe->push(System::runningThread->myPCB);
	unlock();

	System::dispatch();
}

Thread::~Thread(){
	//lock();
	waitToComplete();
	lock();
	delete myPCB;
	unlock();
}

ID Thread::getId(){
	return myPCB->getThreadId();
}

ID Thread::getRunningId(){
	return System::runningThread->getId();
}

Thread * Thread::getThreadById(ID id){
	PCB* pcbById = System::allThreadsPCBs->getThreadsPCBbyId(id);
	if (pcbById==0)
		return 0;
	Thread* thrById = pcbById->getMyThread();
	return thrById;
}

void dispatch(){
	System::dispatch();
}


