/*
 * SingletonInstance.hpp
 *
 *  Created on: Mar 14, 2018
 *      Author: suoalex
 */

#ifndef LIB_COMMON_SRC_SINGLETONINSTANCE_HPP_
#define LIB_COMMON_SRC_SINGLETONINSTANCE_HPP_

#include <functional>
#include <memory>
#include <unordered_set>

#include <boost/thread/locks.hpp>
#include <boost/thread/shared_mutex.hpp>

#include "SFINAE.hpp"

namespace mm
{
	template<typename ValueType> struct SingletonHash
	{
		std::size_t operator()(const ValueType& value) const
		{
			return value.hashValue();
		}
	};

	//
	// This class defines a singleton instance set where each constructed instance must be a singleton
	// within the same process.
	//
	// The class requires its instances to have a proper hash function implemented.
	//
	template<
		typename ValueType,
		typename Hash = SingletonHash<ValueType>
	> class SingletonInstance
	{
	public:

		//
		// Constructor.
		//
		// size : The default size of the internal set.
		//
		SingletonInstance()
		{
			static_assert(sfinae::has_equal<ValueType>::value, "VaueType must implement == operator.");
			static_assert(sfinae::has_hash_value<ValueType>::value, "ValueType must implement hashValue() function");
		}

		//
		// Get the singleton instance for the value type class on the process.
		//
		// In case the value is requested the first time, create and store it.
		//
		// params : The parameter sets for the constructor of the value type.
		//
		// return : The singleton for the value type given the params for its constructor.
		//
		template<typename ... Params> static const ValueType& getInstance(Params ... params)
		{
			ValueType value(params...);

			{
				boost::upgrade_lock<boost::shared_mutex> lock(MUTEX);

				// insert if not found
				typename std::unordered_set<ValueType, Hash>::iterator it = INSTANCES.find(value);
				if (it == INSTANCES.end())
				{
					boost::upgrade_to_unique_lock<boost::shared_mutex> uniqueLock(lock);

					INSTANCES.insert(value);
					it = INSTANCES.find(value);
				}

				return *it;
			}
		}

	private:

		// The set of instances.
		static std::unordered_set<ValueType, Hash> INSTANCES;

		// The mutex for accessing the instances.
		static boost::shared_mutex MUTEX;
	};
}

template<typename ValueType, typename Hash>
std::unordered_set<ValueType, Hash> mm::SingletonInstance<ValueType, Hash>::INSTANCES;

template<typename ValueType, typename Hash>
boost::shared_mutex mm::SingletonInstance<ValueType, Hash>::MUTEX;


#endif /* LIB_COMMON_SRC_SINGLETONINSTANCE_HPP_ */
