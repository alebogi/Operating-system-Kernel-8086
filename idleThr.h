/*
 * idleThr.h
 *
 *  Created on: Sep 7, 2020
 *      Author: OS1
 */

#ifndef IDLETHR_H_
#define IDLETHR_H_

#include "thread.h"

class Thread;

/**
 * If all systems threads are BLOCKED, we still need some thread to run.
 * Thats why we have idle thread.
 */
class IdleThread: public Thread{
public:
	IdleThread():Thread(512, 1){}

	void start();
	void run();
};



#endif /* IDLETHR_H_ */
