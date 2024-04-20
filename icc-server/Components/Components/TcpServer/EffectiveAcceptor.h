#pragma once
//Interface
#include <TcpServer/ITcpServer.h>
#include <TcpServer/IConnection.h>
//Project
#include "Concurrency.h"

namespace ICC
{
	namespace Tcp
	{
		typedef boost::shared_ptr<boost::asio::ip::tcp::acceptor> AcceptorPtr;

		class IConnectionCreater;
		typedef boost::shared_ptr<IConnectionCreater> IConnectionCreaterPtr;

		/*
		* class   接收器实现
		* author  w16314
		* purpose
		* note
		*/
		class EffectiveAcceptor
		{
		public:
			bool Listen(std::string p_strIp, unsigned short p_ushPort, unsigned int p_ThreadCount, IConnectionCreaterPtr p_ConnectionCreaterPtr);
			void Close();
			unsigned int Send(std::string p_strClientTag, const char* p_pData, unsigned int p_ilength);
		public:
			EffectiveAcceptor(Log::ILogPtr p_pILog);
			~EffectiveAcceptor();
		private:
			void AsyncAccept();
		private:
			static void S_AsyncAccept(EffectiveAcceptor *p_pEffectiveAcceptor, ConnectorPtr p_ConnectorPtr, const boost::system::error_code& p_Err);
		private:
			std::string m_strIP;
			unsigned short m_iPort;
			IConnectionCreaterPtr m_ConnectionCreaterPtr;
			AsioEngine m_AcceptionEngine;
			AcceptorPtr m_AcceptorPtr;
			ThreadPtr m_AcceptionThreadPtr;
			std::string m_AcceptionMsg;
			ConcurrencyPtr m_ConcurrencyPtr;//并发控制块
			std::map<std::string, ConnectorPtr> m_ConnectorMap;
			Log::ILogPtr m_pLog;
		};

		typedef boost::shared_ptr<EffectiveAcceptor> EffectiveAcceptorPtr;
	}
}