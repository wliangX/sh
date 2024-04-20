#pragma once 
#include <Protocol/IRespond.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CUploadConfigRespond :
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

				p_pJson->SetNodeValue("/body/result", m_oBody.m_strResult);

				return p_pJson->ToString();
			}

		public:
			CHeader m_oHeader;
			class CBody
			{
			public:
				std::string m_strResult;
			};
			CBody m_oBody;
		};


		class CGetAllConfigRespond :
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

				for (unsigned int i = 0; i < m_oBody.m_vData.size();++i)
				{
					p_pJson->SetNodeValue("/body/data/" + std::to_string(i) + "/ip", m_oBody.m_vData[i].m_strIP);
					p_pJson->SetNodeValue("/body/data/" + std::to_string(i) + "/config", m_oBody.m_vData[i].m_strConfigValue);
				}

				return p_pJson->ToString();
			}

		public:
			CHeader m_oHeader;
			class CData
			{
			public:
				std::string m_strIP;
				std::string m_strConfigValue;
			};
			class CBody
			{
			public:
				std::string m_strCount;
				std::vector<CData>m_vData;
			};
			CBody m_oBody;
		};
	}
}
