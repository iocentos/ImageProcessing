#ifndef _SERIAL_IMAGE_PROCESSING_H_
#define _SERIAL_IMAGE_PROCESSING_H_


namespace IMAGE {

	class ImageRaster;

namespace PROCESS {


void reverseColor( const IMAGE::ImageRaster& );

void adjustBrightness( const IMAGE::ImageRaster& , const signed short );

void adjustContrast( const IMAGE::ImageRaster& , const unsigned char , const unsigned char );

void convertRGB2GREY( const IMAGE::ImageRaster& );

void converGREY2RGB( const IMAGE::ImageRaster& );

void convertRGB2BW( const IMAGE::ImageRaster& );

void convertRGB2SEPIA( const IMAGE::ImageRaster& );

int rotateImage( IMAGE::ImageRaster& );

void zoomImage( IMAGE::ImageRaster& , unsigned int , unsigned int , unsigned int , unsigned int );

void scaleImage( IMAGE::ImageRaster&  , double );

void scaleImage( IMAGE::ImageRaster&  , unsigned int , unsigned int );

void blurImage( IMAGE::ImageRaster& , unsigned int );

}//end of namespace PROCESS

}//end of namespace PROCESS


#endif
