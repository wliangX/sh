#pragma once

#include <string>
#include <IObject.h>

namespace ICC
{ 
	namespace Lock
	{
		/*
		* class   锁接口类
		* author  w16314
		* purpose
		* note
		*/
		class ILock : public IObject
		{
		public:
			//************************************
			// Method:    Lock
			// FullName:  ACS::ILock::Lock
			// Access:    virtual public 
			// Returns:   void
			// Qualifier:
			//************************************
			virtual void Lock() = 0;
			//************************************
			// Method:    Lock
			// FullName:  ACS::ILock::Lock
			// Access:    virtual public 
			// Returns:   void
			// Qualifier:
			// Parameter: int msec
			//************************************
			virtual void Lock(int msec) = 0;
			//************************************
			// Method:    Unlock
			// FullName:  ACS::ILock::Unlock
			// Access:    virtual public 
			// Returns:   void
			// Qualifier:
			//************************************
			virtual void Unlock() = 0;
			//************************************
			// Method:    TryLock
			// FullName:  ACS::ILock::TryLock
			// Access:    virtual public 
			// Returns:   bool
			// Qualifier:
			//************************************
			virtual bool TryLock() = 0;
		};

		typedef boost::shared_ptr<ILock> ILockPtr;

		/*
		* class   自动锁
		* author  w16314
		* purpose
		* note
		*/
		class AutoLock
		{
		public:
			AutoLock(ILockPtr p_LockPtr) : m_LockPtr(p_LockPtr)
			{
				m_LockPtr->Lock();
			}
			~AutoLock()
			{
				m_LockPtr->Unlock();
			}
		private:
			ILockPtr m_LockPtr;
		};
	}
}
