#pragma once 
#include <Protocol/IRespond.h>
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CGetBJRSFZHRespond :
			public IRespond, public IReceive
		{
		public:
			//组装为响应消息返回给ICC客户端
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);
				p_pJson->SetNodeValue("/body/xm", m_oBody.m_strXM);
				p_pJson->SetNodeValue("/body/sfzh", m_oBody.m_strSFZH);
				for (size_t i = 0; i < m_oBody.m_vecLYS.size(); ++i)
				{
					std::string l_strIndex = std::to_string(i);
					p_pJson->SetNodeValue("/body/lys/" + l_strIndex + "/ly", m_oBody.m_vecLYS[i].m_strLY);
					p_pJson->SetNodeValue("/body/lys/" + l_strIndex + "/cjsj", m_oBody.m_vecLYS[i].m_strCJSJ);
					p_pJson->SetNodeValue("/body/lys/" + l_strIndex + "/txlx", m_oBody.m_vecLYS[i].m_strTXLX);	
                    p_pJson->SetNodeValue("/body/lys/" + l_strIndex + "/zt", m_oBody.m_vecLYS[i].m_strZT);
                    p_pJson->SetNodeValue("/body/lys/" + l_strIndex + "/lylx", m_oBody.m_vecLYS[i].m_strLYLX);					
				}
				return p_pJson->ToString();
			}

			virtual std::string ToLYSString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}				
				
				for (size_t i = 0; i < m_oBody.m_vecLYS.size(); ++i)
				{
					std::string l_strIndex = std::to_string(i);
					p_pJson->SetNodeValue("/body/lys/" + l_strIndex + "/ly", m_oBody.m_vecLYS[i].m_strLY);
					p_pJson->SetNodeValue("/body/lys/" + l_strIndex + "/cjsj", m_oBody.m_vecLYS[i].m_strCJSJ);
					p_pJson->SetNodeValue("/body/lys/" + l_strIndex + "/txlx", m_oBody.m_vecLYS[i].m_strTXLX);
					p_pJson->SetNodeValue("/body/lys/" + l_strIndex + "/zt", m_oBody.m_vecLYS[i].m_strZT);
					p_pJson->SetNodeValue("/body/lys/" + l_strIndex + "/lylx", m_oBody.m_vecLYS[i].m_strLYLX);
				}
				return p_pJson->ToString();
			}

			//解析来自第三方返回的消息
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


				m_oBody.m_strCode = p_pJson->GetNodeValue("/code", "");
				m_oBody.m_strMessage = p_pJson->GetNodeValue("/message", "");

				int tmp_iDataCount = p_pJson->GetCount("/data");

				//存在才取值
				if (tmp_iDataCount > 0)
				{
					m_oBody.m_strXM = p_pJson->GetNodeValue("/data/0/xm", "");
					m_oBody.m_strSFZH = p_pJson->GetNodeValue("/data/0/zjhm", "");
					m_oBody.m_strLYS = p_pJson->GetNodeValue("/data/0/lys", "");

					int tmp_iLysCount = p_pJson->GetCount("/data/0/lys");

					CLYS tmp_oLYS;
					for (int i = 0; i > tmp_iLysCount; ++i)
					{
						tmp_oLYS.m_strLY = p_pJson->GetNodeValue("/data/0/lys/"+ std::to_string(i)+"/ly", "");
						tmp_oLYS.m_strCJSJ = p_pJson->GetNodeValue("/data/0/lys/" + std::to_string(i) + "/cjsj", "");
						tmp_oLYS.m_strTXLX = p_pJson->GetNodeValue("/data/0/lys/" + std::to_string(i) + "/txlx", "");
						tmp_oLYS.m_strZT = p_pJson->GetNodeValue("/data/0/lys/" + std::to_string(i) + "/zt", "");
						tmp_oLYS.m_strLYLX = p_pJson->GetNodeValue("/data/0/lys/" + std::to_string(i) + "/lylx", "");

						m_oBody.m_vecLYS.push_back(tmp_oLYS);
					}
				}
				return true;
			}
		public:
			CHeaderEx m_oHeader;
			
			class CLYS
			{
			public:	
				std::string m_strLY;  //来源
				std::string m_strCJSJ; //触警时间
				std::string m_strTXLX; //联系电话
				std::string m_strZT;   //状态
				std::string m_strLYLX;  //
			};
			class CBody
			{
			public:	
				std::string m_strCode;  //HTTP的结果码  第三方响应消息才有
				std::string m_strMessage;  //结果消息   第三方响应消息才有
				std::string m_strXM;
				std::string m_strSFZH;
				std::string m_strLYS;
				std::vector<CLYS> m_vecLYS;
			};
			CBody m_oBody;
		};
	}
}
