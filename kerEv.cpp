/*
 * kerEv.cpp
 *
 *  Created on: Sep 16, 2020
 *      Author: OS1
 */

#include "kerEv.h"
#include "system.h"
#include "lock.h"
#include "SCHEDULE.H"

KernelEv* KernelEv::events[256] = { 0 };

KernelEv::KernelEv(IVTNo ivtNo){
	entryIVTNo = ivtNo;
	owner = ((PCB*)System::runningPCB);
	ownerBlocked = 0;
	value = 0;

	if ((ivtNo >= 0) && (ivtNo <= 255)) {
		events[entryIVTNo] = this;
	}
}

KernelEv::~KernelEv(){
	if(ownerBlocked){
		owner->setState(PCB::READY);
		Scheduler::put(owner);
		ownerBlocked = 0;
	}
	owner = 0;
}

/**
 * only owner of event can call wait.
 */
void  KernelEv::wait(){
	if (System::runningThread->getId() != owner->getThreadId()){
		return;
	}
	lock();
	if(value == 0){
		ownerBlocked = 1;
		owner->setState(PCB::BLOCKED);
		unlock();
		System::dispatch();
	}else if(value == 1){
		value = 0;
		unlock();
	}

}

void KernelEv::signal(){
	lock();
	if(ownerBlocked){
		owner->setState(PCB::READY);
		Scheduler::put(owner);
		ownerBlocked = 0;
	}else{
		value = 1;
	}
	unlock();
}

PCB* KernelEv::getOwner(){
	return owner;
}

int KernelEv::getValue(){
	return value;
}

