#pragma once 
#include <Protocol/IRespond.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CGetHistoryCallListRespond :
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
				p_pJson->SetNodeValue("/body/msgid", m_oBody.m_strMsgID);
				p_pJson->SetNodeValue("/body/clientid", m_oBody.m_strClientID);
				p_pJson->SetNodeValue("/body/page_size", m_oBody.m_strPageSize);
				p_pJson->SetNodeValue("/body/page_index", m_oBody.m_strPageIndex);
				p_pJson->SetNodeValue("/body/time", m_oBody.m_strTime);
				return p_pJson->ToString();
			}

			virtual std::string ToExString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);
				p_pJson->SetNodeValue("/body/count", m_oBody.m_strCount);
				p_pJson->SetNodeValue("/body/all_count", m_oBody.m_strAllCount);
				for (size_t i = 0; i < m_oBody.m_vecData.size(); i++)
				{
					std::string l_strValueNum = std::to_string(i);
					std::string l_strPrefixPath("/body/data/" + std::to_string(i) + "/");
					CData& data = m_oBody.m_vecData[i];
					p_pJson->SetNodeValue(l_strPrefixPath + "acd", data.m_strACD);
					p_pJson->SetNodeValue(l_strPrefixPath + "call_direction", data.m_strCallDirection);
					p_pJson->SetNodeValue(l_strPrefixPath + "callref_id", data.m_strCallRefID);
					p_pJson->SetNodeValue(l_strPrefixPath + "caller_id", data.m_strCallerID);
					p_pJson->SetNodeValue(l_strPrefixPath + "called_id", data.m_strCalledID);
					p_pJson->SetNodeValue(l_strPrefixPath + "original_caller", data.m_strOriginalCaller);
					p_pJson->SetNodeValue(l_strPrefixPath + "original_called", data.m_strOriginalCalled);
					p_pJson->SetNodeValue(l_strPrefixPath + "record_files", data.m_strRecrdFiles);
					p_pJson->SetNodeValue(l_strPrefixPath + "incomming_time", data.m_strIncomingTime);
					p_pJson->SetNodeValue(l_strPrefixPath + "waiting_time", data.m_strWaitingTime);
					p_pJson->SetNodeValue(l_strPrefixPath + "assigned_time", data.m_strAssignedTime);
					p_pJson->SetNodeValue(l_strPrefixPath + "ring_time", data.m_strRingTime);
					p_pJson->SetNodeValue(l_strPrefixPath + "talk_time", data.m_strTalkTime);
					p_pJson->SetNodeValue(l_strPrefixPath + "hangup_time", data.m_strHangupTime);
				}
				return p_pJson->ToString();
			}

			virtual bool ParseExString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_oBody.m_strAllCount = p_pJson->GetNodeValue("/body/all_count", "");
				m_oBody.m_strCount = p_pJson->GetNodeValue("/body/count", "");

				int iCount = p_pJson->GetCount("/body/data");
				for (int i = 0; i < iCount; i++)
				{
					std::string l_strPrefixPath("/body/data/" + std::to_string(i) + "/");
					CData l_CData;
					//l_CData.m_strAgent = p_pJson->GetNodeValue(l_strPrefixPath + "agent", "");
					l_CData.m_strCallRefID = p_pJson->GetNodeValue(l_strPrefixPath + "callref_id", "");
					l_CData.m_strCallDirection = p_pJson->GetNodeValue(l_strPrefixPath + "call_direction", "");
					l_CData.m_strACD = p_pJson->GetNodeValue(l_strPrefixPath + "acd", "");
					l_CData.m_strCallerID = p_pJson->GetNodeValue(l_strPrefixPath + "caller_id", "");
					l_CData.m_strCalledID = p_pJson->GetNodeValue(l_strPrefixPath + "called_id", "");
					l_CData.m_strOriginalCaller = p_pJson->GetNodeValue(l_strPrefixPath + "original_caller", "");
					l_CData.m_strOriginalCalled = p_pJson->GetNodeValue(l_strPrefixPath + "original_called", "");
					l_CData.m_strRecrdFiles = p_pJson->GetNodeValue(l_strPrefixPath + "record_files", "");
					l_CData.m_strIncomingTime = p_pJson->GetNodeValue(l_strPrefixPath + "incomming_time", "");
					l_CData.m_strWaitingTime = p_pJson->GetNodeValue(l_strPrefixPath + "waiting_time", "");
					l_CData.m_strAssignedTime = p_pJson->GetNodeValue(l_strPrefixPath + "assigned_time", "");
					l_CData.m_strRingTime = p_pJson->GetNodeValue(l_strPrefixPath + "ring_time", "");
					l_CData.m_strTalkTime = p_pJson->GetNodeValue(l_strPrefixPath + "talk_time", "");
					l_CData.m_strHangupTime = p_pJson->GetNodeValue(l_strPrefixPath + "hangup_time", "");
					m_oBody.m_vecData.push_back(l_CData);
				}

				return true;
			}

		public:
			CHeaderEx m_oHeader;
			class CData
			{
			public:
				//std::string m_strAgent;				//��ϯ�ֻ���
				std::string m_strACD;				//ACD �����ƣ�110��119��120��122��
				std::string m_strCallDirection;		//���з���
				std::string m_strCallRefID;			//����id
				std::string m_strCallerID;			//���к�
				std::string m_strCalledID;			//���к�
				std::string m_strOriginalCaller;	//ԭ����
				std::string m_strOriginalCalled;	//ԭ����
				std::string m_strRecrdFiles;		//¼���ļ�·�������һ�������ж��¼�����÷ֺš���������
				std::string m_strIncomingTime;		//����ʱ��
				std::string m_strWaitingTime;		//�Ŷӿ�ʼʱ��
				std::string m_strAssignedTime;		//�������ʱ��
				std::string m_strRingTime;			//��ʼ����ʱ��
				std::string m_strTalkTime;			//��ʼͨ��ʱ��
				std::string m_strHangupTime;		//�һ�ʱ��
			};
			class CBody
			{
			public:
				std::string m_strPageIndex;
				std::string m_strPageSize;
				std::string m_strClientID;
				std::string m_strTime;
				std::string m_strCode;				//200���ɹ�����0��ʧ�ܣ�����ʧ��ԭ��� message�ֶζ�ȡ
				std::string m_strMessage;			//code����
				std::string m_strMsgID;				//��ϢΨһID
				std::string m_strCount;				//��ǰҳ����
				std::string m_strAllCount;			//��ʷ��������	
				std::vector<CData> m_vecData;
			};
			CBody m_oBody;
		};
	}
}
