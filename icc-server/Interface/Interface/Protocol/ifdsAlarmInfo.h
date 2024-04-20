#ifndef __IfdsAlarmInfo_H__
#define __IfdsAlarmInfo_H__

#include <Protocol/CHeader.h>
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CIfdsAlarmInfo : public ISend
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);

				p_pJson->SetNodeValue("/body/total_count", m_oBody.m_strTotalCount);

				unsigned int l_uiInfoIndex = 0;
				unsigned int l_uiProcessIndex = 0;
				unsigned int l_uiRemarkIndex = 0;
				unsigned int l_uiFeedbackIndex = 0;
				for (auto info : m_oBody.m_vecInfos)
				{
					std::string l_strAlarmPrefixPath("/body/infos/" + std::to_string(l_uiInfoIndex) + "/");

					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "alarm_id", info.m_strAlarmId);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "content", info.m_strContent);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "addr", info.m_strAddr);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "caller_no", info.m_strCallerNo);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "caller_name", info.m_strCallerName);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "time", info.m_strTime);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "actual_occur_time", info.m_strActualOccurTime);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "state", info.m_strState);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "state_code", info.m_strStateCode);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "first_type_code", info.m_strFirstTypeCode);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "first_type", info.m_strFirstType);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "second_type", info.m_strSecondType);
					p_pJson->SetNodeValue(l_strAlarmPrefixPath + "second_type_code", info.m_strSecondTypeCode);
					l_uiInfoIndex++;
				}				

				return p_pJson->ToString();
			}

			class CActualAlarmInfo
			{
			public:
				std::string m_strAlarmId;
				std::string m_strContent;
				std::string m_strCallerNo;
				std::string m_strCallerName;
				std::string m_strTime;
				std::string m_strAddr;
				std::string m_strActualOccurTime;
				std::string m_strState;
				std::string m_strStateCode;
				std::string m_strFirstTypeCode;
				std::string m_strFirstType;
				std::string m_strSecondType;
				std::string m_strSecondTypeCode;

			public:
				CActualAlarmInfo(){}
				~CActualAlarmInfo(){}
				CActualAlarmInfo(const CActualAlarmInfo& other)
				{
					_Assign(other);
				}
				CActualAlarmInfo& operator=(const CActualAlarmInfo& other)
				{
					if (&other != this)
					{
						_Assign(other);
					}
					return *this;
				}
				void _Assign(const CActualAlarmInfo& other)
				{
					m_strAlarmId = other.m_strAlarmId;
					m_strContent = other.m_strContent;
					m_strCallerNo = other.m_strCallerNo;
					m_strCallerName = other.m_strCallerName;
					m_strTime = other.m_strTime;
					m_strAddr = other.m_strAddr;
					m_strActualOccurTime = other.m_strActualOccurTime;
					m_strState = other.m_strState;
					m_strStateCode = other.m_strStateCode;
					m_strFirstTypeCode = other.m_strFirstTypeCode;
					m_strFirstType = other.m_strFirstType;
					m_strSecondType = other.m_strSecondType;
					m_strSecondTypeCode = other.m_strSecondTypeCode;
				}
			};
			
			class CBody
			{
			public:
				std::string m_strTotalCount;						// 记录总数						
				std::vector<CActualAlarmInfo> m_vecInfos;						// 警情信息
			};
			CHeader m_oHeader;
			CBody	m_oBody;
		};

	}
}

#endif