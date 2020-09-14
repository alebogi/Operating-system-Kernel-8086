/*
 * queue.cpp
 *
 *  Created on: Sep 7, 2020
 *      Author: OS1
 */


#include <iostream.h>
#include <stdlib.h>
#include "queue.h"
#include "lock.h"


 /**
  * Tests whether container is empty.
  */
int Queue::isEmpty(){
	 if(cnt==0)
		 return 1;
	 return 0;
 }

 /**
  * Returns number of elements in queue.
  */
 int Queue::size() const{
	 return cnt;
 }

 /**
  * Returns "the oldest" element.
  */
 PCB* Queue::front() const{
	 if (cnt == 0)
		 return 0;
	 return frontPtr->pcb;
 }

 /**
  * Returns "the newest" element.
  */
 PCB* Queue::back() const{
	 if (cnt == 0)
	 		 return 0;
	 return backPtr->pcb;
 }


 /**
  * Adds element in queue in FIFO order (push back)
  */
 void Queue::push(PCB* p){
	 lock();
	 Node* toAdd = new Node(p);
	 if(backPtr == 0){
	 	frontPtr = backPtr = toAdd;
	 } else {
	 	backPtr->next = toAdd;
	 	backPtr = backPtr->next;
	 }
	 cnt++;
	 unlock();

 }

 /**
  * Returns element from queue in FIFO order (pop front) and removes it from queue.
  */
 PCB* Queue::pop(){
	 lock();
	 if(cnt==0)
		 return 0;
	 Node* retNode = frontPtr;
	 frontPtr = frontPtr->next;
	 if(frontPtr == 0)
	 	backPtr = 0;
	 PCB* ret = retNode->pcb;
	 cnt--;
	 retNode->pcb = 0;
	 delete retNode;
	 unlock();

	 return ret;
 }

 /**
  * Returns element with id given in arg. If there is none, returns 0
  */
 PCB* Queue::getThreadsPCBbyId(int id){
	 Node* cur = frontPtr;
	 while((cur) && (cur->pcb->getThreadId() != id)){
		 cur = cur->next;
	 }
	 if(cur){
		 return cur->pcb;
	 }else
		 return 0;
 }

 /**
   * Pop element with id given in arg. If there is none, returns 0
   */
 PCB* Queue::popThreadsPCBbyId(int id){
	 lock();
	 Node* prev = 0;
	 Node* cur = frontPtr;
	 Node* ret = 0;
	 while(cur){
		 if (cur->pcb->getThreadId() == id){
			 cnt--;
			 ret = cur;
			 if(prev){
				 prev->next = cur->next;
				 cur->next = 0;
				 if(cur == backPtr){
					 backPtr = prev;
				 }
			 }else{
				 frontPtr = frontPtr->next;
				 if(frontPtr == 0)
					 backPtr = 0;
			 }
			 break;
		 }
		 prev = cur;
		 cur = cur->next;
	 }
	 unlock();

	 return ret->pcb;
 }

 Queue::~Queue(){
	 Node* cur = frontPtr;
	 Node* old = 0;
	 while(cur){
		 old = cur;
		 cur = cur->next;
		 delete cur;
	 }
	 frontPtr = backPtr = 0;
	 cnt = 0;
 }

