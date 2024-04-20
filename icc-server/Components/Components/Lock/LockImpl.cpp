#include "Boost.h"
#include "LockImpl.h"

Mutex::Mutex()
{

}

Mutex::~Mutex()
{

}

void Mutex::Lock()
{
	try
	{
		m_mutex.lock();
	}
	catch (...)
	{

	}
}

void Mutex::Lock(int msec)
{

}

void Mutex::Unlock()
{
	try
	{
		m_mutex.unlock();
	}
	catch (...)
	{

	}
}

bool Mutex::TryLock()
{
	try
	{
		return m_mutex.try_lock();
	}
	catch (...)
	{

	}
	return false;
}

TimeMutex::TimeMutex()
{

}

TimeMutex::~TimeMutex()
{

}

void TimeMutex::Lock()
{
	try
	{
		m_timeMutex.lock();
	}
	catch (...)
	{

	}
}

void TimeMutex::Lock(int msec)
{
	try
	{
		m_timeMutex.timed_lock(boost::posix_time::microseconds(msec));
	}
	catch (...)
	{

	}
}

void TimeMutex::Unlock()
{
	try
	{
		m_timeMutex.unlock();
	}
	catch (...)
	{

	}
}

bool TimeMutex::TryLock()
{
	try
	{
		return m_timeMutex.try_lock();
	}
	catch (...)
	{

	}

	return false;
}

RecursiveMutex::RecursiveMutex()
{

}

RecursiveMutex::~RecursiveMutex()
{

}

void RecursiveMutex::Lock()
{
	try
	{
		m_recursiveMutex.lock();
	}
	catch (...)
	{

	}
}

void RecursiveMutex::Lock(int msec)
{

}

void RecursiveMutex::Unlock()
{
	try
	{
		m_recursiveMutex.unlock();
	}
	catch (...)
	{

	}
}

bool RecursiveMutex::TryLock()
{
	try
	{
		return m_recursiveMutex.try_lock();
	}
	catch (...)
	{

	}
	return false;
}



RecursiveTimeMutex::RecursiveTimeMutex()
{

}

RecursiveTimeMutex::~RecursiveTimeMutex()
{

}

void RecursiveTimeMutex::Lock()
{
	m_recursiveTimeMutex.lock();
}

void RecursiveTimeMutex::Lock(int msec)
{
	m_recursiveTimeMutex.timed_lock(boost::posix_time::microseconds(msec));
}

void RecursiveTimeMutex::Unlock()
{
	m_recursiveTimeMutex.unlock();
}

bool RecursiveTimeMutex::TryLock()
{
	return m_recursiveTimeMutex.try_lock();
}