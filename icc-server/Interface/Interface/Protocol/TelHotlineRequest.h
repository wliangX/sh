#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/IRespond.h>
#include "Protocol/CHeader.h"

namespace ICC
{
	namespace PROTOCOL
	{
		//110ת��12345�Ǿ�������Ϣ
		class CTelHotlineVisitRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_oBody.m_strAlarmID = p_pJson->GetNodeValue("/body/JJDBH", "");
				m_oBody.m_strFlowID = p_pJson->GetNodeValue("/body/FLOWID", "");
				m_oBody.m_strVisitTime = p_pJson->GetNodeValue("/body/HFSJ", "");
				m_oBody.m_strSendReason = p_pJson->GetNodeValue("/body/DYYY", "");
				//�طò���(Visit)
				m_oBody.m_strVisitContent = p_pJson->GetNodeValue("/body/CLJGMC", "");
				m_oBody.m_strVisitResult = p_pJson->GetNodeValue("/body/CLJGDM", ""); 
				m_oBody.m_strTelVisitResult = p_pJson->GetNodeValue("/body/DHHFHSQK", "");
				m_oBody.m_strSendTime = p_pJson->GetNodeValue("/body/TSSJ", "");
				//��������(Feedback)
				m_oBody.m_strFeedbackTime = p_pJson->GetNodeValue("/body/FKSJ", "");
				m_oBody.m_strAlarmDisposal = p_pJson->GetNodeValue("/body/CJCZQK", "");
				m_oBody.m_strAlarmResult = p_pJson->GetNodeValue("/body/JQCLJGSM", "");
				m_oBody.m_strFeedbackId = p_pJson->GetNodeValue("/body/FKYBH", "");
				m_oBody.m_strFeedbackName = p_pJson->GetNodeValue("/body/FKYXM", "");
				m_oBody.m_strFeedbackDeptCode = p_pJson->GetNodeValue("/body/FKDWDM", "");
				m_oBody.m_strFeedbackDeptName = p_pJson->GetNodeValue("/body/FKDWMC", "");
				//�˻ز���(Return)
				m_oBody.m_strReturnAlarmer = p_pJson->GetNodeValue("/body/HANDLEUSERNAME", "");
				m_oBody.m_strReturnDeptName = p_pJson->GetNodeValue("/body/HANDLEBMNAME", "");
				m_oBody.m_strReturnResult = p_pJson->GetNodeValue("/body/HANDLEOPINION", "");
				m_oBody.m_strReturnTime = p_pJson->GetNodeValue("/body/HANDLETIME", "");
				return true;
			}
		public:
			CHeaderEx m_oHeader;
			class CBody
			{
			public:
				std::string m_strAlarmID;             //�Ӿ���ID
				std::string m_strFlowID;              //����ID
				std::string m_strSendTime;            //����ʱ��
				std::string m_strSendReason;		  //����ԭ��

				std::string m_strVisitTime;           //�ط�ʱ��  YYMMDDhhmmss
				std::string m_strVisitContent;        //�ط���������
				std::string m_strVisitResult;         //�ط����۽��  1 - �ǳ����⡢2 - ���⡢3 - һ�㡢4 - �����⡢5 - �ǳ�������
				std::string m_strTelVisitResult;      //�绰�طú�ʵ���

				std::string m_strFeedbackTime;        //����ʱ��
				std::string m_strAlarmDisposal;		  //�����������
				std::string m_strAlarmResult;		  //����������
				std::string m_strFeedbackId;		  //����������Ա���
				std::string m_strFeedbackName;		  //����������Ա����
				std::string m_strFeedbackDeptCode;    //����������λ���
				std::string m_strFeedbackDeptName;    //����������λ����

				std::string m_strReturnAlarmer;       //�˻ؽӾ�Ա
				std::string m_strReturnDeptName;      //�˻ز���
				std::string m_strReturnResult;        //�˻�ԭ��
				std::string m_strReturnTime;          //�˻�ԭ��
			};
			CBody m_oBody;
		};

		//110ת��12345ȷ����Ϣ
		class CTelHotlineConfirmRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_oBody.m_strAlarmID = p_pJson->GetNodeValue("/body/JJDBH", "");
				m_oBody.m_strReceiveState = p_pJson->GetNodeValue("/body/RECEIVE", "");
				m_oBody.m_strReceiveTime = p_pJson->GetNodeValue("/body/RECEIVETIME", "");
				return true;
			}
		public:
			CHeaderEx m_oHeader;
			class CBody
			{
			public:
				std::string m_strAlarmID;                //�Ӿ���ID
				std::string m_strReceiveState;           //��ȡ״̬ 0:δ��ȡ, 1:��ȡ
				std::string m_strReceiveTime;            //��ȡʱ��
			};
			CBody m_oBody;
		};

		class CSyncLinkedTableState :
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

				p_pJson->SetNodeValue("/body/id", m_oBody.id);
				p_pJson->SetNodeValue("/body/alarm_id", m_oBody.alarm_id);
				p_pJson->SetNodeValue("/body/state", m_oBody.state);
				p_pJson->SetNodeValue("/body/linked_org_code", m_oBody.linked_org_code);
				p_pJson->SetNodeValue("/body/linked_org_name", m_oBody.linked_org_name);
				p_pJson->SetNodeValue("/body/linked_org_type", m_oBody.linked_org_type);
				p_pJson->SetNodeValue("/body/dispatch_code", m_oBody.dispatch_code);
				p_pJson->SetNodeValue("/body/dispatch_name", m_oBody.dispatch_name);
				p_pJson->SetNodeValue("/body/create_user", m_oBody.create_user);
				p_pJson->SetNodeValue("/body/create_time", m_oBody.create_time);
				p_pJson->SetNodeValue("/body/update_user", m_oBody.update_user);
				p_pJson->SetNodeValue("/body/update_time", m_oBody.update_time);
				p_pJson->SetNodeValue("/body/result", m_oBody.result);

				return p_pJson->ToString();
			}

			std::string ToStringEx(JsonParser::IJsonPtr p_pJson)
			{
				m_oHeader.SaveTo(p_pJson);
				p_pJson->SetNodeValue("/body/msg_source", m_oBody.m_strMsgSource);
				unsigned int l_uiIndex = 0;

				std::string l_strPrefixPath("/body/linked_data/" + std::to_string(l_uiIndex) + "/");

				p_pJson->SetNodeValue(l_strPrefixPath + "id", m_oBody.id);
				p_pJson->SetNodeValue(l_strPrefixPath + "alarm_id", m_oBody.alarm_id);
				p_pJson->SetNodeValue(l_strPrefixPath + "state", m_oBody.state);
				p_pJson->SetNodeValue(l_strPrefixPath + "linked_org_code", m_oBody.linked_org_code);
				p_pJson->SetNodeValue(l_strPrefixPath + "linked_org_name", m_oBody.linked_org_name);
				p_pJson->SetNodeValue(l_strPrefixPath + "linked_org_type", m_oBody.linked_org_type);
				p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_code", m_oBody.dispatch_code);
				p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_name", m_oBody.dispatch_name);
				p_pJson->SetNodeValue(l_strPrefixPath + "create_user", m_oBody.create_user);
				p_pJson->SetNodeValue(l_strPrefixPath + "create_time", m_oBody.create_time);
				p_pJson->SetNodeValue(l_strPrefixPath + "update_user", m_oBody.update_user);
				p_pJson->SetNodeValue(l_strPrefixPath + "update_time", m_oBody.update_time);
				p_pJson->SetNodeValue(l_strPrefixPath + "result", m_oBody.result);

				return p_pJson->ToString();
			}

		public:
			class CBody
			{
			public:
				std::string id;
				std::string alarm_id;
				std::string state;
				std::string linked_org_code;
				std::string linked_org_name;
				std::string linked_org_type;
				std::string dispatch_code;
				std::string dispatch_name;
				std::string create_user;
				std::string create_time;
				std::string update_user;
				std::string update_time;
				std::string result;

				std::string m_strMsgSource;
			};
			CHeaderEx m_oHeader;
			CBody m_oBody;
		};

		//---------------���ݰ汾����----------------
		//110ת��12345�������̰���
		//class CTelHotlineAddRequest :
		//	public IRequest
		//{
		//public:
		//	virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
		//	{
		//		if (!m_oHeader.ParseString(p_strReq, p_pJson))
		//		{
		//			return false;
		//		}
		//		m_oBody.m_strRunID = p_pJson->GetNodeValue("/body/runid", "");
		//		m_oBody.m_strFlowID = p_pJson->GetNodeValue("/body/flowid", "");
		//		m_oBody.m_strCode = p_pJson->GetNodeValue("/body/code", "");
		//		m_oBody.m_strNodeName = p_pJson->GetNodeValue("/body/node_name", "");
		//		m_oBody.m_strNodeType = p_pJson->GetNodeValue("/body/node_type", "");
		//		m_oBody.m_strHandleBMName = p_pJson->GetNodeValue("/body/handle_bm_name", "");
		//		m_oBody.m_strHandleUserName = p_pJson->GetNodeValue("/body/handle_user_name", "");
		//		m_oBody.m_strHandleTime = p_pJson->GetNodeValue("/body/handle_time", "");
		//		m_oBody.m_strHandleOpinion = p_pJson->GetNodeValue("/body/handle_opinion", "");
		//		m_oBody.m_strIsPolice = p_pJson->GetNodeValue("/body/is_police", "");
		//		m_oBody.m_strPoliceCode = p_pJson->GetNodeValue("/body/police_code", "");
		//		return true;
		//	}
		//public:
		//	CHeaderEx m_oHeader;
		//	class CBody
		//	{
		//	public:
		//		std::string m_strRunID;              //��ת����ID
		//		std::string	m_strFlowID;	         //����ID
		//		std::string	m_strCode;	             //������
		//		std::string m_strNodeName;	         //�ڵ�����
		//		std::string m_strNodeType;	         //�ڵ���� 1 - ���졢2 - �ɵ���3 - �˻ء�4 - ��ᡢ5 - �鵵��6 - ��ǩ��7 - ���ᡢ8 - �ذ�
		//		std::string m_strHandleBMName;	     //������              
		//		std::string m_strHandleUserName;	 //������               
		//		std::string m_strHandleTime;	     //����ʱ��           
		//		std::string m_strHandleOpinion;	     //�������            
		//		std::string m_strIsPolice;	         //ת110����  0 - 12345����1 - 12345ת110����
		//		std::string m_strPoliceCode;	     //�Ӿ������           jjdbh
		//	};
		//	CBody m_oBody;
		//};

		////110ת��12345������������
		//class CTelHotlineFpideRequest :
		//	public IRequest
		//{
		//public:
		//	virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
		//	{
		//		if (!m_oHeader.ParseString(p_strReq, p_pJson))
		//		{
		//			return false;
		//		}
		//		m_oBody.m_strFPID = p_pJson->GetNodeValue("/body/fpid", "");
		//		m_oBody.m_strFlowID = p_pJson->GetNodeValue("/body/flowid", "");
		//		m_oBody.m_strCode = p_pJson->GetNodeValue("/body/code", "");
		//		m_oBody.m_strRunID = p_pJson->GetNodeValue("/body/runid", "");
		//		m_oBody.m_strApplyBMName = p_pJson->GetNodeValue("/body/apply_bm_name", "");
		//		m_oBody.m_strApplyUserName = p_pJson->GetNodeValue("/body/apply_user_name", "");
		//		m_oBody.m_strApplyTimeLimit = p_pJson->GetNodeValue("/body/apply_time_limit", "");
		//		m_oBody.m_strApplyContent = p_pJson->GetNodeValue("/body/apply_content", "");
		//		m_oBody.m_strApplyTime = p_pJson->GetNodeValue("/body/apply_time", "");
		//		m_oBody.m_strPoliceCode = p_pJson->GetNodeValue("/body/police_code", "");
		//		return true;
		//	}
		//public:
		//	CHeaderEx m_oHeader;
		//	class CBody
		//	{
		//	public:
		//		std::string m_strFPID;	            //��������ID
		//		std::string m_strFlowID;	        //����ID
		//		std::string m_strCode;	            //������
		//		std::string m_strRunID;	            //��ת����ID
		//		std::string m_strApplyBMName;	    //���벿��
		//		std::string m_strApplyUserName;	    //������
		//		std::string m_strApplyTimeLimit;	//����ʱ��
		//		std::string m_strApplyContent;	    //��������
		//		std::string m_strApplyTime;	        //����ʱ�� YYMMDDhhmmss
		//		std::string m_strPoliceCode;	    //�Ӿ������
		//	};
		//	CBody m_oBody;
		//};

		////110ת��12345��������״̬
		//class CTelHotlineHandleStateRequest :
		//	public IRequest
		//{
		//public:
		//	virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
		//	{
		//		if (!m_oHeader.ParseString(p_strReq, p_pJson))
		//		{
		//			return false;
		//		}
		//		m_oBody.m_strFlowID = p_pJson->GetNodeValue("/body/flowid", "");
		//		m_oBody.m_strCode = p_pJson->GetNodeValue("/body/code", "");
		//		m_oBody.m_strPoliceCode = p_pJson->GetNodeValue("/body/police_code", "");
		//		m_oBody.m_strHandleState = p_pJson->GetNodeValue("/body/handle_state", "");
		//		return true;
		//	}
		//public:
		//	CHeaderEx m_oHeader;
		//	class CBody
		//	{
		//	public:
		//		std::string m_strFlowID;           //����ID
		//		std::string m_strCode;             //������
		//		std::string m_strPoliceCode;       //�Ӿ������
		//		std::string m_strHandleState;      //����״̬   1 - ��ǩ�ա�2 - �ѽ��졢3 - ������4 - �Ѱ�ᡢ5 - �ѹ鵵
		//	};
		//	CBody m_oBody;
		//};

		//110ת��12345�����ط���Ϣ
		// class CTelHotlineVisitRequest :
			// public IRequest
		// {
		// public:
			// virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			// {
				// if (!m_oHeader.ParseString(p_strReq, p_pJson))
				// {
					// return false;
				// }
				// m_oBody.m_strVisitID = p_pJson->GetNodeValue("/body/visitid", "");
				// m_oBody.m_strFlowID = p_pJson->GetNodeValue("/body/flowid", "");
				// m_oBody.m_strCode = p_pJson->GetNodeValue("/body/code", "");
				// m_oBody.m_strVisitMethod = p_pJson->GetNodeValue("/body/visit_method", "");
				// m_oBody.m_strUserName = p_pJson->GetNodeValue("/body/user_nmae", "");
				// m_oBody.m_strVisitTime = p_pJson->GetNodeValue("/body/visit_time", "");
				// m_oBody.m_strSeatsContent = p_pJson->GetNodeValue("/body/seats_content", "");
				// m_oBody.m_strSeatsResult = p_pJson->GetNodeValue("/body/seats_result", "");
				// m_oBody.m_strBMContent = p_pJson->GetNodeValue("/body/bm_content", "");
				// m_oBody.m_strBMResult = p_pJson->GetNodeValue("/body/bm_result", "");
				// m_oBody.m_strPoliceCode = p_pJson->GetNodeValue("/body/police_code", "");
				// return true;
			// }
		// public:
			// CHeaderEx m_oHeader;
			// class CBody
			// {
			// public:
				// std::string m_strVisitID;             //�ط�ID
				// std::string m_strFlowID;              //����ID
				// std::string m_strCode;                //������
				// std::string m_strVisitMethod;         //�ط÷�ʽ 1 - �绰��2 - ���š�3 - �Ż���վ
				// std::string m_strUserName;            //�ط���
				// std::string m_strVisitTime;           //�ط�ʱ��  YYMMDDhhmmss
				// std::string m_strSeatsContent;        //����Ա��������
				// std::string m_strSeatsResult;         //����Ա���۽��  1 - �ǳ����⡢2 - ���⡢3 - һ�㡢4 - �����⡢5 - �ǳ�������
				// std::string m_strBMContent;           //������������
				// std::string m_strBMResult;            //�������۽��  1 - �ǳ����⡢2 - ���⡢3 - һ�㡢4 - �����⡢5 - �ǳ�������
				// std::string m_strPoliceCode;          //�Ӿ������
			// };
			// CBody m_oBody;
		// };

		////110ת��12345����������
		//class CTelHotlineRequest :
		//	public IRequest
		//{
		//public:
		//	virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
		//	{
		//		if (!m_oHeader.ParseString(p_strReq, p_pJson))
		//		{
		//			return false;
		//		}
		//		m_oBody.m_strGDBH = p_pJson->GetNodeValue("/body/code", "");
		//		m_oBody.m_strBLR = p_pJson->GetNodeValue("/body/handleusername", "");
		//		m_oBody.m_strBLZT = p_pJson->GetNodeValue("/body/handlestate", "");
		//		m_oBody.m_strBLBM = p_pJson->GetNodeValue("/body/handlebmname", "");
		//		m_oBody.m_strBLJG = p_pJson->GetNodeValue("/body/handleopinion", "");
		//		m_oBody.m_strBLSJ = p_pJson->GetNodeValue("/body/handletime", "");
		//		m_oBody.m_strPoliceCode = p_pJson->GetNodeValue("/body/police_code", "");
		//		return true;
		//	}
		//public:
		//	CHeaderEx m_oHeader;
		//	class CBody
		//	{
		//	public:
		//		std::string m_strGDBH;	        //�������
		//		std::string m_strBLR;	        //������
		//		std::string m_strBLZT;          //����״̬
		//		std::string m_strBLBM;	        //������
		//		std::string m_strBLJG;	        //������  1��� 2�˻�
		//		std::string m_strBLSJ;          //����ʱ��
		//		std::string m_strPoliceCode;	//�Ӿ������

		//	};
		//	CBody m_oBody;
		//};

	}
}