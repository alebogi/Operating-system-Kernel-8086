/*
 * semaphor.h
 *
 *  Created on: Sep 14, 2020
 *      Author: OS1
 */

#ifndef SEMAPHOR_H_
#define SEMAPHOR_H_


typedef unsigned int Time;

class KernelSem;


/**
 * This class is a wrapper for class KernelSem.
 */
class Semaphore{
public:
	Semaphore(int init=1);
	virtual ~Semaphore();

	virtual int wait(Time maxTimeToWait);
	virtual int signal(int n=0);

	int val() const; //Returns the current value of the semaphore
protected:
	friend class System;
	friend class KernelSem;

private:
	KernelSem* myImpl;
};


#endif /* SEMAPHOR_H_ */
