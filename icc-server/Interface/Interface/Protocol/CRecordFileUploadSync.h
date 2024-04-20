#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CRecordFileUploadSync :
			public IRequest
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (p_pJson == nullptr)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);
				std::string l_strPrefixPath("/body/");
				p_pJson->SetNodeValue(l_strPrefixPath + "file_id", m_oBody.m_strFileID);
				p_pJson->SetNodeValue(l_strPrefixPath + "callref_id", m_oBody.m_strCallRefID);
				p_pJson->SetNodeValue(l_strPrefixPath + "agent", m_oBody.m_strAgentID);
				p_pJson->SetNodeValue(l_strPrefixPath + "start_time", m_oBody.m_strStartTime);
				p_pJson->SetNodeValue(l_strPrefixPath + "file_name", m_oBody.m_strFileName);
				p_pJson->SetNodeValue(l_strPrefixPath + "result", m_oBody.m_strResult);

				return p_pJson->ToString();
			}

			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strFileID = p_pJson->GetNodeValue("/body/file_id", "");
				m_oBody.m_strCallRefID = p_pJson->GetNodeValue("/body/callref_id", "");
				m_oBody.m_strAgentID = p_pJson->GetNodeValue("/body/agent", "");
				m_oBody.m_strStartTime = p_pJson->GetNodeValue("/body/start_time", "");
				m_oBody.m_strFileName = p_pJson->GetNodeValue("/body/file_name", "");
				m_oBody.m_strResult = p_pJson->GetNodeValue("/body/result", "");

				return true;
			}
		public:
			class CBody
			{
			public:
				std::string	m_strFileID;					//minio文件ID
				std::string	m_strCallRefID;					//话务ID
				std::string m_strAgentID;                   //坐席分机号码
				std::string m_strStartTime;                 //话务开始时间
				std::string	m_strFileName;					//文件名称
				std::string	m_strResult;					//上传文件结果
			};
			CHeader m_oHeader;
			CBody	m_oBody;
		};

	}
}
