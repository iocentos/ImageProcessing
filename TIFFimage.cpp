#include <iostream>
#include <string>
#include "TIFFimage.h"
#include "imageRaster.h"



namespace IMAGE {


#ifdef DEBUG
	AutoCounter<TIFFimage> TIFFimage::counter;
#endif


//public

TIFFimage::TIFFimage() throw() : imageFile_m(NULL)  {
#ifdef DEBUG
	counter.increase();
#endif

}



TIFFimage::~TIFFimage() throw() {
#ifdef DEBUG
	counter.decrease();
#endif
		
	}



void TIFFimage::open( const std::string& n , const char mode ) throw( IMAGE::file_io_failed ){
	
	imageFile_m = TIFFOpen( n.c_str() , &mode );
	//failed to open
	if( !imageFile_m )
		throw IMAGE::file_io_failed( "Could not open file " + n + " in mode " + mode );	

	//image is open
	unsigned int tempW , tempH , tempS;
	
	TIFFGetField( imageFile_m , TIFFTAG_IMAGEWIDTH , &tempW);
	TIFFGetField( imageFile_m , TIFFTAG_IMAGELENGTH , &tempH);
	tempS = 4;
	this->raster.setWidth( tempW );
	this->raster.setHeight( tempH );
	this->raster.setSamplesPerPixel( tempS );
	
	name_m = n;

}



void  TIFFimage::close() {
	if( imageFile_m )
		TIFFClose( imageFile_m );
}








void TIFFimage::readImageRaster() throw( IMAGE::bad_alloc , IMAGE::image_format_error , IMAGE::empty_image){

	//checking for open image
	if( !imageFile_m )
		throw IMAGE::empty_image( "Tried to read from unopened image " + name_m );


	//create raster failed to allocate enough space
	try {
		this->raster.createRaster(); 
	}
	catch ( IMAGE::bad_alloc& e ){
		throw IMAGE::bad_alloc( "Not enough space for image   " + name_m );
	}

	
	unsigned char* ptr = this->raster.getRasterPointer();

	if( !TIFFReadRGBAImage( imageFile_m , raster.getWidth() , raster.getHeight() , (unsigned int*)ptr , 0 ) )
		throw IMAGE::image_format_error( "TIFF image internal error   " + name_m );


}













void TIFFimage::writeRasterToImage() throw( IMAGE::image_format_error , IMAGE::empty_raster ){
	
	//if there is no raster return
	if( !raster.hasRaster() )
		throw IMAGE::empty_raster("Raster of image " + name_m + " is empty");


	
	//setting up specific TIFF info
	unsigned int temp_width = raster.getWidth();
	unsigned int temp_height = raster.getHeight();
	unsigned int temp_samples = raster.getSamplesPerPixel();
	unsigned char* raster_m = raster.getRasterPointer();

	TIFFSetField( imageFile_m , TIFFTAG_IMAGEWIDTH , temp_width );
	TIFFSetField( imageFile_m , TIFFTAG_IMAGELENGTH , temp_height );		
	TIFFSetField( imageFile_m , TIFFTAG_COMPRESSION , COMPRESSION_LZW );
	TIFFSetField( imageFile_m , TIFFTAG_SAMPLESPERPIXEL , temp_samples );  //samplesPerPixel );
	TIFFSetField( imageFile_m , TIFFTAG_BITSPERSAMPLE , 8 );  //bitspersample
 	TIFFSetField( imageFile_m , TIFFTAG_PHOTOMETRIC , PHOTOMETRIC_RGB  );
	TIFFSetField( imageFile_m , TIFFTAG_PLANARCONFIG , PLANARCONFIG_CONTIG );
	TIFFSetField( imageFile_m , TIFFTAG_ROWSPERSTRIP ,  TIFFDefaultStripSize( imageFile_m , temp_width * 4 ));
	TIFFSetField( imageFile_m , TIFFTAG_ORIENTATION , ORIENTATION_TOPLEFT );


				
	for( unsigned int row = 0 ; row < temp_height ; row++ ) {

		if( TIFFWriteScanline( imageFile_m , &raster_m[(temp_height-row-1) * temp_width * temp_samples] , row , 0 ) < 0 ){
			throw IMAGE::image_format_error("TIFF image internal error   " +name_m);
		} 
			
	}



}






}//end of namespace IMAGE


