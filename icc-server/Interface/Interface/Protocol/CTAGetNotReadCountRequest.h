#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CTAGetNotReadCountRequest :
			public IReceive
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strCurrDeptCode = p_pJson->GetNodeValue("/body/curr_dept_code", "");
				return true;
			}

		public:
			class CBody
			{
			public:
				std::string m_strCurrDeptCode;            //���ű��루��ѯ�����Ҫ�����¼����ţ�
			};
			CHeaderEx m_oHeader;
			CBody m_oBody;
		};		
	}
}
