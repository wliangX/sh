#pragma once 
#include <Protocol/IRespond.h>
#include "Protocol/CHeader.h"
namespace ICC
{
	namespace PROTOCOL
	{
        class CFeedBackInfoChange :
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

                for (size_t i = 0; i < m_oBody.m_vecData.size(); i++)
                {
                    std::string l_strIndex = std::to_string(i);
                    p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/FKDBH", m_oBody.m_vecData[i].m_strID);
                    p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/JJDBH", m_oBody.m_vecData[i].m_strCaseId);
                    p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/PJDBH", m_oBody.m_vecData[i].m_strDisPosalId);
                    p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/casedispatchstatus", m_oBody.m_vecData[i].m_strCaseDispatchStatus);
                    p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/FORCEID", m_oBody.m_vecData[i].m_strInterphoneId);
                    p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/fbway", m_oBody.m_vecData[i].m_strFBWay);
                    p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/fbcontent", m_oBody.m_vecData[i].m_strFBContent);
                    p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/FKYBH", m_oBody.m_vecData[i].m_strFeedBacker);
                    p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/FKDWDM", m_oBody.m_vecData[i].m_strFeedBackerOrg);
                    p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/FKSJ", m_oBody.m_vecData[i].m_strFBTime);
                    p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/JQLBDM", m_oBody.m_vecData[i].m_strCaseType);
                    p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/JQLXDM", m_oBody.m_vecData[i].m_strCaseSubType);
                    p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/JQXLDM", m_oBody.m_vecData[i].m_strCaseThreeType);
                    p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/case_processor", m_oBody.m_vecData[i].m_strCaseProcessor);
                    p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/createuser", m_oBody.m_vecData[i].m_strCreateUser);
                    p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/RKSJ", m_oBody.m_vecData[i].m_strCreateTime);
                    p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/updateuser", m_oBody.m_vecData[i].m_strUpdateUser);
                    p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/GXSJ", m_oBody.m_vecData[i].m_strUpdateTime);
                    p_pJson->SetNodeValue("/body/data/" + l_strIndex + "/delflag", m_oBody.m_vecData[i].m_strDelFlag);
                }

                return p_pJson->ToString();
            }


		public:
			CHeader m_oHeader;

            class CBody
            {
			public:
                class CData
                {
                public:
                public:
                    std::string m_strID;
                    std::string m_strCaseId;					//����ID
                    std::string m_strDisPosalId;				//������ID
                    std::string m_strCaseDispatchStatus;		//��������
                    std::string m_strInterphoneId;              //�豸ID�б�
                    std::string m_strFBWay;                     //������ʽ:1�ն˿�ݼ���2���ţ�3�绰
                    std::string m_strFBContent;                 //��������
                    std::string m_strFeedBacker;                //������
                    std::string m_strFeedBackerOrg;             //������λ����
                    std::string m_strFBTime;                    //����ʱ��
                    std::string m_strCaseType;					//��������
                    std::string m_strCaseSubType;				//��������
                    std::string m_strCaseThreeType;				//����С��
                    std::string m_strCaseProcessor;				//�������б����ŷָ
                    std::string m_strCreateUser;				//������
                    std::string m_strCreateTime;				//����ʱ��
                    std::string m_strUpdateUser;				//�޸���,ȡ���һ���޸�ֵ
                    std::string m_strUpdateTime;				//�޸�ʱ��,ȡ���һ���޸�ֵ
                    std::string m_strDelFlag;				    //0��Ч��1ɾ��
                };

                std::vector<CData> m_vecData;
            };
			CBody m_oBody;
		};
	}
}
