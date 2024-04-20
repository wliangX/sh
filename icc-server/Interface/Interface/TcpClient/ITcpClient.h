#pragma once
#include <string>
#include <IObject.h>

#define TCP_RECV_BUF_SIZE 2048

namespace ICC
{
	namespace Tcp
	{
		/*
		* class   Tcp客户端回调
		* author  w16314
		* purpose
		* note
		*/
		class ITcpClientCallBack
		{
		public:
			//************************************
			// Method:    OnConnected
			// FullName:  ACS::ITcpClientCallBack::OnConnected
			// Access:    virtual public 
			// Returns:   void
			// Qualifier:
			// Parameter: std::string p_strError
			//************************************
			virtual void OnConnected(std::string p_strError) = 0;
			//************************************
			// Method:    OnDisconnected
			// FullName:  ACS::ITcpClientCallBack::OnDisconnected
			// Access:    virtual public 
			// Returns:   void
			// Qualifier:
			// Parameter: std::string p_strError
			//************************************
			virtual void OnDisconnected(std::string p_strError) = 0;
			//************************************
			// Method:    OnReceived
			// FullName:  ACS::ITcpClientCallBack::OnReceived
			// Access:    virtual public 
			// Returns:   void
			// Qualifier:
			// Parameter: const char * p_pData
			// Parameter: unsigned int p_iLength
			//************************************
			virtual void OnReceived(const char* p_pData, unsigned int p_iLength) = 0;
		};
		typedef boost::shared_ptr<ITcpClientCallBack> ITcpClientCallBackPtr;

		/*
		* class   Tcp客户端接口
		* author  w16314
		* purpose
		* note
		*/
		class ITcpClient :
			public IObject
		{
		public:

			//************************************
			// Method:    Connect
			// FullName:  ACS::ITcpClient::Connect
			// Access:    virtual public 
			// Returns:   bool
			// Qualifier:
			// Parameter: std::string p_strIp
			// Parameter: unsigned short p_ushPort
			// Parameter: ITcpClientCallBackPtr p_UserCallbackPtr
			//************************************
			virtual bool Connect(std::string p_strIp, unsigned short p_ushPort, ITcpClientCallBackPtr p_UserCallbackPtr) = 0;
			//************************************
			// Method:    Send
			// FullName:  ACS::ITcpClient::Send
			// Access:    virtual public 
			// Returns:   unsigned int
			// Qualifier:
			// Parameter: const char *
			//************************************
			virtual unsigned int Send(const char* p_Msg, unsigned int p_size) = 0;

			//************************************
			// Method:    Close
			// FullName:  ACS::ITcpClient::Close
			// Access:    virtual public 
			// Returns:   void
			// Qualifier:
			//************************************
			virtual void Close() = 0;
		};

		typedef boost::shared_ptr<ITcpClient> ITcpClientPtr;
	}
}