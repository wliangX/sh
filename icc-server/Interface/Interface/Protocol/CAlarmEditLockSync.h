#pragma once 
#include <Protocol/ISync.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CAlarmEditLockSync :
			public ISync
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);
				p_pJson->SetNodeValue("/body/alarm_id", m_oBody.m_strId);
				p_pJson->SetNodeValue("/body/user_code", m_oBody.m_strCode);
				p_pJson->SetNodeValue("/body/lock_status", m_oBody.m_strLockStatus);
				return p_pJson->ToString();
			}

		public:
			CHeader m_oHeader;
			class CBody
			{
			public:
				std::string m_strId;		        //警情ID
				std::string m_strCode;			    //用户名
				std::string m_strLockStatus;			//锁定时长
			};
			CBody m_oBody;
		};
	}
}
