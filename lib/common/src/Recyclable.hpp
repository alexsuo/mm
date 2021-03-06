/*
 * Recyclable.hpp
 *
 *  Created on: Nov 4, 2017
 *      Author: suoalex
 */

#ifndef LIB_COMMON_SRC_RECYCLABLE_HPP_
#define LIB_COMMON_SRC_RECYCLABLE_HPP_

#include <mutex>

#include "LockFreeObjectPool.hpp"

namespace mm
{
	//
	// The class as root class for all object in the pool. Implementing methods for intrusive_ptr.
	//
	template<typename ObjectType, typename Mutex = std::recursive_mutex> class Recyclable
	{
	public:

		// Type for the object pool node.
		typedef typename LockFreeObjectPool<ObjectType, Mutex>::Node Node;

		// Constructor - initialize the ref count.
		Recyclable()
		{
			Node* node = reinterpret_cast<Node*> (reinterpret_cast<char*> (this) - sizeof(void*));
			node->refCount.store(0);
		}

		// virtual destructor.
		virtual ~Recyclable()
		{
		}

		//
		// Increase the reference count. Use node next as a hack because its not used when object is out of pool.
		//
		void addRef()
		{
			Node* node = reinterpret_cast<Node*> (reinterpret_cast<char*> (this) - sizeof(void*));
			node->refCount.fetch_add(1);
		}

		//
		// Decrease the reference count and optionally release the object.
		//
		void release()
		{
			Node* node = reinterpret_cast<Node*> (reinterpret_cast<char*> (this) - sizeof(void*));
			if (node->refCount.fetch_sub(1) == 1)
			{
				this->~Recyclable();
				node->pool->release(node);
			}
		}
	};

	//
	// Intrusive pointer functions.
	//
	template<typename ObjectType, typename Mutex = std::recursive_mutex> inline void intrusive_ptr_add_ref(
			Recyclable<ObjectType, Mutex>* recyclable)
	{
		recyclable->addRef();
	}

	//
	// Intrusive pointer functions.
	//
	template<typename ObjectType, typename Mutex = std::recursive_mutex> inline void intrusive_ptr_release(
			Recyclable<ObjectType, Mutex>* recyclable)
	{
		recyclable->release();
	}
}



#endif /* LIB_COMMON_SRC_RECYCLABLE_HPP_ */
