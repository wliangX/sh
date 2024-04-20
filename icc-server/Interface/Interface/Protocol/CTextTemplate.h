#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CTextTemplate : public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strPageSize = p_pJson->GetNodeValue("/body/page_size", "");
				m_oBody.m_strPageIndex = p_pJson->GetNodeValue("/body/page_index", "");

				m_oBody.m_data.m_strGuid = p_pJson->GetNodeValue("/body/guid", "");
				m_oBody.m_data.m_strType = p_pJson->GetNodeValue("/body/type", "");
				m_oBody.m_data.m_strContent = p_pJson->GetNodeValue("/body/content", "");
				m_oBody.m_data.m_strAlarm_type = p_pJson->GetNodeValue("/body/alarm_type", "");
				m_oBody.m_data.m_strHandly_type = p_pJson->GetNodeValue("/body/handly_type", "");

				return true;
			}
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);
				p_pJson->SetNodeValue("/body/count", m_oBody.m_strCount);
				p_pJson->SetNodeValue("/body/all_count", m_oBody.m_strAllCount);
				p_pJson->SetNodeValue("/body/page_index", m_oBody.m_strPageIndex);

				unsigned int l_uiIndex = 0;
				for (Text data : m_oBody.m_vecData)
				{
					std::string l_strPrefixPath("/body/data/" + std::to_string(l_uiIndex) + "/");
					p_pJson->SetNodeValue(l_strPrefixPath + "guid", data.m_strGuid);
					p_pJson->SetNodeValue(l_strPrefixPath + "type", data.m_strType);
					p_pJson->SetNodeValue(l_strPrefixPath + "content", data.m_strContent);
					p_pJson->SetNodeValue(l_strPrefixPath + "alarm_type", data.m_strAlarm_type);
					p_pJson->SetNodeValue(l_strPrefixPath + "handly_type", data.m_strHandly_type);
					++l_uiIndex;
				}

				return p_pJson->ToString();
			}

			//Ӧ��
			std::string ToStringResponse(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);
				//p_pJson->SetNodeValue("/body/result", m_oBody.m_strResult);
				return p_pJson->ToString();
			}

			//ͬ��
			std::string Sync(JsonParser::IJsonPtr p_pJson, std::string strType)
			{
				m_oHeader.SaveTo(p_pJson);
				p_pJson->SetNodeValue("/body/sync_type", strType);
				p_pJson->SetNodeValue("/body/guid", m_oBody.m_data.m_strGuid);
				p_pJson->SetNodeValue("/body/type", m_oBody.m_data.m_strType);
				p_pJson->SetNodeValue("/body/content", m_oBody.m_data.m_strContent);
				p_pJson->SetNodeValue("/body/alarm_type", m_oBody.m_data.m_strAlarm_type);
				p_pJson->SetNodeValue("/body/handly_type", m_oBody.m_data.m_strHandly_type);

				return p_pJson->ToString();
			}
		public:
			class Text
			{
			public:
				std::string m_strGuid;		        //ģ��guid
				std::string m_strType;			    //ģ������
				std::string m_strContent;			//ģ������
				std::string m_strAlarm_type;			//ģ������
				std::string m_strHandly_type;			//ģ������
			};
			class CBody
			{
			public:
				std::string m_strCount;
				std::string m_strAllCount;
				std::string m_strPageSize;
				std::string m_strPageIndex;

				//��ȡ���е�λ�µ����пɵ��ɵ�λ��Ϣʱ��Ч
				std::vector<Text> m_vecData;
				//����ĳһ��λ��ĳһ���ɵ��ɵ�λ��Ϣʱ��Ч
				Text m_data;
				//Ӧ����
				//std::string m_strResult;
				//ͬ����Ϣ 1����ʾ��ӣ�2����ʾ���£�3����ʾɾ��
				std::string m_strSyncType;
			};
			CHeaderEx m_oHeader;
			CBody	m_oBody;
		};
	}
}
