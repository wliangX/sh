#pragma once 
#include <Protocol/IRespond.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CGetHistoryCallListRespond :
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
				p_pJson->SetNodeValue("/body/msgid", m_oBody.m_strMsgID);
				p_pJson->SetNodeValue("/body/clientid", m_oBody.m_strClientID);
				p_pJson->SetNodeValue("/body/page_size", m_oBody.m_strPageSize);
				p_pJson->SetNodeValue("/body/page_index", m_oBody.m_strPageIndex);
				p_pJson->SetNodeValue("/body/time", m_oBody.m_strTime);
				return p_pJson->ToString();
			}

			virtual std::string ToExString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);
				p_pJson->SetNodeValue("/body/count", m_oBody.m_strCount);
				p_pJson->SetNodeValue("/body/all_count", m_oBody.m_strAllCount);
				for (size_t i = 0; i < m_oBody.m_vecData.size(); i++)
				{
					std::string l_strValueNum = std::to_string(i);
					std::string l_strPrefixPath("/body/data/" + std::to_string(i) + "/");
					CData& data = m_oBody.m_vecData[i];
					p_pJson->SetNodeValue(l_strPrefixPath + "acd", data.m_strACD);
					p_pJson->SetNodeValue(l_strPrefixPath + "call_direction", data.m_strCallDirection);
					p_pJson->SetNodeValue(l_strPrefixPath + "callref_id", data.m_strCallRefID);
					p_pJson->SetNodeValue(l_strPrefixPath + "caller_id", data.m_strCallerID);
					p_pJson->SetNodeValue(l_strPrefixPath + "called_id", data.m_strCalledID);
					p_pJson->SetNodeValue(l_strPrefixPath + "original_caller", data.m_strOriginalCaller);
					p_pJson->SetNodeValue(l_strPrefixPath + "original_called", data.m_strOriginalCalled);
					p_pJson->SetNodeValue(l_strPrefixPath + "record_files", data.m_strRecrdFiles);
					p_pJson->SetNodeValue(l_strPrefixPath + "incomming_time", data.m_strIncomingTime);
					p_pJson->SetNodeValue(l_strPrefixPath + "waiting_time", data.m_strWaitingTime);
					p_pJson->SetNodeValue(l_strPrefixPath + "assigned_time", data.m_strAssignedTime);
					p_pJson->SetNodeValue(l_strPrefixPath + "ring_time", data.m_strRingTime);
					p_pJson->SetNodeValue(l_strPrefixPath + "talk_time", data.m_strTalkTime);
					p_pJson->SetNodeValue(l_strPrefixPath + "hangup_time", data.m_strHangupTime);
				}
				return p_pJson->ToString();
			}

			virtual bool ParseExString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_oBody.m_strAllCount = p_pJson->GetNodeValue("/body/all_count", "");
				m_oBody.m_strCount = p_pJson->GetNodeValue("/body/count", "");

				int iCount = p_pJson->GetCount("/body/data");
				for (int i = 0; i < iCount; i++)
				{
					std::string l_strPrefixPath("/body/data/" + std::to_string(i) + "/");
					CData l_CData;
					//l_CData.m_strAgent = p_pJson->GetNodeValue(l_strPrefixPath + "agent", "");
					l_CData.m_strCallRefID = p_pJson->GetNodeValue(l_strPrefixPath + "callref_id", "");
					l_CData.m_strCallDirection = p_pJson->GetNodeValue(l_strPrefixPath + "call_direction", "");
					l_CData.m_strACD = p_pJson->GetNodeValue(l_strPrefixPath + "acd", "");
					l_CData.m_strCallerID = p_pJson->GetNodeValue(l_strPrefixPath + "caller_id", "");
					l_CData.m_strCalledID = p_pJson->GetNodeValue(l_strPrefixPath + "called_id", "");
					l_CData.m_strOriginalCaller = p_pJson->GetNodeValue(l_strPrefixPath + "original_caller", "");
					l_CData.m_strOriginalCalled = p_pJson->GetNodeValue(l_strPrefixPath + "original_called", "");
					l_CData.m_strRecrdFiles = p_pJson->GetNodeValue(l_strPrefixPath + "record_files", "");
					l_CData.m_strIncomingTime = p_pJson->GetNodeValue(l_strPrefixPath + "incomming_time", "");
					l_CData.m_strWaitingTime = p_pJson->GetNodeValue(l_strPrefixPath + "waiting_time", "");
					l_CData.m_strAssignedTime = p_pJson->GetNodeValue(l_strPrefixPath + "assigned_time", "");
					l_CData.m_strRingTime = p_pJson->GetNodeValue(l_strPrefixPath + "ring_time", "");
					l_CData.m_strTalkTime = p_pJson->GetNodeValue(l_strPrefixPath + "talk_time", "");
					l_CData.m_strHangupTime = p_pJson->GetNodeValue(l_strPrefixPath + "hangup_time", "");
					m_oBody.m_vecData.push_back(l_CData);
				}

				return true;
			}

		public:
			CHeaderEx m_oHeader;
			class CData
			{
			public:
				//std::string m_strAgent;				//坐席分机号
				std::string m_strACD;				//ACD 组名称（110、119、120、122）
				std::string m_strCallDirection;		//呼叫方向
				std::string m_strCallRefID;			//话务id
				std::string m_strCallerID;			//主叫号
				std::string m_strCalledID;			//被叫号
				std::string m_strOriginalCaller;	//原主叫
				std::string m_strOriginalCalled;	//原被叫
				std::string m_strRecrdFiles;		//录音文件路径，如果一个话务有多个录音，用分号“；”隔开
				std::string m_strIncomingTime;		//呼入时间
				std::string m_strWaitingTime;		//排队开始时间
				std::string m_strAssignedTime;		//话务分配时间
				std::string m_strRingTime;			//开始振铃时间
				std::string m_strTalkTime;			//开始通话时间
				std::string m_strHangupTime;		//挂机时间
			};
			class CBody
			{
			public:
				std::string m_strPageIndex;
				std::string m_strPageSize;
				std::string m_strClientID;
				std::string m_strTime;
				std::string m_strCode;				//200：成功；非0：失败，具体失败原因从 message字段读取
				std::string m_strMessage;			//code描述
				std::string m_strMsgID;				//消息唯一ID
				std::string m_strCount;				//当前页数量
				std::string m_strAllCount;			//历史话务总量	
				std::vector<CData> m_vecData;
			};
			CBody m_oBody;
		};
	}
}
