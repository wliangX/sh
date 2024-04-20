/**
 @Copyright Copyright (C), 2023
 @file		workthread.h
 @created	2023/07/01
 @brief

 @author	tt
*/
#pragma once
#include "Boost.h"
namespace ICC
{
	class IWorkThread
	{
	public:
		IWorkThread() {};
		virtual ~IWorkThread() {};

		virtual bool StartThread() = 0;
		virtual void StopThread() = 0;

		virtual bool isAlive() = 0;
	};

	template <class C>
	class CWorkThread : public IWorkThread
	{
	public:
		typedef void (C::* CallbackThreadFun)();
		CWorkThread(C* p_object, CallbackThreadFun p_method) :
			m_pObject(p_object),
			m_method(p_method),
			m_bThreadAlive(false)
		{};
		virtual ~CWorkThread() {};
	public:
		virtual bool StartThread()
		{
			if (!m_bThreadAlive)
			{
				m_bThreadAlive = true;
				m_ThreadHandle = boost::make_shared<boost::thread>(boost::bind(&CWorkThread::_ThreadFunc, this));
			}
			return true;
		}

		virtual void StopThread()
		{
			if (m_bThreadAlive)
			{
				m_bThreadAlive = false;
				m_ThreadHandle->join();
			}
		}

		bool isAlive() { return m_bThreadAlive; };
	public:
		static void _ThreadFunc(CWorkThread* p_pThread)
		{
			if (p_pThread)
			{
				p_pThread->_DOThreadFunc();
			}
		}

		void _DOThreadFunc()
		{
			(m_pObject->*m_method)();
		}
	private:
		bool m_bThreadAlive;
		boost::shared_ptr<boost::thread> m_ThreadHandle;

		C* m_pObject;
		CallbackThreadFun m_method;
	};

	typedef boost::shared_ptr<IWorkThread> IWorkThreadPtr;

}// end namespace ICC
