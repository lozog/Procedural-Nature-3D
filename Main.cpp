#include "A5.hpp"
#include <iostream>

int main( int argc, char **argv ) 
{
	switch( argc ) {
		case 3:
			std::cout << "Usage: ./A5 [ input-param-file ]" << std::endl;
			return 0;
	} // switch

	CS488Window::launch( argc, argv, new A5(argc, argv), 1024, 768, "Assignment 5" );
	return 0;
}
