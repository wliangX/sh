#pragma once 
#include <Protocol/IRespond.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CCheckKeyDeptRespond :
			public IRespond
		{

		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_oBody.m_strResult = p_pJson->GetNodeValue("/body/result", "");
				return true;
			}
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);
				p_pJson->SetNodeValue("/body/result", m_oBody.m_strResult);
				p_pJson->SetNodeValue("/body/is_key_dept", m_oBody.m_strIsKeyDept);
				p_pJson->SetNodeValue("/body/count", m_oBody.m_strCount);
				for (size_t i = 0; i < m_oBody.m_vecData.size(); i++)
				{
					std::string l_strIndex = std::to_string(i);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/type", m_oBody.m_vecData[i].m_strType);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/name", m_oBody.m_vecData[i].m_strName);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/phone", m_oBody.m_vecData[i].m_strPhone);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/address", m_oBody.m_vecData[i].m_strAddress);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/longitude", m_oBody.m_vecData[i].m_strLongitude);
					p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/latitude", m_oBody.m_vecData[i].m_strLattitude);
				}

				return p_pJson->ToString();
			}

		public:
			CHeader m_oHeader;
			class CBody
			{
			public:
				class CData
				{
				public:
					std::string m_strType;		//�ص㵥λ����		
					std::string m_strName;
					std::string m_strPhone;
					std::string m_strAddress;
					std::string m_strLongitude; //����  
					std::string m_strLattitude;	//γ��	
				};

				std::string m_strCount;
				std::vector<CData>m_vecData;
				std::string m_strIsKeyDept;
				std::string m_strResult;     //0:�ɹ���1:ʧ��
			};
			CBody m_oBody;
		};
	}
}
