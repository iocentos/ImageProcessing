#ifndef _JPEG_IMAGE_H_
#define _JPEG_IMAGE_H_

#include "../image.h"
#include "../imageRaster.h"
#include <stdio.h>
#include "jpeglib.h"

namespace IMAGE {


class JPEGimage : public Image {
private:
	JPEGimage( const JPEGimage& );

	JPEGimage& operator= ( const JPEGimage& ) { return *this; }

	FILE* imageFile_m;

#ifdef DEBUG
	static AutoCounter<JPEGimage> counter;
#endif


public:
	JPEGimage() throw();

	~JPEGimage() throw();

	void open( const std::string& , const char ) throw( IMAGE::file_io_failed );

	void close();

	void readImageRaster() throw( IMAGE::bad_alloc , IMAGE::empty_image );

	void writeRasterToImage() throw( IMAGE::empty_raster );


};//and of class JPEGimage

}//end of namespace IMAGE




#endif
