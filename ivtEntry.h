/*
 * ivtEntry.h
 *
 *  Created on: Sep 16, 2020
 *      Author: OS1
 */

#ifndef IVTENTRY_H_
#define IVTENTRY_H_

#include "event.h"
#include <iostream.h>
#include <dos.h>

typedef unsigned char IVTNo;
class KernelEv;
typedef void interrupt (*pInterruptR)(...);


#define PREPAREENTRY(numEntry,flag)\
	void interrupt inter##numEntry(...);\
	IVTEntry ivte##numEntry = IVTEntry(numEntry,inter##numEntry);\
	void interrupt inter##numEntry(...){\
		ivte##numEntry.signal();\
		if (flag == 1)\
			ivte##numEntry.callOld();\
	}


class IVTEntry{
public:
	IVTEntry(IVTNo no, pInterruptR newRoutine);
	~IVTEntry();

	void signal();
	void callOld();

	static IVTEntry* IVT[256];
	static IVTEntry* getEntry(IVTNo i);
protected:
	friend class KernelEv;
private:
	IVTNo entryIvtNo;
	pInterruptR oldRoutine; //stored old interrupt routine
};


#endif /* IVTENTRY_H_ */
