#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>
#include <Protocol/IRespond.h>
#include <Protocol/CAlarmInfo.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CTodayAlarm :
			public IRequest, public IRespond
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
				m_oBody.m_strReceiptDeptCode = p_pJson->GetNodeValue("/body/receipt_dept_code", "");
				m_oBody.m_strCalledNoType = p_pJson->GetNodeValue("/body/called_no_type", "");
				m_oBody.m_strState = p_pJson->GetNodeValue("/body/state", "");
				m_oBody.m_strReceiptCode = p_pJson->GetNodeValue("/body/receipt_code", "");
				m_oBody.m_strOnlyShowReal = p_pJson->GetNodeValue("/body/only_show_real", "");
				m_oBody.m_strQueryKey = p_pJson->GetNodeValue("/body/query_key", "");

				m_oBody.m_strLevel = p_pJson->GetNodeValue("/body/level", "");
				m_oBody.m_strIgnorePrivacy = p_pJson->GetNodeValue("/body/ignore_privacy", "false");
                m_oBody.m_strReceiptDeptCodeRecursion= p_pJson->GetNodeValue("/body/receipt_dept_code_recursion", "");

				m_oBody.m_strIsInvalid = p_pJson->GetNodeValue("/body/is_invalid", "");
				m_oBody.m_strSeatNo = p_pJson->GetNodeValue("/body/receipt_seatno", "");

				m_oBody.m_strDays = p_pJson->GetNodeValue("/body/days", "");
				m_oBody.m_strCallerKey = p_pJson->GetNodeValue("/body/caller_key", "");

				m_oBody.m_strIsMajorAlarmFlag = p_pJson->GetNodeValue("/body/majoralarm_flag", "");

				m_oBody.m_strIsAttentionAlarmFlag = p_pJson->GetNodeValue("/body/attentionalarm_flag", "");
				m_oBody.m_strAttentionCode = p_pJson->GetNodeValue("/body/attention_code", "");

				m_oBody.m_strIsClosure = p_pJson->GetNodeValue("/body/is_over", "");

				m_oBody.m_strReceiverCode = p_pJson->GetNodeValue("/body/receiver_code", "");

				m_oBody.m_strUpAttention = p_pJson->GetNodeValue("/body/up_attention", "");

				m_oBody.m_strSortParam = p_pJson->GetNodeValue("/body/sort_param", "");//jqclztdm, bjsj desc
				m_oBody.m_strIsSigned = p_pJson->GetNodeValue("/body/is_signed", "");
				//按接处警合一的结构解析，其他新增警情方式再处理

				return true;
			}

			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);
				p_pJson->SetNodeValue("/body/all_count", m_oBody.m_strAllCount);
				p_pJson->SetNodeValue("/body/count", m_oBody.m_strCount);

				unsigned int l_uiIndex = 0;
				for (CAlarmInfo data : m_oBody.m_vecData)
				{
					//std::string l_strPrefixPath("/body/data/" + std::to_string(l_uiIndex) + "/");
					std::string l_strPrefixPath("/body/data/" + std::to_string(l_uiIndex));
					data.ComJson(l_strPrefixPath, p_pJson);

					l_uiIndex++;
				}

				return p_pJson->ToString();
			}

			//同步
			std::string Sync(JsonParser::IJsonPtr p_pJson, std::string strType, std::string strAlarmId)
			{
				if (NULL == p_pJson)
				{
					return "";
				}
				m_oHeader.SaveTo(p_pJson);
				p_pJson->SetNodeValue("/body/sync_type", strType);
				p_pJson->SetNodeValue("/body/id", strAlarmId);

				return p_pJson->ToString();
			}

		public:

			class CBody
			{
			public:
				std::string m_strPageSize;        //分页每页数量
				std::string m_strPageIndex;       //当前第几页
				std::string m_strReceiptDeptCode; //接警单位编码
				std::string m_strCalledNoType;  //报警号码类型
				std::string m_strState;           //警情状态 
				std::string m_strReceiptCode;     //接警员编码
				std::string m_strOnlyShowReal;    //只显示有效警情
				std::string m_strQueryKey;        //关键字
				std::string m_strLevel;           //警情级别
				std::string m_strIgnorePrivacy;   //忽略隐私保护
                std::string m_strReceiptDeptCodeRecursion;  //receipt_dept_code_recursion	是否包容子单位，0不包，1包含
				std::string m_strReceiverCode;
				
				std::string m_strIsInvalid;  //无效警情
				std::string m_strSeatNo;
				std::string m_strDays;
				std::string m_strCallerKey;  

				std::string m_strIsMajorAlarmFlag;//1：查询重大警情
				std::string m_strIsAttentionAlarmFlag;//1：查询关注警情
				std::string m_strAttentionCode;

				std::string m_strIsClosure; // 是否结案，0：未结案，1：已结案

				std::string m_strUpAttention; //1:置顶；

				std::string m_strCount;
				std::string m_strAllCount;

				std::string m_strSortParam;
				std::string m_strIsSigned;
				//获取所有单位下的所有可调派单位信息时有效
				std::vector<CAlarmInfo> m_vecData;
			};
			CHeaderEx m_oHeader;
			CBody	m_oBody;
			
		};
	}
}
