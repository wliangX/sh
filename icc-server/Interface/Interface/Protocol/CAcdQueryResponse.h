#pragma once 
#include <Protocol/IRespond.h>
#include <Protocol/IRequest.h>
#include "Protocol/CHeader.h"
namespace ICC
{
	namespace PROTOCOL
	{
		class CAcdQueryResponse :
			public IRespond
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				m_oHeader.SaveTo(p_pJson);

				p_pJson->SetNodeValue("/body/count", std::to_string(m_oBody.m_vecData.size()));
				for (unsigned int i = 0; i < m_oBody.m_vecData.size(); i++)
				{
					std::string l_strPrefixPath("/body/data/" + std::to_string(i) + "/");
					p_pJson->SetNodeValue(l_strPrefixPath + "acd", m_oBody.m_vecData[i].m_acd);
					p_pJson->SetNodeValue(l_strPrefixPath + "dept_code", m_oBody.m_vecData[i].m_dept_code);
					p_pJson->SetNodeValue(l_strPrefixPath + "dept_name", m_oBody.m_vecData[i].m_dept_name);
					p_pJson->SetNodeValue(l_strPrefixPath + "remark", m_oBody.m_vecData[i].m_remark);
					p_pJson->SetNodeValue(l_strPrefixPath + "dept_code_path", m_oBody.m_vecData[i].m_dept_code_path);
					p_pJson->SetNodeValue(l_strPrefixPath + "dept_name_path", m_oBody.m_vecData[i].m_dept_name_path);
				}

				return p_pJson->ToString();
			}


		public:
			CHeaderEx m_oHeader;

			class CData
			{
			public:
				std::string m_acd;
				std::string m_dept_code;
				std::string m_dept_name;
				std::string m_remark;
				std::string m_dept_code_path;
				std::string m_dept_name_path;
			};

			class CBody
			{
			public:
				std::vector<CData> m_vecData;
			};
			CBody m_oBody;
		};
	}
}