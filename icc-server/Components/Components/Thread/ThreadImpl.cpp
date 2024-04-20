#include "Boost.h"

//Boost
#include <boost/thread/thread.hpp>
#include <boost/bind/bind.hpp>

//Project
#include "ThreadImpl.h"

CThreadImpl::CThreadImpl()
    :m_pThread(nullptr),
	m_bIsRunning(false),
    m_Permanence(m_IoService), 
    m_MethodCount(0),
	m_IsStarted(false)
{

}

CThreadImpl::~CThreadImpl()
{

}

//�����߳�
bool CThreadImpl::Start()
{
	if (m_bIsRunning)
	{
		return false;
	}

    m_pThread = new boost::thread(boost::bind(&CThreadImpl::S_Run, this));
	{
		boost::lock_guard<boost::mutex> guard(m_Mutex);
		while (!m_IsStarted)
		{
			boost::posix_time::seconds const delay(5);
			boost::system_time const start = boost::get_system_time();
			boost::system_time const timeout = start + delay;
			m_Conditon.timed_wait(m_Mutex, timeout);
		}
	}
	m_bIsRunning = true;
	return true;
}

//ֹͣ�߳�
void CThreadImpl::Stop()
{
    m_bIsRunning = false;
    m_IoService.stop();
    if (m_pThread)
    {
        m_pThread->join();
        delete m_pThread;
        m_pThread = nullptr;
    }
    m_IsStarted = false;
}

//�ж��߳����г���
bool CThreadImpl::IsRunning()
{
    return m_bIsRunning;
}

//���߳��첽���÷�ʽ
bool CThreadImpl::Post(IMethodRequestPtr p_MethodPtr)
{
    if (0 == p_MethodPtr.get() || !m_bIsRunning)
    {
        return false;
    }
	
    m_IoService.post(boost::bind(&CThreadImpl::S_HandleJob, this, p_MethodPtr));

    ++m_MethodCount;
    const long count = m_MethodCount.operator long();
    if (2048 < count)
    {
        std::cout << "thread queue overflow, count : " << count << std::endl;
    }
    return true;
}

//�߳����
void CThreadImpl::S_Run(CThreadImpl* p_pThreadImpl)
{
    p_pThreadImpl->Run();
}

//�߳�������Ĭ��ʵ����
void CThreadImpl::Run()
{
	{
		boost::lock_guard<boost::mutex> guard(m_Mutex);
		m_IsStarted = true;
		m_Conditon.notify_all();
	}
    m_IoService.run();
}

void CThreadImpl::S_HandleJob(CThreadImpl* p_pThreadImpl, IMethodRequestPtr p_MethodPtr)
{
	p_pThreadImpl->HandleJob(p_MethodPtr);
}

void CThreadImpl::HandleJob(IMethodRequestPtr p_MethodPtr)
{
	p_MethodPtr->AsyncCall();
	--m_MethodCount;	
}