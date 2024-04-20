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
	* class   线程实现类
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
		//启动线程
		virtual bool Start();
		//停止线程
		virtual void Stop();
		//判断线程运行程序
		virtual bool IsRunning();
		//跨线程异步调用方式
		virtual bool Post(IMethodRequestPtr p_MethodPtr);
    private:
        //线程主函数默认实现体
        void Run();
		//异步调用默认实现体
		void HandleJob(IMethodRequestPtr p_MethodPtr);
    private:
        static void S_Run(CThreadImpl* p_pThreadImpl);		
		static void S_HandleJob(CThreadImpl* p_pThreadImpl, IMethodRequestPtr p_MethodPtr);		
	private:
        //运行标志
		volatile bool m_bIsRunning;
		//线程实体采用boost的线程
		boost::thread* m_pThread;
		//io服务，用作事件驱动引擎
		boost::asio::io_service m_IoService;
		//该对象是为了保证io_service在不调用stop的情况永远不退出，确保其生命周期
		boost::asio::io_service::work m_Permanence;
		//消息计数器
		boost::detail::atomic_count m_MethodCount;
	private:
		boost::mutex m_Mutex;
		boost::condition m_Conditon;
		bool m_IsStarted;
	};
}