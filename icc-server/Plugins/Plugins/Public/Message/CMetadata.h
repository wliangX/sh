#pragma once

#include <string>
#include <Json/IJson.h>

namespace ICC
{
	// nacosע��ʱЯ����metadata
	class CMetadata
	{
	public:
		CMetadata() = default;
	
		~CMetadata() = default;

	public:
		std::string ToJson(JsonParser::IJsonPtr p_pJson)
		{
			if (NULL == p_pJson.get())
			{
				return "";
			}

			p_pJson->SetNodeValue("/preserved.heart.beat.timeout", m_strBeatTimeOut);
			p_pJson->SetNodeValue("/preserved.ip.delete.timeout", m_strDeleteTimeOut);
			p_pJson->SetNodeValue("/preserved.heart.beat.interval", m_strBeatInterval);

			return p_pJson->ToString();
		}
		
	public:
		//������ʱʱ�䣨15��û��������Ϣ��Ϊ������״̬��
		std::string m_strBeatTimeOut;
		
		//# ɾ��IPʱ��
		std::string m_strDeleteTimeOut;

		//�������ʱ��
		std::string m_strBeatInterval;
	};

}