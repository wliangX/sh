#pragma once 
#include <Protocol/IRequest.h>
#include "Protocol/CSmpHeader.h"
namespace ICC
{
	namespace PROTOCOL
	{
		class CSMPLangInfoChangeNotify :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_Header.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				if (m_Header.m_strAction == ACTION_ADD || m_Header.m_strAction == ACTION_UPDATE)
				{
					for (int i = 0; i < p_pJson->GetCount("/body/dataList"); i++)
					{
						std::string l_strPath = "/body/dataList/";
						std::string l_strNum = std::to_string(i);
						CBody::CData l_CData;
						l_CData.m_strGuid = p_pJson->GetNodeValue(l_strPath + l_strNum + "/guid", "");
						l_CData.m_strLangCode = p_pJson->GetNodeValue(l_strPath + l_strNum + "/language_code", "");
						l_CData.m_strLangName = p_pJson->GetNodeValue(l_strPath + l_strNum + "/describe_name", "");
						l_CData.m_strStatus = p_pJson->GetNodeValue(l_strPath + l_strNum + "/status", "");
						l_CData.m_strSort = p_pJson->GetNodeValue(l_strPath + l_strNum + "/sort", "");
						l_CData.m_strIconPath = p_pJson->GetNodeValue(l_strPath + l_strNum + "/icon_path", "");					
						m_Body.m_vecData.push_back(l_CData);					
					}
					return true;
				}	

				if (m_Header.m_strAction == ACTION_DELETE)
				{
					for (int i = 0; i < p_pJson->GetCount("/body/dataList"); i++)
					{
						std::string l_strPath = "/body/dataList/";
						std::string l_strNum = std::to_string(i);
						CBody::CData l_CData;
						l_CData.m_strGuid = p_pJson->GetNodeValue(l_strPath + l_strNum + "/guid", "");
						m_Body.m_vecData.push_back(l_CData);
					}
					return true;
				}
				return true;
			}
			public:
				class CBody
				{
				public:
					class CData
					{
					public:
						std::string m_strGuid;
						std::string m_strLangCode;		//”Ô—‘±‡¬Î
						std::string m_strLangName;		//”Ô—‘√˚≥∆
						std::string m_strStatus;		//”Ô—‘◊¥Ã¨£®Y|N£©
						std::string m_strIconPath;		
						std::string m_strSort;
					};
					std::vector<CData>m_vecData;
				};

				CSmpHeader m_Header;
				CBody m_Body;
			};
		}
	}
