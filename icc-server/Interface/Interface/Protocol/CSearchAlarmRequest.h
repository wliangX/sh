#pragma once 
#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CSearchAlarmRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_oBody.m_strVcsSyncFlag = p_pJson->GetNodeValue("/body/vcs_sync_flag", "");

				m_oBody.m_strPageSize = p_pJson->GetNodeValue("/body/page_size", "");
				m_oBody.m_strPageIndex = p_pJson->GetNodeValue("/body/page_index", "");
				m_oBody.m_strBeginTime = p_pJson->GetNodeValue("/body/begin_time", "");
				m_oBody.m_strEndTime = p_pJson->GetNodeValue("/body/end_time", "");
				m_oBody.m_strID = p_pJson->GetNodeValue("/body/id", "");

				m_oBody.m_strTitle = p_pJson->GetNodeValue("/body/title", "");
				m_oBody.m_strContent = p_pJson->GetNodeValue("/body/content", "");

				m_oBody.m_strAddr = p_pJson->GetNodeValue("/body/addr", "");
				m_oBody.m_strState = p_pJson->GetNodeValue("/body/state", "");
				m_oBody.m_strLevel = p_pJson->GetNodeValue("/body/level", "");
				m_oBody.m_strSourceType = p_pJson->GetNodeValue("/body/source_type", "");

				m_oBody.m_strHandleType = p_pJson->GetNodeValue("/body/handle_type", "");
				m_oBody.m_strIsVerity = p_pJson->GetNodeValue("/body/is_verity", "");
			/*	m_oBody.m_strFirstType = p_pJson->GetNodeValue("/body/first_type", "");
				m_oBody.m_strSecondType = p_pJson->GetNodeValue("/body/second_type", "");
				m_oBody.m_strThirdType = p_pJson->GetNodeValue("/body/third_type", "");
				m_oBody.m_strFourthType = p_pJson->GetNodeValue("/body/fourth_type", "");*/
				m_oBody.m_strAlarmType = p_pJson->GetNodeValue("/body/alarm_type", "");


				m_oBody.m_strFeedbackFirstType = p_pJson->GetNodeValue("/body/feedback_first_type", "");
				m_oBody.m_strFeedbackSecondType = p_pJson->GetNodeValue("/body/feedback_second_type", "");
				m_oBody.m_strFeedbackThirdType = p_pJson->GetNodeValue("/body/feedback_third_type", "");
				m_oBody.m_strFeedbackFourthType = p_pJson->GetNodeValue("/body/feedback_fourth_type", "");
/*
				m_oBody.m_strIsInvolveForeign = p_pJson->GetNodeValue("/body/is_involve_foreign", "");
				m_oBody.m_strIsInvolvePolice = p_pJson->GetNodeValue("/body/is_involve_police", "");
				m_oBody.m_strIsInvolveGun = p_pJson->GetNodeValue("/body/is_involve_gun", "");
				m_oBody.m_strIsInvolveKnife = p_pJson->GetNodeValue("/body/is_involve_knife", "");
				m_oBody.m_strIsInvolveTerror = p_pJson->GetNodeValue("/body/is_involve_terror", "");
				m_oBody.m_strIsInvolvePornography = p_pJson->GetNodeValue("/body/is_involve_pornograp", "");
				m_oBody.m_strIsInvolveGamble = p_pJson->GetNodeValue("/body/is_involve_gamble", "");
				m_oBody.m_strIsInvolvePoison = p_pJson->GetNodeValue("/body/is_involve_poison", "");*/
				m_oBody.m_strEventType = p_pJson->GetNodeValue("/body/event_type", "");
								
				m_oBody.m_strCalledNoType = p_pJson->GetNodeValue("/body/called_no_type", "");
				m_oBody.m_strCallerNo = p_pJson->GetNodeValue("/body/caller_no", "");
				m_oBody.m_strCallerName = p_pJson->GetNodeValue("/body/caller_name", "");

				m_oBody.m_strContactNo = p_pJson->GetNodeValue("/body/contact_no", "");
				m_oBody.m_strContactName = p_pJson->GetNodeValue("/body/contact_name", "");

				m_oBody.m_strAdminDeptCode = p_pJson->GetNodeValue("/body/admin_dept_code", "");
				m_oBody.m_strAdminDeptCodeRecursion = p_pJson->GetNodeValue("/body/admin_dept_code_recursion", "");
				m_oBody.m_strReceiptDeptCode = p_pJson->GetNodeValue("/body/receipt_dept_code", "");
				m_oBody.m_strReceiptDeptCodeRecursion = p_pJson->GetNodeValue("/body/receipt_dept_code_recursion", "");
				m_oBody.m_strLeaderCode = p_pJson->GetNodeValue("/body/leader_code", "");
				m_oBody.m_strReceiptCode = p_pJson->GetNodeValue("/body/receipt_code", "");

				m_oBody.m_strCurUserDeptCode = p_pJson->GetNodeValue("/body/cuur_user_dept_code", "");

				m_oBody.m_strIsFeedBack = p_pJson->GetNodeValue("/body/is_feedback", "");
				m_oBody.m_strIsVisitor = p_pJson->GetNodeValue("/body/is_visitor", "");

				m_oBody.m_strSeatNo = p_pJson->GetNodeValue("/body/receipt_seatno", "");
				/*if (m_oBody.m_strSeatNo.empty())
				{
					m_oBody.m_strSeatNo = m_oHeader.m_strSeatNo;
				}*/

				m_oBody.m_strCityCode = p_pJson->GetNodeValue("/body/city_code", "");
				
				m_oBody.m_strCallerKey = p_pJson->GetNodeValue("/body/caller_key", "");
				
				m_oBody.m_strIsInvalid = p_pJson->GetNodeValue("/body/is_invalid", "");
				
				m_oBody.m_strQueryKey = p_pJson->GetNodeValue("/body/query_key", "");

				m_oBody.m_strMajorAlarmFlag = p_pJson->GetNodeValue("/body/majoralarm_flag", "");

				m_oBody.m_strIsClosure = p_pJson->GetNodeValue("/body/is_over", "");

				m_oBody.m_strReceiverCode = p_pJson->GetNodeValue("/body/receiver_code", "");

				m_oBody.m_strCodeWhenQueryAll = p_pJson->GetNodeValue("/body/code_when_query_all", "");

				m_oBody.m_strTelHotLine = p_pJson->GetNodeValue("/body/is_telhotline", "");
				m_oBody.m_str110Transfer12345 = p_pJson->GetNodeValue("/body/110_transfer_12345", "");
				m_oBody.m_str12345Transfer110 = p_pJson->GetNodeValue("/body/12345_transfer_110", "");
				m_oBody.m_strIsSigned = p_pJson->GetNodeValue("/body/is_signed", "");
				return true;
			}

		public:

			class CBody
			{
			public:
				std::string m_strVcsSyncFlag;
				std::string	m_strPageSize;					//ÿҳ����������Ϊ�գ�
				std::string	m_strPageIndex;					//ҳ�룬1��ʾ��һҳ������Ϊ�գ�
				std::string	m_strBeginTime;					//��ѯ��ʼʱ�䣨����Ϊ�գ�
				std::string	m_strEndTime;					//��ѯ����ʱ�䣨����Ϊ�գ�
				std::string	m_strID;						//����id��ģ����ѯ��
				std::string	m_strTitle;						//������⣨ģ����ѯ��
				std::string	m_strContent;					//�������ݣ�ģ����ѯ��
				std::string	m_strAddr;						//������ϸ������ַ��ģ����ѯ��
				std::string	m_strLevel;						//����״̬����ֵ��ѯ�����ŷָ���
				std::string	m_strState;						//���鼶�𣨶�ֵ��ѯ�����ŷָ���
				std::string	m_strSourceType;				//���鱨����Դ���ͣ���ֵ��ѯ�����ŷָ���
				std::string	m_strHandleType;				//���鴦�����ͣ���ֵ��ѯ�����ŷָ���
				std::string m_strIsVerity;					//ֻ��ʾ��Ч�����־
				std::string	m_strFirstType;					//����һ�����ͣ���ֵ��ѯ�����ŷָ���
				std::string	m_strSecondType;				//����������ͣ���ֵ��ѯ�����ŷָ���
				std::string	m_strThirdType;					//�����������ͣ���ֵ��ѯ�����ŷָ���
				std::string	m_strFourthType;				//�����ļ����ͣ���ֵ��ѯ�����ŷָ�)

				std::string	m_strFeedbackFirstType;			//��������һ�����ͣ���ֵ��ѯ�����ŷָ���
				std::string	m_strFeedbackSecondType;		//��������������ͣ���ֵ��ѯ�����ŷָ���
				std::string	m_strFeedbackThirdType;			//���������������ͣ���ֵ��ѯ�����ŷָ���
				std::string	m_strFeedbackFourthType;		//���������ļ����ͣ���ֵ��ѯ�����ŷָ���

				std::string m_strEventType;					//�¼����ͣ�����汩�ȣ����룬���Ÿ�����
				std::string	m_strCalledNoType;				//���鱨���������ͣ���ֵ��ѯ�����ŷָ���
				std::string	m_strCallerNo;					//���鱨���˺��루ģ����ѯ��
				std::string	m_strCallerName;				//���鱨����������ģ����ѯ��
				std::string	m_strContactNo;					//������ϵ�˺��루ģ����ѯ��
				std::string	m_strContactName;				//������ϵ��������ģ����ѯ��
				std::string	m_strAdminDeptCode;				//�����Ͻ��λ���루��ֵ��ѯ�����ŷָ���
				std::string	m_strAdminDeptCodeRecursion;	//0��ʾ���ݹ��ѯ��1��ʾ�ݹ��ѯ
				std::string	m_strReceiptDeptCode;			//����Ӿ���λ���루�ݹ��ѯ��
				std::string	m_strReceiptDeptCodeRecursion;	//0��ʾ���ݹ��ѯ��1��ʾ�ݹ��ѯ
				std::string	m_strLeaderCode;				//����ֵ���쵼���ţ���ֵ��ѯ�����ŷָ���
				std::string	m_strReceiptCode;				//����Ӿ��˾��ţ���ֵ��ѯ�����ŷָ���
				std::string m_strCurUserDeptCode;			//��ǰ�û��������ű�ţ�ֻ�����ѯ�û��������ż����Ӳ��ŵľ��飩

				std::string m_strIsFeedBack;				//�Ƿ��ѷ���0��δ������1���ѷ���				
				std::string m_strIsVisitor;					//�Ƿ��ѻط�0��δ�طã�1���ѻط�		

				std::string m_strSeatNo;                    //�Ӿ�̨��(ϯλ��)
				std::string m_strCityCode;
				std::string m_strCallerKey;                 //��ѯ��ֵ������д�ֵ
                                                            //ֻ�������������������˵绰���룬������֤���ž�ȷƥ��
				std::string m_strIsInvalid;                 //�Ƿ���Ч
				
				std::string m_strQueryKey;                  //��ѯ�����ֵ������д�ֵ
                                                            //ֻ�������ţ������ַ����������ģ��ƥ��

				std::string m_strMajorAlarmFlag;			//�ش����ѯ��ʶ��1����ѯ�ش���

				std::string m_strIsClosure;					// �Ƿ�᰸��0��δ�᰸��1���ѽ᰸

				std::string m_strReceiverCode;

				std::string m_strCodeWhenQueryAll;

				std::string m_strAlarmType;					//������𣨶�ֵ��ѯ�����ŷָ���

				std::string m_strTelHotLine;				//�Ƿ��ѯ����12345����

				std::string m_str12345Transfer110;
				std::string m_str110Transfer12345;
				std::string m_strIsSigned;
			};


			CHeaderEx m_oHeader;
			CBody m_oBody;
		};
	}
}