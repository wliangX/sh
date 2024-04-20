#pragma once

#include <string>
#include <IObject.h>

namespace ICC
{ 
	namespace Thread
	{
		/*
		* class   �첽���÷�����
		* author  w16314
		* purpose
		* note
		*/
		class IMethodRequest
		{
		public:
			//************************************
			// Method:    AsyncCall
			// FullName:  ACS::IMethodRequest::AsyncCall
			// Access:    virtual public 
			// Returns:   void
			// Qualifier: �û�ʵ�ֵ��첽���ýӿ�
			//************************************
			virtual void AsyncCall() = 0;
		};

		typedef boost::shared_ptr<IMethodRequest> IMethodRequestPtr;

		/*
		* class   �߳���
		* author  w16314
		* purpose
		* note
		*/
		class IThread : public IObject
		{
		public:

			//************************************
			// Method:    Start
			// FullName:  ACS::IThread::Start
			// Access:    virtual public 
			// Returns:   bool
			// Qualifier: �����߳�
			//************************************
			virtual bool Start() = 0;

			//************************************
			// Method:    Stop
			// FullName:  ACS::IThread::Stop
			// Access:    virtual public 
			// Returns:   void
			// Qualifier: ֹͣ�߳�
			//************************************
			virtual void Stop() = 0;

			//************************************
			// Method:    IsRunning
			// FullName:  ACS::IThread::IsRunning
			// Access:    virtual public 
			// Returns:   bool
			// Qualifier: �Ƿ�����
			//************************************
			virtual bool IsRunning() = 0;

			//************************************
			// Method:    Post
			// FullName:  ACS::IThread::Post
			// Access:    virtual public 
			// Returns:   bool
			// Qualifier: ���߳��첽���÷�ʽ
			// Parameter: IMethodRequestPtr p_methodPtr
			//************************************
			virtual bool Post(IMethodRequestPtr p_methodPtr) = 0;
		};

		typedef boost::shared_ptr<IThread> IThreadPtr;
	}
}
