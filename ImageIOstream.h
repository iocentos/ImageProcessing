#ifndef IMAGE_IO_STREAM
#define IMAGE_IO_STREAM


#include <iostream>
#include <queue>
#include <omp.h>
#include "image.h"
#include "imageProcessing.h"
#include <dirent.h>
#include <vector>
#include "imageRaster.h"
#include <utility>



namespace IMAGE {




typedef volatile bool Flag;



class ImageIOstream {

	std::queue<std::pair<std::string , IMAGE::ImageRaster*> > stage1Queue , stage2Queue;

	std::queue<std::string> inputNames;

	unsigned int readerCount;
	
	unsigned int processorCount;
		
	unsigned int writerCount;

	//shared flags for the three workers to signal when they finish their work
	Flag readerDone;
	 
	Flag writerDone;
	
	Flag processorDone;

	//private copy-constructor and operator = to avoid passing by value
	ImageIOstream( const ImageIOstream& ) {}

	ImageIOstream& operator =( const ImageIOstream& ) { return *this ;}

	//from which directory to read the images
	std::string readPath;
	
	//to which directory to write the images
	std::string writePath;

	//which operation to perform in the processing unitc
	std::string operation;

	//each stream has 3 worker threads.One reader one writer and one processor
	void readStreamWorker();

	void processStreamWorker();

	void writeStreamWorker();
	
		


public:

	ImageIOstream() ;

	~ImageIOstream() ;
	
 	double time;
	//sets the read and write path
	ImageIOstream( std::string , std::string );

	std::string getReadPath() const ;

	void setReadPath( std::string ) ;

	std::string getWritePath( ) const ;

	void setWritePath( std::string ) ;

	std::string getOperation() const ;

	void setOperation( std::string ) ;

	int readDirectoryContents();

	int startStreaming();

};


}//end of namespace IMAGE


#endif




	




