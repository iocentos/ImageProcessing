#include "imageProcessing.h"
#include <string.h>
#include <iostream>
#include <omp.h>
//#include <cilk/cilk.h>
//#include <cilk/cilk_api.h>

/*#ifdef SERIAL
	#include <cilk/cilk_stub.h>
#endif
*/



namespace IMAGE {


namespace PROCESS {



const std::string ROTATE_LEFT = "-left";
const std::string ROTATE_RIGHT = "-right";
const std::string ROTATE_180 = "-reverse";





/////////////////////////////////////////////////              Static helper functions                ////////////////////////////////////////////////

static inline unsigned int getComponents( unsigned char* add ) {

	return (unsigned int) ((((unsigned int) add[0] ) | (unsigned int)(add[1]<<8) | (unsigned int)(add[2]<<16) ) & 0x00ffffff );
}


	//return (unsigned int) 0x00ffffff & ((add[0]) | (unsigned int)(add[1]<<8) | (unsigned int)(add[2]<<16) );


static inline void setComponents( unsigned char* add , unsigned int color ) {
	add[0] = (unsigned char)(0x000000ff & color);
	add[1] = (unsigned char)((0x0000ff00 & color )>>8 );
	add[2] = (unsigned char)((0x00ff0000 & color )>>16 );
}





static void rotate180( IMAGE::ImageRaster& raster ) {

        IMAGE::ImageRaster tempRaster;
        unsigned int width = raster.getWidth();
        unsigned int height = raster.getHeight();
        unsigned int samples_per_pixel = raster.getSamplesPerPixel();

        tempRaster.createRaster( width , height , samples_per_pixel );

        unsigned char* to = tempRaster.getRasterPointer();
        unsigned char* from = raster.getRasterPointer();



        //copy first row from raster to last row from new raster
	#pragma omp parallel for 
	for( unsigned int firstRow = 0 ; firstRow < height ; firstRow++  ){
                unsigned int lastRow = height - firstRow - 1;
               //1st pixel from 1st row from original raster is placed in last pixel in last row in the new raster
                for( unsigned int firstColumn = 0 ,  lastColumn = width * samples_per_pixel - samples_per_pixel ; firstColumn < width * samples_per_pixel && lastColumn >= 0 ; firstColumn += samples_per_pixel , lastColumn -= samples_per_pixel ) {


                        //assign each component of every pixel
                        for( unsigned int i = 0 ; i < samples_per_pixel ; i++ )
                                to[width * samples_per_pixel * lastRow + lastColumn + i] = from[width * samples_per_pixel * firstRow + firstColumn +i];

                }

        }
                raster.attachRaster( tempRaster );

}


static void rotateLeft( IMAGE::ImageRaster& raster ) {

        unsigned int width = raster.getWidth();
        unsigned int height = raster.getHeight();
        unsigned int samples_per_pixel = raster.getSamplesPerPixel();

        IMAGE::ImageRaster tempRaster;
        tempRaster.createRaster( height , width , samples_per_pixel );

        unsigned char* to = tempRaster.getRasterPointer();
        unsigned char* from = raster.getRasterPointer();

	#pragma omp parallel for 
        for( unsigned int row = 0  ; row < height ; row++  ){

                unsigned int tempColumn = height - row;
                for( unsigned int column = 0 , tempRow = tempColumn  * samples_per_pixel - samples_per_pixel ; column < width * samples_per_pixel && tempColumn < width * tempColumn * samples_per_pixel ; column += samples_per_pixel , tempRow += height * samples_per_pixel )
                        for( unsigned int i = 0 ; i < samples_per_pixel ; i++ )
                                to[tempRow + i] = from[width * samples_per_pixel * row + column + i];

        }



        raster.attachRaster( tempRaster );

}




static void rotateRight( IMAGE::ImageRaster& raster ) {

        unsigned int width = raster.getWidth();
        unsigned int height = raster.getHeight();
        unsigned int samples_per_pixel = raster.getSamplesPerPixel();

        IMAGE::ImageRaster tempRaster;
        tempRaster.createRaster( height , width , samples_per_pixel );

        unsigned char* to = tempRaster.getRasterPointer();
        unsigned char* from = raster.getRasterPointer();

	#pragma omp parallel for 
        for( unsigned int tempRow = 0 ; tempRow < width ; tempRow++ ){

                unsigned int column = width - tempRow;
                for( unsigned int tempColumn = 0 , row = column * samples_per_pixel - samples_per_pixel ; tempColumn < height * samples_per_pixel && row < height*width*samples_per_pixel - (tempRow*samples_per_pixel) ; tempColumn += samples_per_pixel , row += width * samples_per_pixel )
                        for( unsigned int i = 0 ; i < samples_per_pixel ; i++ )
                                to[height * tempRow * samples_per_pixel + tempColumn + i] = from[row + i];


        }

        raster.attachRaster( tempRaster );

}




static void ScaleLine( unsigned char* target , unsigned char* source , unsigned int srcWidth , unsigned int trgWidth   , unsigned int samples ) {

        unsigned int intPart = srcWidth / trgWidth;
        unsigned int fraqPart = srcWidth % trgWidth;
        unsigned int E = 0;

        for( unsigned int i = 0 ; i < trgWidth ; i++ ) {
                for( unsigned int j = 0 ; j < samples ; j++ )
                        *target++ = source[j];

                source += intPart * samples;
                E += fraqPart;
                if( E >= trgWidth ){
                        E -= trgWidth;
                        source += samples;
                }
        }



}



////////////////////////////////////////////      Public functions          ///////////////////////////////////////////////////






///////////////////////////////////////////////////////////////ReverseColor//////////////////////////////////////////////////////////

/*Reverse color.Reverses every component of every pixel
form its initial value assuming that the components are RGBor RGBA*/
void reverseColor( const IMAGE::ImageRaster& raster ) {

	
	#pragma omp parallel for
	for( unsigned int i = 0 ; i < raster.total_pixels_m * raster.samples_per_pixel_m ; i += raster.samples_per_pixel_m ) {

		raster.raster_m[i] = ~raster.raster_m[i];
		raster.raster_m[i+1] = ~raster.raster_m[i+1];
		raster.raster_m[i+2] = ~raster.raster_m[i+2];
	}
}


	////////////////////////////////////////////////////////////AdjustBrightness///////////////////////////////////////////////////////////
/*Adust Brightness.Increase or decrease brightness.
Arguments ImageRaster and integer for percentage adjustment of brightness -100% < factor <= 100%
from every component of every pixel.*/

void adjustBrightness( const IMAGE::ImageRaster & raster , const signed short factor ) {

        signed short brightness = factor;

        if( brightness > 100 )  brightness = 100;
        else if( brightness < -100 ) brightness = -100;

        brightness = (brightness * 128 ) / 100;

	#pragma omp parallel for
        for( unsigned int i = 0 ; i < raster.total_pixels_m * raster.samples_per_pixel_m ; i += raster.samples_per_pixel_m ) {

                //increace brightness
                if( brightness > 0 ) {
                        ( raster.raster_m[i] + brightness > 255 ) ? raster.raster_m[i] = 255 : raster.raster_m[i] += brightness;
                        ( raster.raster_m[i+1] + brightness > 255 ) ? raster.raster_m[i+1] = 255 : raster.raster_m[i+1] += brightness;
                        (raster.raster_m[i+2] + brightness > 255 ) ? raster.raster_m[i+2] = 255 : raster.raster_m[i+2] += brightness ;
                }
                //decrease brightness
                else {
                        unsigned char temp = (unsigned char) (-brightness );
                        ( raster.raster_m[i] - temp <= 0 ) ? raster.raster_m[i] = 0 : raster.raster_m[i] -= temp;
                        ( raster.raster_m[i+1] - temp <= 0 ) ? raster.raster_m[i+1] = 0 : raster.raster_m[i+1] -= temp;
                        ( raster.raster_m[i+2] - temp <= 0 ) ? raster.raster_m[i+2] = 0 : raster.raster_m[i+2] -= temp;
                }
        }
}



	///////////////////////////////////////////////////////////AdjustContrast////////////////////////////////////////////////////////////////

/*Adjust Contrast.
Arguments ImageRaster , int lowLimit , int highLimit.
Makes 0 every component that is lower than lowLimit and makes 255 every 
component that higher than the highLimit.*/

void adjustContrast ( const IMAGE::ImageRaster& raster , const unsigned char lowLimit , const unsigned char highLimit ) {

	#pragma omp parallel for
        for( unsigned int i = 0 ; i < raster.total_pixels_m * raster.samples_per_pixel_m ; i += raster.samples_per_pixel_m ) {

                if( raster.raster_m[i] < lowLimit )     raster.raster_m[i] = 0;
                else if ( raster.raster_m[i] > highLimit )      raster.raster_m[i] = 255;

                if( raster.raster_m[i+1] < lowLimit )   raster.raster_m[i+1] = 0;
                else if( raster.raster_m[i+1] > highLimit )     raster.raster_m[i+1] = 255;

                if( raster.raster_m[i+2] < lowLimit )   raster.raster_m[i+2] = 0;
                else if( raster.raster_m[i+2] > highLimit )      raster.raster_m[i+2] = 255;
        }

}



void adjustContrast( const IMAGE::ImageRaster& raster , const unsigned short percent ) {

        unsigned short contrast = percent;
        if( contrast > 100 ) contrast = 100;

        contrast = ( contrast * 128 ) / 100;

        adjustContrast( raster , contrast , 255 - contrast );
}




////////////////////////////////////////////////////Ratation////////////////////////////////////////////////////////////////
int rotateImage( IMAGE::ImageRaster& raster , const std::string rotation ) {

        if( rotation == ROTATE_RIGHT )
                rotateRight( raster );
        else if( rotation == ROTATE_LEFT )
                rotateLeft( raster );
        else if( rotation == ROTATE_180 )
                rotate180( raster );
        else
                return 0;

        return 1;
}



//////////////////////////////////////////////////////////        ZOOM        ////////////////////////////////////////
/*Stretches a specified rectangular in the image to initials dimensions of the image*/

void zoomImage( IMAGE::ImageRaster& raster , unsigned int x_start , unsigned int y_start , unsigned int x_end , unsigned int y_end ) {

        IMAGE::ImageRaster tempRaster;
        tempRaster.createRaster( raster.width_m , raster.height_m , raster.samples_per_pixel_m );

        double x_range = (double)x_end - x_start;
        double y_range = (double)y_end - y_start;

        double x_ratio = x_range / raster.width_m;
        double y_ratio = y_range / raster.height_m;


        unsigned int step = raster.width_m * raster.samples_per_pixel_m;
        //move pointer of raster to point( x_start , y_start )
        unsigned char* source = raster.raster_m;
        unsigned char* dest = tempRaster.raster_m;
        source += ( y_start * step ) + ( x_start * raster.samples_per_pixel_m );

	//which row in destination image to writ
        unsigned int source_y ;
        unsigned char* src ;


	#pragma omp parallel for private( source_y , src )
        for( unsigned int dest_y = 0 ; dest_y < raster.height_m ; dest_y++ ) {
                //which row in destination image to write
                source_y = (unsigned int)(dest_y * y_ratio);
                src = source + source_y * step;

                for( unsigned int dest_x = 0 ; dest_x < raster.width_m ; dest_x++ ) {

                        unsigned int offset = (unsigned int)(dest_x * x_ratio) ;
                        for( unsigned int i = 0 ; i < raster.samples_per_pixel_m ; i++ )
                                tempRaster.raster_m[dest_y * step + dest_x * raster.samples_per_pixel_m + i] = src[offset * raster.samples_per_pixel_m + i];

                }

        }


        tempRaster.raster_m = dest;
        raster.attachRaster( tempRaster );
}


void zoomImage( IMAGE::ImageRaster& raster , unsigned int factor ) {


	if( factor > 0 && factor <= 100 ){
		factor /= 10;
		factor = 10 - factor;
		//factor /= 100/factor;
		zoomImage( raster , raster.width_m / factor , raster.height_m / factor , raster.width_m - raster.width_m / factor , raster.height_m - raster.height_m / factor );
	}

}




        //////////////////////////////////////             Scale Image        //////////////////////////////////////////

void scaleImage( IMAGE::ImageRaster& raster , unsigned int target_width , unsigned int target_height ) {

        IMAGE::ImageRaster tempRaster;
        tempRaster.createRaster( target_width , target_height , raster.samples_per_pixel_m );
        double factor =  (double)raster.height_m / target_height  ;
        unsigned int intPart = ( raster.height_m / tempRaster.height_m ) * raster.width_m;
        unsigned int fraqPart = raster.height_m % tempRaster.height_m;

	unsigned int E = 0;
	unsigned char* prevSource = NULL;
	unsigned char* source = NULL;
	unsigned char* destination = NULL;




#pragma omp parallel   shared( factor , intPart , fraqPart )  firstprivate( E , prevSource , source , destination )
{
//#pragma omp for schedule( static , 1 ) nowait
//for( unsigned int j = 0 ; j < target_height  ; j += target_height / 4 ) {
	unsigned int workers = omp_get_num_threads();	
       // unsigned int E = 0;
	unsigned int j = omp_get_thread_num() * (target_height/workers);
        //unsigned char* prevSource = NULL;
        source = &raster.raster_m[((unsigned int)(j * factor) * raster.width_m * raster.samples_per_pixel_m)];
        destination = &tempRaster.raster_m[j * tempRaster.width_m * raster.samples_per_pixel_m];

        unsigned int iterations = target_height / workers;
        if( (j + target_height / workers) > target_height )
                iterations = target_height -  j ;


        for( unsigned int i = 0 ; i < iterations ; i++ ) {

                if( source == prevSource ) {
                        memcpy( destination , destination - tempRaster.width_m * tempRaster.samples_per_pixel_m , tempRaster.width_m * tempRaster.samples_per_pixel_m );
                }
                else {
                        ScaleLine( destination , source , raster.width_m , tempRaster.width_m , raster.samples_per_pixel_m);
                        prevSource = source;
                }

                destination += tempRaster.width_m * tempRaster.samples_per_pixel_m;
                source += intPart * raster.samples_per_pixel_m ;
                E += fraqPart;
                if( E >= tempRaster.height_m  ){

                        E -= tempRaster.height_m;
                        source += raster.width_m * raster.samples_per_pixel_m ;
                }
        }
}


         raster.attachRaster( tempRaster );

}



/*Overloaded scalImage function 
Arguments: ImageRaster as source image and factor to multiply the dimensions of the new image.*/

void scaleImage( IMAGE::ImageRaster& raster , double factor ) {


        scaleImage( raster , raster.width_m * factor , raster.height_m * factor );
}


///////////////////////////////////////////////////////////////////////////////////////
/*Blur image.The smaller the second argument is the less blur the image get.*/

void blurImage( IMAGE::ImageRaster& raster , unsigned int step ) {


        unsigned int factor = step/2;
        unsigned int w = raster.width_m;
        unsigned int s = raster.samples_per_pixel_m;
        IMAGE::ImageRaster temp;
        temp.createRaster( raster.width_m , raster.height_m , raster.samples_per_pixel_m );

        //sum of all the elements in the table step*step
        unsigned int sum = 0;
        //counts the elements that was actually added in the sum
        unsigned int counter = 0;
        //defines the last row that the table step*step can be computed normal
        unsigned int max_row = raster.height_m - factor - 1;
        //defines the last column that the table step*step can be computed normal
        unsigned int max_col = raster.width_m - factor - 1;

	#pragma omp parallel for firstprivate( sum , counter )
        //for each for
        for( unsigned int row = 0 ; row < raster.height_m ; row++ )
                //for each column
                for( unsigned int col = 0 ; col < raster.width_m  ; col++ )
                        //for each color in every pixel
                        for( unsigned int i = 0 ; i < raster.samples_per_pixel_m ; i++ ) {

                                /*if the current pixel is in posiotion that the table step*step can be computed normal r , c , end_r , end_c are using the default values
                                  if the pixel is at the edge of the image this varaibles must be computed in order not to exit the corners of the image
                                */
                                //holds the start row and start column for the table step*step for every pixel
                                signed int r = factor, c = factor;
                                ////holds the end row and end column of the table step*step for every pixel
                                signed int end_r = factor+1 , end_c = factor+1;
                                //if 0 <= pixel < step/2 the the start row and column must change
                                if( row < factor )
                                        r = row;
                                if( col < factor )
                                        c = col;

                                //if pixel height-step/2 < pixel < height then both the start end end column of the step*step stable must change
                                if( row > max_row ){
                                        r = raster.height_m - row;
                                        end_r = r;
                                }
                                if( col > max_col ){
					c = raster.width_m - col;
                                        end_c  = c;
                                }
                                for( signed int p = -r ; p < end_r ; p++ )
                                        for( signed int k = -c ; k < end_c ; k++ ){

                                                counter++;
                                                sum += raster.raster_m[(row+p)*w*s + (col+k)*s +i];
                                        }
                                sum /= counter;
                                temp.raster_m[row*w*s + col*s +i] = (unsigned char)sum;
                                sum = 0;
                                counter = 0;


                        }

        raster.attachRaster( temp );

}









}//end of namespace PROCESS



namespace FILTERS {

        ////////////////////////////////////////////////////////ConvertRGB2Grey//////////////////////////////////////////////////////////
/*Convert RGB image to grey image
Arguments ImageRaster*/


void convertRGB2GREY( const IMAGE::ImageRaster& raster  , const unsigned short percentOfGrey ) {

        double factor = ( percentOfGrey * 255 ) / 100;
        factor = 255 / ( factor - 1 );
	double average;
	unsigned int grey;

	#pragma omp parallel for  private( average , grey )
        for( unsigned int i = 0 ; i < raster.total_pixels_m * raster.samples_per_pixel_m ; i += raster.samples_per_pixel_m ) {

                average = (raster.raster_m[i] + raster.raster_m[i+1] + raster.raster_m[i+2] ) / 3;
                grey = (unsigned int) ((average/factor) + 0.5)*factor;
                raster.raster_m[i] = (unsigned char)(grey);
                raster.raster_m[i+1] = (unsigned char)(grey);
                raster.raster_m[i+2] = (unsigned char)(grey);
        }

}



//////////////////////////////////////////////////////////ConvertRGB2BW/////////////////////////////////////////////////////////
/*convert RGB image to black and white*/

void convertRGB2BW( const IMAGE::ImageRaster& raster ) {

	unsigned int pixel;

	#pragma omp parallel for private( pixel )
        for( unsigned int i = 0 ; i < raster.total_pixels_m * raster.samples_per_pixel_m ; i += raster.samples_per_pixel_m ) {

                pixel = PROCESS::getComponents( &raster.raster_m[i] );
                if( pixel > 0xffffff/2 ) pixel |= 0xffffffff;
                else    pixel &= 0xff000000;

                PROCESS::setComponents( &raster.raster_m[i] , pixel );
        }

}


        ////////////////////////////////////////////////////////ConvertRGB2SEPIA////////////////////////////////////////////////////////////
/*convert RGB image to sepia filter*/

void convertRGB2SEPIA( const IMAGE::ImageRaster& raster ) {

	double factor;

	#pragma omp parallel for private( factor )
        for( unsigned int i = 0 ; i < raster.total_pixels_m * raster.samples_per_pixel_m ; i += raster.samples_per_pixel_m ) {

                //calculating factor for red first
                factor = raster.raster_m[i] * 0.393 + raster.raster_m[i+1] * 0.769 + raster.raster_m[i+2] * 0.189;
                ( factor > 255 ) ? raster.raster_m[i] = 255 : raster.raster_m[i] = factor;

                //green
                factor = raster.raster_m[i] * 0.349 + raster.raster_m[i+1] * 0.686 + raster.raster_m[i+2] * 0.168;
                ( factor > 255 ) ? raster.raster_m[i+1] = 255 : raster.raster_m[i+1] = factor;

                //blue
                factor = raster.raster_m[i] * 0.272 + raster.raster_m[i+1] * 0.534 + raster.raster_m[i+2] * 0.131;
                ( factor > 255 ) ? raster.raster_m[i+2] = 255 : raster.raster_m[i+2] = factor;

        }

}


//////////////////////////////////////////////////////////////////ConvertGREY2RGB////////////////////////////////////////////////
/*Convert Image from greyscale back to RGB*/

void convertGREY2RGB( const IMAGE::ImageRaster& raster ) {

        unsigned int color;


        for( unsigned int i = 0 ; i < raster.total_pixels_m * raster.samples_per_pixel_m ; i += raster.samples_per_pixel_m ) {

                color = PROCESS::getComponents( &raster.raster_m[i] );
                color *= 21;
                PROCESS::setComponents( &raster.raster_m[i] , color );

        }

}



}//end of namespace FILTERS


}//end of namespace IMAGE
