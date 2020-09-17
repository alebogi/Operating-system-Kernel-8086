/*
 * kerEv.h
 *
 *  Created on: Sep 16, 2020
 *      Author: OS1
 */

#ifndef KEREV_H_
#define KEREV_H_

#include "event.h"
#include "system.h"

typedef unsigned char IVTNo;

class KernelEv{
public:
	KernelEv(IVTNo ivtNo);
	~KernelEv();

	void wait();
	void signal();

	PCB* getOwner();
	int getValue();

	static KernelEv* events[256];
protected:
	friend class PCB;
	friend class Thread;
	friend class System;
	friend class IVTEntry;
	friend class Event;
private:
	PCB* owner;
	IVTNo entryIVTNo;
	int value;
	int ownerBlocked; //0==not blocked, 1==blocked
};



#endif /* KEREV_H_ */
