#pragma once
#include <Threadpool/IThreadpool.h>

//boost
#include <boost/function.hpp>
#include <boost/asio.hpp>
#include <boost/thread.hpp>  
#include <boost/smart_ptr/detail/atomic_count.hpp>

namespace ICC
{
	namespace Thread
	{
		/*
		* class   �߳�ʵ����
		* author  w16314
		* purpose
		* note
		*/
		class CThreadPoolImpl : public IThreadPool
		{
		public:
			CThreadPoolImpl(unsigned int p_iThreadNum);
			virtual ~CThreadPoolImpl();
		public:
			//�������
			virtual bool Post(IMethodRequestPtr p_MethodPtr);
		private:
			static void S_HandleJob(CThreadPoolImpl* p_pThreadPoolImpl, IMethodRequestPtr p_MethodPtr);
		private:
			//�߳��� 
			boost::thread_group m_ThreadGroup;
			//�߳���
			unsigned int m_iThreadNum;
			//io���������¼���������
			boost::asio::io_service m_IoService;
			//�ö�����Ϊ�˱�֤io_service�ڲ�����stop�������Զ���˳�
			boost::asio::io_service::work m_Permanence;
			//��֤��ʱ����˳��ִ��
			//boost::asio::strand m_Strand;
			boost::detail::atomic_count m_MethodCount;
			//
			bool m_bIsStopped;
		};
	}
}