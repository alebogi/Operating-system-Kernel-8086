/*
 * queue.h
 *
 *  Created on: Sep 7, 2020
 *      Author: OS1
 */

#ifndef QUEUE_H_
#define QUEUE_H_

/**
 * Class that acts like queue
 * It operates in FIFO context.
 * It is implemented like linked list and it contains PCBs
 */

#include "pcb.h"
#include <iostream.h>

class PCB;

class Queue{
public:
	Queue(){
		 cnt = 0;
		 frontPtr = backPtr = 0;
	}

	int isEmpty();
	int size() const;
	PCB* front() const;
	PCB* back() const;
	void push(PCB*);
	PCB* pop();

	PCB* getThreadsPCBbyId(int id);
	PCB* popThreadsPCBbyId(int id);

	~Queue();
protected:
	friend class PCB;
private:
	struct Node{
			PCB* pcb;
			Node* next;
			Node(PCB* p){
				pcb= p;
				next = 0;
			}
		}*frontPtr, *backPtr;

	int cnt;

};




#endif /* QUEUE_H_ */
