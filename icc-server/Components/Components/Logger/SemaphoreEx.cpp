#include "SemaphoreEx.h"


Semaphore::Semaphore()
{
    m_count = 0;
}

Semaphore::~Semaphore()
{

}

void Semaphore::Wait()
{
    boost::unique_lock<boost::mutex> lock(m_mutex);
    while (m_count == 0)
    {
        m_condition.wait(lock);
    }
    --m_count;
}

void Semaphore::Post()
{
    {
        boost::lock_guard<boost::mutex> lock(m_mutex);
        ++m_count;
    }

    m_condition.notify_one();
}
