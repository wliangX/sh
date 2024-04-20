#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/IRespond.h>
#include <Protocol/CSmpHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		//************************************
		// Class:  CGetLicenseRequest
		// Brief:  
		//************************************
		class CGetLicenseRequest :
			public IRequest, public IRespond
		{
		public:
			virtual bool ParseString(std::string p_strJson, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_Header.ParseString(p_strJson, p_pJson))
				{
					return false;
				}

				m_Body.m_strCount = p_pJson->GetNodeValue("/body/count", "");
				m_Body.m_strVersionData = p_pJson->GetNodeValue("/body/version_data", "");

				int l_iCount = atoi(m_Body.m_strCount.c_str());
				for (int i = 0; i < p_pJson->GetCount("/body/data"); i++)
				{
					std::string l_strPath = "/body/data/";
					std::string l_strNum = std::to_string(i);
					CBody::CData l_CData;
					l_CData.m_strEnableTimeLimit = p_pJson->GetNodeValue(l_strPath + l_strNum + "/enable_time_limit", "");
					l_CData.m_strEnableSuperdogCode = p_pJson->GetNodeValue(l_strPath + l_strNum + "/enable_superdog_code", "");
					l_CData.m_strNodeDesp = p_pJson->GetNodeValue(l_strPath + l_strNum + "/node_desp", "");
					l_CData.m_strNodeName = p_pJson->GetNodeValue(l_strPath + l_strNum + "/node_name", "");
					l_CData.m_strSuperdogCode = p_pJson->GetNodeValue(l_strPath + l_strNum + "/superdog_code", "");

					l_CData.m_strMachineCode = p_pJson->GetNodeValue(l_strPath + l_strNum + "/machine_code", "");
					l_CData.m_strGenDate = p_pJson->GetNodeValue(l_strPath + l_strNum + "/gen_date", "");
					l_CData.m_strDefaultNum = p_pJson->GetNodeValue(l_strPath + l_strNum + "/default_num", "");
					l_CData.m_strValidDate = p_pJson->GetNodeValue(l_strPath + l_strNum + "/valid_date", "");
					l_CData.m_strVenderCode = p_pJson->GetNodeValue(l_strPath + l_strNum + "/vender_code", "");

					l_CData.m_strEnableMachineCode = p_pJson->GetNodeValue(l_strPath + l_strNum + "/enable_machine_code", "");
					l_CData.m_strLicenseDesp = p_pJson->GetNodeValue(l_strPath + l_strNum + "/license_desp", "");
					l_CData.m_strValidDays = p_pJson->GetNodeValue(l_strPath + l_strNum + "/valid_days", "");
					l_CData.m_strStatus = p_pJson->GetNodeValue(l_strPath + l_strNum + "/status", "");

					m_Body.m_vecData.push_back(l_CData);
				}	

				return true;
			}

			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				return "";
			}

		public:
			class CBody
			{
			public:
				class CData
				{
				public:
					std::string m_strEnableTimeLimit;		//是否启用限制时间控制
					std::string m_strEnableSuperdogCode;	//是否启用超级狗控制
					std::string m_strNodeDesp;				//系统编码
					std::string m_strNodeName;				//系统名
					std::string m_strSuperdogCode;			//超级狗码
					std::string m_strMachineCode;			//机器码
					std::string m_strGenDate;				//License key生成日期
					std::string m_strDefaultNum;			//客户端授权数量
					std::string m_strValidDate;				//有效天数
					std::string m_strVenderCode;			//厂商代码
					std::string m_strEnableMachineCode;		//是否启用机器码控制
					std::string m_strLicenseDesp;			//license_desp
					std::string m_strValidDays;				//截止日期
					std::string m_strStatus;				//License状态
				};

				std::vector<CData> m_vecData;
				std::string m_strCount;
				std::string m_strVersionData;
			};

			CSmpHeader m_Header;
			CBody m_Body;
		};
	}
}
