#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CAppGetAlarmIdRequest:
			public IReceive
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				
				m_oBody.m_oData.m_strReceiptDeptCode = p_pJson->GetNodeValue("/body/receipt_dept_code", "");
				m_oBody.m_oData.m_strReceiptDeptName = p_pJson->GetNodeValue("/body/receipt_dept_name", "");
				m_oBody.m_oData.m_strReceiptUserCode = p_pJson->GetNodeValue("/body/receipt_code", "");
				m_oBody.m_oData.m_strReceiptUserName = p_pJson->GetNodeValue("/body/receipt_name", "");
				return true;
			}

		public:
			class CData
			{
			public:				
				std::string m_strReceiptUserCode;			//�Ӿ���id
				std::string m_strReceiptUserName;			//�Ӿ�������
				std::string m_strReceiptDeptCode;			//�Ӿ��˵�λid
				std::string m_strReceiptDeptName;			//�Ӿ��˵�λ����
				
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
