/*
 * ServiceContextManager.cpp
 *
 *  Created on: Oct 1, 2018
 *      Author: suoalex
 */

#include <csignal>
#include <signal.h>

#include "DelegateServiceFactory.hpp"
#include "ServiceContextManager.hpp"

mm::Logger mm::ServiceContextManager::logger;

namespace mm
{
	void ServiceContextManager::handle(int signal)
	{
		if (signal == SIGTERM || signal == SIGUSR1)
		{
			shutDownFlag.store(true);
		}
	}

	ServiceContextManager::createContextAndStart(const std::string path)
	{
		shutDownFlag.store(false);

		// register the signal handler
		std::signal(SIGTERM, handle);
		std::signal(SIGUSR1, handle);

		// start the context
		DelegateServiceFactory& factory = DelegateServiceFactory::getFactory();
		context = new ServiceContext(path, factory);

		if (!context->start())
		{
			LOGFATAL("Error starting context. Stopping...");
		}

		// block and shut down
		{
			sigset_t signals;
			sigemptyset(&signals);
			sigaddset(&signals, SIGTERM);
			sigaddset(&signals, SIGUSR1);
			sigaddset(&signals, SIGUSR2);

			int result = 0;
			if (sigwait(&signals, &result) != 0)
			{
				LOGERR("Error on signal waiting: {}", result);
			}
		}

		if (shutDownFlag.load())
		{
			shutdown();
		}
	}

	ServiceContextManager::shutdown()
	{
		LOGINFO("Shutting down service context");
		context->stop();

		LOGINFO("Service context shut down successfully");

		// notify all the threads to finish
		{
			std::lock_guard<Mutex> guard(mutex);
			condition.notify_all();
		}
	}

}
