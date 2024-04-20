#pragma once
#include <Protocol/IRequest.h>
#include <Protocol/IRespond.h>
#include <Protocol/CHeader.h>
namespace ICC
{
	namespace PROTOCOL
	{
		class CDeleteBindRespond :
			public IRequest,IRespond
		{
		public:
			//SMP推送解绑关系时使用
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
                m_oBody.m_strResult = p_pJson->GetNodeValue("/body/result", "");
				return true;
			}
			//基础数据返回删除绑定关系结果
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				m_oHeader.SaveTo(p_pJson);
				p_pJson->SetNodeValue("/body/result", m_oBody.m_strResult);
				return p_pJson->ToString();
			}

		public:			
			class CBody
			{
			public:
				std::string m_strResult;
			};
			CHeader m_oHeader;
			CBody m_oBody;
		};
	}
}