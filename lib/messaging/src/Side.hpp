/*
 * Side.hpp
 *
 *  Created on: Sep 14, 2018
 *      Author: suoalex
 */

#ifndef LIB_MESSAGING_SRC_SIDE_HPP_
#define LIB_MESSAGING_SRC_SIDE_HPP_

#include <cstdint>

namespace mm
{
	//
	// The enum for market data side.
	//
	enum class Side : std::int64_t
	{
		BID = 0,
		ASK = 1
	};

}

#endif /* LIB_MESSAGING_SRC_SIDE_HPP_ */
