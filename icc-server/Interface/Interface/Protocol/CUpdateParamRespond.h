#pragma once 
#include <Protocol/IRespond.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CUpdateParamRespond :
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


		class CGetAllParamRespond :
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
				p_pJson->SetNodeValue("/body/count", m_oBody.m_strCount);
				for (unsigned int i = 0; i < m_oBody.m_vData.size();++i)
				{
					p_pJson->SetNodeValue("/body/data/" + std::to_string(i) + "/name", m_oBody.m_vData[i].m_strName);
					p_pJson->SetNodeValue("/body/data/" + std::to_string(i) + "/value", m_oBody.m_vData[i].m_strValue);
					p_pJson->SetNodeValue("/body/data/" + std::to_string(i) + "/is_visibility", m_oBody.m_vData[i].m_strIsVisibility);
					p_pJson->SetNodeValue("/body/data/" + std::to_string(i) + "/is_readonly", m_oBody.m_vData[i].m_strIsReadonly);
					p_pJson->SetNodeValue("/body/data/" + std::to_string(i) + "/create_user", m_oBody.m_vData[i].m_strCreateUser);
					p_pJson->SetNodeValue("/body/data/" + std::to_string(i) + "/description", m_oBody.m_vData[i].m_strDescription);
				}

				return p_pJson->ToString();
			}

		public:
			CHeader m_oHeader;
			class CData
			{
			public:
				std::string m_strName;
				std::string m_strValue;
				std::string m_strIsVisibility;
				std::string m_strIsReadonly;
				std::string m_strCreateUser;
				std::string m_strDescription;
			};
			class CBody
			{
			public:
				std::string m_strCount;
				std::string m_strResult;
				std::vector<CData>m_vData;
			};
			CBody m_oBody;
		};
	}
}
