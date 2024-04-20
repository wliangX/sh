#pragma once 
#include <Protocol/IRespond.h>
#include <Protocol/CHeader.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CGetAllRedListRespond :
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
				for (size_t i = 0; i < m_oBody.m_vecData.size(); i++)
				{
					std::string l_strValueNum = std::to_string(i);
					//p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/id", m_oBody.m_vecData.at(i).m_strId);
					p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/phone_num", m_oBody.m_vecData.at(i).m_strPhoneNum);
					p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/name", m_oBody.m_vecData.at(i).m_strName);
					p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/department", m_oBody.m_vecData.at(i).m_strDepartment);
					p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/position", m_oBody.m_vecData.at(i).m_strPosition);
					p_pJson->SetNodeValue("/body/data/" + l_strValueNum + "/remark", m_oBody.m_vecData.at(i).m_strRemark);
				}

				return p_pJson->ToString();
			}

			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return false;
				}
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				int l_nCount = p_pJson->GetCount("/body/data");
				m_oBody.m_strCount = std::to_string(l_nCount);

				CBody::CData l_oData;
				for (int i = 0; i < l_nCount; ++i)
				{
					std::string l_strIndex = std::to_string(i);
					l_oData.m_strId = p_pJson->GetNodeValue("/body/data/" + l_strIndex + "/id", "");
					l_oData.m_strPhoneNum = p_pJson->GetNodeValue("/body/data/" + l_strIndex + "/phone_num", "");
					l_oData.m_strName = p_pJson->GetNodeValue("/body/data/" + l_strIndex + "/name", "");
					l_oData.m_strDepartment = p_pJson->GetNodeValue("/body/data/" + l_strIndex + "/department", "");
					l_oData.m_strPosition = p_pJson->GetNodeValue("/body/data/" + l_strIndex + "/position", "");
					l_oData.m_strRemark = p_pJson->GetNodeValue("/body/data/" + l_strIndex + "/remark", "");

					m_oBody.m_vecData.push_back(l_oData);
				}

				return true;
			}


		public:
			CHeaderEx m_oHeader;

			class CBody
			{
			public:
				std::string m_strCount = "0";

				class CData
				{
				public:
					std::string m_strId;			//GUID
					std::string m_strPhoneNum;		//电话
					std::string m_strName;			//姓名
					std::string m_strDepartment;	//部门
					std::string m_strPosition;		//职务
					std::string m_strRemark;		//备注
				};
				std::vector<CData>m_vecData;
			};
			CBody m_oBody;
		};
	}
}
