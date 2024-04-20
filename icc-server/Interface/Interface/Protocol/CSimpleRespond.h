#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		//	�򵥵Ļظ� �ɹ�/ʧ�� ���ô���
		class CSimpleRespond :
			public ISend
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				m_oHeader.SaveTo(p_pJson);

				//p_pJson->SetNodeValue("/body/result", m_oBody.m_strResult);
				//p_pJson->SetNodeValue("/body/message", m_oBody.m_strMessage);
				
				return p_pJson->ToString();
			}

		public:
			class CBody
			{
			public:
				//std::string m_strResult;			//0����ʾ�ɹ���1����ʾʧ��
				std::string m_strMessage;
			};

			CHeaderEx m_oHeader;
			CBody	m_oBody;
		};
	}
}