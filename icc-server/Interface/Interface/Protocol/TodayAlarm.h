#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>
#include <Protocol/IRespond.h>
#include <Protocol/CAlarmInfo.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CTodayAlarm :
			public IRequest, public IRespond
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
			    m_oBody.m_strPageSize = p_pJson->GetNodeValue("/body/page_size", "");
				m_oBody.m_strPageIndex = p_pJson->GetNodeValue("/body/page_index", "");
				m_oBody.m_strReceiptDeptCode = p_pJson->GetNodeValue("/body/receipt_dept_code", "");
				m_oBody.m_strCalledNoType = p_pJson->GetNodeValue("/body/called_no_type", "");
				m_oBody.m_strState = p_pJson->GetNodeValue("/body/state", "");
				m_oBody.m_strReceiptCode = p_pJson->GetNodeValue("/body/receipt_code", "");
				m_oBody.m_strOnlyShowReal = p_pJson->GetNodeValue("/body/only_show_real", "");
				m_oBody.m_strQueryKey = p_pJson->GetNodeValue("/body/query_key", "");

				m_oBody.m_strLevel = p_pJson->GetNodeValue("/body/level", "");
				m_oBody.m_strIgnorePrivacy = p_pJson->GetNodeValue("/body/ignore_privacy", "false");
                m_oBody.m_strReceiptDeptCodeRecursion= p_pJson->GetNodeValue("/body/receipt_dept_code_recursion", "");

				m_oBody.m_strIsInvalid = p_pJson->GetNodeValue("/body/is_invalid", "");
				m_oBody.m_strSeatNo = p_pJson->GetNodeValue("/body/receipt_seatno", "");

				m_oBody.m_strDays = p_pJson->GetNodeValue("/body/days", "");
				m_oBody.m_strCallerKey = p_pJson->GetNodeValue("/body/caller_key", "");

				m_oBody.m_strIsMajorAlarmFlag = p_pJson->GetNodeValue("/body/majoralarm_flag", "");

				m_oBody.m_strIsAttentionAlarmFlag = p_pJson->GetNodeValue("/body/attentionalarm_flag", "");
				m_oBody.m_strAttentionCode = p_pJson->GetNodeValue("/body/attention_code", "");

				m_oBody.m_strIsClosure = p_pJson->GetNodeValue("/body/is_over", "");

				m_oBody.m_strReceiverCode = p_pJson->GetNodeValue("/body/receiver_code", "");

				m_oBody.m_strUpAttention = p_pJson->GetNodeValue("/body/up_attention", "");

				m_oBody.m_strSortParam = p_pJson->GetNodeValue("/body/sort_param", "");//jqclztdm, bjsj desc
				m_oBody.m_strIsSigned = p_pJson->GetNodeValue("/body/is_signed", "");
				//���Ӵ�����һ�Ľṹ�����������������鷽ʽ�ٴ���

				return true;
			}

			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);
				p_pJson->SetNodeValue("/body/all_count", m_oBody.m_strAllCount);
				p_pJson->SetNodeValue("/body/count", m_oBody.m_strCount);

				unsigned int l_uiIndex = 0;
				for (CAlarmInfo data : m_oBody.m_vecData)
				{
					//std::string l_strPrefixPath("/body/data/" + std::to_string(l_uiIndex) + "/");
					std::string l_strPrefixPath("/body/data/" + std::to_string(l_uiIndex));
					data.ComJson(l_strPrefixPath, p_pJson);

					l_uiIndex++;
				}

				return p_pJson->ToString();
			}

			//ͬ��
			std::string Sync(JsonParser::IJsonPtr p_pJson, std::string strType, std::string strAlarmId)
			{
				if (NULL == p_pJson)
				{
					return "";
				}
				m_oHeader.SaveTo(p_pJson);
				p_pJson->SetNodeValue("/body/sync_type", strType);
				p_pJson->SetNodeValue("/body/id", strAlarmId);

				return p_pJson->ToString();
			}

		public:

			class CBody
			{
			public:
				std::string m_strPageSize;        //��ҳÿҳ����
				std::string m_strPageIndex;       //��ǰ�ڼ�ҳ
				std::string m_strReceiptDeptCode; //�Ӿ���λ����
				std::string m_strCalledNoType;  //������������
				std::string m_strState;           //����״̬ 
				std::string m_strReceiptCode;     //�Ӿ�Ա����
				std::string m_strOnlyShowReal;    //ֻ��ʾ��Ч����
				std::string m_strQueryKey;        //�ؼ���
				std::string m_strLevel;           //���鼶��
				std::string m_strIgnorePrivacy;   //������˽����
                std::string m_strReceiptDeptCodeRecursion;  //receipt_dept_code_recursion	�Ƿ�����ӵ�λ��0������1����
				std::string m_strReceiverCode;
				
				std::string m_strIsInvalid;  //��Ч����
				std::string m_strSeatNo;
				std::string m_strDays;
				std::string m_strCallerKey;  

				std::string m_strIsMajorAlarmFlag;//1����ѯ�ش���
				std::string m_strIsAttentionAlarmFlag;//1����ѯ��ע����
				std::string m_strAttentionCode;

				std::string m_strIsClosure; // �Ƿ�᰸��0��δ�᰸��1���ѽ᰸

				std::string m_strUpAttention; //1:�ö���

				std::string m_strCount;
				std::string m_strAllCount;

				std::string m_strSortParam;
				std::string m_strIsSigned;
				//��ȡ���е�λ�µ����пɵ��ɵ�λ��Ϣʱ��Ч
				std::vector<CAlarmInfo> m_vecData;
			};
			CHeaderEx m_oHeader;
			CBody	m_oBody;
			
		};
	}
}
