/*
 * event.cpp
 *
 *  Created on: Sep 16, 2020
 *      Author: OS1
 */

#include "event.h"
#include "system.h"
#include "lock.h"
#include "kerEv.h"

Event::Event(IVTNo ivtNo){
	lock();
	myImpl = new KernelEv(ivtNo);
	unlock();
}

Event::~Event(){
	lock();
	delete myImpl;
	myImpl = 0;
	unlock();
}

void Event::wait(){
	myImpl->wait();
}

void Event::signal(){
	myImpl->signal();
}


