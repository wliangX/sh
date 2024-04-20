#pragma once
#include <Lock/ILock.h>

//boost
#include <boost/thread.hpp>

namespace ICC
{
	namespace Lock
	{
		/*
		* class   ������ʵ����
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
		* class   ʱ����ʵ����
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
		* class   �ݹ���ʵ����
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
		* class   ʱ��ݹ���ʵ����
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