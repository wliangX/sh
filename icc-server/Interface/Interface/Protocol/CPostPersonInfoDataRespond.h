/*
post ��������
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
			std::string m_strtype;		    //�û��������ͣ�00:�����񾯣�01:������02:����
			std::string m_strpoliceNo; 	    //��Ա���
			std::string m_struserId;		//��ѯ�û��Ĺ�����ݺ���
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
			userToken usertoken;   //�û���Ϣ
			appToken apptoken;     //Ӧ����Ϣ
			std::string m_strusertoken;
		};
	}
}