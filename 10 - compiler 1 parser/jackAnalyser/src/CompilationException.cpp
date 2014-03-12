#include "CompilationException.h"

CompilationException::CompilationException(const char* errorMessage)
    : errorMessage(errorMessage)
{
}

const char* CompilationException::what() const throw()
{
    return errorMessage;
}
