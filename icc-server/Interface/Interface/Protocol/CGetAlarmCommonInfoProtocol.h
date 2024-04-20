/*
常用信息（管辖单位/警情级别）请求
*/
#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>
#include <Protocol/IRespond.h>


namespace ICC
{
	namespace PROTOCOL
	{
		class CGetAlarmCommonInfoRequest :
			public IRequest
		{
		public:
			
            virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_oBody.m_strBeginTime = p_pJson->GetNodeValue("/body/begin_time", "");
                m_oBody.m_strEndTime = p_pJson->GetNodeValue("/body/end_time", "");
				m_oBody.m_strDeptCode = p_pJson->GetNodeValue("/body/dept_code", "");
				m_oBody.m_strSeatNo = p_pJson->GetNodeValue("/body/seat_no", "");
				return true;
			}


		public:
			class CBody
			{
			public:
				std::string m_strBeginTime;  //开始时间
				std::string  m_strEndTime;  //结束时间，允许没有
				std::string m_strDeptCode;  //不传seat_on以及本字段表示统计本坐席的，传了dept_code否则统计此部门的
				std::string m_strSeatNo;  //此值以及dept_code不传值时，统计自已，如果传了此值则只统计此坐席的
			};

			CHeaderEx m_oHeader;			
			CBody	m_oBody;
		};

		class CGetAlarmCommonDeptRespond :
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

					p_pJson->SetNodeValue(l_strPrefixPath + "dept_code", data.m_strDeptCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "dept_name", data.m_strDeptName);

					++l_uiIndex;
				}

				return p_pJson->ToString();
			}

			virtual std::string ToExString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				p_pJson->SetNodeValue("/count", m_oBody.m_strCount);
				p_pJson->SetNodeValue("/update_time", m_oBody.m_strUpdateTime);

				unsigned int l_uiIndex = 0;
				for (CData data : m_oBody.m_vecData)
				{
					std::string l_strPrefixPath("/data/" + std::to_string(l_uiIndex) + "/");

					p_pJson->SetNodeValue(l_strPrefixPath + "dept_code", data.m_strDeptCode);
					p_pJson->SetNodeValue(l_strPrefixPath + "dept_name", data.m_strDeptName);

					++l_uiIndex;
				}

				return p_pJson->ToString();
			}

			virtual bool ParseExString(std::string l_strJson, JsonParser::IJsonPtr p_pJson)
			{
				if (!p_pJson || !p_pJson->LoadJson(l_strJson))
				{
					return false;
				}

				m_oBody.m_strCount=p_pJson->GetNodeValue("/count", "");
				m_oBody.m_strUpdateTime=p_pJson->GetNodeValue("/update_time","");

				int l_iCount = p_pJson->GetCount("/data");
				CData l_CData;
				for (int i = 0; i < l_iCount; i++)
				{
					std::string l_strDataNum = std::to_string(i);

					l_CData.m_strDeptCode=p_pJson->GetNodeValue("/data/" +l_strDataNum + "/dept_code", "");
					l_CData.m_strDeptName=p_pJson->GetNodeValue("/data/" + l_strDataNum + "/dept_name","");
					m_oBody.m_vecData.push_back(l_CData);
				}

				return true;
			}

		public:
			class CData
			{
			public:
				std::string	m_strDeptCode;			
				std::string m_strDeptName;	
			};
			class CBody
			{
			public:
				std::string m_strReceive_dept_code;
				std::string m_strUpdateTime;
				std::string m_strCount;									
				std::vector<CData> m_vecData;
			};
			CHeaderEx m_oHeader;
			CBody	m_oBody;
		};
		
		class CGetAlarmCommonTypeRespond :
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

					p_pJson->SetNodeValue(l_strPrefixPath + "first_type", data.m_strFirstType);
					p_pJson->SetNodeValue(l_strPrefixPath + "second_type", data.m_strSecondType);
					p_pJson->SetNodeValue(l_strPrefixPath + "third_type", data.m_strThirdType);
					p_pJson->SetNodeValue(l_strPrefixPath + "fourth_type", data.m_strFourthType);

					++l_uiIndex;
				}

				return p_pJson->ToString();
			}

			virtual std::string ToExString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				p_pJson->SetNodeValue("/count", m_oBody.m_strCount);
				p_pJson->SetNodeValue("/update_time", m_oBody.m_strUpdateTime);

				unsigned int l_uiIndex = 0;
				for (CData data : m_oBody.m_vecData)
				{
					std::string l_strPrefixPath("/data/" + std::to_string(l_uiIndex) + "/");

					p_pJson->SetNodeValue(l_strPrefixPath + "first_type", data.m_strFirstType);
					p_pJson->SetNodeValue(l_strPrefixPath + "second_type", data.m_strSecondType);
					p_pJson->SetNodeValue(l_strPrefixPath + "third_type", data.m_strThirdType);
					p_pJson->SetNodeValue(l_strPrefixPath + "fourth_type", data.m_strFourthType);

					++l_uiIndex;
				}

				return p_pJson->ToString();
			}

			virtual bool ParseExString(std::string l_strJson, JsonParser::IJsonPtr p_pJson)
			{
				if (!p_pJson || !p_pJson->LoadJson(l_strJson))
				{
					return false;
				}

				m_oBody.m_strCount = p_pJson->GetNodeValue("/count", "");
				m_oBody.m_strUpdateTime = p_pJson->GetNodeValue("/update_time", "");

				int l_iCount = p_pJson->GetCount("/data");
				CData l_CData;
				for (int i = 0; i < l_iCount; i++)
				{
					std::string l_strDataNum = std::to_string(i);

					l_CData.m_strFirstType = p_pJson->GetNodeValue("/data/" + l_strDataNum + "/first_type", "");
					l_CData.m_strSecondType = p_pJson->GetNodeValue("/data/" + l_strDataNum + "/second_type", "");
					l_CData.m_strThirdType = p_pJson->GetNodeValue("/data/" + l_strDataNum + "/third_type", "");
					l_CData.m_strFourthType = p_pJson->GetNodeValue("/data/" + l_strDataNum + "/fourth_type", "");
					m_oBody.m_vecData.push_back(l_CData);
				}

				return true;
			}
			

		public:
			class CData
			{
			public:
				std::string m_strFirstType;					//警情id一级类型
				std::string m_strSecondType;				//警情id二级类型
				std::string m_strThirdType;					//警情id三级类型
				std::string m_strFourthType;				//警情id四级类型						
			};
			class CBody
			{
			public:
				std::string m_strUpdateTime;
				std::string m_strCount;									
				std::vector<CData> m_vecData;
			};
			CHeaderEx m_oHeader;
			CBody	m_oBody;
		};
	}
}
