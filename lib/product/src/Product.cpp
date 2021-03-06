/*
 * Product.cpp
 *
 *  Created on: Sep 29, 2018
 *      Author: suoalex
 */

#include "Product.hpp"

namespace mm
{
	Product::Product()
	{
	}

	Product::Product(const ProductMessage& content, const std::shared_ptr<const Product>& underlying) :
			content(content), underlying(underlying)
	{
	}

	Product::Product(
			const ProductMessage& content,
			const std::shared_ptr<const Product>& underlying,
			const std::vector<std::shared_ptr<const Product> >& constituents) :
			content(content),
			underlying(underlying),
			constituents(constituents)
	{
	}

}


