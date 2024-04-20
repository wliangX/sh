#pragma once
#include <Protocol/IRespond.h>

namespace ICC
{
    namespace PROTOCOL
    {
        class CCaseFeedBackVCSRespond :
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
                    p_pJson->SetNodeValue(l_strPrefixPath + "disposalid", data.m_strDisPosalId);
                    p_pJson->SetNodeValue(l_strPrefixPath + "casedispatchstatus", data.m_strCaseDispatchStatus);
                    p_pJson->SetNodeValue(l_strPrefixPath + "interphone_id", data.m_strInterphoneId);
                    p_pJson->SetNodeValue(l_strPrefixPath + "fbway", data.m_strFBWay);
                    p_pJson->SetNodeValue(l_strPrefixPath + "fbcontent", data.m_strFBContent);
                    p_pJson->SetNodeValue(l_strPrefixPath + "feedbacker", data.m_strFeedBacker);
                    p_pJson->SetNodeValue(l_strPrefixPath + "fbtime", data.m_strFBTime);
                    p_pJson->SetNodeValue(l_strPrefixPath + "casetype", data.m_strCaseType);
                    p_pJson->SetNodeValue(l_strPrefixPath + "casesubtype", data.m_strCaseSubType);
                    p_pJson->SetNodeValue(l_strPrefixPath + "casethreetype", data.m_strCaseThreeType);
                    p_pJson->SetNodeValue(l_strPrefixPath + "case_processor", data.m_strCaseProcessor);
                    p_pJson->SetNodeValue(l_strPrefixPath + "createuser", data.m_strCreateUser);
                    p_pJson->SetNodeValue(l_strPrefixPath + "createtime", data.m_strCreateTime);
                    p_pJson->SetNodeValue(l_strPrefixPath + "updateuser", data.m_strUpdateUser);
                    p_pJson->SetNodeValue(l_strPrefixPath + "updatetime", data.m_strUpdateTime);
                    p_pJson->SetNodeValue(l_strPrefixPath + "delflag", data.m_strDelFlag);
                 


                    ++l_uiIndex;
                }

                return p_pJson->ToString();
            }

        public:
            class CData
            {
            public:
                std::string m_strID;
                std::string m_strCaseId;					//警情ID
                std::string m_strDisPosalId;				//处警单ID
                std::string m_strCaseDispatchStatus;		//反馈类型
                std::string m_strInterphoneId;              //设备ID列表
                std::string m_strFBWay;                     //反馈方式:1终端快捷键；2短信；3电话
                std::string m_strFBContent;                 //反馈内容
                std::string m_strFeedBacker;                //反馈人
                std::string m_strFBTime;                    //反馈时间
                std::string m_strCaseType;					//案件类型
                std::string m_strCaseSubType;				//案件中类
                std::string m_strCaseThreeType;				//案件小类
                std::string m_strCaseProcessor;				//处警人列表（逗号分割）
                std::string m_strCreateUser;				//创建人
                std::string m_strCreateTime;				//创建时间
                std::string m_strUpdateUser;				//修改人,取最后一次修改值
                std::string m_strUpdateTime;				//修改时间,取最后一次修改值
                std::string m_strDelFlag;				    //0有效，1删除
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
