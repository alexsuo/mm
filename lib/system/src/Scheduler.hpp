/*
 * Scheduler.hpp
 *
 *  Created on: Oct 11, 2017
 *      Author: suoalex
 */

#ifndef LIB_SYSTEM_SRC_SCHEDULER_HPP_
#define LIB_SYSTEM_SRC_SCHEDULER_HPP_

#include <atomic>
#include <cstdint>
#include <mutex>
#include <thread>

#include <tbb/concurrent_priority_queue.h>

#include <Timer.hpp>
#include <TimeUtil.hpp>

#include "Dispatcher.hpp"

namespace mm
{
	//
	// The struct for a delayed runnable.
	//
	template<typename Key> struct DelayedRunnable
	{
		// The key for the runnable to be dispatched.
		Key key;

		// The runnable
		Runnable runnable;

		// The timestamp as number of nanoseconds from epoch.
		std::int64_t timestampInNanos;

		//
		// Overload the operator for comparison.
		//
		// rhs : Comparand.
		//
		// return : true if timestampInNanos in lhs less than that in rhs.
		//
		bool operator < (const DelayedRunnable& rhs) const
		{
			return timestampInNanos < rhs.timestampInNanos;
		}
	};

	//
	// The scheduler working together with dispatcher.
	//
	template<typename Dispatcher, typename Key = std::int32_t, typename Timer = HighResTimer> class DefaultScheduler
	{
	public:

		//
		// Constructor.
		//
		// dispatcher : The dispatcher which will execute the actual task.
		//
		DefaultScheduler(Dispatcher& dispatcher) :
			dispatcher(dispatcher),
			stopRequested(false),
			scheduleThread([this] () { this->run(); } )
		{
		}

		//
		// Destructor. Stop the schedule thread.
		//
		~DefaultScheduler()
		{
			if (!stopRequested.load() || scheduleThread.joinable())
			{
				stopRequested.store(true);

				// notify then wait for thread
				{
					std::lock_guard<std::recursive_mutex> guard(mutex);
					condition.notify_all();
				}

				scheduleThread.join();
			}
		}

		//
		// Schedule a given task.
		//
		// key : The key for the runnable.
		// runnable : The task to schedule.
		// delay : The delay as duration object.
		//
		template <typename Rep, typename Period> inline void schedule(
				const Key key,
				const Runnable& runnable,
				const std::chrono::duration<Rep, Period>& delay)
		{
			schedule(key, runnable, durationToEpochNanos(delay));
		}

		//
		// Schedule a given task.
		//
		// key : The key for the runnable.
		// runnable : The task to schedule.
		// delay : The delay in nanoseconds.
		//
		void schedule(const Key key, const Runnable& runnable, std::int64_t delay)
		{
			scheduleAt(key, runnable, timer.getTimeInNanos() + delay);
		}

		//
		// Schedule a given task.
		//
		// key : The key for the runnable.
		// runnable : The task to schedule.
		// timestamp : The time to execute the task as time point.
		//
		template<typename Clock, typename Duration> inline void scheduleAt(
				const Key key,
				const Runnable& runnable,
				const std::chrono::time_point<Clock, Duration>& timestamp)
		{
			scheduleAt(key, runnable, timePointToEpochNanos(timestamp));
		}

		//
		// Schedule a given task.
		//
		// key : The key for the runnable.
		// runnable : The task to schedule.
		// timestamp : The time to execute the task.
		//
		void scheduleAt(const Key key, const Runnable& runnable, std::int64_t timestamp)
		{
			bool wasEmpty = queue.empty();
			queue.push({key, runnable, timestamp});

			if (wasEmpty || timestamp < nextTaskTimestamp.load())
			{
				std::lock_guard<std::recursive_mutex> guard(mutex);

				if (timestamp < nextTaskTimestamp.load())
				{
					nextTaskTimestamp.store(timestamp);
				}

				condition.notify_all();
			}
		}

		//
		// Schedule a task at fixed rate.
		//
		// key : The key for the runnable.
		// runnable : The task to schedule.
		// delay : The initial delay as duration object.
		// interval : The interval as duration object between 2 task start time.
		//
		template <typename Rep, typename Period> inline void scheduleAtFixedRate(
				const Key key,
				const Runnable& runnable,
				const std::chrono::duration<Rep, Period>& delay,
				const std::chrono::duration<Rep, Period>& interval)
		{
			scheduleAtFixedRate(key, runnable,
					durationToEpochNanos(delay),
					durationToEpochNanos(interval));
		}

		//
		// Schedule a task at fixed rate.
		//
		// key : The key for the runnable.
		// runnable : The task to schedule.
		// delay : The initial delay in nanoseconds.
		// interval : The interval in nanoseconds between 2 task start time.
		//
		void scheduleAtFixedRate(
				const Key key,
				const Runnable& runnable,
				std::int64_t delay,
				std::int64_t interval)
		{
			Runnable recursiveRunnable = [this, &recursiveRunnable, key, runnable, interval]()
			{
				std::int64_t startTime = timer.getTimeInNanos();
				runnable();

				this->scheduleAt(key, recursiveRunnable, startTime + interval);
			};

			scheduleAt(key, recursiveRunnable, timer.getTimeInNanos() + delay);
		}

		//
		// Schedule a task with fixed delay.
		//
		// key : The key for the runnable.
		// runnable : The task to schedule.
		// delay : The initial delay as duration object.
		// interval : The interval as duration object between task end and next task start.
		//
		template <typename Rep, typename Period> inline void scheduleWithFixedDelay(
				const Key key,
				const Runnable& runnable,
				const std::chrono::duration<Rep, Period>& delay,
				const std::chrono::duration<Rep, Period>& interval)
		{
			scheduleWithFixedDelay(key, runnable,
					durationToEpochNanos(delay),
					durationToEpochNanos(interval));
		}

		//
		// Schedule a task with fixed delay.
		//
		// key : The key for the runnable.
		// runnable : The task to schedule.
		// delay : The initial delay in nanoseconds.
		// interval : The interval in nanoseconds between task end and next task start.
		//
		void scheduleWithFixedDelay(const Key key, const Runnable& runnable, std::int64_t delay, std::int64_t interval)
		{
			Runnable recursiveRunnable = [this, &recursiveRunnable, key, runnable, interval]()
			{
				runnable();
				this->scheduleAt(key, recursiveRunnable, timer.getTimeInNanos() + interval);
			};

			scheduleAt(key, recursiveRunnable, timer.getTimeInNanos() + delay);
		}

	protected:

		//
		// Execute the tasks from the queue.
		//
		void run()
		{
			bool hasTask = false;
			std::cv_status status;
			DelayedRunnable<Key> runnable;

			while (!stopRequested.load())
			{
				// make sure the runnable is the next to execute
				if (hasTask)
				{
					if (nextTaskTimestamp.load() < runnable.timestampInNanos)
					{
						// we know the try pop will at least retrieve 1 item
						queue.push(runnable);
						queue.try_pop(runnable);
					}
				}
				else
				{
					hasTask = queue.try_pop(runnable);
				}

				// deal with the task or wait
				if (hasTask)
				{
					std::int64_t periodInNanos = timer.getTimeInNanos() - runnable.timestampInNanos;

					if (periodInNanos <= 0)
					{
						dispatcher.submit(runnable.key, runnable.runnable);
						hasTask = false;
					}
					else
					{
						std::unique_lock<std::recursive_mutex> lock(mutex);
						lock.lock();

						// when this happens we need to re-examine the queue and stop flag.
						if (nextTaskTimestamp.load() < runnable.timestampInNanos || stopRequested.load())
						{
							continue;
						}

						nextTaskTimestamp.store(runnable.timestampInNanos);

						status = condition.wait_for(lock, std::chrono::nanoseconds(periodInNanos));
						if (status == std::cv_status::timeout)
						{
							dispatcher.submit(runnable.key, runnable.runnable);
							hasTask = false;
						}
					}
				}
				else
				{
					// enter inactive thread wait for next task
					std::unique_lock<std::recursive_mutex> lock(mutex);
					while (queue.empty() && !stopRequested.load())
					{
						condition.wait(lock);
					}
				}
			}
		}

	private:

		// The dispatcher used.
		Dispatcher& dispatcher;

		// The timer used for scheduling.
		Timer timer;

		// The next task to execute with the smallest timestamp (i.e. fastest execution).
		std::atomic<std::int64_t> nextTaskTimestamp;

		// The priority queue for the tasks.
		tbb::concurrent_priority_queue<DelayedRunnable<Key> > queue;

		// The mutex for scheduling.
		std::recursive_mutex mutex;

		// The condition variable used.
		std::condition_variable_any condition;

		// Flag if stop is requested
		std::atomic<bool> stopRequested;

		// The schedule thread.
		std::thread scheduleThread;
	};
}



#endif /* LIB_SYSTEM_SRC_SCHEDULER_HPP_ */