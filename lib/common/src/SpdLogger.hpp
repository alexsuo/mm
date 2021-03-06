/*
 * SpgLogger.hpp
 *
 *  Created on: Sep 16, 2018
 *      Author: suoalex
 */

#ifndef LIB_COMMON_SRC_SPDLOGGER_HPP_
#define LIB_COMMON_SRC_SPDLOGGER_HPP_

#include <memory>

#include "spdlog/spdlog.h"
#include "LogLevel.hpp"

namespace mm
{
	//
	// The wrapper for spglog as a singleton per process.
	//
	class SpdLogger
	{
	public:

		//
		// Constructor.
		//
		// level : The log level.
		// path : The log file path.
		// loggerName : The name of the logger.
		// pattern : The log pattern.
		//
		SpdLogger(LogLevel level, const std::string& path, const std::string& loggerName, const std::string& pattern);

		//
		// Set the global log level.
		//
		// level : The log level.
		//
		void setLevel(LogLevel level);

		//
		// Get the logger.
		//
		// return : The spd logger.
		//
		inline std::shared_ptr<spdlog::logger> getLogger()
		{
			return logger;
		}

	private:

		// Pointer to the underlying logger.
		std::shared_ptr<spdlog::logger> logger;
	};

	class SpdLoggerSingleton
	{
	public:

		// The default log pattern.
		static const std::string defaultPattern;

		//
		// Initialize the global logger singleton.
		//
		// level : The log level.
		// path : The log file path.
		// loggerName : The logger name.
		// pattern : The log pattern.
		//
		static void init(LogLevel level, const std::string& path, const std::string& loggerName, const std::string& pattern);

		//
		// Get the logger.
		//
		// return : The logger for logging.
		//
		static std::shared_ptr<spdlog::logger> getLogger();

	private:

		// The global log level.
		static LogLevel level;

		// The file path.
		static std::string path;

		// The logger name.
		static std::string loggerName;

		// The log pattern.
		static std::string pattern;
	};

}



#endif /* LIB_COMMON_SRC_SPDLOGGER_HPP_ */
