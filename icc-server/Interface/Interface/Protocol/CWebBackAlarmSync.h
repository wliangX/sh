#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CWebBackAlarmSync :
			public ISend
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);
				
				p_pJson->SetNodeValue("/body/data/alarm_id", m_oBody.m_oData.m_strAlarmID);
				p_pJson->SetNodeValue("/body/data/process_id", m_oBody.m_oData.m_strProcessID);
				p_pJson->SetNodeValue("/body/data/dispatch_id", m_oBody.m_oData.m_strDispatchID);
				p_pJson->SetNodeValue("/body/data/dispatch_name", m_oBody.m_oData.m_strDispatchName);
				p_pJson->SetNodeValue("/body/data/back_user_id", m_oBody.m_oData.m_strBackUserID);
				p_pJson->SetNodeValue("/body/data/back_user_name", m_oBody.m_oData.m_strBackUserName);
				p_pJson->SetNodeValue("/body/data/back_dept_id", m_oBody.m_oData.m_strBackDeptID);
				p_pJson->SetNodeValue("/body/data/back_dept_name", m_oBody.m_oData.m_strBackDeptName);
				p_pJson->SetNodeValue("/body/data/reason", m_oBody.m_oData.m_strReason);

				return p_pJson->ToString();
			}

		public:
			class CData
			{
			public:
				std::string m_strAlarmID;						//警情id
				std::string m_strProcessID;						//处警单id
				std::string m_strDispatchID;					//调派人id
				std::string m_strDispatchName;					//调派人姓名
				std::string m_strBackUserID;					//退警人id
				std::string m_strBackUserName;					//退警人姓名
				std::string m_strBackDeptID;					//退警单位id
				std::string m_strBackDeptName;					//退警单位名称
				std::string m_strReason;						//原因
			};
			class CBody
			{
			public:
				CData m_oData;
			};
			CHeader m_oHeader;
			CBody m_oBody;
		};
	}
}