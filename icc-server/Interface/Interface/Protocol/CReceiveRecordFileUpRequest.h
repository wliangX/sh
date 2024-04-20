#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CRecordFileUpSyncRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_oBody.m_strFileID = p_pJson->GetNodeValue("/body/file_id","");
				m_oBody.m_strCallrefID = p_pJson->GetNodeValue("/body/callref_id", "");
				m_oBody.m_strFileName = p_pJson->GetNodeValue("/body/file_name", "");
				m_oBody.m_strResult = p_pJson->GetNodeValue("/body/result", "");
				m_oBody.m_strStartTime = p_pJson->GetNodeValue("/body/start_time","");
				m_oBody.m_strAgent = p_pJson->GetNodeValue("/body/agent", "");

				return true;
			}

		public:
			CHeader m_oHeader;
			class CBody
			{
			public:
				std::string m_strFileID;		//�ļ�ID
				std::string m_strCallrefID;		//����ID
				std::string m_strFileName;		//�ļ���
				std::string m_strResult;		//�ϴ���� 0���ɹ��� ������ʧ
				std::string m_strStartTime;		//¼����ʼʱ��
				std::string m_strAgent;			//�豸��
			};
			CBody m_oBody;
		};
	}
}
