#include <iostream>
#include <string>
#include "image.h"
#include "TIFFimage.h"
#include "JPEGimage.h"
#include "PNGimage.h"

#ifdef DEBUG
	#include "MemCheck.h"
#endif


namespace IMAGE {

	/*Public functions*/
#ifdef DEBUG
AutoCounter<Image> Image::imageCounter;
#endif
	Image::Image() throw() { 

	#ifdef DEBUG
		imageCounter.increase();
	#endif
	}

	Image::~Image() throw (){
	#ifdef DEBUG
		imageCounter.decrease();
	#endif
	}


	Image* Image::createInstance( const std::string& s ) throw( IMAGE::bad_alloc , IMAGE::not_supported_format )  {

		//chekcing if the format image is supported
		
		try {
			if( s.find( ".tiff" ) != std::string::npos )
				return new IMAGE::TIFFimage();
			
			else if( s.find( ".jpeg" ) != std::string::npos || s.find( ".jpg" ) != std::string::npos || s.find(".JPEG") != std::string::npos || s.find(".JPG") != std::string::npos )
				return new IMAGE::JPEGimage();
			
			else if( s.find( ".png" ) != std::string::npos )
				return new IMAGE::PNGimage();
			
			else	//not supported format . throw exception
				throw IMAGE::not_supported_format( "Format of  " + s + "   is not supported");
		}
		catch( std::bad_alloc& e ) {
			throw IMAGE::bad_alloc( "Not enough memory to create image    " + s );
		}
		return NULL;


}











}
