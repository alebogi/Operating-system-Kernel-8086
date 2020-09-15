/*
 * list.h
 *
 *  Created on: Sep 14, 2020
 *      Author: OS1
 */

#ifndef LIST_H_
#define LIST_H_

#include "kerSem.h"

class KernelSem;

class List{
public:
	List(){
			 cnt = 0;
			 headPtr = tailPtr = 0;
	}

	int isEmpty();
	int size() const;
	KernelSem* head() const;
	KernelSem* tail() const;
	void push(KernelSem*);
	KernelSem* pop();

	~List();
protected:
	friend class KernelSem;
	friend class System;
private:
	struct ListNode{
			KernelSem* sem;
			ListNode* next;
			ListNode(KernelSem* s){
				sem= s;
				next = 0;
			}
		}*headPtr, *tailPtr;

	int cnt;
};



#endif /* LIST_H_ */
