#ifndef _IMAGE_RASTER_H_
#define _IMAGE_RASTER_H_


#include "imgError.h"
#ifdef DEBUG
	#include "../utils/autoCounter.h"
#endif
/*Every image object has one ImageRaster and every reference 
to width , height , samples per pixel and the raster pointer
is made through the ImageRaster object.*/

namespace IMAGE {
	//definition for the processing functions to see the class
	class ImageRaster;

	

	namespace PROCESS {

		void reverseColor( const IMAGE::ImageRaster& );

		void adjustBrightness( const IMAGE::ImageRaster& , const signed short );

		void adjustContrast( const IMAGE::ImageRaster& , const unsigned char , const unsigned char );	
	
		void adjustContrast( const IMAGE::ImageRaster& , const unsigned short );

		int rotateImage( IMAGE::ImageRaster&  , const std::string );

		void zoomImage( IMAGE::ImageRaster& , unsigned int , unsigned int , unsigned int , unsigned int );

		void zoomImage( IMAGE::ImageRaster& , unsigned int );

		void scaleImage( IMAGE::ImageRaster& , double );
		
		void scaleImage( IMAGE::ImageRaster& , unsigned int , unsigned int );

		void blurImage( IMAGE::ImageRaster&  , unsigned int );

	}


	namespace FILTERS {

		void convertRGB2GREY( const IMAGE::ImageRaster&   , const unsigned short );

                void convertGREY2RGB( const IMAGE::ImageRaster& );

                void convertRGB2BW( const IMAGE::ImageRaster& );

                void convertRGB2SEPIA( const IMAGE::ImageRaster& );
 	}






	class ImageRaster {
	private:
		//width of image
		unsigned int width_m;
		
		//height of image
		unsigned int height_m;
		
		//number of components per pixel
		unsigned int samples_per_pixel_m;
		
		//total pixels of the image height*width
		unsigned int total_pixels_m;
		
		//pointer to raster of image
		unsigned char* raster_m;

		//bool to check if there is already created a raster for this image
		bool has_raster_m;

		//private copy constructor and operator= to avoid passing by value
		ImageRaster( const ImageRaster& );
	
		ImageRaster& operator= ( const ImageRaster& );

		//called from createRaster functions to allocate enough space
		//for the raster.Returns 0 in success and 1 otherwise
		void allocateRaster() throw( IMAGE::bad_alloc );

		void copyRaster( const ImageRaster&  ) throw();

	#ifdef DEBUG
	static	AutoCounter<ImageRaster> counter;
	#endif

	public:
		
		

		//default constructor only to set the raster pointer to NULL
		ImageRaster() throw();

		//destructor
		~ImageRaster() throw();

		//the only way to have access to the raster pointer
		unsigned char* getRasterPointer() const;

		/*used  only within image object
		setWidth , setHeight , setSamplesPerPIxel must be called before 
		a call to createRaster() */
		void createRaster() throw( IMAGE::bad_alloc );

		/*you have to ask explicitly for ther raster to be created
		Raster is created from an open Image.If there was an old active raster
		it is first destroyed and then the new one is created*/
		void createRaster( const ImageRaster& ) throw( IMAGE::bad_alloc  , IMAGE::empty_raster );

		/*create a new raster with only width , height and samples per pixel
		the area is created but there are is no image
		used to create individual rasters without being part of an image*/
		void createRaster( const unsigned int , const unsigned int , const unsigned int ) throw( IMAGE::bad_alloc , IMAGE::invalid_argument );

		//attach a new raster without copying it.The  other's raster are is destroyed
		//and the new one is attached to raster
		int attachRaster( ImageRaster& ) throw() ;

		/*detach raster area from raster.deallocate area from raster 		
		and set values to 0*/
		void detachRaster() throw() ;


		//you can destroy the raster and set it to initial values
		void destroyRaster() throw();

		// get methods for every private member
		unsigned int getWidth() const { return width_m; };

		unsigned int getHeight()  const { return height_m; }
	
		unsigned int getSamplesPerPixel() const { return samples_per_pixel_m; }
		
		//total pixels are computed inside the object and you cant set this value
		unsigned int getTotalPixels()  const { return total_pixels_m; }


		//chacks if the raster is active has some data or not
		bool hasRaster()  const { return has_raster_m; }
		
		
		//set methods for every private member
		void setWidth( unsigned int w )  { width_m = w; }

		void setHeight( unsigned int h )  { height_m = h; }

		void setSamplesPerPixel( unsigned int s )  { samples_per_pixel_m = s; }


	
		/*make friend all the processing functions declared in imageProcessing.h
		to have access to private members of ImageRaster*/

		friend void PROCESS::reverseColor( const IMAGE::ImageRaster& );

		friend void PROCESS::adjustBrightness( const IMAGE::ImageRaster& , const signed short );
		
		friend void PROCESS::adjustContrast( const IMAGE::ImageRaster& , const unsigned char , const unsigned char );

		friend void PROCESS::adjustContrast( const IMAGE::ImageRaster& , const unsigned short );
		
		friend void FILTERS::convertRGB2GREY( const IMAGE::ImageRaster& , const unsigned short );

		friend void FILTERS::convertGREY2RGB( const IMAGE::ImageRaster& );	
	
		friend void FILTERS::convertRGB2BW( const IMAGE::ImageRaster& );

		friend void FILTERS::convertRGB2SEPIA( const IMAGE::ImageRaster& );

		friend int PROCESS::rotateImage( IMAGE::ImageRaster&  , const std::string );

		friend void PROCESS::zoomImage( IMAGE::ImageRaster& , unsigned int , unsigned int , unsigned int , unsigned int );
		
		friend void PROCESS::zoomImage( IMAGE::ImageRaster& , unsigned int );		

		friend void PROCESS::scaleImage( IMAGE::ImageRaster& , double  );

		friend void PROCESS::scaleImage( IMAGE::ImageRaster& , unsigned int , unsigned int );

		friend void PROCESS::blurImage( IMAGE::ImageRaster& , unsigned int );


	};//end of class ImageRaster

}//end of namespace IMAGE



#endif
