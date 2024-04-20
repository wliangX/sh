#pragma once 
#include <Protocol/IRespond.h>
#include <Protocol/IRequest.h>
#include "Protocol/CHeader.h"
namespace ICC
{
	namespace PROTOCOL
	{
		// 警情附件
		class CNotifiAlarmAttachResponse :
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
				p_pJson->SetNodeValue("/code", m_oBody.m_strcode);
				p_pJson->SetNodeValue("/message", m_oBody.m_strmessage);
				if (!m_oBody.m_vecdata.empty())
				{
					for (size_t i = 0; i < m_oBody.m_vecdata.size(); i++)
					{
						std::string l_strPrefixPath("/data/" + std::to_string(i));
						p_pJson->SetNodeValue(l_strPrefixPath , m_oBody.m_vecdata.at(i));
					}
				}
				else
				{
					p_pJson->SetNodeValue("/data", m_oBody.m_strdata);
				}
				return p_pJson->ToString();
			}

			virtual std::string ToString_Body(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}
				m_oHeader.SaveTo(p_pJson);
				p_pJson->SetNodeValue("/body/code", m_oBody.m_strcode);
				p_pJson->SetNodeValue("/body/message", m_oBody.m_strmessage);
				if (!m_oBody.m_vecdata.empty())
				{
					for (size_t i = 0; i < m_oBody.m_vecdata.size(); i++)
					{
						std::string l_strPrefixPath("/body/data/" + std::to_string(i));
						p_pJson->SetNodeValue(l_strPrefixPath, m_oBody.m_vecdata.at(i));
					}
				}
				else
				{
					p_pJson->SetNodeValue("/data", m_oBody.m_strdata);
				}
				return p_pJson->ToString();
			}

		public:
			CHeaderEx m_oHeader;

			class CBody
			{
			public:
				std::string m_strcode;                         // 返回信息码,默认:200
				std::string m_strdata;                         // 返回的数据
				std::string m_strmessage;                      // 返回信息内容，默认:success
				std::vector<std::string> m_vecdata;
			};
			CBody m_oBody;
		};

		// 查询警情附件
		class CNotifiSearchAlarmAttachResponse :
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
				p_pJson->SetNodeValue("/body/code", m_oBody.m_strcode);
				p_pJson->SetNodeValue("/body/message", m_oBody.m_strmessage);
				for (size_t i = 0; i < m_oBody.m_vecdata.size(); ++i)
				{
					std::string l_strPrefixPath("/body/data/" + std::to_string(i));
					p_pJson->SetNodeValue(l_strPrefixPath + "/id", m_oBody.m_vecdata.at(i).m_strId);
					p_pJson->SetNodeValue(l_strPrefixPath + "/alarm_id", m_oBody.m_vecdata.at(i).m_stralarmId);
					p_pJson->SetNodeValue(l_strPrefixPath + "/attach_id", m_oBody.m_vecdata.at(i).m_strattachId);
					p_pJson->SetNodeValue(l_strPrefixPath + "/attach_name", m_oBody.m_vecdata.at(i).m_strattachName);
					p_pJson->SetNodeValue(l_strPrefixPath + "/attach_type", m_oBody.m_vecdata.at(i).m_strattachType);
					p_pJson->SetNodeValue(l_strPrefixPath + "/attach_path", m_oBody.m_vecdata.at(i).m_strattachPath);
					p_pJson->SetNodeValue(l_strPrefixPath + "/attach_source", m_oBody.m_vecdata.at(i).m_strattachSource);
					p_pJson->SetNodeValue(l_strPrefixPath + "/create_user", m_oBody.m_vecdata.at(i).m_strcreateUser);
					p_pJson->SetNodeValue(l_strPrefixPath + "/create_time", m_oBody.m_vecdata.at(i).m_strcreateTime);
				}
				return p_pJson->ToString();
			}

		public:
			CHeaderEx m_oHeader;
	
			struct data
			{
				std::string m_strId;                            // id
				std::string m_stralarmId;                       // 警情id
				std::string m_strattachId;                      // 警情附件id
				std::string m_strattachName;                    // 警情附件名称
				std::string m_strattachType;                    // 警情附件类型
				std::string m_strattachPath;                    // 警情附件路径
				std::string m_strattachSource;                  // 警情附件来源
				std::string m_strcreateUser;                    // 创建者
				std::string m_strcreateTime;                    // 创建时间
			};

			class CBody
			{
			public:
				std::string m_strcode;                          // 返回信息码,默认:200                       
				std::string m_strmessage;                       // 返回信息内容，默认:success 
	            std::vector<data> m_vecdata;                    // 返回的数据(Object)
				data m_data;
			};
			CBody m_oBody;
		};

		//聊天记录信息 alarm_chatinfo_change

		class CNotifiAlarmChatinfoChangeResponse :
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
				p_pJson->SetNodeValue("/code", m_oBody.m_strcode);
				p_pJson->SetNodeValue("/message", m_oBody.m_strmessage);
				p_pJson->SetNodeValue("/source_id", m_oBody.m_strsourceId);
				p_pJson->SetNodeValue("/alarm_id", m_oBody.m_stralarmId);
				return p_pJson->ToString();
			}

		public:
			CHeader m_oHeader;

			class CBody
			{
			public:
				std::string m_strcode;                          // 返回信息码,默认:200                       
				std::string m_strmessage;                       // 返回信息内容，默认:success 
				std::string m_stralarmId;                       // 警情id
				std::string m_strsourceId;                      // sourceid
			};
			CBody m_oBody;
		};

	}
}
