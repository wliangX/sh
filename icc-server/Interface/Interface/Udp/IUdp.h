#pragma once
#include <string>
#include <IObject.h>

#define UDP_RECV_BUF_SIZE 4096

namespace ICC
{
	namespace Udp
	{
		enum UDP_RESULT
		{
			UDP_RESULT_SUCCESS = 0,
			UDP_RESULT_UDPERROR,
		};

		/*
		* class   UDP回调
		* author  w16314
		* purpose
		* note
		*/
		class IUdpCallBack
		{
		public:
			virtual void OnReport(int iCode, const std::string& strMessage) {};
			//************************************
			// Method:    OnReceived
			// FullName:  ACS::Udp::IUdpCallBack::OnReceived
			// Access:    virtual public 
			// Returns:   void
			// Qualifier:
			// Parameter: const char * p_pData
			// Parameter: unsigned int p_iLength
			// Parameter: std::string p_strRemoteIP
			// Parameter: size_t p_iPort
			//************************************
			virtual void OnReceived(const char* p_pData, unsigned int p_iLength, std::string p_strRemoteIP, size_t p_iPort) = 0;
		};
		typedef boost::shared_ptr<IUdpCallBack> IUdpCallBackPtr;

		/*
		* class   UDP接口
		* author  w16314
		* purpose
		* note
		*/
		class IUdp :
			public IObject
		{
		public:

			//************************************
			// Method:    Bind
			// FullName:  ACS::Udp::IUdp::Bind
			// Access:    virtual public 
			// Returns:   void
			// Qualifier:
			// Parameter: unsigned short p_ushPort
			// Parameter: IUdpCallBackPtr p_IUdpServerCallBackPtr
			//************************************
			virtual void Bind(unsigned short p_ushPort, IUdpCallBackPtr p_IUdpServerCallBackPtr) = 0;

			//************************************
			// Method:    Send
			// FullName:  ACS::Udp::IUdp::Send
			// Access:    virtual public 
			// Returns:   unsigned int
			// Qualifier:
			// Parameter: std::string p_strMsg
			// Parameter: std::string p_strDestIp
			// Parameter: unsigned short p_ushDestPort
			//************************************
			virtual unsigned int Send(std::string p_strMsg, std::string p_strDestIp, unsigned short p_ushDestPort) = 0;

			//************************************
			// Method:    Send
			// FullName:  ACS::Udp::IUdp::Send
			// Access:    virtual public 
			// Returns:   unsigned int
			// Qualifier:
			// Parameter: char * p_pbuf
			// Parameter: unsigned int p_ibuffLen
			// Parameter: std::string p_strDestIp
			// Parameter: unsigned short p_ushDestPort
			//************************************
			virtual unsigned int Send(char* p_pbuf, unsigned int p_ibuffLen, std::string p_strDestIp, unsigned short p_ushDestPort) = 0;


			//************************************
			// Method:    Close
			// FullName:  ACS::Udp::IUdp::Close
			// Access:    virtual public 
			// Returns:   void
			// Qualifier:
			//************************************
			virtual void Close() = 0;
		};

		typedef boost::shared_ptr<IUdp> IUdpPtr;
	}
}

