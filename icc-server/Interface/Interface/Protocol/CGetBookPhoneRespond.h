#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CGetBookPhoneRespond :
			public IRespond
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);
				p_pJson->SetNodeValue("/body/result", m_oBody.m_strResult);
				p_pJson->SetNodeValue("/body/count", m_oBody.m_strCount);

				unsigned int l_uiIndex = 0;
				for (CStaff data : m_oBody.m_vecData)
				{
					std::string l_strPrefixPath("/body/data/" + std::to_string(l_uiIndex) + "/");
					p_pJson->SetNodeValue(l_strPrefixPath + "guid", data.m_strGuid);
					p_pJson->SetNodeValue(l_strPrefixPath + "org_guid", data.m_strDeptGuid);
					p_pJson->SetNodeValue(l_strPrefixPath + "name", data.m_strName);
					p_pJson->SetNodeValue(l_strPrefixPath + "sex", data.m_strSex);
                    p_pJson->SetNodeValue(l_strPrefixPath + "code", data.m_strCode);
                    p_pJson->SetNodeValue(l_strPrefixPath + "mobile", data.m_strMobile);
                    p_pJson->SetNodeValue(l_strPrefixPath + "phone", data.m_strPhone);
                    p_pJson->SetNodeValue(l_strPrefixPath + "sort", data.m_strSort);
					p_pJson->SetNodeValue(l_strPrefixPath + "remark", data.m_strRemark);

					++l_uiIndex;
				}

				return p_pJson->ToString();
			}

		public:
			class CStaff
			{
			public:
				std::string m_strGuid;			//��Աguid
				std::string m_strDeptGuid;		//��Ա���ڵ�λguid
				std::string m_strCode;			//��Ա����
				std::string m_strName;			//��Ա����
				std::string m_strSex;			//��Ա�Ա�
				std::string m_strMobile;		//��Ա�ֻ�
				std::string m_strPhone;			//��Ա����
				std::string m_strSort;			//��Ա��ţ�һ���1��ʼ����
				std::string m_strRemark;		//
			};
			class CBody
			{
			public:
				std::string m_strResult;
				std::string m_strCount;
				std::vector<CStaff> m_vecData;
			};
			CHeaderEx m_oHeader;
			CBody	m_oBody;
		};
	}
}