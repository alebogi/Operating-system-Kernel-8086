/*
 * test1.cpp
 *
 *  Created on: May 15, 2019
 *      Author: OS1
 */

#include "system.h"
#include <stdio.h>
#include "lock.h"

/*
	Test: cekanje niti
*/

class TestThread : public Thread
{
public:

	TestThread(): Thread() {};
	~TestThread()
	{
		waitToComplete();
	}
protected:

	void run();

};

void TestThread::run()
{
	lock();
	printf("Thread %d: loop1 starts\n", getId());
	unlock();

	for(int i=0;i<32000;i++)
	{
		for (int j = 0; j < 32000; j++);
	}

	lock();
	printf("Thread %d: loop1 ends, dispatch\n",getId());
	unlock();

	System::dispatch();

	lock();
	printf("Thread %d: loop2 starts\n",getId());
	unlock();

	for (int k = 0; k < 20000; k++);

	lock();
	printf("Thread %d: loop2 ends\n",getId());
	unlock();

}

class WaitThread: public Thread
{
private:
	TestThread *t1_,*t2_;

public:
	WaitThread(TestThread *t1, TestThread *t2): Thread()
	{
		t1_ = t1;
		t2_ = t2;
	};

	~WaitThread()
		{
			waitToComplete();
		}

protected:

	void run();
};

void WaitThread::run(){
		lock();
		printf("Starting tests...\n");
		unlock();

		t1_->waitToComplete();
		lock();
		printf("Test 1 completed!\n");
		unlock();

		t2_->waitToComplete();
		lock();
		printf("Test 2 completed!\n");
		unlock();
	}

void tick() {}

int userMain(int argc, char** argv) {
	lock();
	printf("User main starts\n");
	unlock();
	TestThread t1,t2;
	WaitThread w(&t1,&t2);
	t1.start();
	t2.start();
	w.start();
	lock();
	printf("User main ends\n");
	unlock();
	return 16;
}


