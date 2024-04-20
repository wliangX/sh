#include "Boost.h"
#include "MessageCenterFactoryImpl.h"
#include "MessageCenterImpl.h"

ICCFactoryCppInitialize(CMessageCenterFactoryImpl)

boost::mutex CMessageCenterFactoryImpl::sm_Mutex;
IMessageCenterPtr CMessageCenterFactoryImpl::sm_pMsgCenter = nullptr;

IMessageCenterPtr CMessageCenterFactoryImpl::CreateMessageCenter()
{
	return Instance(m_IResourceManagerPtr);
}

IMessageCenterPtr& CMessageCenterFactoryImpl::Instance(IResourceManagerPtr p_pResourceManagerPtr)
{
	if (nullptr == sm_pMsgCenter)
	{
		boost::lock_guard<boost::mutex> lock(sm_Mutex);
		if (nullptr == sm_pMsgCenter)
		{
			sm_pMsgCenter = boost::make_shared<CMessageCenterImpl>(p_pResourceManagerPtr);
		}
	}
	return sm_pMsgCenter;
}
