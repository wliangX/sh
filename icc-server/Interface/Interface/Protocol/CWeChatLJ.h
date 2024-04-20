#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CWeChatRespond
		{
		public:
			std::string ToString(Xml::IXmlPtr p_pXml)
			{
				p_pXml->SetText("icc/type", m_strType);
				p_pXml->SetText("icc/typeid", m_strTypeid);
				p_pXml->SetText("icc/caseprocessresult", m_strProcessRes);

				std::string l_str = "<?xml version=\"1.0\" encoding=\"UTF8\"?>\n"\
					+ p_pXml->GetXml();
				return l_str;
			}

			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_strType = p_pJson->GetNodeValue("/body/type", "");
				m_strTypeid = p_pJson->GetNodeValue("/body/typeid", "");
				m_strProcessRes = p_pJson->GetNodeValue("/body/content", "");

				return true;
			}

		public:
			std::string m_strType;
			std::string m_strTypeid;
			std::string m_strProcessRes;
			CHeader m_oHeader;
		};

		class CWeChatRequest
		{
		public:
			 bool ParseString(std::string m_strFilePath, Xml::IXmlPtr p_pXml,
				StringUtil::IStringUtilPtr  m_pStrUtil)
			{
				if (!p_pXml->LoadFile(m_strFilePath))
				{
					return false;
				}

				m_oBody.m_strType = p_pXml->GetText("icc/type", "");
				m_oBody.m_strTypeid = p_pXml->GetText("icc/typeid", "");
				m_oBody.m_strCaseTime = p_pXml->GetText("icc/casetime", "");
				m_oBody.m_strCaseAddress = p_pXml->GetText("icc/caseaddress", "");
				m_oBody.m_strCaseDesc = p_pXml->GetText("icc/casedesc", "");
				m_oBody.m_strContact = p_pXml->GetText("icc/contact", "");
				m_oBody.m_strContactNo = p_pXml->GetText("icc/contactno", "");
				m_oBody.m_strContactAddress = p_pXml->GetText("icc/contactaddress", "");
				m_oBody.m_strLongitude = p_pXml->GetText("icc/longitude", "");
				m_oBody.m_strLatitude = p_pXml->GetText("icc/latitude", "");


				m_pStrUtil->Split(m_pStrUtil->ReplaceAll(p_pXml->GetText("icc/imagelist", "")
					, " ", ""), ";", m_oBody.m_vecimagelist, 1);

				std::string l_strrecordlist = p_pXml->GetText("icc/recordlist", "");
				m_pStrUtil->Split(m_pStrUtil->ReplaceAll(p_pXml->GetText("icc/recordlist", "")
					, " ", ""), ";", m_oBody.m_vecrecordlist, 1);

				m_pStrUtil->Split(m_pStrUtil->ReplaceAll(p_pXml->GetText("icc/videolist", "")
					, " ", ""),";", m_oBody.m_vecvideolist, 1);

				return true;
			}
			 std::string ToString(JsonParser::IJsonPtr p_pJson)
			 {
				 m_oHeader.SaveTo(p_pJson);

				 p_pJson->SetNodeValue("/body/type", m_oBody.m_strType);
				 p_pJson->SetNodeValue("/body/typeid", m_oBody.m_strTypeid);
				 p_pJson->SetNodeValue("/body/casetime", m_oBody.m_strCaseTime);
				 p_pJson->SetNodeValue("/body/caseaddress", m_oBody.m_strCaseAddress);
				 p_pJson->SetNodeValue("/body/casedesc", m_oBody.m_strCaseDesc);
				 p_pJson->SetNodeValue("/body/contact", m_oBody.m_strContact);
				 p_pJson->SetNodeValue("/body/contactno", m_oBody.m_strContactNo);
				 p_pJson->SetNodeValue("/body/contactaddress", m_oBody.m_strContactAddress);
				 p_pJson->SetNodeValue("/body/longitude", m_oBody.m_strLongitude);
				 p_pJson->SetNodeValue("/body/latitude", m_oBody.m_strLatitude);

				 return p_pJson->ToString();
			 }

		public:
			class CBody
			{
			public:
				std::string m_strType;
				std::string m_strTypeid;
				std::string m_strCaseTime;
				std::string m_strCaseAddress;
				std::string m_strCaseDesc;
				std::string m_strContact;
				std::string m_strContactNo;
				std::string m_strContactAddress;
				std::string m_strLongitude;
				std::string m_strLatitude;
				std::vector<std::string> m_vecimagelist;
				std::vector<std::string> m_vecrecordlist;
				std::vector<std::string> m_vecvideolist;
			};

			CHeader m_oHeader;
			CBody m_oBody;
		};
	}
}
