#pragma once
#include <Protocol/IRespond.h>
#include <Protocol/CHeader.h>

namespace ICC
{
#define TYPE_TIME          "type_time"
#define TYPE_SYSTEM        "type_system"
#define TYPE_SYSTEM_TIME   "type_system_time"
#define TYPE_ALL           "type_all"
	namespace PROTOCOL
	{
        class CSmpSyncRequest :
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
                if (m_strType == TYPE_TIME)
                {
                    //p_pJson->SetNodeValue("/body/begin_time", m_oBody.m_strBeginTime);
                    //p_pJson->SetNodeValue("/body/end_time", m_oBody.m_strEndTime);
					p_pJson->SetNodeValue("/body/puc_id", m_oBody.m_strPucID);
					p_pJson->SetNodeValue("/body/puc_sys_type", m_oBody.m_strPucSysType);
                    p_pJson->SetNodeValue("/body/version", m_oBody.m_strVersion);
                }
                else if (m_strType == TYPE_SYSTEM)
                {
                    p_pJson->SetNodeValue("/body/systemNo", m_oBody.m_strSystemNo);
                    p_pJson->SetNodeValue("/body/version", m_oBody.m_strVersion);
                }
                else if (m_strType == TYPE_ALL)
                {                    
                    p_pJson->SetNodeValue("/body/version", m_oBody.m_strVersion);
                }    
                else if (m_strType == TYPE_SYSTEM_TIME)
                {
                   // p_pJson->SetNodeValue("/body/begin_time", m_oBody.m_strBeginTime);
                    p_pJson->SetNodeValue("/body/systemNo", m_oBody.m_strSystemNo);
                   // p_pJson->SetNodeValue("/body/end_time", m_oBody.m_strEndTime);
                    p_pJson->SetNodeValue("/body/version", m_oBody.m_strVersion);
                }
				return p_pJson->ToString();
			}
		public:
			class CBody
			{
			public:
				std::string m_strBeginTime;			//��ʼʱ��
				std::string m_strEndTime;		    //����ʱ��
                std::string m_strVersion;			//�汾��
                std::string m_strSystemNo;			//ϵͳ���
				std::string m_strPucID;
				std::string m_strPucSysType;		//ϵͳ���ͣ���λͬ����������ָ����ͬ�� ICC ϵͳ�ĵ�λ
			};
			CHeader m_oHeader;			
			CBody	m_oBody;
            std::string m_strType;
		};				
	}
}
