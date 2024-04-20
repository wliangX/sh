#ifndef __DATALIST_H__
#define __DATALIST_H__

#include <list>

#include "SemaphoreEx.h"
#include "Boost.h"

template<class T> 
class SemDataList
{
public:
    SemDataList();
    ~SemDataList();
    void Put(T& t);
    void Get(T& t);

private:  
    Semaphore     m_Sem;
    boost::mutex  m_Mutex;
    std::list<T>  m_Data;    
};

#include "SemDataList_T.cpp"

#endif

