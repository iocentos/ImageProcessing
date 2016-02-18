#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <string>
#include "utils/timer.h"
#include "image/img.h"
#include <dirent.h>
#include <vector>
#include <sstream>
#include <omp.h>


#ifdef DEBUG
	#include "MemCheck.h"
#endif






/* 
read contents of directory and place the names in vector*/
int GetDirFileNames( const std::string dir , std::vector<std::string>& files ) {
	DIR* dp;
	dirent *dirp;
	if(! ( dp = opendir( dir.c_str()))) {
		std::cout<<"Could not open directory \""<<dir<<"\"\n";
		exit(4);
	}

	while(( dirp = readdir(dp)) ) 	
		files.push_back(std::string( dirp->d_name) );

	return 1;
		
}
////////////////////////////////////////////////////////////////




int main( int argc , char** argv ){ 
#ifdef DEBUG
	MEM_ON();
	TRACE_OFF();
#endif

	//total execution timer
	Timer totalTimer;
	totalTimer.start();


	std::cout<<"****************************************************************"<<std::endl;
	std::cout<<"*            OpenMP execution with "<<omp_get_max_threads()<<" threads                   *"<<std::endl;
	std::cout<<"****************************************************************"<<std::endl;

	std::cout<<"\n\n\n";



	std::vector<std::string> imageName;
	
	

	std::stringstream input(argv[4]);
	double factor;
	input >> factor;


	int succeded = 0;
	int failed = 0;

	std::string operation( argv[3] );
	//parallel code timer
	Timer parallelTimer;


	//how many images to run in parallel.Number of threads created for the program
        unsigned int parallelImages = omp_get_max_threads();
        //counter for total files for parallel iterations
        unsigned int counter = 0;
        //how many iterations to run in parallel
        unsigned int parallelIterations = parallelImages;
	


	if( GetDirFileNames ( argv[1] , imageName ) )
	
	try {
		parallelTimer.start();
		for( std::vector<std::string>::iterator it = imageName.begin() ; it < imageName.end() ; it += parallelImages ){
				
			counter += parallelImages;
                        parallelIterations = parallelImages;
                        if( counter > imageName.size() )
                                parallelIterations =  imageName.size() - ( counter - parallelImages );

			#pragma omp parallel for
			for( unsigned int i = 0 ; i < parallelIterations ; i++ ) {				

			
				std::cout<<(*(it + i))<<"\n";
				IMAGE::Image* oldImage = NULL;
				IMAGE::Image* newImage = NULL;
				std::string oldName  = argv[1] + (*(it + i));
				std::string newName  = argv[2] + (*(it + i));
				try{

					oldImage= IMAGE::Image::createInstance( oldName );
					newImage = IMAGE::Image::createInstance( newName ); 
				
					////////////////////
					try{
						oldImage->open( oldName, 'r' );
						newImage->open( newName , 'w' );
						oldImage->readImageRaster();
						newImage->raster.createRaster( oldImage->raster );
					
						//check which operation to do
						if( operation == REVERSE ){
	                                                IMAGE::PROCESS::reverseColor( newImage->raster );
        	                                }
						else if( operation == BRIGHTNESS ) {
							IMAGE::PROCESS::adjustBrightness( newImage->raster , atoi( argv[4] ));
						}
						else if( operation == CONTRAST ) {
							IMAGE::PROCESS::adjustContrast( newImage->raster , atoi( argv[4] ) );
						}
                        	                else if( operation == RGB2GREY ) {
                                	                IMAGE::FILTERS::convertRGB2GREY( newImage->raster , atoi( argv[4] ));
                                        	}
						else if( operation == RGB2BW ) {
							IMAGE::FILTERS::convertRGB2BW( newImage->raster );
						}
						else if( operation == RGB2SEPIA ) {
							IMAGE::FILTERS::convertRGB2SEPIA( newImage->raster );
						}
						else if( operation == BLUR ) {
							IMAGE::PROCESS::blurImage( newImage->raster , atoi(argv[4]) );
						}
						else if( operation == ROTATE ) {
							IMAGE::PROCESS::rotateImage( newImage->raster , argv[4] ) ;
						}
						else if( operation == ZOOM ) {
							IMAGE::PROCESS::zoomImage( newImage->raster , 1 , 1 , 400 , 300 );
						}
						else if( operation == SCALE ) {
							IMAGE::PROCESS::scaleImage( newImage->raster , factor );
						}
                        	                else {
                                	                std::cout<<"Not a valid operation \n";
                                        	        exit(0);
                                       	 	}





						newImage->writeRasterToImage();
						succeded++;

					}
					catch( IMAGE::file_io_failed& e ) {
						std::cout<<e.what()<<"\n";
						failed++;
					}
					catch ( IMAGE::image_format_error& e ) {
						std::cout<<e.what()<<"\n";
						failed++;
					}		
					catch( IMAGE::empty_image& e ){
						std::cout<<e.what()<<"\n";
						failed++;
					}	
					catch ( IMAGE::empty_raster& e ) {
						std::cout<<e.what()<<"\n";
						failed++;
					}
				

						oldImage->close();
						newImage->close();
						delete oldImage;
						delete newImage;

					
			
				}
			catch( IMAGE::not_supported_format& e ) {
				std::cout<<e.what()<<"\n";
				failed++;
			}
			
		}//end of omp_parallel_for


		}//end of for
		
	}
	catch( IMAGE::bad_alloc& e ){
		std::cout<<e.what()<<std::endl;
		std::cout<<"Exiting program...\n";
		exit(1);

	}
	catch(...){
		std::cout<<"cought unexpected exception...\n";
		std::cout<<"Exitig program...\n";
		exit(1);
	}



        totalTimer.stop();
        parallelTimer.stop();


	//final output         //////////////////////////////////////////////////////////////

        std::cout<<"\n\n\n";
        std::cout<<"*************************************************************"<<std::endl;
        std::cout<<"*                          Results                          *"<<std::endl;

        std::cout<<"* total files      :  "<<imageName.size()<<"                                     *"<<std::endl;
        std::cout<<"* Succeded images  :  "<<succeded<<"                                     *"<<std::endl;
        std::cout<<"* Failed images    :  "<<failed<<"                                     *"<<std::endl;


        std::cout<<"*                                                           *"<<std::endl;
        std::cout<<"* Total execution time             :  "<<totalTimer.getReal()<<" sec          *"<<std::endl;
        std::cout<<"* Parallel partial execution time  :  "<<parallelTimer.getReal()<<" sec          *"<<std::endl;

        std::cout<<"*                                                           *"<<std::endl;
        std::cout<<"*                        END OF PROGRAM                     *"<<std::endl;
        std::cout<<"*************************************************************"<<std::endl;


	///////////////////////////////////////////////////////////////////////////////////////


#ifdef DEBUG
	MEM_OFF();
#endif


	return 0;
}

