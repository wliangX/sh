#pragma once
#include <string>
#include <iostream>

#define ICCWaitUserInputQ \
while (true) \
{ \
	std::string strUserInput; \
	std::cin >> strUserInput; \
	if (strUserInput.compare("q") == 0 || strUserInput.compare("quit") == 0) \
		break; \
}
