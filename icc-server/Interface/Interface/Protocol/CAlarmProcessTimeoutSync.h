#pragma once 
#include <Protocol/ISync.h>
#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>
namespace ICC
{
	namespace PROTOCOL
	{
        class CGetAlarmProcessTimeoutRespond :
			public ISync
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				m_oHeader.SaveTo(p_pJson);

				p_pJson->SetNodeValue("/body/count", m_oBody.m_strCount);
				for (unsigned int i = 0; i < m_oBody.m_vData.size(); i++)
				{
					p_pJson->SetNodeValue("/body/data/" + std::to_string(i) + "/id", m_oBody.m_vData[i].m_strID);
					p_pJson->SetNodeValue("/body/data/" + std::to_string(i) + "/process_id", m_oBody.m_vData[i].m_strProcessID);
					p_pJson->SetNodeValue("/body/data/" + std::to_string(i) + "/called_no_type", m_oBody.m_vData[i].m_strCalledNoType);
					p_pJson->SetNodeValue("/body/data/" + std::to_string(i) + "/content", m_oBody.m_vData[i].m_strContent);
					p_pJson->SetNodeValue("/body/data/" + std::to_string(i) + "/time_submit", m_oBody.m_vData[i].m_strTimeSubmit);
					p_pJson->SetNodeValue("/body/data/" + std::to_string(i) + "/process_dept_code", m_oBody.m_vData[i].m_strProcessDeptCode);
					p_pJson->SetNodeValue("/body/data/" + std::to_string(i) + "/process_dept_name", m_oBody.m_vData[i].m_strProcessDeptName);
					p_pJson->SetNodeValue("/body/data/" + std::to_string(i) + "/dispatch_code", m_oBody.m_vData[i].m_strDispatchCode);
					p_pJson->SetNodeValue("/body/data/" + std::to_string(i) + "/dispatch_name", m_oBody.m_vData[i].m_strDispatchName);
					p_pJson->SetNodeValue("/body/data/" + std::to_string(i) + "/dispatch_dept_code", m_oBody.m_vData[i].m_strDispatchDeptCode);
					p_pJson->SetNodeValue("/body/data/" + std::to_string(i) + "/dispatch_dept_name", m_oBody.m_vData[i].m_strDispatchDeptName);
				}
				return p_pJson->ToString();
			}

		public:
			CHeader m_oHeader;
			class CBody
			{
			public:
				class CData
				{
				public:
					std::string m_strID;		  //警情id
					std::string m_strProcessID;
					std::string m_strCalledNoType;//警情报警号码字典类型
					std::string m_strContent;     //警情类型
					std::string m_strTimeSubmit;  //提交时间
					std::string m_strProcessDeptCode;
					std::string m_strProcessDeptName;

					std::string m_strDispatchCode;
					std::string m_strDispatchName;

					std::string m_strDispatchDeptCode;
					std::string m_strDispatchDeptName;
				};
				std::string m_strCount;
				std::vector<CData> m_vData;
			};
			CBody m_oBody;
		};

		class CAlarmProcessTimeoutSync :
			public ISync
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}
				m_oHeader.SaveTo(p_pJson);

				unsigned int i = 0;
				for (auto iter = m_oBody.m_mapData.begin(); iter != m_oBody.m_mapData.end();iter++)
				{
					p_pJson->SetNodeValue("/body/data/" + std::to_string(i) + "/count", iter->second);
					p_pJson->SetNodeValue("/body/data/" + std::to_string(i) + "/dept_code", iter->first);
					++i;
				}

				return p_pJson->ToString();
			}

		public:
			CHeader m_oHeader;
			class CBody
			{
			public:
				class CData
				{
				public:
					std::string m_strCount;
					std::string m_strDeptCode;
				};
				std::map<std::string,std::string> m_mapData;
			};
			CBody m_oBody;
		};


		class CGetAlarmProcessTimeout :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_body.m_strPageSize = p_pJson->GetNodeValue("/body/page_size", "");
				m_body.m_strPageIdex = p_pJson->GetNodeValue("/body/page_index", "");
				m_body.m_strDeptCode = p_pJson->GetNodeValue("/body/dept_code", "");
				return true;
			}


		public:
			class CBody
			{
			public:
				std::string m_strPageSize;
				std::string m_strPageIdex;
				std::string m_strDeptCode;
			};
			CBody   m_body;
			CHeader m_oHeader;
		};


	}
}
