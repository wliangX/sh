#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CAppGetAlarmIdFromICCRequest :
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
				p_pJson->SetNodeValue("/body/receipt_dept_code", m_oBody.m_oData.m_strReceiptDeptCode);
				p_pJson->SetNodeValue("/body/receipt_dept_name", m_oBody.m_oData.m_strReceiptDeptName);
				p_pJson->SetNodeValue("/body/receipt_code", m_oBody.m_oData.m_strReceiptCode);
				p_pJson->SetNodeValue("/body/receipt_name", m_oBody.m_oData.m_strReceiptName);
				p_pJson->SetNodeValue("/body/time", m_oBody.m_oData.m_strTime);
				return p_pJson->ToString();
			}

		public:
			class CData
			{
			public:
				std::string m_strTime;				            //��������ʱ��
				std::string m_strReceiptCode;					//�Ӿ��˾�Ա���
				std::string m_strReceiptName;					//�˽Ӿ�������
				std::string m_strReceiptDeptCode;				//�Ӿ��˲��ű���
				std::string m_strReceiptDeptName;				//�Ӿ��˲�������
				
			};
			class CBody
			{
			public:				
				CData m_oData;
			};
			CHeader m_oHeader;
			CBody m_oBody;
		};
	}
}
