#pragma once

namespace ICC
{
	namespace Udp
	{
		typedef boost::shared_ptr<boost::thread> ThreadPtr;

		/*
		* class   UPD服务端实现类
		* author  w16314
		* purpose
		* note
		*/
		class CUdpImpl :
			public IUdp
		{
		public:
			CUdpImpl(IResourceManagerPtr p_ResourceManagerPtr);
			virtual ~CUdpImpl();
		public:
			virtual void Bind(unsigned short p_ushPort, IUdpCallBackPtr p_IUdpCallBackPtr);
			virtual unsigned int Send(std::string p_strMsg, std::string p_strDestIp, unsigned short p_ushDestPort);
			virtual unsigned int Send(char* p_pbuf, unsigned int p_ibuffLen, std::string p_strDestIp, unsigned short p_ushDestPort);
			virtual void Close();
		public:
			static void S_RunEventLoop(boost::asio::io_service* p_Engine);
		private:
			void OnReceive(const boost::system::error_code& error, size_t bytes_transferred);
		private:
			virtual IResourceManagerPtr GetResourceManager();
		private:
			boost::asio::io_service m_IoService;
			boost::shared_ptr<boost::asio::ip::udp::socket> m_pSockUDP;
			boost::asio::ip::udp::endpoint m_endpointRemote;
			ThreadPtr m_pThread;
			char* m_pBuffer;
			unsigned int m_BufferSize;
			IUdpCallBackPtr m_IUdpCallBackPtr;
			IResourceManagerPtr m_pResourceManager;
			Log::ILogPtr m_pLog;
		};
	}
}
