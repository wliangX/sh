#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CWebBackAlarmRequest :
			public IReceive
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_oBody.m_strToken = p_pJson->GetNodeValue("/body/token", "");
				m_oBody.m_oData.m_strAlarmID = p_pJson->GetNodeValue("/body/data/alarm_id", "");
				m_oBody.m_oData.m_strProcessID = p_pJson->GetNodeValue("/body/data/process_id", "");
				m_oBody.m_oData.m_strBackUserID = p_pJson->GetNodeValue("/body/data/back_user_id", "");
				m_oBody.m_oData.m_strBackUserName = p_pJson->GetNodeValue("/body/data/back_user_name", "");
				m_oBody.m_oData.m_strBackDeptID = p_pJson->GetNodeValue("/body/data/back_dept_id", "");
				m_oBody.m_oData.m_strBackDeptName = p_pJson->GetNodeValue("/body/data/back_dept_name", "");
				m_oBody.m_oData.m_strReason = p_pJson->GetNodeValue("/body/data/reason", "");
				return true;
			}

		public:
			class CData
			{
			public:
				std::string m_strAlarmID;						//警情id
				std::string m_strProcessID;						//处警单id
				std::string m_strBackUserID;					//退警人id
				std::string m_strBackUserName;					//退警人姓名
				std::string m_strBackDeptID;					//退警单位id
				std::string m_strBackDeptName;					//退警单位名称
				std::string m_strReason;						//原因
			};
			class CBody
			{
			public:
				std::string m_strToken;					//授权码
				CData m_oData;
			};
			CHeader m_oHeader;
			CBody m_oBody;
		};
	}
}
