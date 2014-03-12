#ifndef COMPILATIONEXCEPTION_H
#define COMPILATIONEXCEPTION_H

#include <exception>

using namespace std;

class CompilationException : public exception
{

	public:

		CompilationException(const char* errorMessage);

  		const char* what() const throw();

 	private:

  		const char* errorMessage;

};

#endif // COMPILATIONEXCEPTION_H
