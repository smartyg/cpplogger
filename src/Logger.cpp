#include "config.h"

#include "Logger.hpp"

#include <string>
#include <string_view>
#include <exception>
#include <iostream>
#include <fstream>
#include <cstdarg>

using cpplogger::Level;
using cpplogger::Logger;

Logger::Logger (void) noexcept {
	this->_stream = &std::cout;
	this->_level = Level::DEBUG;
}

Logger::~Logger (void) {
	this->_stream = &std::cout;
	if (this->_fstream.is_open ())
		this->_fstream.close ();
	this->_filename.clear ();
}

[[nodiscard]] Logger& Logger::getInstance (void) noexcept {
	static Logger instance;
	return instance;
}

std::ostream& Logger::getStream_int (void) const noexcept {
	return *(this->_stream);
}

Level Logger::getLoglevel_int (void) const noexcept {
	return this->_level;
}

const std::string_view Logger::getFilename_int (void) const noexcept {
	if (!this->_filename.empty ())
		return this->_filename;
	return {};
}

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

bool Logger::setLoglevel_int (const Level& level) noexcept {
	this->_level = level;
	return (this->_level == level);
}

bool Logger::setSplit_int (const bool& split) noexcept {
	this->_split = split;
	return (this->_split == split);
}

bool Logger::setIncludeFunctionNames_int (const bool& func_names) noexcept {
	this->_include_function_names = func_names;
	return (this->_include_function_names == func_names);
}
