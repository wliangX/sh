#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/IRespond.h>
#include "Protocol/CHeader.h"


namespace ICC
{
	namespace PROTOCOL
	{
		// 新增警情附件
		class CNotifiAddAlarmAttachRequest :
			public IRespond
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				int iCount = p_pJson->GetCount("/data");
				for (int i = 0; i < iCount; i++)
				{
					std::string tmp_path("/data/" + std::to_string(i));
					m_oBody.m_data.m_stralarmId = p_pJson->GetNodeValue(tmp_path + "/alarm_id", "");
					m_oBody.m_data.m_strattachId = p_pJson->GetNodeValue(tmp_path + "/attach_id", "");
					m_oBody.m_data.m_strattachName = p_pJson->GetNodeValue(tmp_path + "/attach_name", "");
					m_oBody.m_data.m_strattachType = p_pJson->GetNodeValue(tmp_path + "/attach_type", "");
					m_oBody.m_data.m_strattachPath = p_pJson->GetNodeValue(tmp_path + "/attach_path", "");
					m_oBody.m_data.m_strattachSource = p_pJson->GetNodeValue(tmp_path + "/attach_source", "");
					m_oBody.m_data.m_strcreateUser = p_pJson->GetNodeValue(tmp_path + "/create_user", "");
					m_oBody.m_data.m_strcreateTime = p_pJson->GetNodeValue(tmp_path + "/create_time", "");
					m_oBody.m_vecdata.push_back(m_oBody.m_data);
				}
				
				return true;
			}

			virtual bool ParseStringSync(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				int iCount = p_pJson->GetCount("/body/data");
				for (int i = 0; i < iCount; i++)
				{
					std::string tmp_path("/body/data/" + std::to_string(i));
					m_oBody.m_data.m_stralarmId = p_pJson->GetNodeValue(tmp_path + "/alarm_id", "");
					m_oBody.m_data.m_strattachId = p_pJson->GetNodeValue(tmp_path + "/attach_id", "");
					m_oBody.m_data.m_strattachName = p_pJson->GetNodeValue(tmp_path + "/attach_name", "");
					m_oBody.m_data.m_strattachType = p_pJson->GetNodeValue(tmp_path + "/attach_type", "");
					m_oBody.m_data.m_strattachPath = p_pJson->GetNodeValue(tmp_path + "/attach_path", "");
					m_oBody.m_data.m_strattachSource = p_pJson->GetNodeValue(tmp_path + "/attach_source", "");
					m_oBody.m_data.m_strcreateUser = p_pJson->GetNodeValue(tmp_path + "/create_user", "");
					m_oBody.m_data.m_strcreateTime = p_pJson->GetNodeValue(tmp_path + "/create_time", "");
					m_oBody.m_vecdata.push_back(m_oBody.m_data);
				}

				return true;
			}


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
			virtual std::string ToStringSync(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}
				m_oHeader.SaveTo(p_pJson);
				p_pJson->SetNodeValue("/code", m_oBody.m_strcode);
				p_pJson->SetNodeValue("/message", m_oBody.m_strmessage);
				for (size_t i = 0; i < m_oBody.m_vecdata.size(); ++i)
				{
					std::string l_strPrefixPath("/data/" + std::to_string(i));
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
				std::vector<data> m_vecdata;                    // 返回的数据(Object)
				data m_data;
				std::string m_strcode;                          // 返回信息码,默认:200                       
				std::string m_strmessage;                       // 返回信息内容，默认:success 
			};

			CBody m_oBody;
		};

		// 删除警情附件
		class CNotifiDeleteAlarmAttachRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				int uCount = p_pJson->GetCount("/ids");

				for (int i = 0; i < uCount; ++i)
				{
					std::string l_strPrefixPath("/ids/" + std::to_string(i));
					std::string l_strId = p_pJson->GetNodeValue(l_strPrefixPath, "");
					m_oBody.m_vecIds.push_back(l_strId);
				}
				return true;
			}
		public:
			CHeaderEx m_oHeader;

			class CBody
			{
			public:
				std::vector<std::string>m_vecIds;     // 基础信息ID集合
			};

			CBody m_oBody;
		};

		// 查询警情附件
		class CNotifiSearchAlarmPlanRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_oBody.m_stralarmId = p_pJson->GetNodeValue("/alarm_id", "");
				return true;
			}
		public:
			CHeaderEx m_oHeader;

			class CBody
			{
			public:
				std::string m_stralarmId;                           // 接警单号
			};

			CBody m_oBody;
		};

		// 微信报警附件
		class CNotifiWechatAttachRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				int iCount = p_pJson->GetCount("/data");
				for (int i = 0; i < iCount; i++)
				{
					std::string tmp_path("/data/" + std::to_string(i));
					m_oBody.m_data.m_stralarmId = p_pJson->GetNodeValue(tmp_path + "/alarmId", "");
					m_oBody.m_data.m_strattachId = p_pJson->GetNodeValue(tmp_path + "/fileid", "");
					m_oBody.m_data.m_strattachName = p_pJson->GetNodeValue(tmp_path + "/filename", "");
					m_oBody.m_data.m_strattachType = p_pJson->GetNodeValue(tmp_path + "/jqfjlx", "");

					m_oBody.m_vecdata.push_back(m_oBody.m_data);
				}

				return true;
			}
		public:
			CHeaderEx m_oHeader;

			struct data
			{
				std::string m_stralarmId;                       // 警情id
				std::string m_strattachId;                      // 警情附件id
				std::string m_strattachName;                    // 警情附件名称
				std::string m_strattachType;                    // 警情附件类型
			};

			class CBody
			{
			public:
				std::vector<data> m_vecdata;                    // 返回的数据(Object)
				data m_data;
				std::string m_strwechatAttachIp;                //微信报警服务ip
				std::string	m_strwechatAttachPort;              //微信报警服务Port
				std::string	m_strfileService;                   //微信附件服务名称
				std::string m_strID;
			};
			CBody m_oBody;
		};

		// 微信报警信息变更
		class CNotifiAlarmInfoChangeRequest :
			public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_oBody.m_strsourceId = p_pJson->GetNodeValue("/source_id", "");
			
				return true;
			}
		public:
			CHeaderEx m_oHeader;


			class CBody
			{
			public:
				std::string m_strsourceId;                       // 微信报警id
			};
			CBody m_oBody;
		};

		// 附件信息同步
		class SynAlarmAttachRequest :
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
				m_oBody.m_strCurrent = p_pJson->GetNodeValue("/body/current", "");
				m_oBody.m_strSize = p_pJson->GetNodeValue("/body/size", "");

				return true;
			}
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}
			    m_oHeader.SaveTo(p_pJson);
				p_pJson->SetNodeValue("/body/code", m_oBody.m_strcode);
				p_pJson->SetNodeValue("/body/message", m_oBody.m_strmessage);
				p_pJson->SetNodeValue("/body/count", m_oBody.m_strCount);
				p_pJson->SetNodeValue("/body/current", m_oBody.m_strCurrent);
				p_pJson->SetNodeValue("/body/pages", m_oBody.m_strPages);
				p_pJson->SetNodeValue("/body/size", m_oBody.m_strSize);

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
				std::string m_strBeginTime;                     // 查询开始时间
				std::string m_strEndTime;                       // 查询结束时间

				std::string m_strCurrent;                       // 页码,默认为1,示例值(1)
				std::string m_strSize;                          // 每页条数,默认为20,示例值(20)
				std::string m_strPages;                         // 页数
				std::string m_strCount;                         // 总数
			};
			CBody m_oBody;
		};
	}
}

