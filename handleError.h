#ifndef HANDLEERROR_H_
#define HANDLEERROR_H_

#include <cstdlib>
#include <string>

struct errorInfo {
	int8_t errorCode = 0;
	std::string errorMessage = "";
};

void handleError(errorInfo const& error);

#endif /* HANDLEERROR_H_ */
