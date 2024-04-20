#pragma once
#include <Protocol/IRespond.h>
namespace ICC
{
	namespace PROTOCOL
	{
		class CGetViolationRespond:
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

				p_pJson->SetNodeValue("/body/count", m_oBody.m_strCount);
				p_pJson->SetNodeValue("/body/all_count", m_oBody.m_strAllCount);
				//p_pJson->SetNodeValue("/body/page_index", m_oBody.m_strPageIndex);				
				for (size_t i = 0; i < m_oBody.m_vecData.size(); i++)
				{
					std::string l_strValueNum = std::to_string(i);
					p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/guid", m_oBody.m_vecData.at(i).m_strGuid);

					p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/violation_id", m_oBody.m_vecData.at(i).m_strViolationID);
					p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/violation_time", m_oBody.m_vecData.at(i).m_strViolationTime);
					p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/violation_type", m_oBody.m_vecData.at(i).m_strViolationType);
					p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/violation_target", m_oBody.m_vecData.at(i).m_strViolationTarget);
					p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/violation_user", m_oBody.m_vecData.at(i).m_strViolationUser);
					p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/dept_code", m_oBody.m_vecData.at(i).m_strDeptCode);
				}

				return p_pJson->ToString();
			}

		public:
			CHeaderEx m_oHeader;
			class CBody
			{
			public:
				std::string m_strCount = "0";
				std::string m_strAllCount;
				
				class CData
				{
				public:
					std::string m_strGuid;
					std::string m_strViolationTime; 
					std::string m_strViolationType;
					std::string m_strViolationTarget;
					std::string m_strViolationUser;
					std::string m_strViolationID;
					std::string m_strDeptCode;
				};
				std::vector<CData>m_vecData;
			};
			CBody m_oBody;
		};
	}
}
