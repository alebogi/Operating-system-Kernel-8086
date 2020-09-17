/*
 * ivtEntry.cpp
 *
 *  Created on: Sep 16, 2020
 *      Author: OS1
 */

#include "ivtEntry.h"
#include <dos.h>
#include <iostream.h>
#include "kerEv.h"

IVTEntry* IVTEntry::IVT[256] = { 0 };

IVTEntry::IVTEntry(IVTNo no, pInterruptR newRoutine){
	entryIvtNo = no;
	if ((entryIvtNo >= 0) && (entryIvtNo <= 255)) {
		IVT[entryIvtNo] = this;
		#ifndef BCC_BLOCK_IGNORE
		asm cli;
		//go to IVT, find entry number entryIvtNo, copy address to oldRoutine
		oldRoutine = getvect(entryIvtNo);
		//at that same entry set new address for routine
		setvect(entryIvtNo, newRoutine);
		asm sti;
		#endif
	}
}

IVTEntry::~IVTEntry(){
	if ((entryIvtNo < 0) || (entryIvtNo > 255))
		return;
	#ifndef BCC_BLOCK_IGNORE
	asm cli;
	setvect(entryIvtNo, oldRoutine); //return old routine
	asm sti;
	#endif
	IVT[entryIvtNo] = 0;
}

void IVTEntry::signal(){
	if ((entryIvtNo >= 0) && (entryIvtNo <= 255) && (KernelEv::events[entryIvtNo] != 0))
		KernelEv::events[entryIvtNo]->signal();
}

void IVTEntry::callOld(){
	if ((entryIvtNo >= 0) && (entryIvtNo <= 255) )
		oldRoutine();
}

IVTEntry* IVTEntry::getEntry(IVTNo i){
	if (i >= 0 && i < 256)
		return IVT[i];
	return 0;
}
