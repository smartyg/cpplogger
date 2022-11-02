#include "config.h"
#include <features.h>

#include <Logger.hpp>

int main (void) {
	cpplogger::Logger::setLoglevel (cpplogger::Level::DEBUG + 5);
	cpplogger::Logger::setFile ("/tmp/log");
	cpplogger::Logger::setSplit (true);
	DEBUG_MSG ("{}!\n", "Hello World");
	INFO_MSG ("{}!\n", "Hello World");
	DEBUG_MSG ("{}!\n", "Hello World");
	DEBUG_2_MSG (2, "{}!\n", "Hello World");

	try {
		throw std::runtime_error ("Bye Cruel World!");
	} catch (std::exception& e) {
		EXCEPTION_DEBUG_MSG (e);
	}

	return 0;
}

