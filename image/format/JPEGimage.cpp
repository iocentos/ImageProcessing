#include "JPEGimage.h"
#include <iostream>
#include <string.h>
#include <stdlib.h>



namespace IMAGE {

#ifdef DEBUG
	AutoCounter<JPEGimage> JPEGimage::counter;
#endif



//public

JPEGimage::JPEGimage() throw() : imageFile_m(NULL) { 
#ifdef DEBUG
	counter.increase();
#endif
}

JPEGimage::~JPEGimage() throw() {
#ifdef DEBUG
	counter.decrease();
#endif

 }


void JPEGimage::open( const std::string& n , const char mode ) throw( IMAGE::file_io_failed ){
	
	name_m = n;
	if( mode == 'r' ) {
		imageFile_m = fopen( name_m.c_str() , "rb" );
		//failed to open
		if ( !imageFile_m )
			throw IMAGE::file_io_failed( "Could not open file " + n + "in mode " + mode);

		struct jpeg_decompress_struct decompressor;
		struct jpeg_error_mgr jerr;

		//start decompression and read all the private data
		decompressor.err = jpeg_std_error( &jerr );
		jpeg_create_decompress( &decompressor );
		jpeg_stdio_src( &decompressor , imageFile_m );
	
		(void) jpeg_read_header( &decompressor , true );	

		raster.setWidth( decompressor.image_width );
		raster.setHeight( decompressor.image_height );
		raster.setSamplesPerPixel( decompressor.num_components );

		jpeg_destroy_decompress( &decompressor );
		

	}

	if( mode == 'w' ) 
		imageFile_m = fopen( name_m.c_str() , "wb" );
	if( !imageFile_m )
		throw IMAGE::file_io_failed( "Could not open file " + n + " in mode " + mode );

}

void JPEGimage::close() {

	if( imageFile_m ) 
		fclose( imageFile_m );
}


void JPEGimage::readImageRaster() throw( IMAGE::bad_alloc , IMAGE::empty_image ) {

	if( !imageFile_m ) 
		throw IMAGE::empty_image( "Tried to read from unopened image " + name_m );	
	//createRaster can throw for a bad_alloc
	try{
		raster.createRaster(); 
	}
	catch( IMAGE::bad_alloc& e ){
		throw IMAGE::bad_alloc("Not enough memory for image   " + name_m );
	}


	imageFile_m = freopen( name_m.c_str() , "rb" , imageFile_m );

//	reopen( 'r' ); //return all the file pointer at the beggining	

	struct jpeg_decompress_struct decompressor;
	struct jpeg_error_mgr jerr;

	unsigned int row_stride;
	unsigned int temp_width= raster.getWidth();
	unsigned int temp_height = raster.getHeight();
	unsigned char* raster_m = raster.getRasterPointer();



	decompressor.err = jpeg_std_error( &jerr );
	jpeg_create_decompress( &decompressor );
	jpeg_stdio_src( &decompressor , imageFile_m );
	jpeg_read_header( &decompressor , true );
	jpeg_start_decompress( &decompressor );

	row_stride = temp_width * decompressor.num_components;

	JSAMPARRAY row_buffer;
	row_buffer = ( *decompressor.mem->alloc_sarray ) (( j_common_ptr )&decompressor , JPOOL_IMAGE , row_stride , 1 ) ;
	
	unsigned int i = temp_height - 1;
	//read each line and copy it to raster_m array
	while( decompressor.output_scanline < temp_height ) {
		
		(void) jpeg_read_scanlines( &decompressor , row_buffer , 1 );
		memcpy( &raster_m[i*row_stride] , *row_buffer , row_stride );
		i--;
	} 	
	

	
	jpeg_finish_decompress( &decompressor );
	jpeg_destroy_decompress( &decompressor );
	


}




void  JPEGimage::writeRasterToImage() throw( IMAGE::empty_raster ){
	
	if( !raster.hasRaster() )
		throw IMAGE::empty_raster("Raster of image " + name_m + " is empty");



	struct jpeg_compress_struct compressor;
	struct jpeg_error_mgr jerr;

	unsigned int row_stride;
	unsigned int temp_width = raster.getWidth();
	unsigned int temp_height = raster.getHeight();
	unsigned int temp_samples = raster.getSamplesPerPixel();
	unsigned char* raster_m = raster.getRasterPointer();	




	compressor.err = jpeg_std_error( &jerr );
	jpeg_create_compress( &compressor );
	jpeg_stdio_dest( &compressor , imageFile_m );

	compressor.image_width = temp_width;
	compressor.image_height = temp_height;
	compressor.input_components = temp_samples;
	compressor.in_color_space = JCS_RGB;
	
	jpeg_set_defaults( &compressor );
	jpeg_start_compress( &compressor , true );


	row_stride = temp_width * temp_samples;


	
	JSAMPROW row_pointer[1];
	unsigned int i = temp_height - 1;
	//write to raster to image line by line from raster_m
	while( compressor.next_scanline < temp_height ) {
	
		row_pointer[0] = &raster_m[i * row_stride] ;
		(void) jpeg_write_scanlines( &compressor , row_pointer , 1 );
		i--;
	}
	

	jpeg_finish_compress( &compressor );
	jpeg_destroy_compress( &compressor );


}








}//end of namespace IMAGE


