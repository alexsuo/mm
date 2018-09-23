/*
 * FemasOrderSession.hpp
 *
 *  Created on: Sep 18, 2018
 *      Author: suoalex
 */

#ifndef LIB_FEMAS_SRC_FEMASORDERSESSION_HPP_
#define LIB_FEMAS_SRC_FEMASORDERSESSION_HPP_

#include <atomic>
#include <memory>
#include <string>

#include <femas/USTPFtdcTraderApi.h>

#include <EnumType.hpp>
#include <IService.hpp>
#include <Logger.hpp>
#include <OrderMessage.hpp>
#include <OrderSummaryMessage.hpp>
#include <PublisherAdapter.hpp>

#include <FemasUserDetail.hpp>

namespace mm
{
	//
	// This class defines an order session.
	//
	// An order session subcribes to order message for order status manipulation and publishes order summary.
	//
	class FemasOrderSession :
			public IService,
			public CUstpFtdcTraderSpi,
			public IConsumer<OrderMessage>,
			public PublisherAdapter<OrderSummaryMessage>
	{
	public:

		//
		// Constructor.
		//
		// userDetail : The configuration details.
		//
		FemasOrderSession(const FemasUserDetail& detail);

		// virtual destructor.
		virtual ~FemasOrderSession();

		//
		// service interface.
		//
		virtual bool start() override;
		virtual void stop() override;

		//
		// publisher functionality.
		//
		virtual void subscribe(const Subscription& subscription, const std::shared_ptr<IConsumer<MarketDataMessage> >& consumer) override;
		virtual void unsubscribe(const Subscription& subscription, const std::shared_ptr<IConsumer<MarketDataMessage> >& consumer) override;

		//
		// Send new order to exchange.
		//
		// message : The message representing new order.
		//
		void sendOrder(const std::shared_ptr<OrderMessage>& message);

		//
		// Cancel order.
		//
		// message : The message representing order to cancel.
		//
		void cancel(const std::shared_ptr<OrderMessage>& message);

		//
		// Fired when the session is connected.
		//
		virtual void OnFrontConnected() override;

		//
		// Fired when the session is disconnected.
		//
		// reason : The reason number.
		//
		virtual void OnFrontDisconnected(int reason) override;

		//
		// Fired when the heart beat got error.
		//
		// timeLapse : The time passed.
		//
		virtual void OnHeartBeatWarning(int timeLapse) override;

		//
		// Fired when the packet start is received.
		//
		// topicID : The topic on which the packet is received.
		// sequenceNo : The sequence number.
		//
		virtual void OnPackageStart(int topicID, int sequenceNo) override;

		//
		// Fired when the packet end is received.
		//
		// topicID : The topic on which the packet is received.
		// sequenceNo : The sequence number.
		//
		virtual void OnPackageEnd(int topicID, int sequenceNo) override;

		//
		// Fired when the response got error.
		//
		// info : Structure with details.
		// requestID : The request got the error.
		// isLast : Flag if this is the last request.
		//
		virtual void OnRspError(CUstpFtdcRspInfoField *info, int requestID, bool isLast) override;

		//
		// Fired for the login response.
		//
		// userLogin : details for the login
		// info : information details.
		// requestID : The request ID.
		// isLast : Flag if this is the last response.
		//
		virtual void OnRspUserLogin(CUstpFtdcRspUserLoginField *userLogin, CUstpFtdcRspInfoField *info, int requestID, bool isLast) override;

		//
		// Fired for the logout response.
		//
		// userLogout : details for the logout
		// info : information details.
		// requestID : The request ID.
		// isLast : Flag if this is the last response.
		//
		virtual void OnRspUserLogout(CUstpFtdcRspUserLogoutField *userLogout, CUstpFtdcRspInfoField *info, int requestID, bool isLast) override;

		//
		// Fired for the subscription response.
		//
		// dissemination : details for the subscription.
		// info : information details.
		// requestID : The request ID.
		// isLast : Flag if this is the last response.
		//
		virtual void OnRspSubscribeTopic(CUstpFtdcDisseminationField *dissemination, CUstpFtdcRspInfoField *info, int requestID, bool isLast) override;

		//
		// Fired for the topic query response.
		//
		// dissemination : details for the subscription.
		// info : information details.
		// requestID : The request ID.
		// isLast : Flag if this is the last response.
		//
		virtual void OnRspQryTopic(CUstpFtdcDisseminationField *dissemination, CUstpFtdcRspInfoField *info, int requestID, bool isLast) override;

		//
		// Fired for user password updated.
		//
		// userPasswordUpdate : The password change detail.
		// info : The returned info object.
		// requestID : The request ID.
		// isLast : Flag if this is the last response.
		//
		virtual void OnRspUserPasswordUpdate(CUstpFtdcUserPasswordUpdateField *userPasswordUpdate, CUstpFtdcRspInfoField *info, int requestID, bool isLast) override;

		//
		// Fired for order insertion.
		//
		// inputOrder : The order insertion detail.
		// info : The returned info object.
		// requestID : The request ID.
		// isLast : Flag if this is the last response.
		//
		virtual void OnRspOrderInsert(CUstpFtdcInputOrderField *inputOrder, CUstpFtdcRspInfoField *info, int requestID, bool isLast) override;

		//
		// Fired for order action.
		//
		// orderAction : The order action detail.
		// info : The returned info object.
		// requestID : The request ID.
		// isLast : Flag if this is the last response.
		//
		virtual void OnRspOrderAction(CUstpFtdcOrderActionField *orderAction, CUstpFtdcRspInfoField *info, int requestID, bool isLast) override;

		//
		// Fired for flow message canceled.
		//
		// flowMessageCancel : Detail of the canceled flow.
		//
		virtual void OnRtnFlowMessageCancel(CUstpFtdcFlowMessageCancelField *flowMessageCancel) override;

		//
		// Fired for trade.
		//
		// trade : Detail of the trade.
		//
		virtual void OnRtnTrade(CUstpFtdcTradeField *trade) override;

		//
		// Fired for order.
		//
		// order : Detail of the order.
		//
		virtual void OnRtnOrder(CUstpFtdcOrderField *order) override;

		//
		// Fired for order insertion.
		//
		// inputOrder : The input order detail.
		// info : The returned info object.
		//
		virtual void OnErrRtnOrderInsert(CUstpFtdcInputOrderField *inputOrder, CUstpFtdcRspInfoField *info) override;

		//
		// Fired for order action.
		//
		// orderAction : The order action detail.
		// info : The returned info object.
		//
		virtual void OnErrRtnOrderAction(CUstpFtdcOrderActionField *orderAction, CUstpFtdcRspInfoField *info) override;

		//
		// Fired for instrument status update.
		//
		// instrumentStatus : The instrument status.
		//
		virtual void OnRtnInstrumentStatus(CUstpFtdcInstrumentStatusField *instrumentStatus) override;

		//
		// Fired for account deposit update.
		//
		// investorAccountDepositRes : The acciont deposit detail.
		//
		virtual void OnRtnInvestorAccountDeposit(CUstpFtdcInvestorAccountDepositResField *investorAccountDepositRes) override;

		//
		// Fired for order query.
		//
		// orderAction : The order action detail.
		// info : The returned info object.
		// requestID : The request ID.
		// isLast : Flag if this is the last response.
		//
		virtual void OnRspQryOrder(CUstpFtdcOrderField *order, CUstpFtdcRspInfoField *info, int requestID, bool isLast) override;

		//
		// Fired for trade query.
		//
		// trade : The trade detail.
		// info : The returned info object.
		// requestID : The request ID.
		// isLast : Flag if this is the last response.
		//
		virtual void OnRspQryTrade(CUstpFtdcTradeField *trade, CUstpFtdcRspInfoField *info, int requestID, bool isLast) override;

		//
		// Fired for user query
		//
		// userInvestor : The user detail.
		// info : The returned info object.
		// requestID : The request ID.
		// isLast : Flag if this is the last response.
		//
		virtual void OnRspQryUserInvestor(CUstpFtdcRspUserInvestorField *userInvestor, CUstpFtdcRspInfoField *info, int requestID, bool isLast) override;

		//
		// Fired for trading code query
		//
		// tradingCode : The trading code detail.
		// info : The returned info object.
		// requestID : The request ID.
		// isLast : Flag if this is the last response.
		//
		virtual void OnRspQryTradingCode(CUstpFtdcRspTradingCodeField *tradingCode, CUstpFtdcRspInfoField *info, int requestID, bool isLast) override;

		//
		// Fired for investor account detail.
		//
		// investorAccount : The account detail.
		// info : The returned info object.
		// requestID : The request ID.
		// isLast : Flag if this is the last response.
		//
		virtual void OnRspQryInvestorAccount(CUstpFtdcRspInvestorAccountField *investorAccount, CUstpFtdcRspInfoField *info, int requestID, bool isLast) override;

		//
		// Fired for instrument detail.
		//
		// instrument : The instrument detail.
		// info : The returned info object.
		// requestID : The request ID.
		// isLast : Flag if this is the last response.
		//
		virtual void OnRspQryInstrument(CUstpFtdcRspInstrumentField *instrument, CUstpFtdcRspInfoField *info, int requestID, bool isLast) override;

		//
		// Fired for exchange detail.
		//
		// exchange : The exchange detail.
		// info : The returned info object.
		// requestID : The request ID.
		// isLast : Flag if this is the last response.
		//
		virtual void OnRspQryExchange(CUstpFtdcRspExchangeField *exchange, CUstpFtdcRspInfoField *info, int requestID, bool isLast) override;

		//
		// Fired for position detail.
		//
		// investorPosition : The position detail.
		// info : The returned info object.
		// requestID : The request ID.
		// isLast : Flag if this is the last response.
		//
		virtual void OnRspQryInvestorPosition(CUstpFtdcRspInvestorPositionField *investorPosition, CUstpFtdcRspInfoField *info, int requestID, bool isLast) override;

		//
		// Fired for compliance detail.
		//
		// complianceParam : The compliance detail.
		// info : The returned info object.
		// requestID : The request ID.
		// isLast : Flag if this is the last response.
		//
		virtual void OnRspQryComplianceParam(CUstpFtdcRspComplianceParamField *complianceParam, CUstpFtdcRspInfoField *info, int requestID, bool isLast) override;

		//
		// Fired for investor fee detail.
		//
		// investorFee : The fee detail.
		// info : The returned info object.
		// requestID : The request ID.
		// isLast : Flag if this is the last response.
		//
		virtual void OnRspQryInvestorFee(CUstpFtdcInvestorFeeField *investorFee, CUstpFtdcRspInfoField *info, int requestID, bool isLast) override;

		//
		// Fired for investor margin detail.
		//
		// investorMargin : The margin detail.
		// info : The returned info object.
		// requestID : The request ID.
		// isLast : Flag if this is the last response.
		//
		virtual void OnRspQryInvestorMargin(CUstpFtdcInvestorMarginField *investorMargin, CUstpFtdcRspInfoField *info, int requestID, bool isLast) override;

	private:

		inline std::shared_ptr<OrderSummaryMessage> getMessage()
		{
			return std::make_shared<OrderSummaryMessage>(new OrderSummaryMessage());
		}

		// The int value for bid
		static constexpr int BID = toValue(Side::BID);

		// The int value for ask
		static constexpr int ASK = toValue(Side::ASK);

		// The logger for this class.
		static Logger logger;

		// The login user detail.
		const FemasUserDetail userDetail;

		// The actual API session.
		// Note that we cannot use unique_ptr etc here as the destructor is protected.
		CUstpFtdcTraderApi* session;

		// Flag if stop is called or in process.
		std::atomic<bool> stopFlag;

		// The request ID value keeper.
		std::atomic<int> requestId;

	};
}



#endif /* LIB_FEMAS_SRC_FEMASORDERSESSION_HPP_ */
