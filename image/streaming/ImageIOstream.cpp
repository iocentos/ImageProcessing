#include "ImageIOstream.h"
#include <stdlib.h>
#include "../../utils/timer.h"
namespace IMAGE {


//constructor
ImageIOstream::ImageIOstream() : readerDone( false ) , writerDone( false ) , processorDone( false ),  time(0)   { 

	readerCount = 0;
	processorCount = 0;
	writerCount = 0;
}

//destructor
ImageIOstream::~ImageIOstream() {}

ImageIOstream::ImageIOstream( std::string rp , std::string wp ) {
	time = 0;
	readerDone = writerDone = processorDone = false;
	readPath = rp;
	writePath = wp;
	readerCount = 0;
	processorCount = 0;
	writerCount = 0;
}


std::string ImageIOstream::getReadPath() const { return readPath; }

void ImageIOstream::setReadPath( std::string s ) { readPath = s; }

std::string ImageIOstream::getWritePath() const { return writePath; }

void ImageIOstream::setWritePath( std::string s ) { writePath = s; }

std::string ImageIOstream::getOperation() const { return operation; }

void ImageIOstream::setOperation( std::string s ) { operation = s; }



/* 
read contents of directory and place the names in vector*/
int ImageIOstream::readDirectoryContents( ) {
        DIR* dp;
        dirent *dirp;
        if(! ( dp = opendir( readPath.c_str()))) {
                std::cout<<"Could not open directory \""<<readPath<<"\"\n";
                exit(4);
        }

        while(( dirp = readdir(dp)) ){
		std::string s = dirp->d_name;
		if( s.find(".jpg") != std::string::npos || s.find(".tiff") != std::string::npos || s.find(".jpeg") != std::string::npos || s.find(".PNG") != std::string::npos || s.find(".png") != std::string::npos ){
        	        inputNames.push(std::string( s ) );
		}
	}
        return 1;

}
////////////////////////////////////////////////////////////////



int ImageIOstream::startStreaming() {

	#pragma omp parallel 
	//#pragma omp sections
	{
		#pragma omp single nowait
		readStreamWorker();
		#pragma omp single nowait
		processStreamWorker();
		#pragma omp single nowait	
		writeStreamWorker();

	}

	return 1;
}




/* reader worker.The worker opens one image a time reads it and puts the raster of that image
to the list for process and then goes to open the next image*/

void ImageIOstream::readStreamWorker() {

	
	#pragma omp critical (OUTPUT)
	std::cout<<"Reader  "<<omp_get_thread_num()<<" worker\n";


	IMAGE::Image* tempImage;
	std::pair<std::string , IMAGE::ImageRaster*> tempPair;
	IMAGE::ImageRaster* tempRaster;
	std::string tempName;
	
	while( !inputNames.empty() ){

		
	
		#pragma omp critical (READ_INPUT)
		{	
			if( !inputNames.empty() ) {
				tempName = inputNames.front();
				inputNames.pop();
			}
		}

	
		try {
			tempImage = IMAGE::Image::createInstance( readPath + tempName );

			try{

				tempImage->open( readPath + tempName , 'r' );
				tempImage->readImageRaster();
				tempRaster = new IMAGE::ImageRaster();
				
				tempRaster->attachRaster( tempImage->raster );
				tempPair.first = tempName;
				tempPair.second = tempRaster;

				#pragma omp critical (STAGE1)
				{
					stage1Queue.push( tempPair );
					readerCount++;
				}	
				
				
			}
			
      	                catch( IMAGE::file_io_failed& e ) {
                      	       std::cout<<e.what()<<"\n";
                        }
                        catch ( IMAGE::image_format_error& e ) {
                               std::cout<<e.what()<<"\n";
                        }
                        catch( IMAGE::empty_image& e ){
                               std::cout<<e.what()<<"\n";
                        }
                        catch ( IMAGE::empty_raster& e ) {
                               std::cout<<e.what()<<"\n";
                        }

			tempImage->close();
			delete tempImage;


		}
		catch( IMAGE::not_supported_format& e ) {
			std::cout<<e.what()<<std::endl;
		}			
		


	}//end of while.The reader finished his work and updates his flag

	readerDone = true;
	#pragma omp critical (OUTPUT)
	std::cout<<"Reader finished with "<<readerCount<<std::endl;

}



//processor worker.Takes the pointers from the read list processes them and pushes them to the 
//write list for the writer
void ImageIOstream::processStreamWorker() {

	#pragma omp critical (OUTPUT)
	std::cout<<"Processor "<<omp_get_thread_num()<<" worker\n";

	std::pair<std::string , IMAGE::ImageRaster*> tempPair;

	
	
	while( !readerDone || !processorDone ) {

		processorDone = false;
		if( !(stage1Queue.size() == 0 )) {
			#pragma omp critical (STAGE1)
			{	
				if( !(stage1Queue.size() == 0 )) {
					tempPair = stage1Queue.front();
					stage1Queue.pop();
					processorCount++;
				} else processorDone = true;
			}
			Timer t;
			t.start();
			IMAGE::PROCESS::adjustBrightness( *tempPair.second , 80 );
			t.stop();
			time += t.getReal();


			#pragma omp critical (STAGE2)
				stage2Queue.push( tempPair );

		}
		else processorDone = true;

	}

	processorDone = true;
	#pragma omp critical (OUTPUT)
	std::cout<<"Processor finished with "<<processorCount<<std::endl;

}




/*the writer worker is responsible for writing the raster to the image and closing the image and destroying the image*/

void ImageIOstream::writeStreamWorker() {

	#pragma omp critical (OUTPUT)
	std::cout<<"Writer "<<omp_get_thread_num()<<" worker\n";


	IMAGE::Image* tempImage;
	std::pair<std::string , IMAGE::ImageRaster*> tempPair;


	while(  !readerDone || !processorDone || !writerDone ) {

		writerDone = false;
		if( !(stage2Queue.size() == 0 )) {

			#pragma omp critical (STAGE2)
			{
				if( !(stage2Queue.size() == 0 ) ){
					tempPair = stage2Queue.front();
					stage2Queue.pop();
					//writerCount++;
				}else writerDone = true;
			}	
		 try {
			tempImage = IMAGE::Image::createInstance( writePath + tempPair.first );			
                        try{
				tempImage->open( writePath + tempPair.first , 'w' );
				tempImage->raster.attachRaster( *tempPair.second );
				tempImage->writeRasterToImage();
                        }

                        catch( IMAGE::file_io_failed& e ) {
                               std::cout<<e.what()<<"\n";
                        }
                        catch ( IMAGE::image_format_error& e ) {
                               std::cout<<e.what()<<"\n";
                        }
                        catch( IMAGE::empty_image& e ){
                               std::cout<<e.what()<<"\n";
                        }
                        catch ( IMAGE::empty_raster& e ) {
                               std::cout<<e.what()<<"\n";
                        }

			tempImage->close();
			delete tempImage;
			delete tempPair.second;
                }
                catch( IMAGE::not_supported_format& e ) {
                        std::cout<<e.what()<<std::endl;
                }
		
	}
	else writerDone = true;


}

	#pragma omp critical (OUTPUT)
	std::cout<<"Writer finished with "<<writerCount<<std::endl;

}


}//end of namespace IMAGE










