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
		* class   线程实现类
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
			//添加任务
			virtual bool Post(IMethodRequestPtr p_MethodPtr);
		private:
			static void S_HandleJob(CThreadPoolImpl* p_pThreadPoolImpl, IMethodRequestPtr p_MethodPtr);
		private:
			//线程组 
			boost::thread_group m_ThreadGroup;
			//线程数
			unsigned int m_iThreadNum;
			//io服务，用作事件驱动引擎
			boost::asio::io_service m_IoService;
			//该对象是为了保证io_service在不调用stop的情况永远不退出
			boost::asio::io_service::work m_Permanence;
			//保证定时器的顺序执行
			//boost::asio::strand m_Strand;
			boost::detail::atomic_count m_MethodCount;
			//
			bool m_bIsStopped;
		};
	}
}