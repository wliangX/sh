#ifndef __SEMDATALIST_CPP__
#define __SEMDATALIST_CPP__

#include "SemDataList_T.h"

template <class T>
SemDataList<T>::SemDataList()
{
};

template <class T>
SemDataList<T>::~SemDataList()
{

};

template <class T>
void SemDataList<T>::Put(T& t)
{
    boost::lock_guard<boost::mutex> lock(m_Mutex);

    m_Data.push_back(t);

    m_Sem.Post();

    return ;
};

template <class T> 
void SemDataList<T>::Get(T& t)
{
    m_Sem.Wait();

    boost::lock_guard<boost::mutex> lock(m_Mutex);

    typename std::list<T>::iterator it = m_Data.begin();

    t = *it;

    m_Data.pop_front();

    return;
};

#endif

