#pragma once 
#include <Protocol/IRespond.h>
#include <Protocol/IRequest.h>
#include "Protocol/CHeader.h"
namespace ICC
{
	namespace PROTOCOL
	{
		class CAcdSync : public IRespond
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				m_oHeader.SaveTo(p_pJson);

				p_pJson->SetNodeValue("/body/acd", m_oBody.m_data.m_acd);
				p_pJson->SetNodeValue("/body/dept_code", m_oBody.m_data.m_dept_code);
				p_pJson->SetNodeValue("/body/dept_name", m_oBody.m_data.m_dept_name);
				p_pJson->SetNodeValue("/body/remark", m_oBody.m_data.m_remark);
				p_pJson->SetNodeValue("/body/update_type", m_oBody.m_data.m_update_type);
				p_pJson->SetNodeValue("/body/dept_code_path", m_oBody.m_data.m_dept_code_path);
				p_pJson->SetNodeValue("/body/dept_name_path", m_oBody.m_data.m_dept_name_path);


				return p_pJson->ToString();
			}


		public:
			CHeader m_oHeader;			
			
			class CAcdData
			{
			public:	
				std::string m_acd;
				std::string m_dept_code;
				std::string m_dept_name;	
				std::string m_remark;
				std::string m_update_type; //1、更新  2、删除 3、更新
				std::string m_dept_code_path;
				std::string m_dept_name_path;
			};
			
			class CBody
			{
			public:
				CAcdData m_data;
			};
			CBody m_oBody;
		};
	}
}
