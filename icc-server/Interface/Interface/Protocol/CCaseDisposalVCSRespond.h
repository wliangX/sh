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
                std::string m_strCaseId;					//警情ID
                std::string m_strSeatNo;		    		//处置坐席
                std::string m_strDispatchOrgId;				//调派单位                
                std::string m_strDispatchTime;				//调派时间           
                std::string m_strCreateUser;				//创建人
                std::string m_strCreateTime;				//创建时间
                std::string m_strUpdateUser;				//修改人,取最后一次修改值
                std::string m_strUpdateTime;				//修改时间,取最后一次修改值
                std::string m_strDelFlag;				    //0有效，1删除
                std::string m_strStatus;				    //状态
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
