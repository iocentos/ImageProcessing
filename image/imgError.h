#ifndef _IMG_ERROR_H_
#define _IMG_ERROR_H_

#include <exception>
#include <stdexcept>
#include <new>


namespace IMAGE {


/*classes to use as exception objects */


	class bad_alloc : public std::runtime_error {
	public:
		explicit bad_alloc( const std::string& arg = "" ) : std::runtime_error( arg ) {}
	};


	class image_format_error : public std::logic_error {
	public:
		explicit image_format_error( const std::string& arg = "" ) : std::logic_error( arg ) {}
	};


	class not_supported_format : public std::runtime_error {
	public:
		explicit not_supported_format( const std::string& arg = "" ) : std::runtime_error( arg ) {}
	};

	
	class invalid_argument : public std::invalid_argument {
	public:
		explicit invalid_argument( const std::string& arg = "" ) : std::invalid_argument( arg ) {}
	};

	
	class file_io_failed : public std::runtime_error {
	public:
		explicit file_io_failed( const std::string& arg = "" ) : std::runtime_error( arg ) {}
	};


	class empty_raster : public std::logic_error {
	public:
		explicit empty_raster( const std::string& arg = "" ) : std::logic_error( arg ) {}
	};


	class empty_image : public std::logic_error {
        public:
                explicit empty_image( const std::string& arg = "" ) : std::logic_error( arg ) {}
	};	

}//end of namespace IMAGE







#endif





























