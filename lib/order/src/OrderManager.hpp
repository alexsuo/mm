/*
 * OrderManager.hpp
 *
 *  Created on: Sep 18, 2018
 *      Author: suoalex
 */

#ifndef LIB_ORDER_SRC_ORDERMANAGER_HPP_
#define LIB_ORDER_SRC_ORDERMANAGER_HPP_

#include <memory>

#include <DispatchableService.hpp>
#include <Dispatcher.hpp>
#include <ExecutionReportMessage.hpp>
#include <IConsumer.hpp>
#include <IService.hpp>
#include <PublisherAdapter.hpp>
#include <OrderMessage.hpp>

#include "Order.hpp"
#include "OrderCache.hpp"

namespace mm
{
	//
	// This class is the generic order manager implementing OrderListener concept.
	//
	template<typename ExchangeInterface, template<typename> class Pool> class OrderManager :
			public DispatchableService,
			public IConsumer<OrderMessage>,
			public IConsumer<ExecutionReportMessage>,
			public PublisherAdapter<OrderSummaryMessage>,
			public PublisherAdapter<TradeMessage>
	{
	public:

		// The exchange order type.
		typedef Order<ExchangeInterface, Pool<OrderSummaryMessage>, Pool<TradeMessage> > ExchangeOrder;

		// The default pool size.
		static constexpr std::size_t POOL_SIZE = 1000;

		//
		// Constructor.
		//
		// dispatchKey : The dispatch key.
		// publisher : The publisher.
		// exchange : The exchange interface.
		//
		OrderManager(
				const KeyType dispatchKey,
				const std::string serviceName,
				ServiceContext& serviceContext,
				ExchangeInterface& exchange) :
			DispatchableService(dispatchKey, serviceName, serviceContext),
			PublisherAdapter<OrderSummaryMessage> (serviceContext.getDispatcher()),
			PublisherAdapter<TradeMessage> (serviceContext.getDispatcher()),
			dispatchKey(dispatchKey),
			exchange(exchange),
			orderPool(POOL_SIZE),
			summaryPool(POOL_SIZE),
			tradePool(POOL_SIZE)
		{
		}

		//
		// Get the dispatch key for the order manager.
		//
		virtual const KeyType getKey() const override
		{
			return dispatchKey;
		}

		//
		// consumer a order message.
		//
		// message : The order message.
		//
		virtual void consume(const std::shared_ptr<const OrderMessage>& message) override
		{
			if (message->status == OrderStatus::NEW || message->status == OrderStatus::LIVE)
			{
				ExchangeOrder* order = orderPool.get(exchange, summaryPool, tradePool, this, this);
				liveCache.addOrder(order);
			}

			ExchangeOrder* order = liveCache.getOrder(message->instrumentId, message->orderId);
			if (UNLIKELY(!order))
			{
				LOGERR("Error getting live order with instrument ID: {}, order ID: {}", message->instrumentId, message->orderId);
				return;
			}

			order->consume(message);
		}

		//
		// consumer an execution message.
		// TODO: review the design. should we consume raw pointer? or template the consumer pointer type?
		//
		// message : The execution message.
		//
		virtual void consume(const std::shared_ptr<const ExecutionReportMessage>& message) override
		{
			ExchangeOrder* order = liveCache.getOrder(message->instrumentId, message->orderId);
			if (UNLIKELY(!order))
			{
				LOGERR("Error getting order with ID: {}, instrument ID: {}", message->orderId, message->instrumentId);
				return;
			}

			order->consume(message);
		}

	private:

		// Logger of the class.
		static Logger logger;

		// The dispatch key.
		const KeyType dispatchKey;

		// The exchange interface.
		ExchangeInterface& exchange;

		// The object pool for orders.
		Pool<ExchangeOrder> orderPool;

		// the object pool for order summary messages.
		Pool<OrderSummaryMessage> summaryPool;

		// The object pool for trade messages.
		Pool<TradeMessage> tradePool;

		// The live orders.
		OrderCache<ExchangeOrder> liveCache;

		// The completed orders.
		OrderCache<ExchangeOrder> completedCache;
	};
}

template<typename ExchangeInterface, template<typename> class Pool> mm::Logger mm::OrderManager<ExchangeInterface, Pool>::logger;

#endif /* LIB_ORDER_SRC_ORDERMANAGER_HPP_ */
