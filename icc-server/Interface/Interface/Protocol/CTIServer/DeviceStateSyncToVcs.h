#pragma once 
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CDeviceStateToVcs :
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

				p_pJson->SetNodeValue("/body/callref_id", m_oBody.m_strCallRefId);
				p_pJson->SetNodeValue("/body/caller_id", m_oBody.m_strCaller);
				p_pJson->SetNodeValue("/body/called_id", m_oBody.m_strCalled);
				p_pJson->SetNodeValue("/body/called_dept_code", m_oBody.m_strCalledDeptCode);
				p_pJson->SetNodeValue("/body/state", m_oBody.m_strState);
				p_pJson->SetNodeValue("/body/org_called_id", m_oBody.m_strOrgCalled);
				p_pJson->SetNodeValue("/body/time", m_oBody.m_strTime);

				return p_pJson->ToString();
			}

		public:
			CHeader m_oHeader;

			class CBody
			{
			public:
				std::string m_strCallRefId;
				std::string m_strCaller;
				std::string m_strCalled;
				std::string m_strOrgCalled;
				std::string m_strCalledDeptCode;
				std::string m_strState;
				std::string m_strTime;
			};
			CBody m_oBody;
		};
	}
}
