#ifndef _CPPLOGGER_LOGGER_
#define _CPPLOGGER_LOGGER_

#include <string>
#include <string_view>
#include <iostream>
#include <fstream>
#include <cstdint>
#include <exception>
#include <source_location>
#include <fmt/core.h>
#include <fmt/ostream.h>

#define LOGGER_MSG(level, format, ...) (cpplogger::Logger::get() (level, format, std::source_location::current(), __VA_ARGS__))

#define CRITICAL_MSG(format, ...) (LOGGER_MSG(cpplogger::Level::CRIT, format, __VA_ARGS__))
#define ERROR_MSG(format, ...) (LOGGER_MSG(cpplogger::Level::ERR, format, __VA_ARGS__))
#define WARNING_MSG(format, ...) (LOGGER_MSG(cpplogger::Level::WARNING, format, __VA_ARGS__))
#define NOTICE_MSG(format, ...) (LOGGER_MSG(cpplogger::Level::NOTICE, format, __VA_ARGS__))
#define INFO_MSG(format, ...) (LOGGER_MSG(cpplogger::Level::INFO, format, __VA_ARGS__))

#ifdef _RELEASE
#define DEBUG_MSG(format, ...) (cpplogger::Logger::emptyFunc())
#define DEBUG_2_MSG(l, format, ...) (cpplogger::Logger::emptyFunc())
#else
#define DEBUG_MSG(format, ...) (LOGGER_MSG(cpplogger::Level::DEBUG, format, __VA_ARGS__))
#define DEBUG_2_MSG(l, format, ...) (LOGGER_MSG((static_cast<cpplogger::Level>(cpplogger::Level::DEBUG + l)), format, __VA_ARGS__))
#endif

#define EXCEPTION_MSG(level, exception) (cpplogger::Logger::get() (level, exception, std::source_location::current()))

#define EXCEPTION_CRITICAL_MSG(exception) (EXCEPTION_MSG(cpplogger::Level::CRIT, exception))
#define EXCEPTION_ERROR_MSG(exception) (EXCEPTION_MSG(cpplogger::Level::ERR, exception))
#define EXCEPTION_WARNING_MSG(exception) (EXCEPTION_MSG(cpplogger::Level::WARNING, exception))
#define EXCEPTION_NOTICE_MSG(exception) (EXCEPTION_MSG(cpplogger::Level::NOTICE, exception))
#define EXCEPTION_INFO_MSG(exception) (EXCEPTION_MSG(cpplogger::Level::INFO, exception))

#ifdef _RELEASE
#define EXCEPTION_DEBUG_MSG(exception) (cpplogger::Logger::emptyFunc())
#define EXCEPTION_DEBUG_2_MSG(l, exception) (cpplogger::Logger::emptyFunc())
#else
#define EXCEPTION_DEBUG_MSG(exception) (EXCEPTION_MSG(cpplogger::Level::DEBUG, exception))
#define EXCEPTION_DEBUG_2_MSG(l, exception) (EXCEPTION_MSG((static_cast<cpplogger::Level>(cpplogger::Level::DEBUG + l)), exception))
#endif

namespace cpplogger {
	enum Level : uint8_t {
		EMERG   = 0, // System is unusable
		ALERT   = 1, // Action must be taken immediately
		CRIT    = 2, // Critical conditions
		ERR     = 3, // Error conditions
		WARNING = 4, // Warning conditions
		NOTICE  = 5, // Normal but significant condition
		INFO    = 6, // Informational
		DEBUG   = 7  // Debug-level messages
	};

	class Logger final {
		std::string _filename;
		std::ofstream _fstream;
		std::ostream* _stream;
		Level _level;
		bool _split;
		bool _include_function_names = false;

		constexpr static const std::string_view prefix_release = "{:s}: ";
		constexpr static const std::string_view prefix_logger = "{:s}: {:s}: ";
		constexpr static const std::string_view prefix_debug = "{:s}: {:s}:{:d}: ";
		constexpr static const std::string_view prefix_debug_ext = "{:s}: {:s}:{:d} {:s}: ";
		constexpr static const std::string_view exception_fmt = "Catch Exception:\n{:s}\n";

		Logger (void) noexcept;
		~Logger (void);
		Logger (const Logger& other)             = delete; // copy constructor
		Logger (Logger&& other) noexcept         = delete; // move constructor
		void operator= (const Logger& other)     = delete; // copy assignment
		void operator= (Logger&& other) noexcept = delete; // move assignment

	public:
		[[nodiscard]] static const Logger& get (void) noexcept {
			return Logger::getInstance ();
		}

		template <typename... Args>
		inline void operator() (const Level& level, const std::string_view format, const std::source_location location = std::source_location::current (), const Args&... args) const noexcept {
			if (level > this->_level) return;

			const char* level_str = Logger::levelToString (level);
			try {
#if defined(_RELEASE)
				if (level >= Level::DEBUG) return;
				const std::string print_format = Logger::concat (Logger::prefix_release, format);
				this->print (print_format, level_str, args...);
#elif defined(_DEBUG)
				if (this->_include_function_names) {
					const std::string print_format = Logger::concat (Logger::prefix_debug_ext, format);
					this->print (print_format, level_str, location.file_name (), location.line (), location.function_name (), args...);
				} else {
					const std::string print_format = Logger::concat (Logger::prefix_debug, format);
					this->print (print_format, level_str, location.file_name (), location.line (), args...);
				}
#else
				const std::string print_format = Logger::concat (Logger::prefix_logger, format);
				this->print (print_format, level_str, location.function_name (), args...);
#endif
			} catch (const std::exception& e) {
				try {
					const std::string print_format = Logger::concat (Logger::prefix_debug_ext, Logger::exception_fmt);
					this->print (print_format, level_str, location.file_name (), location.line (), location.function_name (), e.what ());
				} catch (const std::exception&) {
					this->print ("ERROR: Error while printing error exception.");
				}
			}
		}

		inline void operator() (const Level& level, const std::exception& e, const std::source_location location = std::source_location::current ()) const noexcept {
			if (level > this->_level) return;
#if defined(_RELEASE)
			if (level >= Level::DEBUG) return;
#endif
			this->operator() (level, Logger::exception_fmt, location, e.what ());
		}

		inline static std::ostream& getStream (void) noexcept {
			return Logger::get ().getStream_int ();
		}

		inline static const std::string_view getFilename (void) noexcept {
			return Logger::get ().getFilename_int ();
		}

		inline static Level getLoglevel (void) noexcept {
			return Logger::get ().getLoglevel_int ();
		}

		inline constexpr static void emptyFunc (void) noexcept { }

		inline static bool setStream (std::ostream& stream) {
			return Logger::getInstance ().setStream_int (stream);
		}

		inline static bool setFile (const std::string& file) {
			return Logger::getInstance ().setFile_int (file);
		}

		inline static bool setLoglevel (const Level& level) noexcept {
			return Logger::getInstance ().setLoglevel_int (level);
		}

		inline static bool setLoglevel (const int& level) noexcept {
			return Logger::getInstance ().setLoglevel_int (static_cast<Level>(level));
		}

		inline static bool setSplit (const bool& split = false) noexcept {
			return Logger::getInstance ().setSplit_int (split);
		}

		inline static bool setIncludeFunctionNames (const bool& func_names = false) noexcept {
			return Logger::getInstance ().setIncludeFunctionNames_int (func_names);
		}

	private:
		[[nodiscard]] static Logger& getInstance (void) noexcept;

		template <typename... Args>
		inline void print (const std::string& format, const Args&... args) const {
			fmt::print (*(this->_stream), fmt::runtime (format), args...);
			if (this->_split) {
				std::ostream* second_stream;
				if (this->_stream == &std::cout)
					second_stream = &std::cerr;
				else
					second_stream = &std::cout;
				fmt::print (*second_stream, fmt::runtime (format), args...);
			}
		}

		std::ostream& getStream_int (void) const noexcept;
		const std::string_view getFilename_int (void) const noexcept;
		Level getLoglevel_int (void) const noexcept;

		bool setStream_int (std::ostream&);
		bool setFile_int (const std::string&);
		bool setLoglevel_int (const Level&) noexcept;
		bool setSplit_int (const bool&) noexcept;
		bool setIncludeFunctionNames_int (const bool&) noexcept;

		inline static const std::string concat (const std::string_view s1, const std::string_view s2) noexcept {
			std::string string (s1.length () + s2.length (), 0);
			char *ptr = string.data ();
			for (char c : s1) {
				*ptr = c;
				++ptr;
			}

			for (char c : s2) {
				*ptr = c;
				++ptr;
			}

			return string;
		}

		constexpr static const char *levelToString (const Level& level) noexcept {
			switch (level)
			{
				case Level::EMERG   : return "EMERGENCY";
				case Level::ALERT   : return "ALERT";
				case Level::CRIT    : return "CRITICAL";
				case Level::ERR     : return "ERROR";
				case Level::WARNING : return "WARNING";
				case Level::NOTICE  : return "NOTICE";
				case Level::INFO    : return "INFO";
				case Level::DEBUG   : return "DEBUG";
				default             : return "DEBUG";
			}
		}
	};
}

#endif /* _CPPLOGGER_LOGGER_ */