#include "config.h"
#include <features.h>

#include "Logger.hpp"

#include <string>
#include <string_view>
#include <exception>
#include <iostream>
#include <fstream>
#include <cstdarg>

using cpplogger::Level;
using cpplogger::Logger;

Logger::Logger (void) {
	this->_stream = &std::cout;
	this->_level = Level::DEBUG;
}

Logger::~Logger (void) {
	this->_stream = &std::cout;
	if (this->_fstream.is_open ())
		this->_fstream.close ();
	this->_filename.clear ();
}

[[nodiscard]] Logger& Logger::getInstance (void) {
	static Logger instance;
	return instance;
}

void Logger::Exception (const Level& level, const std::exception& e) const {
	(void)level;
	(void)e;
}

std::ostream& Logger::getStream (void) {
	return *(this->_stream);
}

Level Logger::getLoglevel (void) {
	return this->_level;
}

const std::string_view Logger::getFilename (void) {
	if (!this->_filename.empty ())
		return this->_filename;
	return {};
}
/*
void Logger::print (const std::string& format, ...) const {
	std::va_list ap;
	std::va_start (ap, format);
	std::vfprintf (this->_stream, format, ap);
	if (this->_split) {
		std::ostream = second_stream;
		if (this->_stream == &std::cout)
			second_stream = &std::cerr;
		else
			second_stream = &std::cout;
		std::vfprintf (second_stream, format, ap);
	}
	std::va_end (ap);
}
*/
bool Logger::setStream_int (std::ostream& stream) {
	this->_stream = &stream;
	if (this->_fstream.is_open ())
		this->_fstream.close ();
	this->_filename.clear ();
	return true;
}

bool Logger::setFile_int (const std::string& file) {
	this->_filename = file;
	this->_fstream = std::ofstream (this->_filename, std::ios_base::out);
	this->_stream = &this->_fstream;
	return true;
}

bool Logger::setLoglevel_int (const Level& level) {
	this->_level = level;
	return (this->_level == level);
}

bool Logger::setSplit_int (const bool& split) {
	this->_split = split;
	return (this->_split == split);
}