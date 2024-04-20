#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CSearchStatisticsAlarmRespond :
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
				
				unsigned int l_uiIndex = 0;
				for (const CData& data : m_oBody.m_vecData)
				{
					std::string l_strPrefixPath("/body/data/" + std::to_string(l_uiIndex) + "/");

					p_pJson->SetNodeValue(l_strPrefixPath + "type_code", data.m_strTypeCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "type_name", data.m_strTypeName);
					p_pJson->SetNodeValue(l_strPrefixPath + "dept_code", data.m_strAdminDeptCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "dept_name", data.m_strAdminDeptName);
					p_pJson->SetNodeValue(l_strPrefixPath + "alarm_count", data.m_strAlarmCount);
					p_pJson->SetNodeValue(l_strPrefixPath + "process_count", data.m_strProcessCount);
					p_pJson->SetNodeValue(l_strPrefixPath + "feedback_count", data.m_strFeedbackCount);					

					++l_uiIndex;
				}

				return p_pJson->ToString();
			}

		public:
			class CData
			{
			public:
				std::string	m_strTypeCode;				    //一级警情性质代码
				std::string	m_strTypeName;					//一级警情性质名称
				std::string m_strAdminDeptCode;				//辖区部门代码
				std::string m_strAdminDeptName;				//辖区部门名称
				std::string m_strAlarmCount;				//接警总数量			
				std::string m_strProcessCount; 			    //已处警数量
				std::string m_strFeedbackCount;				//已反馈数量				
			};
			class CBody
			{
			public:
				std::string m_strCount;					//分包总数
				std::string m_strAllCount;		        //符合条件总条数
				std::vector<CData> m_vecData;
			};
			CHeader m_oHeader;
			CBody	m_oBody;
		};
	}
}