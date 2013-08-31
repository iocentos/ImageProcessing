#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <string>
#include "timer.h"
#include "img.h"
#include <dirent.h>
#include <vector>
#include <sstream>
#include <omp.h>
#include "ImageIOstream.h"

#ifdef DEBUG
        #include "MemCheck.h"
#endif

unsigned int MAXIMAGES = 16;











int main( int argc , char** argv ) {

	std::cout<<"start of main...\n";
	std::cout<<"The program is using "<<omp_get_num_procs()<<" threads\n";


	IMAGE::ImageIOstream imgStream( argv[1] , argv[2] );

	Timer t1;
	t1.start();
	
	imgStream.readDirectoryContents();
	imgStream.startStreaming();
	

	
	t1.stop();
	std::cout<<"process time "<<imgStream.time<<"ms\n";
	std::cout<<"total time "<<t1.diff_ms()<<"ms\n";





	std::cout<<"end of main...\n";
	return 0;

}
