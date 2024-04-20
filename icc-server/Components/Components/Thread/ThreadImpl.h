#pragma once
#include <Thread/IThread.h>
//boost
#include <boost/thread.hpp>
#include <boost/function.hpp>
#include <boost/asio.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/smart_ptr/detail/atomic_count.hpp>
#include <boost/thread/condition.hpp>

namespace ICC
{
	/*
	* class   �߳�ʵ����
	* author  w16314
	* purpose
	* note
	*/
	class CThreadImpl : public IThread
	{
	public:
		CThreadImpl();
		virtual ~CThreadImpl();
	public:
		//�����߳�
		virtual bool Start();
		//ֹͣ�߳�
		virtual void Stop();
		//�ж��߳����г���
		virtual bool IsRunning();
		//���߳��첽���÷�ʽ
		virtual bool Post(IMethodRequestPtr p_MethodPtr);
    private:
        //�߳�������Ĭ��ʵ����
        void Run();
		//�첽����Ĭ��ʵ����
		void HandleJob(IMethodRequestPtr p_MethodPtr);
    private:
        static void S_Run(CThreadImpl* p_pThreadImpl);		
		static void S_HandleJob(CThreadImpl* p_pThreadImpl, IMethodRequestPtr p_MethodPtr);		
	private:
        //���б�־
		volatile bool m_bIsRunning;
		//�߳�ʵ�����boost���߳�
		boost::thread* m_pThread;
		//io���������¼���������
		boost::asio::io_service m_IoService;
		//�ö�����Ϊ�˱�֤io_service�ڲ�����stop�������Զ���˳���ȷ������������
		boost::asio::io_service::work m_Permanence;
		//��Ϣ������
		boost::detail::atomic_count m_MethodCount;
	private:
		boost::mutex m_Mutex;
		boost::condition m_Conditon;
		bool m_IsStarted;
	};
}