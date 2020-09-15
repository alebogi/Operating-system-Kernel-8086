/*
 * list.cpp
 *
 *  Created on: Sep 14, 2020
 *      Author: OS1
 */

#include <iostream.h>
#include <stdlib.h>
#include "list.h"
#include "lock.h"
#include "system.h"


 /**
  * Tests whether container is empty.
  */
int List::isEmpty(){
	 if(cnt==0)
		 return 1;
	 return 0;
 }

 /**
  * Returns number of elements in queue.
  */
 int List::size() const{
	 return cnt;
 }

 /**
  * Returns "the oldest" element.
  */
 KernelSem* List::head() const{
	 if (cnt == 0)
		 return 0;
	 return headPtr->sem;
 }

 /**
  * Returns "the newest" element.
  */
 KernelSem* List::tail() const{
	 if (cnt == 0)
	 		 return 0;
	 return tailPtr->sem;
 }


 /**
  * Adds element in queue in FIFO order (push back)
  */
 void List::push(KernelSem* s){
	 lock();
	 ListNode* toAdd = new ListNode(s);
	 if(tailPtr == 0){
	 	headPtr = tailPtr = toAdd;
	 } else {
	 	tailPtr->next = toAdd;
	 	tailPtr = tailPtr->next;
	 }
	 cnt++;
	 unlock();

 }

 /**
  * Returns element from queue in FIFO order (pop front) and removes it from queue.
  */
 KernelSem* List::pop(){
	 lock();
	 if(cnt==0)
		 return 0;
	 ListNode* retNode = headPtr;
	 headPtr = headPtr->next;
	 if(headPtr == 0){
		 tailPtr = 0;
	 }
	 KernelSem* ret = retNode->sem;
	 cnt--;
	 retNode->sem = 0;
	 delete retNode;
	 unlock();

	 return ret;
 }

 List::~List(){
	 ListNode* cur = headPtr;
	 ListNode* old = 0;
	 while(cur){
		 old = cur;
		 cur = cur->next;
		 delete cur;
	 }
	 headPtr = tailPtr = 0;
	 cnt = 0;
 }


