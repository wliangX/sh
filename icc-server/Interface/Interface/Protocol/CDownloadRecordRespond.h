#pragma once 
#include <Protocol/IRespond.h>
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CReceiveDownloadRecordRespond :
			public IRespond, IRequest
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				p_pJson->SetNodeValue("/code", m_oBody.m_strCode);
				p_pJson->SetNodeValue("/message", m_oBody.m_strMessage);
				p_pJson->SetNodeValue("/msgid", m_oBody.m_strMsgId);
				int iCount = m_oBody.m_vecData.size();
				for (int i = 0; i < iCount; ++i)
				{
					std::string l_strPrefixPath("/data/" + std::to_string(i));
					p_pJson->SetNodeValue(l_strPrefixPath + "/trafficId", m_oBody.m_vecData.at(i).m_strTrafficId);
					p_pJson->SetNodeValue(l_strPrefixPath + "/callId", m_oBody.m_vecData.at(i).m_strCallId);
					p_pJson->SetNodeValue(l_strPrefixPath + "/guid", m_oBody.m_vecData.at(i).m_strGuid);
					p_pJson->SetNodeValue(l_strPrefixPath + "/httpUrl", m_oBody.m_vecData.at(i).m_strHttpUrl);
					p_pJson->SetNodeValue(l_strPrefixPath + "/httpsUrl", m_oBody.m_vecData.at(i).m_strHttpsUrl);
					p_pJson->SetNodeValue(l_strPrefixPath + "/startTime", m_oBody.m_vecData.at(i).m_strStartTime);
					p_pJson->SetNodeValue(l_strPrefixPath + "/endTime", m_oBody.m_vecData.at(i).m_strEndTime);
					p_pJson->SetNodeValue(l_strPrefixPath + "/callDuration", m_oBody.m_vecData.at(i).m_strCallDuration);
					p_pJson->SetNodeValue(l_strPrefixPath + "/caller", m_oBody.m_vecData.at(i).m_strCaller);
					p_pJson->SetNodeValue(l_strPrefixPath + "/called", m_oBody.m_vecData.at(i).m_strCalled);
				}
				return p_pJson->ToString();
			}

			virtual bool  ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return false;
				}
				if (!p_pJson->LoadJson(p_strReq))
				{
					return false;
				}

				m_oBody.m_strCode=p_pJson->GetNodeValue("/code", m_oBody.m_strCode);
				m_oBody.m_strMessage=p_pJson->GetNodeValue("/message", m_oBody.m_strMessage);
				int iCount = p_pJson->GetCount("/data");;
				for (int i = 0; i < iCount; i++)
				{
					std::string l_strPrefixPath("/data/" + std::to_string(i));
					CBody::CData l_oData;
					l_oData.m_strTrafficId = p_pJson->GetNodeValue(l_strPrefixPath+"/trafficId", "");
					l_oData.m_strCallId = p_pJson->GetNodeValue(l_strPrefixPath + "/callId", "");
					l_oData.m_strGuid = p_pJson->GetNodeValue(l_strPrefixPath + "/guid", "");
					l_oData.m_strHttpUrl = p_pJson->GetNodeValue(l_strPrefixPath + "/httpUrl", "");
					l_oData.m_strHttpsUrl = p_pJson->GetNodeValue(l_strPrefixPath + "/httpsUrl", "");
					l_oData.m_strStartTime = p_pJson->GetNodeValue(l_strPrefixPath + "/startTime", "");
					l_oData.m_strEndTime = p_pJson->GetNodeValue(l_strPrefixPath + "/endTime", "");
					l_oData.m_strCallDuration = p_pJson->GetNodeValue(l_strPrefixPath + "/callDuration", "");
					l_oData.m_strCaller = p_pJson->GetNodeValue(l_strPrefixPath + "/caller", "");
					l_oData.m_strCalled = p_pJson->GetNodeValue(l_strPrefixPath + "/called", "");
					m_oBody.m_vecData.push_back(l_oData);
				}
				return true;
			}
		public:
			CHeaderEx m_oHeader;
			class CBody
			{
			public:
				class CData{
				public:
					std::string m_strTrafficId;
					std::string m_strCallId;
					std::string m_strGuid;	
					std::string m_strHttpUrl;   
					std::string m_strHttpsUrl;   
					std::string m_strStartTime;
					std::string m_strEndTime;  
					std::string m_strCallDuration;
					std::string m_strCaller;
					std::string m_strCalled;

					std::string m_strRecordId;	//录音ID
					std::string m_strRecord_type;
				};
				std::vector<CData> m_vecData;
				std::string m_strCode;
				std::string m_strMessage;
				std::string m_strMsgId;	//方便排查日志
			};
			CBody m_oBody;
		};
	}
}
