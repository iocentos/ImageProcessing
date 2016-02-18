#include "timer.h"

void Timer::start() {

	gettimeofday( &rS , NULL );
	//rS = clock();
	times( &uS );
}

void Timer::stop() {

	gettimeofday( &rE , NULL );
	//rE = clock();
	times( &uE );
	
	real = (( rE.tv_sec*1000000. + rE.tv_usec ) - ( rS.tv_sec*1000000. + rS.tv_usec )) / 1000000.;

	//real = (double)( rE ) / CLOCKS_PER_SEC - (double)(rS)/CLOCKS_PER_SEC;

        user = (( uE.tms_utime + uE.tms_cutime ) - ( uS.tms_utime + uS.tms_cutime )) / (double)sysconf(_SC_CLK_TCK);

        sys = (double)(( uE.tms_stime + uE.tms_cstime ) - ( uS.tms_stime + uS.tms_cstime )) / (double)sysconf(_SC_CLK_TCK);


}

void Timer::printTimes() {


	std::cout<<"Real time  : "<<real<<std::endl;
	std::cout<<"User time  : "<<user<<std::endl;
	std::cout<<"Sys  time  : "<<sys<<std::endl;
}
