/*
 * idleThr.cpp
 *
 *  Created on: Sep 7, 2020
 *      Author: OS1
 */

#include "idleThr.h"
#include <iostream.h>
#include "pcb.h"
#include "lock.h"

/**
 * we have to override this method because we dont want idle thread in scheduler,
 *  we just want to run it
 */
void IdleThread::start(){
	if(myPCB->getState() == PCB::NEW){
			myPCB->setState(PCB::READY);
	}
}

/**
 * Idle thread must not finish
 */
void IdleThread::run(){
//	lock();
//	cout << "brzo trci idle" << endl;
//	unlock();
	while(1) {
		for(int i = 1; i<10000; i++)
			for(int j = 1; j<10000; j++);

	}
}



