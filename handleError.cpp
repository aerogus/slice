#include <iostream>
#include <cstdlib>

#include "handleError.h"

void handleError(errorInfo const& error)
{
	std::cerr << error.errorMessage << std::endl;
	if ((error.errorCode >= -3) || (error.errorCode <= -6))
	{
		perror(NULL);
	}
	exit(error.errorCode);
}
