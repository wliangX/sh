#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CAddOrUpdateLinkedRequest :
			public IRequest,public IRespond
		{
		public:

			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				int l_iCount = p_pJson->GetCount("/body/linked_data");
				for (int i = 0; i<l_iCount; i++)
				{
					std::string l_strPrefixPath("/body/linked_data/" + std::to_string(i) + "/");
					CLinkedData l_oData;
					l_oData.m_strID = p_pJson->GetNodeValue(l_strPrefixPath + "id", "");
					l_oData.m_strAlarmID = p_pJson->GetNodeValue(l_strPrefixPath + "alarm_id", "");
					l_oData.m_strState = p_pJson->GetNodeValue(l_strPrefixPath + "state", "");
					l_oData.m_strLinkedOrgCode = p_pJson->GetNodeValue(l_strPrefixPath + "linked_org_code", "");
					l_oData.m_strLinkedOrgName = p_pJson->GetNodeValue(l_strPrefixPath + "linked_org_name", "");
					l_oData.m_strLinkedOrgType = p_pJson->GetNodeValue(l_strPrefixPath + "linked_org_type", "");
					l_oData.m_strDispatchCode = p_pJson->GetNodeValue(l_strPrefixPath + "dispatch_code", "");
					l_oData.m_strDispatchName = p_pJson->GetNodeValue(l_strPrefixPath + "dispatch_name", "");
					l_oData.m_strCreateUser = p_pJson->GetNodeValue(l_strPrefixPath + "create_user", "");
					l_oData.m_strCreateTime = p_pJson->GetNodeValue(l_strPrefixPath + "create_time", "");
					l_oData.m_strUpdateUser = p_pJson->GetNodeValue(l_strPrefixPath + "update_user", "");
					l_oData.m_strUpdateTime = p_pJson->GetNodeValue(l_strPrefixPath + "update_time", "");
					l_oData.m_strResult = p_pJson->GetNodeValue(l_strPrefixPath + "result", "");
					m_oBody.m_vecData.push_back(l_oData);
				}
				return true;
			}
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				m_oHeader.SaveTo(p_pJson);
				p_pJson->SetNodeValue("/body/msg_source", m_oBody.m_strMsgSource);
				unsigned int l_uiIndex = 0;

				for (CLinkedData data : m_oBody.m_vecData)
				{
					std::string l_strPrefixPath("/body/linked_data/" + std::to_string(l_uiIndex) + "/");

					p_pJson->SetNodeValue(l_strPrefixPath + "id", data.m_strID);
					p_pJson->SetNodeValue(l_strPrefixPath + "alarm_id", data.m_strAlarmID);
					p_pJson->SetNodeValue(l_strPrefixPath + "state", data.m_strState);
					p_pJson->SetNodeValue(l_strPrefixPath + "linked_org_code", data.m_strLinkedOrgCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "linked_org_name", data.m_strLinkedOrgName);
					p_pJson->SetNodeValue(l_strPrefixPath + "linked_org_type", data.m_strLinkedOrgType);
					p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_code", data.m_strDispatchCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "dispatch_name", data.m_strDispatchName);
					p_pJson->SetNodeValue(l_strPrefixPath + "create_user", data.m_strCreateUser);
					p_pJson->SetNodeValue(l_strPrefixPath + "create_time", data.m_strCreateTime);
					p_pJson->SetNodeValue(l_strPrefixPath + "update_user", data.m_strUpdateUser);
					p_pJson->SetNodeValue(l_strPrefixPath + "update_time", data.m_strUpdateTime);
					p_pJson->SetNodeValue(l_strPrefixPath + "result", data.m_strResult);
					++l_uiIndex;
				}
				return p_pJson->ToString();
			}
		public:
			class CLinkedData
			{
			public:
				CLinkedData()
				{
					m_bIsNewProcess = false;
				}
			public:
				std::string m_strMsgSource;
				std::string m_strUpdateType;					//
				std::string m_strDelFlag;
	
			public:
				bool m_bIsNewProcess;					//是否是新增的处警单，true代表是，false代表否

				std::string m_strID;							//	派警单编号，产生规则为行政区划代码+系统派警单编号，以免不同地区产生同样编号；唯一号，主索引键。
				std::string m_strAlarmID;						//	外键，关联接警单表中的接警单编号字段。
				std::string m_strState;							//	处理状态
				std::string m_strLinkedOrgCode;					//	联动单位代码
				std::string m_strLinkedOrgName;					//	联动单位名称
				std::string m_strLinkedOrgType;					//	联动单位类型
				std::string m_strDispatchCode;					//  发起调派警员编号
				std::string m_strDispatchName;					//	发起调派警员编号				
				std::string m_strCreateUser;					//	创建人
				std::string m_strUpdateUser;					//	修改人,取最后一次修改值修改时间,取最后一次修改值
				std::string m_strCreateTime;					//	创建的时间
				std::string m_strUpdateTime;					//	新增、变更的时间
				std::string m_strResult;						//	新增、变更的时间
			};
			class CBody
			{
			public:
				std::string m_strMsgSource;                 //消息来源，主要需要区分VCS来的，不写警情流水表
				std::vector<CLinkedData> m_vecData;
			};
			CHeaderEx m_oHeader;			
			CBody	m_oBody;
		};	
	}
}
