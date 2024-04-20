#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CGetHistoryCallListRequest :
			public IRequest
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

				return true;
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
					std::string l_strDataNum = std::to_string(i);
					CData l_CData;
					l_CData.m_strAgent = p_pJson->GetNodeValue(l_strPrefixPath + "agent", "");
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
				std::string m_strAgent;				//��ϯ�ֻ���
				std::string m_strACD;				//ACD �����ƣ�110��119��120��122��
				std::string m_strCallDirection;		//���з���
				std::string m_strCallRefID;			//����ID			
				std::string m_strCallerID;			//����
				std::string m_strCalledID;			//����
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
