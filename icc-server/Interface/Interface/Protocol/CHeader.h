#pragma once 
#include <Json/IJson.h>

namespace ICC
{
	namespace PROTOCOL
	{

#define MQTYPE_TOPIC "1"
#define MQTYPE_QUEUE "0"

		class CHeader
		{
		public:
			CHeader()
			{
				m_strSystemID = "ICC";
				m_strSubsystemID = "ICC-ApplicationServer";
			}
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return false;
				}
				if (!p_pJson->LoadJson(p_strReq))
				{
					return false;
				}

				m_strSystemID = p_pJson->GetNodeValue("/header/system_id", "");
				m_strSubsystemID = p_pJson->GetNodeValue("/header/subsystem_id", "");
				m_strMsgid = p_pJson->GetNodeValue("/header/msgid", "");
				m_strRelatedID = p_pJson->GetNodeValue("/header/related_id", "");
				m_strSendTime = p_pJson->GetNodeValue("/header/send_time", "");
				m_strCmd = p_pJson->GetNodeValue("/header/cmd", "");
				m_strRequest = p_pJson->GetNodeValue("/header/request", "");
				m_strRequestType = p_pJson->GetNodeValue("/header/request_type", "");
				m_strResponse = p_pJson->GetNodeValue("/header/reponse", "");
				m_strResponseType = p_pJson->GetNodeValue("/header/reponse_type", "");
				m_strCMSProperty = p_pJson->GetNodeValue("/header/cmsproperty", "");

				return true;
			}

			virtual void SaveTo(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return ;
				}

				p_pJson->SetNodeValue("/header/system_id", m_strSystemID);
				p_pJson->SetNodeValue("/header/subsystem_id", m_strSubsystemID);
				p_pJson->SetNodeValue("/header/msgid", m_strMsgid);
				p_pJson->SetNodeValue("/header/related_id", m_strRelatedID);
				p_pJson->SetNodeValue("/header/send_time", m_strSendTime);
				p_pJson->SetNodeValue("/header/cmd", m_strCmd);
				p_pJson->SetNodeValue("/header/request", m_strRequest);
				p_pJson->SetNodeValue("/header/request_type", m_strRequestType);
				p_pJson->SetNodeValue("/header/reponse", m_strResponse);
				p_pJson->SetNodeValue("/header/reponse_type", m_strResponseType);
				p_pJson->SetNodeValue("/header/cmsproperty", m_strCMSProperty);
			}

		public:
			std::string m_strSystemID;
			std::string m_strSubsystemID;
			std::string m_strMsgid;
			std::string m_strRelatedID;
			std::string m_strSendTime;
			std::string m_strCmd;
			std::string m_strRequest;
			std::string m_strRequestType;
			std::string m_strResponse;
			std::string m_strResponseType;
			std::string m_strCMSProperty;
			std::string m_strServerGuid;
		};

		class CHeaderEx : public  CHeader
		{
		public:
			CHeaderEx() : m_strResult("0"), m_strMsg("success")
			{
				//
			}

		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return false;
				}
				if (!p_pJson->LoadJson(p_strReq))
				{
					return false;
				}

				if (!CHeader::ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_strSrcIP = p_pJson->GetNodeValue("/header/src_ip", ""); 
				m_strMsgId = p_pJson->GetNodeValue("/header/msgid", "");
				m_strSeatNo = p_pJson->GetNodeValue("/header/seat_no", "");
				m_strCode = p_pJson->GetNodeValue("/header/code", "");
				m_strPwd = p_pJson->GetNodeValue("/header/pwd", "");
				m_strToken = p_pJson->GetNodeValue("/header/token", "");
				m_strRequestFlag = p_pJson->GetNodeValue("/header/request", "");

				return true;
			}

			virtual void SaveTo(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return;
				}

				if (!m_strRequestFlag.empty() && m_strRequestFlag != "http")
				{					
					CHeader::SaveTo(p_pJson);
				}
				else
				{
					p_pJson->SetNodeValue("/header/msgid", m_strMsgId);
					p_pJson->SetNodeValue("/header/result", m_strResult);
					if (m_strMsg == "success" && m_strResult != "0")
					{
						m_strMsg = "failed";
					}
					p_pJson->SetNodeValue("/header/msg", m_strMsg);
				}				
			}

		public:
			std::string m_strSrcIP;
			std::string m_strMsgId;
			std::string m_strSeatNo;	//当前坐席号（如8001）
			std::string m_strCode;		//用户编码（如088411）
			std::string m_strPwd;		//密码
			std::string m_strToken;		//	
			std::string m_strRequestFlag;

			std::string m_strResult;	//0：成功；非0：失败
			std::string m_strMsg;		//失败原因
		};
	}
}
