#pragma once

#include <Protocol/CHeader.h>
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CGetNoticesRespond :
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
				p_pJson->SetNodeValue("/body/count", m_oBody.m_strCount);

				unsigned int l_uiIndex = 0;
				for (CData l_oNoticeData : m_oBody.m_vecData)
				{
					std::string l_strPrefixPath("/body/data/" + std::to_string(l_uiIndex) + "/");

					p_pJson->SetNodeValue(l_strPrefixPath + "notice_id", l_oNoticeData.m_strNoticeId);
					p_pJson->SetNodeValue(l_strPrefixPath + "user_code", l_oNoticeData.m_strUserCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "user_name", l_oNoticeData.m_strUserName);
					p_pJson->SetNodeValue(l_strPrefixPath + "seat_no", l_oNoticeData.m_strSeatNo);

					p_pJson->SetNodeValue(l_strPrefixPath + "dept_code", l_oNoticeData.m_strDeptCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "dept_name", l_oNoticeData.m_strDeptName);
					p_pJson->SetNodeValue(l_strPrefixPath + "target_dept_code", l_oNoticeData.m_strTargetDeptCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "target_dept_name", l_oNoticeData.m_strTargetDeptName);

					p_pJson->SetNodeValue(l_strPrefixPath + "notice_type", l_oNoticeData.m_strNoticeType);
					p_pJson->SetNodeValue(l_strPrefixPath + "title", l_oNoticeData.m_strTitle);
					p_pJson->SetNodeValue(l_strPrefixPath + "content", l_oNoticeData.m_strContent);
					p_pJson->SetNodeValue(l_strPrefixPath + "time", l_oNoticeData.m_strTime);

					++l_uiIndex;
				}

				return p_pJson->ToString();
			}

		public:
			class CData
			{
			public:
				std::string m_strNoticeId;			//����/֪ͨID
				std::string m_strUserCode;			//�û�ID
				std::string m_strUserName;			//�û�����
				std::string m_strSeatNo;			//ϯλ��

				std::string m_strDeptCode;			//��λ����
				std::string m_strDeptName;			//��λ����
				std::string m_strTargetDeptCode;	//Ŀ�굥λ����
				std::string m_strTargetDeptName;	//Ŀ�굥λ����

				std::string m_strNoticeType;		//֪ͨ���ͣ�1�����棬2��֪ͨ��
				std::string m_strTitle;				//����
				std::string m_strContent;			//����/֪ͨ����
				std::string m_strTime;				//ʱ��
			};

			class CBody
			{
			public:
				std::string m_strCount;
				std::vector<CData> m_vecData;
			};

			CHeader m_oHeader;
			CBody m_oBody;
		};
	}
}