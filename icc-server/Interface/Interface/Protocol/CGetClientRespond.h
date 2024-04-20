#pragma once 
#include <vector>
#include <Protocol/IRequest.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CGetClientRespond :
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
				for (size_t i = 0; i < m_oBody.m_vecData.size(); i++)
				{
					std::string l_strIndex = std::to_string(i);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/client_id", m_oBody.m_vecData[i].m_strClientID);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/client_type", m_oBody.m_vecData[i].m_strClientType);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/client_name", m_oBody.m_vecData[i].m_strClientName);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/client_ip", m_oBody.m_vecData[i].m_strClientIP);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/staff_code", m_oBody.m_vecData[i].m_strStaffCode);
				    p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/staff_name", m_oBody.m_vecData[i].m_strStaffName);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/district_code", m_oBody.m_vecData[i].m_strDistrictCode);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/dept_code", m_oBody.m_vecData[i].m_strStaffDeptCode);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/dept_name", m_oBody.m_vecData[i].m_strStaffDeptName);
				}
				return p_pJson->ToString();
			}

		public:
			CHeaderEx m_oHeader;
			class CBody
			{
			public:
				std::string m_strCount;
				class CData
				{
				public:
					std::string m_strClientID;
					std::string m_strClientType;
					std::string m_strClientName;
					std::string m_strClientIP;
					std::string m_strStaffCode;		//警员编码
		            std::string m_strStaffName;		//警员姓名

					std::string m_strDistrictCode; //所在行政区划编号					
					std::string m_strStaffDeptCode;
					std::string m_strStaffDeptName;
				};
				std::vector<CData>m_vecData;
			};
			CBody m_oBody;
		};
	}
}
