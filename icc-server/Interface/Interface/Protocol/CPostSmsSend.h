#pragma once 
#include <Protocol/IRespond.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CPostSmsSendRequest :
			public IRespond
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}
				//m_oHeader.SaveTo(p_pJson);

				p_pJson->SetNodeValue("/content", m_oBody.m_strContent);
				p_pJson->SetNodeValue("/elementName", m_oBody.m_strElementName);
				p_pJson->SetNodeValue("/extensionNo", m_oBody.m_strExtensionNo);
				p_pJson->SetNodeValue("/fromUserName", m_oBody.m_strFromUserName);
				p_pJson->SetNodeValue("/phone", m_oBody.m_strPhone);

				return p_pJson->ToString();
			}

			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strContent = p_pJson->GetNodeValue("/content", "");
				m_oBody.m_strElementName = p_pJson->GetNodeValue("/elementName", "");
				m_oBody.m_strExtensionNo = p_pJson->GetNodeValue("/extensionNo", "");
				m_oBody.m_strFromUserName = p_pJson->GetNodeValue("/fromUserName", "");
				m_oBody.m_strPhone = p_pJson->GetNodeValue("/phone", "");
				m_oBody.m_strAlarmId = p_pJson->GetNodeValue("/alarmid", "");
				m_oBody.m_strStaffCode = p_pJson->GetNodeValue("/staffCode", "");
				m_oBody.m_strStaffName = p_pJson->GetNodeValue("/staffName", "");
				return true;
			}

		public:
			CHeader m_oHeader;
			class CBody
			{
			public:
				std::string m_strContent;         // 短信内容
				std::string m_strElementName;     // 网元名称
				std::string m_strExtensionNo;     // 扩展号（为数字类型，长度不超过五位，短信回复时作为唯一性判断）
				std::string m_strFromUserName;    // 发送人署名（示例：李明）
				std::string m_strPhone;           // 电话号码（多个短信使用同一个内容时建议用,隔开）

				std::string m_strAlarmId;		  // 
				std::string m_strStaffCode;		  // 发送短信警员编号
				std::string m_strStaffName;		  // 发送短信警员姓名
			};
			CBody m_oBody;
		};


		class CPostSmsSendResponse :
			public IRespond
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}
				//m_oHeader.SaveTo(p_pJson);

				p_pJson->SetNodeValue("/code", m_oBody.m_strCode);
				p_pJson->SetNodeValue("/data/code", m_oBody.m_strDataCode);
				p_pJson->SetNodeValue("/data/errorCode", m_oBody.m_strDataErrorCode);
				p_pJson->SetNodeValue("/data/message", m_oBody.m_strDataMessage);
				p_pJson->SetNodeValue("/data/smId", m_oBody.m_strDataSmId);
				p_pJson->SetNodeValue("/message", m_oBody.m_strMessage);
				return p_pJson->ToString();
			}

			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strCode = p_pJson->GetNodeValue("/code", "");
				m_oBody.m_strDataCode = p_pJson->GetNodeValue("/data/code", "");
				m_oBody.m_strDataErrorCode = p_pJson->GetNodeValue("/data/errorCode", "");
				m_oBody.m_strDataMessage = p_pJson->GetNodeValue("/data/message", "");
				m_oBody.m_strDataSmId = p_pJson->GetNodeValue("/data/smId", "");
				m_oBody.m_strMessage = p_pJson->GetNodeValue("/message", "");
				return true;
			}

		public:
			CHeader m_oHeader;
			class CBody
			{
			public:
				std::string m_strCode;             // 返回信息码,默认:200
				std::string m_strDataCode;         // 接口调用返回码（1：成功，0：失败）
				std::string m_strDataErrorCode;    // 接口调用错误响应码
				std::string m_strDataMessage;      // 具体的提示信息
				std::string m_strDataSmId;         // 由短信平台生成的消息ID，在后续查询短信状态时需要传入此值
				std::string m_strMessage;          // 返回信息内容，默认:操作成功
			};
			CBody m_oBody;
		};
	}
}
