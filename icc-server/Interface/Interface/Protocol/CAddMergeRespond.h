#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CAddMergeRespond :
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
				//p_pJson->SetNodeValue("/body/result", m_oBody.m_strResult);				
				return p_pJson->ToString();
			}

		public:
			//class CBody
			//{
			//public:
			//	std::string m_strResult;			//0����ʾ�ɹ���1����ʾʧ��
			//};
			CHeaderEx m_oHeader;
			//CBody	m_oBody;
		};
	}
}