#pragma once
#include <string>
#include <IObject.h>

namespace ICC
{
	namespace Tcp
	{
		/*
		* class   �������ӿ�
		* author  w16314
		* purpose
		* note
		*/
		class IConnection
		{
		public:
			//************************************
			// Method:    OnReceived
			// FullName:  ACS::IConnection::OnReceived
			// Access:    virtual public 
			// Returns:   void
			// Qualifier:
			// Parameter: const char * p_pData
			// Parameter: unsigned int p_iLength
			//************************************
			virtual void OnReceived(const char* p_pData, unsigned int p_iLength) = 0;
			//************************************
			// Method:    OnDisconnected
			// FullName:  ACS::IConnection::OnDisconnected
			// Access:    virtual public 
			// Returns:   void
			// Qualifier:
			// Parameter: std::string p_strClientTag �ͻ��˵ı�־
			// Parameter: std::string p_strError
			//************************************
			virtual void OnDisconnected(std::string p_strClientTag, std::string p_strError) = 0;

			//************************************
			// Method:    OnAccepted
			// FullName:  ACS::IConnection::OnAccepted
			// Access:    virtual public 
			// Returns:   void
			// Qualifier: ������Ҫ�����¿ͻ��˵ı�־��������ͻ��˷�������
			// Parameter: std::string p_strClientTag �ͻ��˵ı�־
			//************************************
			virtual void OnAccepted(std::string p_strClientTag) = 0;
		};

		typedef boost::shared_ptr<IConnection> IConnectionPtr;

		/*
		* class   ����������ӿ�
		* author  w16314
		* purpose
		* note
		*/
		class IConnectionCreater
		{
		public:

			//************************************
			// Method:    CreateAcceptingConnection
			// FullName:  ACS::IConnectionCreater::CreateConnection
			// Access:    virtual public 
			// Returns:   ACS::IConnectionPtr
			// Qualifier: ���Ӵ���������Ҫ�û�ʵ��
			//************************************
			virtual IConnectionPtr CreateConnection() = 0;
		};

		typedef boost::shared_ptr<IConnectionCreater> IConnectionCreaterPtr;
	}
}