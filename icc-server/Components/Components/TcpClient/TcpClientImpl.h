#pragma once

#include <boost/asio.hpp>
//#include <boost/bind.hpp>
#include <boost/bind/bind.hpp>
#include <boost/thread.hpp>

using namespace boost::placeholders;

namespace ICC
{
	namespace Tcp
	{
		typedef boost::shared_ptr<boost::thread> ThreadPtr;

		/*
		* class   Tcp客户端实现类
		* author  w16314
		* purpose
		* note
		*/
		class CTcpClientImpl :
			public ITcpClient
		{
		public:
			CTcpClientImpl(IResourceManagerPtr p_ResourceManagerPtr);
			virtual ~CTcpClientImpl();
		public:
			virtual bool Connect(std::string p_strIp, unsigned short p_ushPort, ITcpClientCallBackPtr p_UserCallbackPtr);
			virtual unsigned int Send(const char* p_Msg, unsigned int p_size);
			virtual void Close();
		public:
			static void S_RunEventLoop(boost::asio::io_service* p_Engine);
		private:
			void OnConnected(const boost::system::error_code& error);
			void OnReceive(const boost::system::error_code& error, size_t bytes_transferred);
			void InnerClose();
		private:
			virtual IResourceManagerPtr GetResourceManager();
		private:
			bool Named();
		private:
			boost::asio::io_service m_IoService;
			boost::asio::ip::tcp::resolver m_Resolver;
			boost::asio::ip::tcp::socket m_ClientSock;
		private:
			ITcpClientCallBackPtr m_ITcpClientCallbackPtr;
			IResourceManagerPtr m_pResourceManager;
			Log::ILogPtr m_pLog;
			ThreadPtr m_ThreadPtr;
		private:
			bool m_bReconnect;
			char* m_pBuffer;
			unsigned int m_BufferSize;

			std::string m_strIp;
			unsigned short m_ushPort;
		};
	}
}