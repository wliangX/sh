#pragma once

#define MAX_BUFFER_SIZE 4096*2

namespace ICC
{
	namespace Tcp
	{
		class Connector;
		typedef boost::asio::io_service AsioEngine;
		typedef boost::shared_ptr<Connector> ConnectorPtr;
		typedef boost::shared_ptr<boost::asio::ip::tcp::socket> SocketPtr;

		
		typedef boost::asio::io_service AsioEngine;
		typedef boost::shared_ptr<AsioEngine> AsioEnginePtr;
		typedef std::vector<AsioEnginePtr> AsioEnginePtrs;
		/*
		* class   连接器默认实现
		* author  w16314
		* purpose
		* note
		*/
		class Connector
		{
		public:
			Connector(IConnectionPtr p_IConnectionPtr, AsioEnginePtr p_AsioEngine);
			~Connector();
		public:
			std::string GetLocalAddress();
			std::string GetRemoteAddress();
			char* GetBuffer(unsigned int& p_iSize);
			void SetBuffer(char* p_pBuffer, unsigned int p_iSize);
			unsigned int Send(const char* p_pData, unsigned int p_ilength);
			bool Close();
		public:
			bool Named();
		public:
			IConnectionPtr GetIConnection();
			SocketPtr GetSocket();
		public:
			static void OnAsyncReceive(ConnectorPtr p_ConnectorPtr, const boost::system::error_code& p_err, size_t p_ibytes_transferred);
		private:
			std::string AddressFromEndpoint(const boost::asio::ip::tcp::endpoint& p_Endpoint);
		private:
			AsioEnginePtr m_AsioEngine;
			IConnectionPtr m_IConnectionPtr;
			SocketPtr m_pClientSocket;
			std::string m_LocalAddress;
			std::string m_RemoteAddress;
			char* m_pBuffer;
			unsigned int m_BufferSize;
		};
	}
}
