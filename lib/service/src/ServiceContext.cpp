/*
 * ServiceContext.cpp
 *
 *  Created on: Sep 18, 2018
 *      Author: suoalex
 */

#include <fstream>
#include <stdexcept>

#include <PropertyConfig.hpp>

#include "ServiceContext.hpp"

const std::string mm::ServiceContext::SERVICE_LIST = "ServiceList";
const std::string mm::ServiceContext::SERVICE_CLASS = "Class";

mm::Logger mm::ServiceContext::logger;

namespace mm
{
	ServiceContext::ServiceContext(const std::string configFilePath, IServiceFactory& factory) :
			ServiceContext(std::fstream(configFilePath), factory)
	{
	}

	ServiceContext::ServiceContext(std::istream& is, IServiceFactory& factory) :
			ServiceContext(std::shared_ptr<IConfig> (new PropertyConfig(is)), factory)
	{
	}

	ServiceContext::ServiceContext(std::istream&& is, IServiceFactory& factory) :
			ServiceContext(std::shared_ptr<IConfig> (new PropertyConfig(is)), factory)
	{
	}

	ServiceContext::ServiceContext(const std::shared_ptr<IConfig>& config, IServiceFactory& factory) : config(config)
	{
		// logger
		{
			std::shared_ptr<IConfig> loggerConfig = config->getSubConfig(LoggerConfig::LOG_CONFIG);

			if (!loggerConfig.get())
			{
				LOGWARN("No logger defined in configuration. Using default logger to stdout.");
			}
			else
			{
				const LogLevel level = LogLevelConstant::getLevel(loggerConfig->getString(LoggerConfig::LOG_LEVEL, LogLevelConstant::LOG_TRACE));
				const std::string& path = loggerConfig->getString(LoggerConfig::LOG_PATH, LoggerConfig::DEFAULT_LOG_PATH);
				const std::string& name = loggerConfig->getString(LoggerConfig::LOG_NAME, LoggerConfig::DEFAULT_LOG_NAME);
				const std::string& pattern = loggerConfig->getString(LoggerConfig::LOG_PATTERN, LoggerSingleton::defaultPattern);

				const std::string timedPath = getTimedLogName(path);
				LoggerSingleton::init(level, timedPath, name, pattern);

				LOGINFO("Logger {} created with level {}, log file {}", name, LogLevelConstant::getName(level), timedPath);
			}
		}

		// dispatcher threading model
		{
			std::shared_ptr<IConfig> dispatcherConfig = config->getSubConfig(DispatcherConfig::DISPATCHER_CONFIG);

			const int threadCount = dispatcherConfig.get() ? dispatcherConfig->getInt64(DispatcherConfig::THREAD_COUNT) : 1;
			const bool waitOnEmpty = dispatcherConfig.get() ? dispatcherConfig->getBool(DispatcherConfig::WAIT_ON_EMPTY, false) : false;

			// the CPU affinity list if any
			std::vector<int> cpuIds;
			if (dispatcherConfig.get())
			{
				const std::vector<std::int64_t>& ids = dispatcherConfig->getInt64List(DispatcherConfig::CPU_AFFINITY);
				for (const std::int64_t& id : ids)
				{
					cpuIds.push_back(id);
				}
			}

			if (!dispatcherConfig.get())
			{
				LOGWARN("No dispatcher defined in config. Using default dispatch count = {}", threadCount);
			}

			if (threadCount <= 0)
			{
				LOGERR("Invalid thread count for dispatcher: {}", threadCount);
			}

			dispatcher.reset(new Dispatcher(threadCount, false, waitOnEmpty, cpuIds));
			LOGINFO("Dispatcher created with {} threads. WaitOnEmpty = {}", threadCount, waitOnEmpty);
		}

		// scheduler
		{
			scheduler.reset(new Scheduler(*dispatcher, false));
		}

		// create services
		{
			const std::vector<std::string> serviceList = config->getStringList(ServiceContext::SERVICE_LIST);
			if (serviceList.empty())
			{
				LOGERR("No service specified in {}", ServiceContext::SERVICE_LIST);
			}

			for (const std::string& serviceName : serviceList)
			{
				std::shared_ptr<IConfig> serviceConfig = config->getSubConfig(serviceName);

				if (!serviceConfig.get())
				{
					throw std::runtime_error("No config for service " + serviceName);
				}

				const std::string serviceClass = serviceConfig->getString(ServiceContext::SERVICE_CLASS);
				const std::shared_ptr<IService> service = factory.createService(serviceClass, serviceName, serviceConfig, *this);

				if (!service)
				{
					LOGFATAL("Failed to create service {}", serviceName);
					throw std::runtime_error("Failed to create service " + serviceName);
				}

				if (setService(serviceName, service))
				{
					LOGINFO("Service {} created successfully.", serviceName);
				}
			}
		}

		LOGINFO("Service context created.");
	}

	bool ServiceContext::start()
	{
		if (!dispatcher->start())
		{
			LOGFATAL("Failed to start dispatcher.");
			return false;
		}

		LOGINFO("Dispatcher started.");

		scheduler->start();
		LOGINFO("Scheduler started");

		for (std::pair<const std::string, std::shared_ptr<IService> >& pair : serviceMap)
		{
			if (!pair.second->start())
			{
				LOGERR("Error starting service {}", pair.first);
				return false;
			}

			LOGINFO("Service {} started.", pair.first);
		}

		LOGINFO("Service context started.");
		return true;
	}

	void ServiceContext::stop()
	{
		scheduler->stop();
		LOGINFO("Scheduler stopped.");

		dispatcher->stop();
		LOGINFO("Dispatcher stopped.");

		for (std::pair<const std::string, std::shared_ptr<IService> >& pair : serviceMap)
		{
			pair.second->stop();
			LOGINFO("Service {} stopped.", pair.first);
		}

		LOGINFO("Service context stopped.");
	}

}


