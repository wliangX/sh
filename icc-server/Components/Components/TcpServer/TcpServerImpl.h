#pragma once
//Interface
#include <TcpServer/ITcpServer.h>
//Project
#include "EffectiveAcceptor.h"

namespace ICC
{
	namespace Tcp
	{
		/*
		* class   TcpServer µœ÷¿‡
		* author  w16314
		* purpose
		* note
		*/
		class CTcpServerImpl :
			public ITcpServer
		{
		public:
			CTcpServerImpl(IResourceManagerPtr p_ResourceManagerPtr);
			virtual~CTcpServerImpl();
		public:
			virtual bool Listen(std::string p_strIp, unsigned short p_ushPort, unsigned int p_ThreadCount, IConnectionCreaterPtr p_ConnectionCreaterPtr);
			virtual void Close();
			virtual unsigned int Send(std::string p_strClientTag, const char* p_pData, unsigned int p_ilength);
		private:
			virtual IResourceManagerPtr GetResourceManager();
		private:
			EffectiveAcceptorPtr m_EffectiveAcceptorPtr;
			IResourceManagerPtr m_pResourceManager;
			Log::ILogPtr m_pLog;
		};
	}
}