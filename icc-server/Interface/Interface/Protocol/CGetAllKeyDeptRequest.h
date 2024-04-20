#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CGetAllKeyDeptRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strFilterKey = p_pJson->GetNodeValue("/body/filter_key", "");
				m_oBody.m_strPageSize = p_pJson->GetNodeValue("/body/page_size", "");
				m_oBody.m_strPageIndex = p_pJson->GetNodeValue("/body/page_index", "");
				return true;
			}

		public:
			CHeaderEx m_oHeader;
			class CBody
			{
			public:
				std::string m_strFilterKey;  //���˵ļ�ֵ�������У�����ֶβ���ֵ��������ݴ�ֵ�Ե�λ���ƣ������Լ���ַ����ģ��ƥ��
				std::string m_strPageSize;   //page_size	ÿҳ����������Ϊ�գ�
				std::string m_strPageIndex;	//page_index	ҳ�룬1��ʾ��һҳ������Ϊ�գ�
			};
			CBody m_oBody;
		};
	}
}
