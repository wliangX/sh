#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CSmsSendSmsSync :
			public IReceive
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strSyncType = p_pJson->GetNodeValue("/body/sync_type", "");
				m_oBody.m_strThirdType = p_pJson->GetNodeValue("/body/third_type", "");
				m_oBody.m_strOrientation = p_pJson->GetNodeValue("/body/orientation", "");
				m_oBody.m_strGuid = p_pJson->GetNodeValue("/body/guid", "");
				m_oBody.m_strTAGuid = p_pJson->GetNodeValue("/body/third_alarm_guid", "");
				m_oBody.m_strTelnum = p_pJson->GetNodeValue("/body/phone", "");
				m_oBody.m_strContent = p_pJson->GetNodeValue("/body/change_content", "");
				return true;
			}

		public:
			class CBody
			{
			public:
				std::string	m_strSyncType;			//1：表示添加，2：表示更新，3：表示删除
				std::string	m_strThirdType;			//第三方字典值
				std::string	m_strOrientation;		//0报警人给警员发送，1警员给报警人发送。
				std::string m_strGuid;
				std::string m_strTAGuid;
				std::string m_strTelnum;            //接收方号码
				std::string m_strContent;			//短信内容
			};
			CHeader m_oHeader;
			CBody m_oBody;
		};		
	}
}
