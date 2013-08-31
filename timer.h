#ifndef _TIMER_H_
#define _TIMER_H_

#include <iostream>
#include <sys/times.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>


class Timer {

	struct timeval rS , rE;
	//clock_t rS , rE;
	struct tms uS , uE;
	double real , user , sys;


public:
	void start();
	void stop();
	void printTimes();
	double getReal() { return real; }
	double getUser() { return user; }
	double getSys()  { return sys; }

};


#endif
