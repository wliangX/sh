#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CTAGetNotReadCountRespond :
			public ISend
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
				p_pJson->SetNodeValue("/body/curr_dept_code", m_oBody.m_strCurrDeptCode);

				return p_pJson->ToString();
			}

		public:
			class CBody
			{
			public:
				std::string m_strCount;						//��������
				std::string m_strCurrDeptCode;				//���ű��루��ѯ�����Ҫ�����¼����ţ�
			};
			CHeaderEx m_oHeader;
			CBody	m_oBody;
		};
	}
}