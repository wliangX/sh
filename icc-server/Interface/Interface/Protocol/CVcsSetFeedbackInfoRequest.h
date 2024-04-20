#pragma once 
#include <Protocol/IRequest.h>
#include "Protocol/CHeader.h"
namespace ICC
{
	namespace PROTOCOL
	{
        class CVcsSetFeedBackInfoRequest :
            public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
            {
                if (!m_oHeader.ParseString(p_strReq, p_pJson))
                {
                    return false;
                }
                m_oBody.m_strID = p_pJson->GetNodeValue("/body/id", "");
                m_oBody.m_strCaseId = p_pJson->GetNodeValue("/body/caseid", "");
                m_oBody.m_strDisPosalId = p_pJson->GetNodeValue("/body/disposalid", "");
                m_oBody.m_strCaseDispatchStatus = p_pJson->GetNodeValue("/body/casedispatchstatus", "");
                m_oBody.m_strFBWay = p_pJson->GetNodeValue("/body/fbway", "");
                m_oBody.m_strFBContent = p_pJson->GetNodeValue("/body/fbcontent", "");
				m_oBody.m_strFeedBacker = p_pJson->GetNodeValue("/body/feedbacker", "");
				m_oBody.m_strFeedBackerCode = p_pJson->GetNodeValue("/body/feedbacker_code", "");
				m_oBody.m_strFeedBackDept = p_pJson->GetNodeValue("/body/feedback_dept", "");
				m_oBody.m_strFeedBackDeptCode = p_pJson->GetNodeValue("/body/feedback_dept_code", "");
                m_oBody.m_strFBTime = p_pJson->GetNodeValue("/body/fbtime", "");
				m_oBody.m_strCaseProcessor = p_pJson->GetNodeValue("/body/case_processor", "");
				m_oBody.m_strFileType = p_pJson->GetNodeValue("/body/file_type", "");
				m_oBody.m_strFileGuid = p_pJson->GetNodeValue("/body/file_guid", "");
				m_oBody.m_strLongitude = p_pJson->GetNodeValue("/body/longitude", "");
				m_oBody.m_strLatitude = p_pJson->GetNodeValue("/body/latitude", "");
                
                return true;
            }
			
		public:
			CHeader m_oHeader;
			class CBody
			{
			public:
                std::string m_strID;
                std::string m_strCaseId;					//����ID
                std::string m_strDisPosalId;				//������ID
                std::string m_strCaseDispatchStatus;		//��������                
                std::string m_strFBWay;                     //������ʽ:1�ն˿�ݼ���2���ţ�3�绰
                std::string m_strFBContent;                 //��������
				std::string m_strFeedBacker;                //������
				std::string m_strFeedBackerCode;            //�����˾���
				std::string m_strFeedBackDept;              //�����˵�λ
				std::string m_strFeedBackDeptCode;
                std::string m_strFBTime;                    //����ʱ��     
				std::string m_strCaseProcessor;				//0��Ч��1ɾ��
				std::string m_strFileType;					//�ļ����� ͼƬ0����Ƶ1������2����Ƶ3 ��λ��4������5���Զ���6��Ԥ����
				std::string m_strFileGuid;				    //�ļ���ʶ
				std::string m_strLongitude;					//APP �����ľ���
				std::string m_strLatitude;					//APP ������γ��
			};
			CBody m_oBody;
		};
	}
}
