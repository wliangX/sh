#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>
#include <Protocol/IRespond.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CMonitorSeatApplyQueryRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strPageSize = p_pJson->GetNodeValue("/body/page_size", "");
				m_oBody.m_strPageIndex = p_pJson->GetNodeValue("/body/page_index", "");
				
				m_oBody.m_strApplyType = p_pJson->GetNodeValue("/body/apply_type", "");
				
				m_oBody.m_strApproveResult = p_pJson->GetNodeValue("/body/approve_result", "");
				
				if (m_oBody.m_bSingelRequest)
				{
					m_oBody.m_strApplyTargetID = p_pJson->GetNodeValue("/body/apply_target_id", "");
				}
				else
				{
				    m_oBody.m_strStartTime = p_pJson->GetNodeValue("/body/start_time", "");
				    m_oBody.m_strEndTime = p_pJson->GetNodeValue("/body/end_time", "");
				    m_oBody.m_strApplyOrgCode = p_pJson->GetNodeValue("/body/apply_org_code", "");
				}
				return true;
			}


		public:
			class CBody
			{
			public:
			    //单个请求使用
			    bool        m_bSingelRequest;                //是否单个请求
				
				std::string m_strApplyTargetID;             //目标id  单个精确匹配使用
			    
				std::string	m_strPageSize;					//每页数量    共用
				std::string	m_strPageIndex;						//第几页   共用
				std::string	m_strStartTime;					//开始时间
				std::string m_strEndTime;					//结束时间
				std::string m_strApplyType;						//申请类型
				std::string m_strApplyOrgCode;			    //申请人所属部门编码
				std::string m_strApproveResult;						//空为全部,0未审批、1通过；2不通过   共用
				
			public:
			    CBody():m_bSingelRequest(false)
				{
					
				}
			};

		
			CHeaderEx m_oHeader;			
			CBody	m_oBody;
		};	
	}
}
