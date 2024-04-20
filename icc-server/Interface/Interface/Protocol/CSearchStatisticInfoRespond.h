#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CSearchStatisticInfoRespond :
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
				//p_pJson->SetNodeValue("/body/total_alarm_count", m_oBody.m_strTotalAlarmCount);
				//p_pJson->SetNodeValue("/body/total_invalid_alarm_count", m_oBody.m_strTotalInvalidAlarmCount);

				unsigned int l_uiIndex = 0;
				for (CData data : m_oBody.m_vecData)
				{
					std::string l_strPrefixPath("/body/data/" + std::to_string(l_uiIndex) + "/");

					p_pJson->SetNodeValue(l_strPrefixPath + "client_name", data.m_strClientName);
					p_pJson->SetNodeValue(l_strPrefixPath + "client_id", data.m_strClientID);
					p_pJson->SetNodeValue(l_strPrefixPath + "dept_name", data.m_strDeptName);					
					p_pJson->SetNodeValue(l_strPrefixPath + "online_time", data.m_strOnlineTime);
					p_pJson->SetNodeValue(l_strPrefixPath + "offline_time", data.m_strOfflineTime);
					p_pJson->SetNodeValue(l_strPrefixPath + "online_time_length", data.m_strOnlineTimeLength);
					p_pJson->SetNodeValue(l_strPrefixPath + "offseat_time_length", data.m_strOffSeatTimeLength);
					p_pJson->SetNodeValue(l_strPrefixPath + "release_callref_count", data.m_strReleaseCallRefCount);
					p_pJson->SetNodeValue(l_strPrefixPath + "callref_count", data.m_strCallRefCount);
					p_pJson->SetNodeValue(l_strPrefixPath + "alarm_count", data.m_strAlarmCount);

					++l_uiIndex;
				}

				return p_pJson->ToString();
			}

		public:
			class CData
			{
			public:
				std::string	m_strClientName;			//用户帐号
				std::string m_strClientID;				//客户ID，也是坐席号
				std::string	m_strDeptName;				//用户所在单位名称
				std::string m_strOnlineTime;
				std::string m_strOfflineTime;
				std::string m_strOnlineTimeLength;
				std::string m_strOffSeatTimeLength;
				std::string m_strReleaseCallRefCount;
				std::string m_strCallRefCount;
				std::string	m_strAlarmCount;			//警情数
				//std::string m_strInvalidAlarmCount;	//无效警情数
			};
			class CBody
			{
			public:
				std::string m_strCount;					//分包总数				
				//std::string m_strTotalAlarmCount;		//警情总数
				//std::string m_strTotalInvalidAlarmCount;//无效警情总数
				std::vector<CData> m_vecData;
			};
			CHeader m_oHeader;
			CBody	m_oBody;
		};
	}
}