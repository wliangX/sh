#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		/*****************************************************************
		 * add_or_update_shift_request_ex
		 ****************************************************************/
		class CShiftAddOrUpdateReqeust : public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return false;
				}

				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_strGuid = p_pJson->GetNodeValue("/body/guid", "");
				m_strStartTime = p_pJson->GetNodeValue("/body/start_time", "");
				m_strEndTime = p_pJson->GetNodeValue("/body/end_time", "");
				m_strDeptCode = p_pJson->GetNodeValue("/body/dept_code", "");
				m_strDutyCode = p_pJson->GetNodeValue("/body/duty_code", "");
				m_strDutyName = p_pJson->GetNodeValue("/body/duty_name", "");
				m_strDutyLeaderCode = p_pJson->GetNodeValue("/body/duty_leader_code", "");
				m_strDutyLeaderName = p_pJson->GetNodeValue("/body/duty_leader_name", "");
				m_strWorkContent = p_pJson->GetNodeValue("/body/work_content", "");
				m_strImportantContent = p_pJson->GetNodeValue("/body/important_content", "");
				m_strShiftContent = p_pJson->GetNodeValue("/body/shift_content", "");
				
				m_strReceiverCode = p_pJson->GetNodeValue("/body/receiver_code", "");
				m_strReceiverName = p_pJson->GetNodeValue("/body/receiver_name", "");
				m_strReceiveState = p_pJson->GetNodeValue("/body/receive_state", "");

				int uCount = p_pJson->GetCount("/body/data");

				for (int i = 0; i < uCount; ++i)
				{
					std::string l_strPrefixPath("/body/data/" + std::to_string(i));
					std::string strId = p_pJson->GetNodeValue(l_strPrefixPath, "");
					m_vecDatas.push_back(strId);
				}

				return true;
			}

		public:
			std::string	m_strGuid;						//id
			std::string m_strStartTime;					//��ʼʱ��
			std::string m_strEndTime;					//����ʱ��
			std::string m_strDeptCode;					//��λ����
			std::string m_strDutyCode;					//ֵ��Ա
			std::string m_strDutyName;					//ֵ��Ա����
			std::string m_strDutyLeaderCode;			//ֵ���쵼
			std::string m_strDutyLeaderName;			//ֵ���쵼����

			std::string m_strReceiverCode;              //�����˾�Ա���
			std::string m_strReceiverName;              //����������
			std::string m_strReceiveState;              //����״̬

			std::string m_strWorkContent;				//��������
			std::string m_strImportantContent;			//��Ҫ�¼�
			std::string m_strShiftContent;				//��������
			std::string m_strCreateUser;				//������
			std::string m_strCreateTime;				//����ʱ��
			std::string m_strUpdateUser;				//�޸���,ȡ���һ���޸�ֵ
			std::string m_strUpdateTime;				//�޸�ʱ��,ȡ���һ���޸�ֵ		

			std::vector<std::string> m_vecDatas;

			CHeaderEx m_oHeader;
		};


		/*****************************************************************
		 * 
		 ****************************************************************/
		class CShiftAddOrUpdateRespond : public IRespond
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";

				}
				p_pJson->SetNodeValue("/code", m_strCode);
				p_pJson->SetNodeValue("/message", m_strMessage);
				p_pJson->SetNodeValue("/msgid", m_strMsgId);

				return p_pJson->ToString();
			}

		public:
			std::string m_strCode;
			std::string m_strMessage;
			std::string m_strMsgId;
		};

		/*****************************************************************
		 * get_shift_request_ex
		 ****************************************************************/
		class CShiftQueryReqeust : public IRequest
		{		
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{				
				if (nullptr == p_pJson)
				{
					return false;
				}

				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_strStartTime = p_pJson->GetNodeValue("/body/start_time", "");
				m_strEndTime = p_pJson->GetNodeValue("/body/end_time", "");
				m_strDeptCode = p_pJson->GetNodeValue("/body/dept_code", "");
				m_strIsRecursive = p_pJson->GetNodeValue("/body/is_recursive", "");
				m_strDutyCode = p_pJson->GetNodeValue("/body/duty_code", "");
				
				m_strReceiverCode = p_pJson->GetNodeValue("/body/receiver_code", "");
				m_strPageSize = p_pJson->GetNodeValue("/body/page_size", "");
				m_strPageIndex = p_pJson->GetNodeValue("/body/page_index", "");
				
				m_isSelectEx = p_pJson->GetNodeValue("/body/enable_alarm", "");

				return true;
			}

		public:			
			std::string m_strStartTime;					//��ʼʱ��
			std::string m_strEndTime;					//����ʱ��
			std::string m_strDeptCode;					//��λ����
			std::string m_strIsRecursive;				//�Ƿ��ѯ�¼�����
			std::string m_strDutyCode;					//ֵ��Ա
			std::string m_strReceiverCode;				//�Ӱ���Ա

			std::string m_strPageSize;					//��ҳ��¼����
			std::string m_strPageIndex;			        //�ڼ�ҳ

			std::string m_isSelectEx;					//�Ƿ񲻷���δ����������־

			CHeaderEx m_oHeader;
		};

		/*****************************************************************
		 *
		 ****************************************************************/
		class CShiftQueryRespond : public IRespond
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				p_pJson->SetNodeValue("/code", m_strCode);
				p_pJson->SetNodeValue("/message", m_strMessage);
				p_pJson->SetNodeValue("/msgid", m_strMsgId);

				p_pJson->SetNodeValue("/data/allcount", m_strAllCount);
				p_pJson->SetNodeValue("/data/count", m_strCount);

				
				unsigned int uCount = m_vecDatas.size();
				for (unsigned int i = 0; i < uCount; ++i)
				{					
					std::map<std::string, std::string>::const_iterator itr;
					std::string l_strPrefixPath("/data/list/" + std::to_string(i) + "/");
					for (itr = m_vecDatas[i].begin(); itr != m_vecDatas[i].end(); ++itr)
					{
						p_pJson->SetNodeValue(l_strPrefixPath + itr->first, itr->second);
					}					
				}

				return p_pJson->ToString();
			}

		public:
			std::string m_strCode;
			std::string m_strMessage;
			std::string m_strMsgId;
			std::string m_strAllCount;
			std::string m_strCount;
			std::vector<std::map<std::string, std::string>> m_vecDatas;
		};
	
		 /*****************************************************************
		  * get_shift_relation_request
		  ****************************************************************/
		class CShiftQueryRelationReqeust : public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return false;
				}

				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_strShiftId = p_pJson->GetNodeValue("/body/shift_id", "");

				return true;
			}

		public:
			std::string m_strShiftId;					//���ӵ�ID

			CHeaderEx m_oHeader;
		};

		/*****************************************************************
		 *
		 ****************************************************************/
		class CShiftQueryRelationRespond : public IRespond
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				p_pJson->SetNodeValue("/code", m_strCode);
				p_pJson->SetNodeValue("/message", m_strMessage);
				p_pJson->SetNodeValue("/msgid", m_strMsgId);
				
				p_pJson->SetNodeValue("/data/count", m_strCount);

				unsigned int uCount = m_vecDatas.size();
				for (unsigned int i = 0; i < uCount; ++i)
				{

					std::map<std::string, std::string>::const_iterator itr;
					std::string l_strPrefixPath("/data/list/" + std::to_string(i) + "/");
					for (itr = m_vecDatas[i].begin(); itr != m_vecDatas[i].end(); ++itr)
					{
						p_pJson->SetNodeValue(l_strPrefixPath + itr->first, itr->second);
					}
				}

				return p_pJson->ToString();
			}

		public:
			std::string m_strCode;
			std::string m_strMessage;
			std::string m_strMsgId;			
			std::string m_strCount;
			std::vector<std::map<std::string, std::string>> m_vecDatas;
		};

		/*****************************************************************
		 * topic_notice_syn  shift_syn
		 ****************************************************************/
		class CShiftSyn : public IRespond
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);

				p_pJson->SetNodeValue("/body/shift_id", m_strShiftId);
				p_pJson->SetNodeValue("/body/receiver_code", m_strReceiverCode);				
				p_pJson->SetNodeValue("/body/dept_code", m_strDeptCode);
				p_pJson->SetNodeValue("/body/dept_name", m_strDeptName);
				p_pJson->SetNodeValue("/body/duty_code", m_strDutyCode);
				p_pJson->SetNodeValue("/body/duty_name", m_strDutyName);

				return p_pJson->ToString();
			}

		public:
			std::string m_strShiftId;
			std::string m_strReceiverCode;	
			std::string m_strDeptCode;      // ��λ����
			std::string m_strDeptName;      // ��λ����
			std::string m_strDutyCode;      // ֵ��Ա
			std::string m_strDutyName;      // ֵ��Ա����
			
			CHeader m_oHeader;
			
		};


		/*****************************************************************
		 * 
		 ****************************************************************/
		class CShiftProtocol : public IRequest, public IRespond
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				CShift shift;
				shift.m_strGuid = p_pJson->GetNodeValue("/body/guid", "");
				shift.m_strStartTime = p_pJson->GetNodeValue("/body/start_time", "");
				shift.m_strEndTime = p_pJson->GetNodeValue("/body/end_time", "");
				shift.m_strDeptCode = p_pJson->GetNodeValue("/body/dept_code", "");
				shift.m_strDutyCode = p_pJson->GetNodeValue("/body/duty_code", "");
				shift.m_strDutyName = p_pJson->GetNodeValue("/body/duty_name", "");
				shift.m_strDutyLeaderCode = p_pJson->GetNodeValue("/body/duty_leader_code", "");
				shift.m_strDutyLeaderName = p_pJson->GetNodeValue("/body/duty_leader_name", "");
				shift.m_strWorkContent = p_pJson->GetNodeValue("/body/work_content", "");
				shift.m_strImportantContent = p_pJson->GetNodeValue("/body/important_content", "");
				shift.m_strShiftContent = p_pJson->GetNodeValue("/body/shift_content", "");
				this->m_strPageSize = p_pJson->GetNodeValue("/body/page_size", "");
				this->m_strPageIndex = p_pJson->GetNodeValue("/body/page_index", "");

				m_oBody.m_isSelectEx = p_pJson->GetNodeValue("/body/enable_alarm", "");

				m_oBody.m_strReceiveState = p_pJson->GetNodeValue("/body/receive_state", "");
				m_oBody.m_strReceiveCode = p_pJson->GetNodeValue("/body/receiver_code", "");
				m_oBody.m_vecData.push_back(shift);

				return true;
			}

			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);
				p_pJson->SetNodeValue("/body/all_count", m_oBody.m_strCount);
				p_pJson->SetNodeValue("/body/count", m_oBody.m_strAllCount);
				p_pJson->SetNodeValue("/body/page_index", m_oBody.m_strPageIndex);

				unsigned int l_uiIndex = 0;
				for (CShift data : m_oBody.m_vecData)
				{
					std::string l_strPrefixPath("/body/data/" + std::to_string(l_uiIndex) + "/");
					p_pJson->SetNodeValue(l_strPrefixPath + "guid", data.m_strGuid);
					p_pJson->SetNodeValue(l_strPrefixPath + "start_time", data.m_strStartTime);
					p_pJson->SetNodeValue(l_strPrefixPath + "end_time", data.m_strEndTime);
					p_pJson->SetNodeValue(l_strPrefixPath + "dept_code", data.m_strDeptCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "dept_name", data.m_strDeptName);
					p_pJson->SetNodeValue(l_strPrefixPath + "duty_code", data.m_strDutyCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "duty_name", data.m_strDutyName);
					p_pJson->SetNodeValue(l_strPrefixPath + "duty_leader_code", data.m_strDutyLeaderCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "duty_leader_name", data.m_strDutyLeaderName);
					p_pJson->SetNodeValue(l_strPrefixPath + "work_content", data.m_strWorkContent);
					p_pJson->SetNodeValue(l_strPrefixPath + "important_content", data.m_strImportantContent);
					p_pJson->SetNodeValue(l_strPrefixPath + "shift_content", data.m_strShiftContent);
					l_uiIndex++;
				}

				return p_pJson->ToString();
			}

			virtual std::string UpToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);
			//	p_pJson->SetNodeValue("/body/result", m_strResult);

				return p_pJson->ToString();
			}

		public:
			class CShift
			{
			public:
				std::string	m_strGuid;						//id
				std::string m_strStartTime;					//��ʼʱ��
				std::string m_strEndTime;					//����ʱ��
				std::string m_strDeptCode;					//��λ����
				std::string m_strDeptName;					//��λ����
				std::string m_strDutyCode;					//ֵ��Ա
				std::string m_strDutyName;					//ֵ��Ա����
				std::string m_strDutyLeaderCode;			//ֵ���쵼
				std::string m_strDutyLeaderName;			//ֵ���쵼����
				std::string m_strWorkContent;				//��������
				std::string m_strImportantContent;			//��Ҫ�¼�
				std::string m_strShiftContent;				//��������
				std::string m_strCreateUser;				//������
				std::string m_strCreateTime;				//����ʱ��
				std::string m_strUpdateUser;				//�޸���,ȡ���һ���޸�ֵ
				std::string m_strUpdateTime;				//�޸�ʱ��,ȡ���һ���޸�ֵ
			};
			class CBody
			{
			public:
				std::string m_strCount;
				std::string m_strAllCount;
				std::string m_strPageIndex;
				//��ȡ���е�λ�µ����пɵ��ɵ�λ��Ϣʱ��Ч
				std::vector<CShift> m_vecData;

				std::string m_strReceiveState;				//����״̬
				std::string m_strReceiveCode;				//������Ա����
				std::string m_isSelectEx;					//�Ƿ񲻷���δ����������־
			};
			CHeaderEx m_oHeader;

			CBody	m_oBody;
		//	std::string	m_strResult;
			std::string m_strPageSize;
			std::string m_strPageIndex;
		};
	}
}
