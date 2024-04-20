#pragma once 
#include <Protocol/CHeader.h>
#include <Protocol/IRespond.h>
#include <Protocol/IRequest.h>
#include <Protocol/CFeekBackInfo.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CFeedBackExSync :
			public IRespond,public IRequest
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);
				std::string l_strPrefixPath("/body/");
				p_pJson->SetNodeValue(l_strPrefixPath + "sync_type", m_oBody.m_strSyncType);
				m_oBody.m_oFeekBackInfo.ComJson("/body", p_pJson);
				
				return p_pJson->ToString();
			}

            virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
            {
                if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				//m_oBody.m_oAlarmInfo.m_strMsgSource = p_pJson->GetNodeValue("/body/msg_source", "");
				m_oBody.m_strSyncType = p_pJson->GetNodeValue("/body/sync_type", "");
				m_oBody.m_oFeekBackInfo.ParseString("/body", p_pJson);
			

                return true;
            }

		public:
			class CBody
			{
			public:
				std::string m_strSyncType;		//同步类型 1:add ,2:update,3:delete
				CFeekBackInfo m_oFeekBackInfo;    //反馈消息体
			};
			CHeaderEx m_oHeader;			
			CBody	m_oBody;
		};
	}
}
