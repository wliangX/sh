#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>
#include <Protocol/CAddOrUpdateProcessRequest.h>
namespace ICC
{
    namespace PROTOCOL
    {
        class CUpdateDisposalRequest :
			public IRequest, public IRespond
        {
        public:
            virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
            {
                if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

                CAddOrUpdateProcessRequest::CProcessData l_pData;
                l_pData.m_bIsNewProcess = false;
                int l_iCount = p_pJson->GetCount("/body/data");
                for (int i = 0; i < l_iCount; i++)
                {
                    std::string l_strPrefixPath("/body/data/" + std::to_string(i) + "/");

                    l_pData.m_strID = p_pJson->GetNodeValue(l_strPrefixPath + "PJDBH", "");  //�ɾ������
                    //l_pData.m_strProcessDeptDistrictCode = p_pJson->GetNodeValue(l_strPrefixPath + "XZQHDM", "");  //�������ڵ�������������
                    l_pData.m_strAlarmID = p_pJson->GetNodeValue(l_strPrefixPath + "JJDBH", "");   //�Ӿ������

                    l_pData.m_strDispatchDeptCode = p_pJson->GetNodeValue(l_strPrefixPath + "PJDWDM", "");  //�ɾ���λ��������
                    l_pData.m_strDispatchCode = p_pJson->GetNodeValue(l_strPrefixPath + "PJYBH", "");   //�ɾ�Ա���
                    l_pData.m_strDispatchName = p_pJson->GetNodeValue(l_strPrefixPath + "PJYXM", "");  //�ɾ�Ա����
                    p_pJson->GetNodeValue(l_strPrefixPath + "PJLYH", "");   //�ɾ�¼����
                    l_pData.m_strDispatchSuggestion = p_pJson->GetNodeValue(l_strPrefixPath + "PJYJ", "");   //�ɾ����
                    l_pData.m_strProcessDeptCode = p_pJson->GetNodeValue(l_strPrefixPath + "CJDWDM", "");  //������λ����
                    l_pData.m_strCreateTime = p_pJson->GetNodeValue(l_strPrefixPath + "CJSJ", "");    //�ɾ�ʱ��
                    //l_pData.m_strTimeEdit = p_pJson->GetNodeValue(l_strPrefixPath + "PDJSSJ", "");  //�ɵ�����ʱ�䣨�����´��ɾ����󣬳�����λ�ֹ�ȷ�Ͻӵ����ɾ�����ʱ�䣩
                    p_pJson->GetNodeValue(l_strPrefixPath + "CDRY", "");   //������Ա˵��
                    p_pJson->GetNodeValue(l_strPrefixPath + "CDCL", "");   //��������˵��
                    l_pData.m_strState = p_pJson->GetNodeValue(l_strPrefixPath + "JQCLZTDM", ""); //���鴦��״̬����
                    p_pJson->GetNodeValue(l_strPrefixPath + "RKSJ", "");    //���ʱ��
                    l_pData.m_strUpdateTime = p_pJson->GetNodeValue(l_strPrefixPath + "GXSJ", "");    //����ʱ��
                    l_pData.m_strDispatchCode = p_pJson->GetNodeValue(l_strPrefixPath + "PJYID", "");   //�ɾ�Ա�û�id
                    p_pJson->GetNodeValue(l_strPrefixPath + "ZHTBBTH", "");  //���ͬ���汾��
                    l_pData.m_strDispatchDeptCode = p_pJson->GetNodeValue(l_strPrefixPath + "PJDWID", "");   //�ɾ���λid
                    l_pData.m_strDispatchDeptName = p_pJson->GetNodeValue(l_strPrefixPath + "PJDWMC", "");   //�ɾ���λ����
                    l_pData.m_strProcessCode = p_pJson->GetNodeValue(l_strPrefixPath + "CJDXID", "");   //��������id

                    l_pData.m_strProcessDeptName = p_pJson->GetNodeValue(l_strPrefixPath + "CJDXMC", "");   //������������  -->//����������
                    //l_pData.m_strProcessDeptName;

                    p_pJson->GetNodeValue(l_strPrefixPath + "CJDXLX", "");   //������������

                    p_pJson->GetNodeValue(l_strPrefixPath + "DQYWZT", "");   //��ǰҵ��״̬

                    p_pJson->GetNodeValue(l_strPrefixPath + "KJDPBH", "");  //�缶����ID
                    p_pJson->GetNodeValue(l_strPrefixPath + "TDSJ", "");    //�˵�����ʱ��
                    //l_pData.m_strSeatNo = p_pJson->GetNodeValue(l_strPrefixPath + "ZXBH", "");    //��ϯ���
                    l_pData.m_strProcessDeptCode = p_pJson->GetNodeValue(l_strPrefixPath + "CJDWID", "");   //������λID

                    p_pJson->GetNodeValue(l_strPrefixPath + "PJDWIDENTIFIER", ""); //�ɾ���λ:��֯�ڲ����
                    p_pJson->GetNodeValue(l_strPrefixPath + "CJDWIDENTIFIER", ""); //������λ:��֯�ڲ����
                    p_pJson->GetNodeValue(l_strPrefixPath + "IsDispathNotify", "");
                    p_pJson->GetNodeValue(l_strPrefixPath + "BYAUTOASSIGNJOB", "");

                    l_pData.m_strUpdateType = p_pJson->GetNodeValue(l_strPrefixPath + "update_type", "");
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
                std::vector<CAddOrUpdateProcessRequest::CProcessData> m_vecProcessData;
            };
            CHeaderEx m_oHeader;
            CBody	m_oBody;
        };

    }
}

