#pragma once 
#include <Protocol/IRequest.h>

namespace ICC
{
	namespace PROTOCOL
	{
        class CGetMergeAlarmRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_oBody.m_strAlarmID = p_pJson->GetNodeValue("/body/alarm_id", "");
				m_oBody.m_strMergeID = p_pJson->GetNodeValue("/body/merge_id", "");
				m_oBody.m_strMergeType = p_pJson->GetNodeValue("/body/merge_type", "");
				m_oBody.m_strIgnorePrivacy = p_pJson->GetNodeValue("/body/ignore_privacy", "false");
				return true;
			}

		public:
			CHeaderEx m_oHeader;
			class CBody
			{
			public:
				std::string	m_strAlarmID;					//����id
				std::string m_strMergeID;						//�ϲ�id
				std::string m_strMergeType;					   //�ϲ�type
				std::string m_strIgnorePrivacy;             //������˽����
			};
			CBody m_oBody;
		};
	}
}
