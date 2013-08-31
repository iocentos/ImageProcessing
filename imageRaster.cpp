#include "imageRaster.h"
#include <iostream>
#include <string.h>

#ifdef CILK_PARALLEL
	#include <cilk/cilk.h>
	#include <cilk/cilk_api.h>
#endif

#ifdef OPENMP_PARALLEL
	#include <omp.h>
#endif



#ifdef DEBUG
	#include "MemCheck.h"
#endif





namespace IMAGE {

#ifdef DEBUG
	AutoCounter<ImageRaster> ImageRaster::counter;
#endif
	//default contructor 

	ImageRaster::ImageRaster() throw() : width_m(0) , height_m(0) , samples_per_pixel_m(0) , total_pixels_m(0) , raster_m(NULL) , has_raster_m(false) {
	#ifdef DEBUG
		counter.increase();
	#endif
	}
	

	//destructor .if there is a raster created for this object the momory should
	//be free with the destructor or a call to destroyRaster
	ImageRaster::~ImageRaster() throw() {
	#ifdef DEBUG
		counter.decrease();	
	#endif
		//destroy the raster with an explicit call to the destroyRaster	
		destroyRaster();
	}



	void ImageRaster::allocateRaster() throw( IMAGE::bad_alloc ){
		
		//destroy an active raster if exists
		this->destroyRaster();
	
		//if new failed to find enough space throw an IMAGE::bad_alloc
		try {
			raster_m = new unsigned char[total_pixels_m * samples_per_pixel_m];
			has_raster_m = true;
		}
		catch ( std::bad_alloc& e ) {
			has_raster_m = false;
			throw IMAGE::bad_alloc();
		}
	}		

	



	void ImageRaster::copyRaster( const ImageRaster& copy ) throw() {
		
		if( copy.hasRaster() ) {

			width_m = copy.width_m;
	                height_m = copy.height_m;
	                samples_per_pixel_m = copy.samples_per_pixel_m;
	                total_pixels_m = width_m * height_m;
	
		#ifdef SERIAL		
			memcpy( raster_m , copy.raster_m , total_pixels_m * samples_per_pixel_m );
		#endif

	
		#ifdef CILK_PARALLEL
			cilk_for( unsigned int i = 0 ; i < height_m ; i++ )
				memcpy( &raster_m[i * width_m * samples_per_pixel_m] , &copy.raster_m[i* width_m * samples_per_pixel_m] , width_m * samples_per_pixel_m );
		#endif

		#ifdef OPENMP_PARALLEL
			#pragma omp parallel
			#pragma omp for
			for( unsigned int i = 0 ; i < height_m ; i++ )
				memcpy( &raster_m[i * width_m * samples_per_pixel_m] , &copy.raster_m[i * width_m * samples_per_pixel_m] , width_m * samples_per_pixel_m );

		#endif

			has_raster_m = true;
		}
		else  has_raster_m = false;

				
		




	}



	
	/*create area for the new raster.If there is already an active Raster it is destroyed first
	and then the new area is created .*/

	void ImageRaster::createRaster( const ImageRaster& copy ) throw( IMAGE::bad_alloc , IMAGE::empty_raster )  {
		//if the copy raster is empty
		if( !copy.hasRaster() )
                        throw IMAGE::empty_raster();

		//if an exception is thrown from allocateRaster should be handled by a higher try block                
		width_m = copy.width_m;
		height_m = copy.height_m;
		total_pixels_m = copy.total_pixels_m;
		samples_per_pixel_m = copy.samples_per_pixel_m;
		allocateRaster();  
		copyRaster( copy );            

	}


	void ImageRaster::createRaster( const unsigned int w , const unsigned int h , const unsigned int s ) throw( IMAGE::bad_alloc , IMAGE::invalid_argument ) {
		
		//checking for valid arguments
		if(  s < 3 || s > 4 )
			throw IMAGE::invalid_argument("Wrong arguments for creation of raster"); 
	

		width_m = w;
		height_m = h; 
		samples_per_pixel_m = s;
		total_pixels_m = width_m * height_m;
		
		//allocate enough space for total_pixels_m
		allocateRaster();  
			
}
	

	void ImageRaster::createRaster() throw( IMAGE::bad_alloc ){
		//destroy an active raster if exists
		total_pixels_m = width_m * height_m;
		
		allocateRaster(); 
		
		
	}



        void ImageRaster::destroyRaster()  throw() {
		if( has_raster_m ){
			delete [] raster_m;
			has_raster_m = false;
		}
	}	
		

	unsigned char* ImageRaster::getRasterPointer() const {
		if ( has_raster_m )
			return raster_m;
		else return NULL;
	}
		

	
	int ImageRaster::attachRaster( ImageRaster& from ) throw() {
		//non active raster
		if( !from.hasRaster() )
			return 1;

		this->destroyRaster();
		//take a copy from all members
		width_m = from.width_m;
		height_m = from.height_m;
		samples_per_pixel_m = from.samples_per_pixel_m;
		total_pixels_m = from.total_pixels_m;
		raster_m = from.raster_m;
		//make raster active
		has_raster_m = true;

		//reset raster from so that it doesnt point to the same area with this raster
		from.raster_m = NULL;
		from.has_raster_m = false;
		from.width_m = 0;
		from.height_m = 0;
		from.samples_per_pixel_m = 0;
		from.total_pixels_m = 0;

		//return success		
		return 0;

	}

	//change from active to inactive raster
	void ImageRaster::detachRaster() throw() {
		
		this->destroyRaster();
		//has_raster_m = false;
		width_m = 0;
		height_m = 0;
		samples_per_pixel_m = 0;
		total_pixels_m = 0;
	}

		
			
}//end of namespace IMAGE	

