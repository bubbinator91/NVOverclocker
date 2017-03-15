#ifndef INITEXCEPTION_H
#define INITEXCEPTION_H

#include <exception>

class InitException : public std::exception
{
	virtual const char * what() const throw()
	{
		return "Could not initialize the library";
	}
};

#endif