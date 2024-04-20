#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		//ǰ�˵��ýӿ�
		class CPushAlarmRequest :
			public IReceive
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strAlarmID = p_pJson->GetNodeValue("/body/alarm_id", "");
				m_oBody.m_strJqlbmc = p_pJson->GetNodeValue("/body/jqlbmc", "");
				m_oBody.m_strJqlxmc = p_pJson->GetNodeValue("/body/jqlxmc", "");
				m_oBody.m_strJqxlmc = p_pJson->GetNodeValue("/body/jqxlmc", "");
				m_oBody.m_strJqzlmc = p_pJson->GetNodeValue("/body/jqzlmc", "");
				m_oBody.m_strOptType = p_pJson->GetNodeValue("/body/opt_type", "");

				return true;
			}

		public:
			class CBody
			{
			public:
				std::string m_strAlarmID;						//������
				std::string m_strJqlbmc;
				std::string m_strJqlxmc;
				std::string m_strJqxlmc;
				std::string m_strJqzlmc;

				std::string m_strOptType;						//���ͷ�ʽ 0:�Զ����� 1:�ֶ�����
			};
			CHeaderEx m_oHeader;
			CBody m_oBody;
		};

		class CRepulseAlarmRequest :
			public IReceive
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strWorkID = p_pJson->GetNodeValue("/body/flowid", "");
				m_oBody.m_strAlarmID = p_pJson->GetNodeValue("/body/jjdbh", "");
				m_oBody.m_strRepulser = p_pJson->GetNodeValue("/body/thry", "");
				m_oBody.m_strRepulseAlarmTime = p_pJson->GetNodeValue("/body/thsj", "");
				m_oBody.m_strReason = p_pJson->GetNodeValue("/body/thyy", "");
				m_oBody.m_strRepulseDept = p_pJson->GetNodeValue("/body/dept", "");

				return true;
			}

		public:
			class CBody
			{
			public:
				std::string m_strWorkID;
				std::string m_strAlarmID;						//������
				std::string m_strRepulser;
				std::string m_strRepulseAlarmTime;
				std::string m_strReason;
				std::string m_strRepulseDept;
			};
			CHeaderEx m_oHeader;
			CBody m_oBody;
		};

		//����12345�ӿڲ���
		typedef struct AlarmDatas
		{
			std::string m_strAlarmPhone;						//�����绰
			std::string BJDHYHM;								//�����绰�û���
			std::string m_strReportPlace;						//������ַ
			std::string m_strAlarmContent;						//��������
			std::string m_strReporterSex;						//�������Ա����
			std::string m_strAlarmTime;							//����ʱ��
			std::string GXDWDH;									//��Ͻ��λ�绰
			std::string GXDWDM;									//��Ͻ��λ����
			std::string GXDWMC;									//��Ͻ��λ����
			std::string m_strAlarmId;							//������
			std::string JJDWDM;									//�Ӿ���λ����
			std::string JJDWMC;									//�Ӿ���λ����
			std::string m_strAlarmType;							//�Ӿ�����
			std::string m_strAlarmerID;							//�Ӿ�Ա���
			std::string m_strAlarmCategoryID;					//����������
			std::string m_strAlarmCategoryName;					//�����������
			std::string m_strAlarmTypeID;						//�������ʹ���
			std::string m_strAlarmTypeName;						//������������
			std::string LSH;									//��ˮ��
			std::string m_strCallNumber;						//��ϵ�绰
			std::string m_strAlarmIsSecrecy;					//�Ƿ��� 0����1���ǣ�Ĭ��Ϊ0
			std::string Title;									//����
			std::string m_strAlarmAreaNumber;					//������������
		}AlarmDatas;

		class CPushAlarmTo12345Request :
			public IRespond
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				//m_oHeader.SaveTo(p_pJson);

				p_pJson->SetNodeValue("/BJDH", m_oBody.m_alarmDatas.m_strAlarmPhone);
				p_pJson->SetNodeValue("/BJDHYHM", m_oBody.m_alarmDatas.BJDHYHM);
				p_pJson->SetNodeValue("/BJDZ", m_oBody.m_alarmDatas.m_strReportPlace);
				p_pJson->SetNodeValue("/BJNR", m_oBody.m_alarmDatas.m_strAlarmContent);
				p_pJson->SetNodeValue("/BJRXBDM", m_oBody.m_alarmDatas.m_strReporterSex);
				p_pJson->SetNodeValue("/GXDWDH", m_oBody.m_alarmDatas.GXDWDH);
				p_pJson->SetNodeValue("/GXDWDM", m_oBody.m_alarmDatas.GXDWDM);
				p_pJson->SetNodeValue("/GXDWMC", m_oBody.m_alarmDatas.GXDWMC);
				p_pJson->SetNodeValue("/BJSJ", m_oBody.m_alarmDatas.m_strAlarmTime);
				p_pJson->SetNodeValue("/JJDBH", m_oBody.m_alarmDatas.m_strAlarmId);
				p_pJson->SetNodeValue("/JJDWDM", m_oBody.m_alarmDatas.JJDWDM);
				p_pJson->SetNodeValue("/JJDWMC", m_oBody.m_alarmDatas.JJDWMC);
				p_pJson->SetNodeValue("/JJLX", m_oBody.m_alarmDatas.m_strAlarmType);
				p_pJson->SetNodeValue("/JJYBH", m_oBody.m_alarmDatas.m_strAlarmerID);
				p_pJson->SetNodeValue("/JQLBDM", m_oBody.m_alarmDatas.m_strAlarmCategoryID);
				p_pJson->SetNodeValue("/JQLBMC", m_oBody.m_alarmDatas.m_strAlarmCategoryName);
				p_pJson->SetNodeValue("/LSH", m_oBody.m_alarmDatas.LSH);
				p_pJson->SetNodeValue("/LXDH", m_oBody.m_alarmDatas.m_strCallNumber);
				p_pJson->SetNodeValue("/SFBM", m_oBody.m_alarmDatas.m_strAlarmIsSecrecy);
				p_pJson->SetNodeValue("/Title", m_oBody.m_alarmDatas.Title);
				p_pJson->SetNodeValue("/XZQHDM", m_oBody.m_alarmDatas.m_strAlarmAreaNumber);

				return p_pJson->ToString();
			}

			bool ParseAlarmRecord(DataBase::IResultSetPtr& l_pRSet, PROTOCOL::CPushAlarmRequest& p_sRequest)
			{
				if (NULL == l_pRSet.get())
				{
					return false;
				}
				m_oBody.m_alarmDatas.m_strAlarmPhone = l_pRSet->GetValue("bjdh");
				m_oBody.m_alarmDatas.m_strReportPlace = l_pRSet->GetValue("bjdz");
				if (m_oBody.m_alarmDatas.m_strReportPlace.empty())
				{
					m_oBody.m_alarmDatas.m_strReportPlace = l_pRSet->GetValue("jqdz");
				}
				m_oBody.m_alarmDatas.m_strAlarmContent = l_pRSet->GetValue("bjnr");
				m_oBody.m_alarmDatas.m_strReporterSex = l_pRSet->GetValue("bjrxbdm");
				m_oBody.m_alarmDatas.m_strAlarmTime = l_pRSet->GetValue("bjsj");
				m_oBody.m_alarmDatas.m_strAlarmId = l_pRSet->GetValue("jjdbh");
				m_oBody.m_alarmDatas.m_strAlarmType = l_pRSet->GetValue("jjlx");
				m_oBody.m_alarmDatas.m_strAlarmerID = l_pRSet->GetValue("jjybh");
				m_oBody.m_alarmDatas.m_strAlarmCategoryID = l_pRSet->GetValue("jqlbdm");
				m_oBody.m_alarmDatas.m_strAlarmCategoryName = p_sRequest.m_oBody.m_strJqlbmc;
				m_oBody.m_alarmDatas.m_strAlarmTypeID = l_pRSet->GetValue("jqlxdm");
				m_oBody.m_alarmDatas.m_strAlarmTypeName = p_sRequest.m_oBody.m_strJqlxmc;
				m_oBody.m_alarmDatas.m_strCallNumber = l_pRSet->GetValue("lxdh");
				m_oBody.m_alarmDatas.m_strAlarmIsSecrecy = l_pRSet->GetValue("sfbm");
				m_oBody.m_alarmDatas.m_strAlarmAreaNumber = l_pRSet->GetValue("xzqhdm");
				m_oBody.m_alarmDatas.BJDHYHM = l_pRSet->GetValue("bjdhyhm");
				m_oBody.m_alarmDatas.GXDWDH = l_pRSet->GetValue("bjdhyhm");//�����ѯ
				m_oBody.m_alarmDatas.GXDWDM = l_pRSet->GetValue("gxdwdm");
				m_oBody.m_alarmDatas.GXDWMC = l_pRSet->GetValue("gxdwmc");
				m_oBody.m_alarmDatas.JJDWDM = l_pRSet->GetValue("jjdwdm");
				m_oBody.m_alarmDatas.JJDWMC = l_pRSet->GetValue("gxdwmc");//�����ѯ
				m_oBody.m_alarmDatas.LSH = l_pRSet->GetValue("jjdbh");
				m_oBody.m_alarmDatas.Title = m_oBody.m_alarmDatas.m_strAlarmCategoryName + "-" + m_oBody.m_alarmDatas.m_strAlarmTypeName;
				return true;
			}

		public:
			class CBody
			{
			public:
				AlarmDatas m_alarmDatas;
			};
			CHeaderEx m_oHeader;
			CBody m_oBody;
		};

		class CRepulseAlarmRequestTo12345 :
			public IRespond
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				//m_oHeader.SaveTo(p_pJson);

				p_pJson->SetNodeValue("/FLOWID", m_oBody.m_strWorkID);
				p_pJson->SetNodeValue("/HANDLEBMNAME", m_oBody.HANDLEBMNAME);
				p_pJson->SetNodeValue("/HANDLEOPINION", m_oBody.m_strReason);
				p_pJson->SetNodeValue("/HANDLETIME", m_oBody.m_strRepulseAlarmTime);
				p_pJson->SetNodeValue("/HANDLEUSERNAME", m_oBody.m_strRepulser);
				p_pJson->SetNodeValue("/JJDBH", m_oBody.m_strAlarmID);

				return p_pJson->ToString();
			}

		public:
			class CBody
			{
			public:
				std::string m_strWorkID;						//12345ϵͳ�Ĺ���ID
				std::string m_strAlarmID;						//�Ӿ������
				std::string m_strRepulser;						//�˻ؽӾ�Ա
				std::string m_strRepulseAlarmTime;              //�˻�ʱ��
				std::string m_strReason;						//�˻�ԭ��
				std::string HANDLEBMNAME;						//�˻ز���
			};
			CHeaderEx m_oHeader;
			CBody m_oBody;
		};

		//----------------------------110������������12345-------------------------------------------
		class CPushRevisitRequest :
			public IReceive
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strDisposalDec = p_pJson->GetNodeValue("/body/disposalDec", "");
				m_oBody.m_strFeedbackOrgCode = p_pJson->GetNodeValue("/body/feedbackOrgCode", "");
				m_oBody.m_strFeedbackOrgName = p_pJson->GetNodeValue("/body/feedbackOrgName", "");
				m_oBody.m_strFeedbackTime = p_pJson->GetNodeValue("/body/feedbackTime", "");
				m_oBody.m_strFeedbackCode = p_pJson->GetNodeValue("/body/feedbackCode", "");
				m_oBody.m_strFeedbackName = p_pJson->GetNodeValue("/body/feedbackName", "");
				m_oBody.m_str12345WorkId = "";
				m_oBody.m_strCaseId = p_pJson->GetNodeValue("/body/caseId", "");
				m_oBody.m_strResultDes = p_pJson->GetNodeValue("/body/resultDes", "");
				m_oBody.m_strCreateTime = p_pJson->GetNodeValue("/body/createTime", "");

				return true;
			}

			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				//m_oHeader.SaveTo(p_pJson);

				p_pJson->SetNodeValue("/CJCZQK", m_oBody.m_strDisposalDec);
				p_pJson->SetNodeValue("/FKDWDM", m_oBody.m_strFeedbackOrgCode);
				p_pJson->SetNodeValue("/FKDWMC", m_oBody.m_strFeedbackOrgName);
				p_pJson->SetNodeValue("/FKSJ", m_oBody.m_strFeedbackTime);
				p_pJson->SetNodeValue("/FKYBH", m_oBody.m_strFeedbackCode);
				p_pJson->SetNodeValue("/FKYXM", m_oBody.m_strFeedbackName);
				p_pJson->SetNodeValue("/FLOWID", m_oBody.m_str12345WorkId);
				p_pJson->SetNodeValue("/JJDBH", m_oBody.m_strCaseId);
				p_pJson->SetNodeValue("/JQCLJGSM", m_oBody.m_strResultDes);

				return p_pJson->ToString();
			}

		public:
			class CBody
			{
			public:
				std::string m_strDisposalDec;					//�����������
				std::string m_strFeedbackOrgCode;				//������λ����
				std::string m_strFeedbackOrgName;				//������λ����
				std::string m_strFeedbackTime;					//����ʱ��
				std::string m_strFeedbackCode;					//����Ա���
				std::string m_strFeedbackName;					//����Ա����
				std::string m_str12345WorkId;					//12345ϵͳ�Ĺ���ID
				std::string m_strCaseId;						//�Ӿ������
				std::string m_strResultDes;						//���鴦����˵��
				std::string m_strCreateTime;
			};
			CHeaderEx m_oHeader;
			CBody m_oBody;
		};
	}
}
