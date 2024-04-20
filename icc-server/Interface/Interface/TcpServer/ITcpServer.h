#pragma once
//STL
#include <string>
//Interface
#include <IObject.h>
#include <TcpServer/IConnection.h>

namespace ICC
{
	namespace Tcp
	{
		/*
		* class   TcpServer½Ó¿Ú
		* author  w16314
		* purpose
		* note
		*/
		class ITcpServer :
			public IObject
		{
		public:

			//************************************
			// Method:    Listen
			// FullName:  ACS::ITcpServer::Listen
			// Access:    virtual public 
			// Returns:   bool
			// Qualifier:
			// Parameter: std::string p_strIp
			// Parameter: unsigned short p_ushPort
			// Parameter: uint p_ThreadCount
			// Parameter: IConnectionCreaterPtr p_ConnectionCreaterPtr
			//************************************
			virtual bool Listen(std::string p_strIp, unsigned short p_ushPort, unsigned int p_ThreadCount, IConnectionCreaterPtr p_ConnectionCreaterPtr) = 0;
			//************************************
			// Method:    Close
			// FullName:  ACS::ITcpServer::Close
			// Access:    virtual public 
			// Returns:   void
			// Qualifier:
			//************************************
			virtual void Close() = 0;

			//************************************
			// Method:    Send
			// FullName:  ACS::ITcpServer::Send
			// Access:    virtual public 
			// Returns:   unsigned int
			// Qualifier:
			// Parameter: std::string p_strClientTag
			// Parameter: const char * p_pData
			// Parameter: unsigned int p_ilength
			//************************************
			virtual unsigned int Send(std::string p_strClientTag, const char* p_pData, unsigned int p_ilength) = 0;
		};

		typedef boost::shared_ptr<ITcpServer> ITcpServerPtr;
	}
}

