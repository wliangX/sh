#pragma once
#include <map>
#include <string>
#include <Json/IJsonFactory.h>

namespace ICC
{
    class CAgentPhoneState
	{	
	public:
        virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
        {
            if (nullptr == p_pJson)
            {
                return "";
            }
            p_pJson->SetNodeValue("/SID", m_strSID);
            p_pJson->SetNodeValue("/XZQHBH", m_strDistrictCode);
            p_pJson->SetNodeValue("/FJH", m_strPhoneNum);
            p_pJson->SetNodeValue("/XWZT", m_strState);
            p_pJson->SetNodeValue("/ZXID", m_strAgentID);
            p_pJson->SetNodeValue("/JJYBH", m_strReceiptCode);
            p_pJson->SetNodeValue("/JJYXM", m_strReceiptName);

            p_pJson->SetNodeValue("/ZZJGDM", m_strZZJGDM);

            p_pJson->SetNodeValue("/BMDM", m_strBMDM);
            p_pJson->SetNodeValue("/BMMC", m_strBMMC);

            p_pJson->SetNodeValue("/GXSJ", m_strUpdateTime);

            return p_pJson->ToString();
        }

        virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
        {
            if (!p_pJson || !p_pJson->LoadJson(p_strReq))
            {
                return false;
            }

            m_strSID = p_pJson->GetNodeValue("/SID", "");
            m_strDistrictCode = p_pJson->GetNodeValue("/XZQHBH", "");
            m_strPhoneNum = p_pJson->GetNodeValue("/FJH", "");
            m_strState = p_pJson->GetNodeValue("/XWZT", "");
            m_strAgentID = p_pJson->GetNodeValue("/ZXID", "");
            m_strReceiptCode = p_pJson->GetNodeValue("/JJYBH", "");
            m_strReceiptName = p_pJson->GetNodeValue("/JJYXM", "");
            m_strUpdateTime = p_pJson->GetNodeValue("/GXSJ", "");

            m_strZZJGDM = p_pJson->GetNodeValue("/ZZJGDM", "");

            m_strBMDM = p_pJson->GetNodeValue("/BMDM", "");
            m_strBMMC = p_pJson->GetNodeValue("/BMMC", "");

            return true;
        }


	public:
        std::string m_strSID;           //����]��������������ǰ��λ+����Ψһ��ʶ
        std::string m_strDistrictCode;  //���������������ǰ��λ

        std::string m_strZZJGDM;        //��֯��������
        std::string m_strPhoneNum;      //�ֻ���
        std::string m_strState;         //����״̬ 0���ǳ� 1������ 2��ժ�� 3���һ� 4:��æ 5:ȡ����æ 
        std::string m_strAgentID;       //��ϯID
        std::string m_strReceiptCode;   //�Ӿ�Ա���
        std::string m_strReceiptName;   //�Ӿ�Ա����
        std::string m_strUpdateTime;    //����ʱ��

        std::string m_strBMDM;//���Ŵ���
        std::string m_strBMMC; //��������

	};
}