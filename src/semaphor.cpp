/*
 * semaphor.cpp
 *
 *  Created on: Sep 14, 2020
 *      Author: OS1
 */

#include "semaphor.h"
#include "lock.h"
#include "kerSem.h"


Semaphore::Semaphore(int init){
	lock();
	myImpl = new KernelSem(init, this);
	unlock();
}


Semaphore:: ~Semaphore(){
	lock();
	delete myImpl;
	myImpl = 0;
	unlock();
}

int Semaphore::wait(Time maxTimeToWait){
	return myImpl->wait(maxTimeToWait);
}


int Semaphore::signal(int n){
	return myImpl->signal(n);
}

int Semaphore::val() const{
	return myImpl->val();
}


