/*
 * lock.cpp
 *
 *  Created on: Sep 7, 2020
 *      Author: OS1
 */

#include "lock.h"
#include "system.h"

/**
 * If lock>0, that means system is in critical section and its locked.
 * If locked == 0, system is unlocked.
 */

void lock(){
	System::lck++;
}

void unlock(){
	if(System::lck>0){
		System::lck--;
	}
}


