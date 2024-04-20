#pragma once
#include <Lock/ILock.h>

//boost
#include <boost/thread.hpp>

namespace ICC
{
	namespace Lock
	{
		/*
		* class   互斥锁实现类
		* author  w16314
		* purpose
		* note
		*/
		class Mutex : public ILock
		{
		public:
			Mutex();
			virtual ~Mutex();
		public:
			virtual void Lock();
			virtual void Lock(int msec);
			virtual void Unlock();
			virtual bool TryLock();
		private:
			boost::mutex m_mutex;
		};

		/*
		* class   时间锁实现类
		* author  w16314
		* purpose
		* note
		*/
		class TimeMutex : public ILock
		{
		public:
			TimeMutex();
			virtual ~TimeMutex();
		public:
			virtual void Lock();
			virtual void Lock(int msec);
			virtual void Unlock();
			virtual bool TryLock();
		private:
			boost::timed_mutex m_timeMutex;
		};


		/*
		* class   递归锁实现类
		* author  w16314
		* purpose
		* note
		*/
		class RecursiveMutex : public ILock
		{
		public:
			RecursiveMutex();
			virtual ~RecursiveMutex();
		public:
			virtual void Lock();
			virtual void Lock(int msec);
			virtual void Unlock();
			virtual bool TryLock();
		private:
			boost::recursive_mutex m_recursiveMutex;
		};

		/*
		* class   时间递归锁实现类
		* author  w16314
		* purpose
		* note
		*/
		class RecursiveTimeMutex : public ILock
		{
		public:
			RecursiveTimeMutex();
			virtual ~RecursiveTimeMutex();
		public:
			virtual void Lock();
			virtual void Lock(int msec);
			virtual void Unlock();
			virtual bool TryLock();
		private:
			boost::recursive_timed_mutex m_recursiveTimeMutex;
		};
	}
}