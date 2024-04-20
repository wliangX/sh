#pragma once
#include <string>
#include <IObject.h>

namespace ICC
{
	namespace Tcp
	{
		/*
		* class   连接器接口
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
			// Parameter: std::string p_strClientTag 客户端的标志
			// Parameter: std::string p_strError
			//************************************
			virtual void OnDisconnected(std::string p_strClientTag, std::string p_strError) = 0;

			//************************************
			// Method:    OnAccepted
			// FullName:  ACS::IConnection::OnAccepted
			// Access:    virtual public 
			// Returns:   void
			// Qualifier: 这里需要保存下客户端的标志，用于向客户端发送数据
			// Parameter: std::string p_strClientTag 客户端的标志
			//************************************
			virtual void OnAccepted(std::string p_strClientTag) = 0;
		};

		typedef boost::shared_ptr<IConnection> IConnectionPtr;

		/*
		* class   连接器抽象接口
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
			// Qualifier: 连接创建器，需要用户实现
			//************************************
			virtual IConnectionPtr CreateConnection() = 0;
		};

		typedef boost::shared_ptr<IConnectionCreater> IConnectionCreaterPtr;
	}
}