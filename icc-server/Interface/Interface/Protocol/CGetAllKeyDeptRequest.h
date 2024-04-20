#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CGetAllKeyDeptRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strFilterKey = p_pJson->GetNodeValue("/body/filter_key", "");
				m_oBody.m_strPageSize = p_pJson->GetNodeValue("/body/page_size", "");
				m_oBody.m_strPageIndex = p_pJson->GetNodeValue("/body/page_index", "");
				return true;
			}

		public:
			CHeaderEx m_oHeader;
			class CBody
			{
			public:
				std::string m_strFilterKey;  //过滤的键值。查所有，则此字段不传值，否则根据此值对单位名称，号码以及地址进行模糊匹配
				std::string m_strPageSize;   //page_size	每页数量（不能为空）
				std::string m_strPageIndex;	//page_index	页码，1表示第一页（不能为空）
			};
			CBody m_oBody;
		};
	}
}
