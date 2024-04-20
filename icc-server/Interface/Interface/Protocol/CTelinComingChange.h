#pragma once 
#include <Protocol/IRespond.h>
#include "Protocol/CHeader.h"
namespace ICC
{
	namespace PROTOCOL
	{
        class CTelinComingChange :
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
                p_pJson->SetNodeValue("/body/call_id", m_oBody.m_strCallID);
                p_pJson->SetNodeValue("/body/caseid", m_oBody.m_strCaseID);
                p_pJson->SetNodeValue("/body/phone", m_oBody.m_strPhone);
                p_pJson->SetNodeValue("/body/callingtime", m_oBody.m_strCallingTime);
                p_pJson->SetNodeValue("/body/answeringtime", m_oBody.m_strAnsweringTime);
                p_pJson->SetNodeValue("/body/userid", m_oBody.m_strSeatNo);

                p_pJson->SetNodeValue("/body/callerno", m_oBody.m_strCallerNum);
                p_pJson->SetNodeValue("/body/calledno", m_oBody.m_strCalledNum);
                
                p_pJson->SetNodeValue("/body/updatetime", m_oBody.m_strUpdateTime);

                p_pJson->SetNodeValue("/body/lon", m_oBody.m_strLongitude);
                p_pJson->SetNodeValue("/body/lat", m_oBody.m_strLatitude);
                p_pJson->SetNodeValue("/body/address", m_oBody.m_strAddress);
                p_pJson->SetNodeValue("/body/alarm_person_name", m_oBody.m_strPersonName);
                p_pJson->SetNodeValue("/body/alarm_person_sex", m_oBody.m_strPersonSex);

                p_pJson->SetNodeValue("/body/alarm_person_id", m_oBody.m_strPersonID);
                p_pJson->SetNodeValue("/body/alarm_person_tag", m_oBody.m_strPersonTag);
                return p_pJson->ToString();
            }

            virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
            {
                if (!p_pJson || !p_pJson->LoadJson(p_strReq))
                {
                    return false;
                }

                m_oBody.m_strCallID = p_pJson->GetNodeValue("/body/call_id", "");
                m_oBody.m_strCaseID = p_pJson->GetNodeValue("/body/caseid", "");
                m_oBody.m_strSeatNo = p_pJson->GetNodeValue("/body/userid", "");

                m_oBody.m_strLongitude = p_pJson->GetNodeValue("/body/lon", "");
                m_oBody.m_strLatitude = p_pJson->GetNodeValue("/body/lat", "");
                m_oBody.m_strAddress = p_pJson->GetNodeValue("/body/address", "");
                m_oBody.m_strUpdateTime = p_pJson->GetNodeValue("/body/updatetime", "");

                m_oBody.m_strRegionCode = p_pJson->GetNodeValue("/body/regionCode", "");
                return true;
            }


		public:
			CHeader m_oHeader;
            class CBody
            {
            public:
                std::string m_strCallID;                    //����ID
                std::string m_strCaseID;					//����id
                std::string m_strPhone;		    		    //�����˺���
                std::string m_strCallingTime;		    	//����ʱ��
                std::string m_strAnsweringTime;		    	//Ӧ��ʱ��
                std::string m_strSeatNo;		    		//�Ӿ���ϯ��
                std::string m_strLongitude;				    //����
                std::string m_strLatitude;					//γ��
                std::string m_strAddress;					//
                std::string m_strUpdateTime;				//����ʱ��

                std::string m_strPersonName;
                std::string m_strPersonSex;
                std::string m_strPersonTag;
                std::string m_strPersonID;              //���֤��

                std::string m_strCallerNum;
                std::string m_strCalledNum;

                std::string m_strRegionCode;
            };
			CBody m_oBody;
		};
	}
}
