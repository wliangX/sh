#pragma once 

#include <Protocol/IRespond.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CPhoneMarkSync :
			public IRespond
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);

				p_pJson->SetNodeValue("/body/sync_type", m_oBody.m_strSyncType);
				p_pJson->SetNodeValue("/body/guid", m_oBody.m_strGuid);
				p_pJson->SetNodeValue("/body/phone", m_oBody.m_strPhone);
				p_pJson->SetNodeValue("/body/type", m_oBody.m_strType);
				p_pJson->SetNodeValue("/body/staff_code", m_oBody.m_strStaffCode);
				p_pJson->SetNodeValue("/body/remark", m_oBody.m_strRemark);

				return p_pJson->ToString();
			}

		public:
			CHeader m_oHeader;
			class CBody
			{
			public:
				std::string m_strSyncType;
				std::string m_strGuid;
				std::string m_strPhone;		
				std::string m_strType; 
				std::string m_strStaffCode;
				std::string m_strRemark;
			};
			CBody m_oBody;
		};
	}
}
