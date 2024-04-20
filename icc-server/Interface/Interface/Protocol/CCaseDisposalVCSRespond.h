#pragma once
#include <Protocol/IRespond.h>

namespace ICC
{
    namespace PROTOCOL
    {
        class CCaseDisposalVCSRespond :
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

                p_pJson->SetNodeValue("/body/count", m_oBody.m_strCount);

                unsigned int l_uiIndex = 0;
                for (CData data : m_oBody.m_vecData)
                {
                    std::string l_strPrefixPath("/body/data/" + std::to_string(l_uiIndex) + "/");
                    p_pJson->SetNodeValue(l_strPrefixPath + "id", data.m_strID);
                    p_pJson->SetNodeValue(l_strPrefixPath + "caseid", data.m_strCaseId);
                    p_pJson->SetNodeValue(l_strPrefixPath + "seatno", data.m_strSeatNo);
                    p_pJson->SetNodeValue(l_strPrefixPath + "dispatchorgid", data.m_strDispatchOrgId);
                    p_pJson->SetNodeValue(l_strPrefixPath + "dispatchtime", data.m_strDispatchTime);
                    p_pJson->SetNodeValue(l_strPrefixPath + "status", data.m_strCreateUser);
                    p_pJson->SetNodeValue(l_strPrefixPath + "createuser", data.m_strCreateTime);
                    p_pJson->SetNodeValue(l_strPrefixPath + "createtime", data.m_strUpdateUser);
                    p_pJson->SetNodeValue(l_strPrefixPath + "updateuser", data.m_strUpdateTime);
                    p_pJson->SetNodeValue(l_strPrefixPath + "updatetime", data.m_strDelFlag);
                    p_pJson->SetNodeValue(l_strPrefixPath + "delflag", data.m_strStatus);
                    
                    ++l_uiIndex;
                }

                return p_pJson->ToString();
            }

        public:
            class CData
            {
            public:
                std::string m_strID;
                std::string m_strCaseId;					//����ID
                std::string m_strSeatNo;		    		//������ϯ
                std::string m_strDispatchOrgId;				//���ɵ�λ                
                std::string m_strDispatchTime;				//����ʱ��           
                std::string m_strCreateUser;				//������
                std::string m_strCreateTime;				//����ʱ��
                std::string m_strUpdateUser;				//�޸���,ȡ���һ���޸�ֵ
                std::string m_strUpdateTime;				//�޸�ʱ��,ȡ���һ���޸�ֵ
                std::string m_strDelFlag;				    //0��Ч��1ɾ��
                std::string m_strStatus;				    //״̬
            };
            class CBody
            {
            public:
                std::string m_strCount;
                std::vector<CData> m_vecData;
            };
            CHeader m_oHeader;
            CBody	m_oBody;
        };
    }
}
