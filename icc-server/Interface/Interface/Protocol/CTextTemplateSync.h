#pragma once 
#include <Protocol/ISync.h>

namespace ICC
{
	namespace PROTOCOL
	{
        class CTextTemplateSync :
			public ISync
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);

                p_pJson->SetNodeValue("/body/sync_type", m_oBody.m_strSyncType);
				p_pJson->SetNodeValue("/body/guid", m_oBody.m_strGuid);
				p_pJson->SetNodeValue("/body/type", m_oBody.m_strType);
				p_pJson->SetNodeValue("/body/content", m_oBody.m_strContent);
				p_pJson->SetNodeValue("/body/alarm_type", m_oBody.m_strAlarm_type);
				p_pJson->SetNodeValue("/body/handly_type", m_oBody.m_strHandly_type);

				return p_pJson->ToString();
			}

		public:
			CHeader m_oHeader;
			class CBody
			{
			public:
                std::string m_strGuid;		        //模板guid
				std::string m_strType;			    //模板类型
				std::string m_strContent;			//模板内容
				std::string m_strSyncType;          //同步类型
				std::string m_strAlarm_type;		//警情类型
				std::string m_strHandly_type;		//来话类型
			};
			CBody m_oBody;
		};
	}
}
