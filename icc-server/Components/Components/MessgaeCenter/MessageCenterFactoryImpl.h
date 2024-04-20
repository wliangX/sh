#pragma once

#include <MessageCenter/IMessageCenterFactory.h>

namespace ICC
{
	namespace MsgCenter
	{
		class CMessageCenterFactoryImpl :
			public IMessageCenterFactory
		{
			ICCFactoryHeaderInitialize(CMessageCenterFactoryImpl)
		public:
			virtual IMessageCenterPtr CreateMessageCenter();
		private:
			static IMessageCenterPtr& Instance(IResourceManagerPtr p_pResourceManagerPtr);
		private:
			static boost::mutex sm_Mutex;
			IMessageCenterPtr m_pMsgCenter;
			static IMessageCenterPtr sm_pMsgCenter;
		};
	}
}