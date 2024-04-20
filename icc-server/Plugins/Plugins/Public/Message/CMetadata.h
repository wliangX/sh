#pragma once

#include <string>
#include <Json/IJson.h>

namespace ICC
{
	// nacos注册时携带的metadata
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
		//心跳超时时间（15秒没收心跳信息变为不健康状态）
		std::string m_strBeatTimeOut;
		
		//# 删除IP时间
		std::string m_strDeleteTimeOut;

		//心跳间隔时间
		std::string m_strBeatInterval;
	};

}