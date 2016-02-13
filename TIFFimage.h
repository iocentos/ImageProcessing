#ifndef _TIFF_IMAGE_H_
#define _TIFF_IMAGE_H_

#include <string>
#include "tiffio.h"
#include "image.h"

#ifdef DEBUG
	#include "autoCounter.h"
#endif


namespace IMAGE{ 


/*TIFFimage class inherits from generic image class.TIFFimage should add all the extra 
information and functionality to support the tiff format.
*/

/////////////////////////////////////////////////////////////////////////


class TIFFimage : public Image {
private:
	//private copy-constructor and operator= to avoid passing by value
	TIFFimage( const TIFFimage& ) {}

	TIFFimage& operator= ( const TIFFimage& ) { return *this; }

	//pointer to tiff file
	TIFF* imageFile_m;

#ifdef DEBUG
	static AutoCounter<TIFFimage> counter;
#endif

public:

	TIFFimage() throw();

	~TIFFimage() throw();

	void open( const std::string& , const char ) throw( IMAGE::file_io_failed );

	void close();

	void readImageRaster() throw( IMAGE::bad_alloc , IMAGE::image_format_error , IMAGE::empty_image ) ;

	void writeRasterToImage() throw( IMAGE::image_format_error , IMAGE::empty_raster );


};//end of class TIFFimage

}//end of namespace IMAGE




#endif
