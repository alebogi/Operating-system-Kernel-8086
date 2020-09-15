#include "system.h"
#include <iostream.h>
#include <stdlib.h>
#include <stdio.h>
#include "lock.h"
#include "semaphor.h"

/*
 	 Test: Semafori sa spavanjem 4
*/

int t=0;

const int n=15;

Semaphore s(1);

class TestThread : public Thread
{
private:
	Time waitTime;

public:

	TestThread(Time WT): Thread(), waitTime(WT){}
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
	printf("Thread %d waits for %d units of time.\n",getId(),waitTime);
	unlock();
	int r = s.wait(waitTime);
	if(getId()%2)
		s.signal();
	lock();
	printf("Thread %d finished: r = %d\n", getId(),r);
	unlock();
}

void tick()
{
	t++;
		lock();
		printf("%d\n",t);
		unlock();
}

int userMain(int argc, char** argv)
{
	lock();
	printf("Test starts.\n");
	unlock();
	TestThread* t[n];
	int i;
	for(i=0;i<n;i++)
	{
		t[i] = new TestThread(5*(i+1));
		t[i]->start();
	}
	for(i=0;i<n;i++)
	{
		t[i]->waitToComplete();
	}
	delete t;
	lock();
	printf("Test ends.\n");
	unlock();
	return 0;
}


