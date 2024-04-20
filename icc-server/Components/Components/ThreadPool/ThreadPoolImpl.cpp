#include "Boost.h"

//Boost
#include <boost/thread/thread.hpp>
#include <boost/bind/bind.hpp>
//Project
#include "ThreadPoolImpl.h"

CThreadPoolImpl::CThreadPoolImpl(unsigned int p_iThreadNum)
    : m_iThreadNum(p_iThreadNum), m_bIsStopped(false), m_Permanence(m_IoService), /*m_Strand(m_IoService),*/
    m_MethodCount(0)
{
    if (m_iThreadNum <= 0)
    {
        return;
    }

	for (unsigned int i = 0; i < m_iThreadNum; ++i)
    {
        //生成多个线程，绑定run函数，添加到线程组  
        m_ThreadGroup.add_thread(new boost::thread(boost::bind(&boost::asio::io_service::run, &m_IoService)));
    }
}

CThreadPoolImpl::~CThreadPoolImpl()
{
    if (!m_bIsStopped)
    {
        m_IoService.stop();
        m_ThreadGroup.join_all();
        m_bIsStopped = true;
    }
}

bool CThreadPoolImpl::Post(IMethodRequestPtr p_MethodPtr)
{
    m_IoService.post(boost::bind(&CThreadPoolImpl::S_HandleJob, this, p_MethodPtr));
    return true;
}

void CThreadPoolImpl::S_HandleJob(CThreadPoolImpl* p_pThreadPoolImpl, IMethodRequestPtr p_MethodPtr)
{
    p_MethodPtr->AsyncCall();
    --(p_pThreadPoolImpl->m_MethodCount);
}
