#include "Boost.h"
#include "ResultSetCounter.h"

CResultSetCounter* CResultSetCounter::sm_instance = nullptr;
std::mutex CResultSetCounter::sm_mutexInstance;

CResultSetCounter* CResultSetCounter::Instance()
{
	if (nullptr == sm_instance)
	{
		std::lock_guard<std::mutex> lock(sm_mutexInstance);
		if (nullptr == sm_instance)
		{
			sm_instance = new CResultSetCounter();
		}
	}
	return sm_instance;

}

void CResultSetCounter::AddCounter()
{
	std::lock_guard<std::mutex> lock(m_mutexResultSetCounter);
	m_uResultSetCounter++;
}

void CResultSetCounter::ReduceCounter()
{
	std::lock_guard<std::mutex> lock(m_mutexResultSetCounter);	
	if (m_uResultSetCounter != 0)
	{
		m_uResultSetCounter--;
	}
}

unsigned int CResultSetCounter::GetCounter()
{
	std::lock_guard<std::mutex> lock(m_mutexResultSetCounter);
	return m_uResultSetCounter;
}


/*****************************************************************
 * 
 ****************************************************************/

CResultSetCounter::CResultSetCounter()
{
	m_uResultSetCounter = 0;
}

CResultSetCounter::~CResultSetCounter()
{

}