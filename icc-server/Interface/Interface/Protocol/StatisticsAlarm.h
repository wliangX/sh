#ifndef __StatisticsAlarm_H__
#define __StatisticsAlarm_H__

#include <Protocol/CHeader.h>
#include <Protocol/IRespond.h>
#include <vector>
namespace ICC
{
	namespace PROTOCOL
	{
		class CStatisticsAlarm : public ISend
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);

				p_pJson->SetNodeValue("/body/result", m_oBody.m_strResult);
				p_pJson->SetNodeValue("/body/total_count", m_oBody.m_strTotalCount);	
				p_pJson->SetNodeValue("/body/time", m_oBody.m_strTime);

				unsigned int l_uiInfoIndex = 0;
				unsigned int l_uiProcessIndex = 0;
				unsigned int l_uiRemarkIndex = 0;
				unsigned int l_uiFeedbackIndex = 0;
				for (auto info : m_oBody.m_vecInfos)
				{
					std::string l_strAlarmPrefixPath("/body/infos/" + std::to_string(l_uiInfoIndex) + "/");

					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "dept_code", info.strDeptCode);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "dept_name", info.strDeptName);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "alarm_num", info.strAlarmCount);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "release_num", info.strCallReleaseCount);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "call_num", info.strCallRefCount);
					l_uiInfoIndex++;
				}

				return p_pJson->ToString();
			}
		
			class CInfo
			{
			public:
				std::string strDeptCode;
				std::string strDeptName;
				std::string strAlarmCount;
				std::string strCallRefCount;
				std::string strCallReleaseCount;
			};
			class CBody
			{
			public:
				std::string m_strResult;
				std::string m_strTotalCount;						// 记录总数		
				std::string m_strTime;
				std::vector<CInfo> m_vecInfos;						// 警情信息
			};
			CHeader m_oHeader;
			CBody	m_oBody;
		};

	}
}

#endif