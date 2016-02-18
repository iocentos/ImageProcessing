#ifndef _AUTOCOUNTER_H_
#define _AUTOCOUNTER_H_

#include <iostream>
#include <typeinfo>



template<class T>
class AutoCounter {

	int object_counter;

	AutoCounter( const AutoCounter& s ) {}

	AutoCounter& operator =( const AutoCounter& s ) { return *this; }

public:

	AutoCounter() : object_counter(0) {}
	
	void increase() { object_counter++; }
	
	void decrease() { object_counter--; }

	~AutoCounter() {

        std::cout<<"**********************     Cleanup checking for class :  "<<typeid(T).name()<<"     *****************\n";

        if( object_counter == 0 )
                std::cout<<"All objects of class  "<<typeid(T).name()<<"  were destroyed\n";
        else
                std::cout<<object_counter<<"  objects are still alive\n";
        std::cout<<"**********************************************************************************************\n";

}


};



#endif
