#ifndef _PNGIMAGE_H_
#define _PNGIMAGE_H_

#include "image.h"
#include "libpng/include/png.h"

namespace IMAGE {




class PNGimage : public Image {
private:
	//png specific data
	png_byte color_type;

	png_byte bit_depth;
		
	png_structp png_ptr;

	png_infop info_ptr ; 

	int num_of_passes;

	FILE* imageFile_m;

	//private copy constructor and operator =
	PNGimage( const PNGimage& );
	
	PNGimage& operator =( const PNGimage& ) { return *this; }
#ifdef DEBUG
	static AutoCounter<PNGimage> counter;
#endif	
public:
	
	PNGimage() throw();
	
	~PNGimage() throw();

	void open( const std::string& , const char ) throw( IMAGE::image_format_error , IMAGE::file_io_failed );

	void close();

	void readImageRaster() throw( IMAGE::bad_alloc , IMAGE::image_format_error  , IMAGE::empty_image );

	void writeRasterToImage() throw( IMAGE::image_format_error , IMAGE::empty_raster );

}; //and of class PNGimage

}//end of namespace IMAGE


#endif









