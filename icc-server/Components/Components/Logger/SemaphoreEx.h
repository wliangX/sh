#ifndef __SEMAPHOREEX_H__
#define __SEMAPHOREEX_H__

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#endif

#include "Boost.h"


class Semaphore
{
public:
    Semaphore();

    ~Semaphore();

    void Wait();

    void Post();

private:
    unsigned int              m_count;
    boost::mutex              m_mutex;
    boost::condition_variable m_condition;
};

#endif
