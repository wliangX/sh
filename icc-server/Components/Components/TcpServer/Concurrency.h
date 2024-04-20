#pragma once

#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/shared_ptr.hpp>
#include "Connector.h"

namespace ICC
{
	namespace Tcp
	{
		typedef boost::shared_ptr<boost::thread> ThreadPtr;
		typedef std::vector<ThreadPtr> ThreadPtrs;

		/*
		* class   线程实现类
		* author  w16314
		* purpose
		* note
		*/
		class Concurrency
		{
		public:
			bool Open();
			void Close();
			const AsioEnginePtr GetBalanceAsioEnginePtr();
			void RegisterReceive(ConnectorPtr p_ConnectorPtr);
		public:
			Concurrency(unsigned int sessionEngineCount);
			~Concurrency();
		private:
			AsioEnginePtrs m_SessionEnginePtrs;
			ThreadPtrs m_SessionThreadPtrs;
			unsigned int m_SessionEngineCount;
			unsigned int m_SessionBalanceFactor;
			std::string m_SessionMsg;
			SocketPtr m_pSock;
		};
		typedef boost::shared_ptr<Concurrency> ConcurrencyPtr;
	}
}