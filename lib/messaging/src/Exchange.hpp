/*
 * ExchangeType.hpp
 *
 *  Created on: Sep 27, 2018
 *      Author: suoalex
 */

#ifndef LIB_MESSAGING_SRC_EXCHANGE_HPP_
#define LIB_MESSAGING_SRC_EXCHANGE_HPP_

#include <algorithm>
#include <cstdint>
#include <string>
#include <stdexcept>

namespace mm
{
	//
	// This enum defines the exchanges.
	//
	enum class Exchange : std::int64_t
	{
		// Shanghai stock exchange
		SSE = 1,

		// Shenzhen stock exchange
		SZSE = 2,

		// China financial future exchange
		CFFEX = 3,

		// Shanghai future exchange
		SHFE = 4,

		// Shenzhou commodity exchange
		CZCE = 5,

		// Dalian commodity exchange
		DCE = 6,

		// International energy exchange
		INE = 7,

		// Shanghai gold exchange
		SGE = 8,

		// Hong Kong exchange
		HKEX = 9,

		// Interactive broker
		SMART = 10,

		// Chicago commodity exchange
		GLOBEX = 11,

		// Interactive broker FX
		IDEALPRO = 12,

		// Oanda FX
		OANDA = 13,
	};

	//
	// The utility functions for the exchange enum.
	//
	struct ExchangeUtil
	{
		//
		// Get the exchange value from its string representation.
		//
		// value : The string name of the exchange.
		//
		// return : The exchange.
		//
		static Exchange fromString(const std::string& value)
		{
			static const std::pair<std::string, Exchange> VALUES[] = {
					{"SSE", Exchange::SSE},
					{"SZSE", Exchange::SZSE},
					{"CFFEX", Exchange::CFFEX},
					{"SHFE", Exchange::SHFE},
					{"CZCE", Exchange::CZCE},
					{"DCE", Exchange::DCE},
					{"INE", Exchange::INE},
					{"SGE", Exchange::SGE},
					{"HKEX", Exchange::HKEX},
					{"SMART", Exchange::SMART},
					{"GLOBEX", Exchange::GLOBEX},
					{"IDEALPRO", Exchange::IDEALPRO},
					{"OANDA", Exchange::OANDA},
			};

			for (std::size_t i = 0; i < 13; ++i)
			{
				if (value == VALUES[i].first)
				{
					return VALUES[i].second;
				}
			}

			throw std::runtime_error("Cannot get exchange from string value: " + value);
		}
	};
}



#endif /* LIB_MESSAGING_SRC_EXCHANGE_HPP_ */
