#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>
#include <Protocol/CSetFeedbackRequest.h>
namespace ICC
{
    namespace PROTOCOL
    {
        class CUpdateFeedbackRequest :
			public IRequest, public IRespond
        {
        public:
            virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
            {
                if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

                CSetFeedBackRequest::CBody l_pData;
                int l_iCount = p_pJson->GetCount("/body/data");
                for (int i = 0; i < l_iCount; i++)
                {
                    std::string l_strPrefixPath("/body/data/" + std::to_string(i) + "/");
                    
                    l_pData.m_strID = p_pJson->GetNodeValue(l_strPrefixPath + "FKDBH", ""); //���������
                    l_pData.m_strFeedbackDeptDistrictCode = p_pJson->GetNodeValue(l_strPrefixPath + "XZQHDM", "");//�������ڵ�������������
                    l_pData.m_strAlarmID= p_pJson->GetNodeValue(l_strPrefixPath + "JJDBH", "");//�Ӿ������(���)
                    l_pData.m_strProcessID = p_pJson->GetNodeValue(l_strPrefixPath + "PJDBH", "");//�ɾ������(���)
                    p_pJson->GetNodeValue(l_strPrefixPath + "FKLYH", "");//����¼���� (���)
                    l_pData.m_strFeedbackDeptCode = p_pJson->GetNodeValue(l_strPrefixPath + "FKDWDM", "");//������λ����
                    l_pData.m_strFeedbackCode = p_pJson->GetNodeValue(l_strPrefixPath + "FKYBH", "");//����Ա���
                    l_pData.m_strFeedbackName = p_pJson->GetNodeValue(l_strPrefixPath + "FKYXM", "");//����Ա����
                    l_pData.m_strTimeSubmit = p_pJson->GetNodeValue(l_strPrefixPath + "FKSJ", "");//����ʱ��
                    l_pData.m_strTimePoliceDispatch = p_pJson->GetNodeValue(l_strPrefixPath + "SJCJSJ", "");//����ʱ��
                    l_pData.m_strTimePoliceArrived = p_pJson->GetNodeValue(l_strPrefixPath + "DDXCSJ", "");//�����ֳ�ʱ��
                    p_pJson->GetNodeValue(l_strPrefixPath + "CLWBSJ", "");//�ֳ��������ʱ��
                    p_pJson->GetNodeValue(l_strPrefixPath + "JQLBDM", "");//����������
                    p_pJson->GetNodeValue(l_strPrefixPath + "JQLXDM", "");//�������ʹ���
                    p_pJson->GetNodeValue(l_strPrefixPath + "JQXLDM", "");//����ϸ�����
                    l_pData.m_strActualOccurTime = p_pJson->GetNodeValue(l_strPrefixPath + "JQFSSJ", "");//���鷢��ʱ��
                    l_pData.m_strAlarmLongitude = p_pJson->GetNodeValue(l_strPrefixPath + "FKDWXZB", "");//������λX���꣨�����ֳ�λ�ã�
                    l_pData.m_strAlarmLatitude = p_pJson->GetNodeValue(l_strPrefixPath + "FKDWYZB", "");//������λY���꣨�����ֳ�λ�ã�
                    p_pJson->GetNodeValue(l_strPrefixPath + "CJQK", "");//��������״��
                    l_pData.m_strPoliceCarNumDispatch = p_pJson->GetNodeValue(l_strPrefixPath + "CDCLQK", "");//��������
                    l_pData.m_strPoliceNumDispatch = p_pJson->GetNodeValue(l_strPrefixPath + "CDRYQK", "");//�����˴�
                    l_pData.m_strPeopleNumRescue = p_pJson->GetNodeValue(l_strPrefixPath + "JZRS", "");//��������
                    p_pJson->GetNodeValue(l_strPrefixPath + "JZRSSM", "");//��������˵��
                    p_pJson->GetNodeValue(l_strPrefixPath + "SSRS", "");//��������
                    p_pJson->GetNodeValue(l_strPrefixPath + "SSRSSM", "");//��������˵��
                    l_pData.m_strPeopleNumDeath = p_pJson->GetNodeValue(l_strPrefixPath + "SWRS", "");//��������
                    p_pJson->GetNodeValue(l_strPrefixPath + "SWRSSM", "");//��������˵��
                    p_pJson->GetNodeValue(l_strPrefixPath + "CLJGDM", "");//���鴦��������
                    p_pJson->GetNodeValue(l_strPrefixPath + "JQCLJG", "");//���鴦����˵�����Ծ���Ҫ�ؼ��ֳ������������������
                    p_pJson->GetNodeValue(l_strPrefixPath + "TQQKDM", "");//�����������
                    p_pJson->GetNodeValue(l_strPrefixPath + "SSQKMS", "");//��ʧ�������
                    l_pData.m_strPeopleNumCapture = p_pJson->GetNodeValue(l_strPrefixPath + "ZHRS", "");//ץ������
                    p_pJson->GetNodeValue(l_strPrefixPath + "SARS", "");//�永����
                    p_pJson->GetNodeValue(l_strPrefixPath + "TPRS", "");//��������
                    p_pJson->GetNodeValue(l_strPrefixPath + "JTSGXTDM", "");//��ͨ�¹���̬����
                    p_pJson->GetNodeValue(l_strPrefixPath + "SFWHP", "");//�Ƿ�װ��Σ��Ʒ(0����1���ǣ�Ĭ��Ϊ0��)
                    p_pJson->GetNodeValue(l_strPrefixPath + "SGCCYYDM", "");///��ͨ�¹ʳ���ԭ�����
                    p_pJson->GetNodeValue(l_strPrefixPath + "NJDDM", "");//�ܼ��ȴ���
                    p_pJson->GetNodeValue(l_strPrefixPath + "LMZKDM", "");//·��״������
                    p_pJson->GetNodeValue(l_strPrefixPath + "SHJDCS", "");//�𻵻�������
                    p_pJson->GetNodeValue(l_strPrefixPath + "SHFJDCS", "");//�𻵷ǻ�������
                    p_pJson->GetNodeValue(l_strPrefixPath + "DLLXDM", "");//��·���ʹ���
                    p_pJson->GetNodeValue(l_strPrefixPath + "JQZTDM", "");//���鴦��״̬����
                    p_pJson->GetNodeValue(l_strPrefixPath + "RKSJ", "");//���ʱ��
                    p_pJson->GetNodeValue(l_strPrefixPath + "GXSJ", "");//����ʱ��
                    p_pJson->GetNodeValue(l_strPrefixPath + "JDXZ", "");//�ֵ�������
                    p_pJson->GetNodeValue(l_strPrefixPath + "SDCS", "");//���ش���
                    p_pJson->GetNodeValue(l_strPrefixPath + "SFQZ", "");//�Ƿ�ȡ֤(0����1���ǣ�Ĭ��Ϊ0��)
                    p_pJson->GetNodeValue(l_strPrefixPath + "YJDW", "");//�ƽ���λ
                    p_pJson->GetNodeValue(l_strPrefixPath + "YJDWLXR", "");//�ƽ���λ��ϵ��
                    p_pJson->GetNodeValue(l_strPrefixPath + "YJDWFKQK", "");//�ƽ���λ�������(���ʹ���)
                    p_pJson->GetNodeValue(l_strPrefixPath + "YJDWFKQKSM", "");//�ƽ���λ�������˵��
                    p_pJson->GetNodeValue(l_strPrefixPath + "AJBH", "");//�������
                    p_pJson->GetNodeValue(l_strPrefixPath + "NRBQ", "");//���ݱ�ǩ
                    p_pJson->GetNodeValue(l_strPrefixPath + "JQFSDY", "");//���鷢������
                    p_pJson->GetNodeValue(l_strPrefixPath + "JQFSBW", "");//���鷢����λ(����)
                    p_pJson->GetNodeValue(l_strPrefixPath + "FORCEID", "");//����id
                    p_pJson->GetNodeValue(l_strPrefixPath + "FORCETYPE", "");//�������ͣ��ˡ������豸���ͣ�
                    p_pJson->GetNodeValue(l_strPrefixPath + "ZHTBBTH", "");//���ͬ���汾��
                    p_pJson->GetNodeValue(l_strPrefixPath + "DQYWZT", "");//��ǰҵ��״̬
                    p_pJson->GetNodeValue(l_strPrefixPath + "FKDWIDENTIFIER", "");//������λ:��֯�ڲ����


                    m_oBody.m_vecProcessData.push_back(l_pData);
                }

                return true;
            }

			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}
				return p_pJson->ToString();
			}

        public:
            class CBody
            {
            public:
                std::vector<CSetFeedBackRequest::CBody> m_vecProcessData;
            };
            CHeaderEx m_oHeader;
            CBody	m_oBody;
        };

    }
}

