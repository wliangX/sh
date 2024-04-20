/*
��ȡ�����������Ϣ����
*/
#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>
#include <Protocol/CAuthInfo.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CGetBJRSFZHLoginPespond :
			public IReceive
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}
				p_pJson->SetNodeValue("/username", m_oBody.m_strUsername);
				p_pJson->SetNodeValue("/password", m_oBody.m_strPassword);
				return p_pJson->ToString();
			}

			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strUsername = p_pJson->GetNodeValue("username", "");
				m_oBody.m_strPassword = p_pJson->GetNodeValue("password", "");

				return true;
			}

		public:
			CHeaderEx m_oHeader;

			class CBody
			{
			public:
				std::string m_strUsername;   //�û��� ������
				std::string m_strPassword;   //���� ������
			};
			CBody m_oBody;
		};


		class CGetBJRSFZHByIdCardPespond :public IReceive
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}
				p_pJson->SetNodeValue("/gmsfhm", m_oBody.m_strGmsfhm);
				p_pJson->SetNodeValue("/dwellType", m_oBody.m_strDwellType);

				return p_pJson->ToString();
			}

			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strGmsfhm = p_pJson->GetNodeValue("gmsfhm", "");
				m_oBody.m_strDwellType = p_pJson->GetNodeValue("dwellType", "");

				return true;
			}
		public:
			CHeaderEx m_oHeader;

			class CBody
			{
			public:
				std::string m_strGmsfhm;       //������ݺ���	string	����
				std::string m_strDwellType;    //��ס���ͣ�1��ס  2��ס  string ����
			};
			CBody m_oBody;

		};



		class CGetBJRSFZHByPhonePespond :public IReceive
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}
				p_pJson->SetNodeValue("/lxdh", m_oBody.m_strlxdh);

				return p_pJson->ToString();
			}

			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_oBody.m_strlxdh = p_pJson->GetNodeValue("lxdh", "");

				return true;
			}
		public:
			CHeaderEx m_oHeader;

			class CBody
			{
			public:
				std::string m_strlxdh;       //�ֻ���	string	����
			};
			CBody m_oBody;

		};
	}
}