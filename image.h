#ifndef _IMAGE_BASE_H_
#define _IMAGE_BASE_H_

#include <string>
#include "imageRaster.h"
#include "imgError.h"


#ifdef DEBUG
	#include "autoCounter.h"
#endif

/* Class Image is a base abstract class in order to work with 
multiple types of images that derive from this class
*/

//#define RGBA 4
//#define RGB 3



namespace IMAGE {




class Image {
private:
	//private copy-constructor and operator = to avoid passing by value
	Image( const Image& );

	Image& operator=( const Image& );
#ifdef DEBUG
	static ::AutoCounter<Image> imageCounter;
#endif
	
protected:
	//name of the image
	std::string name_m;

	Image() throw();	
public:
	//public raster object for every image object
	ImageRaster raster;
	
	static Image* createInstance( const std::string& ) throw( IMAGE::bad_alloc , IMAGE::not_supported_format );

	//the contructor is used only to set the private members to NULL
//	Image() throw();//doesnt throw any kind of exception
	
	//if the raster is not destroyed explicilty the destuctor must delete it.
	virtual ~Image() throw();

	virtual void open( const std::string& , const char ) = 0;

	virtual void close() = 0;

        virtual void readImageRaster() = 0;

        virtual void writeRasterToImage() = 0;


		
};//end of Image

}//end of namespace IMAGE



#endif
