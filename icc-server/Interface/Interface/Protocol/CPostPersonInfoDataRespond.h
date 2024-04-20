/*
post 请求数据
*/
#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>
#include <Protocol/CAuthInfo.h>

namespace ICC
{
	namespace PROTOCOL
	{
		struct userToken
		{
			std::string m_strtype;		    //用户所属类型：00:公安民警；01:辅警；02:其它
			std::string m_strpoliceNo; 	    //警员编号
			std::string m_struserId;		//查询用户的公民身份号码
		};

		struct appToken
		{
			std::string m_strappId;		    //appid
			struct userToken m_struserToken;
		};

		class CGetLoginTokenRespond :
			public IReceive
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}
				p_pJson->SetNodeValue("/type", apptoken.m_struserToken.m_strtype);
				p_pJson->SetNodeValue("/policeNo", apptoken.m_struserToken.m_strpoliceNo);
				p_pJson->SetNodeValue("/userId", apptoken.m_struserToken.m_struserId);

				return p_pJson->ToString();
			}

			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHead.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_strusertoken = p_pJson->GetNodeValue("/userToken", "");
				return true;
			}

		public:
			CHeaderEx m_oHead;
			userToken usertoken;   //用户信息
			appToken apptoken;     //应用信息
			std::string m_strusertoken;
		};
	}
}