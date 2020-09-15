#include "system.h"
#include <iostream.h>
#include <stdlib.h>
#include <stdio.h>
#include "semaphor.h"
#include "lock.h"

/*
 	 Test: Semafori sa spavanjem 2
*/

int t=0;

Semaphore s(0);

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
	//s.signal();
	lock();
	printf("Thread %d finished: r = %d\n", getId(),r);
	unlock();
}

void tick(){
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
	TestThread t1(15),t2(10),t3(30);
	t1.start();
	t2.start();
	t3.start();
	s.wait(5);
	int ret = s.signal(2);
	/*s.wait(0);
	s.signal();*/
	t1.waitToComplete(); t2.waitToComplete(); t3.waitToComplete();
	lock();
	printf("Test ends, return value = %d\n", ret);
	unlock();
	return 0;
}



