#pragma once

#include <IResource.h>
#include <Lock/ILock.h>

namespace ICC
{
	namespace Lock
	{
		enum LockType
		{
			TypeMutex,
			TypeTimeMutex,
			TypeRecursiveMutex,
			TypeRecursiveTimeMutex
		};

		class ILockFactory : public IResource
		{
		public:
			//************************************
			// Method:    CreateLock
			// FullName:  ACS::ILockFactory::CreateLock
			// Access:    virtual public 
			// Returns:   ACS::ILockPtr
			// Qualifier:
			// Parameter: LockType p_elockType
			//************************************
			virtual ILockPtr CreateLock(LockType p_elockType) = 0;
		};

		typedef boost::shared_ptr<ILockFactory> ILockFactoryPtr;
	}
}

#define ICCILockFactoryResourceName "ICC.Com.ILockFactory.ResourceName"

#define ICCSetILockFactory(ImplClass) \
this->GetResourceManager()->SetResource(ICCILockFactoryResourceName, boost::make_shared<ImplClass>(this->GetResourceManager())) \

#define ICCSetILockFactoryNull \
this->GetResourceManager()->SetResource(ICCILockFactoryResourceName, nullptr) \

#define ICCGetILockFactory() \
boost::dynamic_pointer_cast<Lock::ILockFactory>(this->GetResourceManager()->GetResource(ICCILockFactoryResourceName))
