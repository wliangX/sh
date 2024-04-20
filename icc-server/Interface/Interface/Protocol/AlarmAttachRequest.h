#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/IRespond.h>
#include "Protocol/CHeader.h"


namespace ICC
{
	namespace PROTOCOL
	{
		// �������鸽��
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
				std::string m_stralarmId;                       // ����id
				std::string m_strattachId;                      // ���鸽��id
				std::string m_strattachName;                    // ���鸽������
				std::string m_strattachType;                    // ���鸽������
				std::string m_strattachPath;                    // ���鸽��·��
				std::string m_strattachSource;                  // ���鸽����Դ
				std::string m_strcreateUser;                    // ������
				std::string m_strcreateTime;                    // ����ʱ��
			};

			class CBody
			{
			public:
				std::vector<data> m_vecdata;                    // ���ص�����(Object)
				data m_data;
				std::string m_strcode;                          // ������Ϣ��,Ĭ��:200                       
				std::string m_strmessage;                       // ������Ϣ���ݣ�Ĭ��:success 
			};

			CBody m_oBody;
		};

		// ɾ�����鸽��
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
				std::vector<std::string>m_vecIds;     // ������ϢID����
			};

			CBody m_oBody;
		};

		// ��ѯ���鸽��
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
				std::string m_stralarmId;                           // �Ӿ�����
			};

			CBody m_oBody;
		};

		// ΢�ű�������
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
				std::string m_stralarmId;                       // ����id
				std::string m_strattachId;                      // ���鸽��id
				std::string m_strattachName;                    // ���鸽������
				std::string m_strattachType;                    // ���鸽������
			};

			class CBody
			{
			public:
				std::vector<data> m_vecdata;                    // ���ص�����(Object)
				data m_data;
				std::string m_strwechatAttachIp;                //΢�ű�������ip
				std::string	m_strwechatAttachPort;              //΢�ű�������Port
				std::string	m_strfileService;                   //΢�Ÿ�����������
				std::string m_strID;
			};
			CBody m_oBody;
		};

		// ΢�ű�����Ϣ���
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
				std::string m_strsourceId;                       // ΢�ű���id
			};
			CBody m_oBody;
		};

		// ������Ϣͬ��
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
				std::string m_stralarmId;                       // ����id
				std::string m_strattachId;                      // ���鸽��id
				std::string m_strattachName;                    // ���鸽������
				std::string m_strattachType;                    // ���鸽������
				std::string m_strattachPath;                    // ���鸽��·��
				std::string m_strattachSource;                  // ���鸽����Դ
				std::string m_strcreateUser;                    // ������
				std::string m_strcreateTime;                    // ����ʱ��
			};

			class CBody
			{
			public:
				std::string m_strcode;                          // ������Ϣ��,Ĭ��:200                       
				std::string m_strmessage;                       // ������Ϣ���ݣ�Ĭ��:success 
				std::vector<data> m_vecdata;                    // ���ص�����(Object)
				data m_data;
				std::string m_strBeginTime;                     // ��ѯ��ʼʱ��
				std::string m_strEndTime;                       // ��ѯ����ʱ��

				std::string m_strCurrent;                       // ҳ��,Ĭ��Ϊ1,ʾ��ֵ(1)
				std::string m_strSize;                          // ÿҳ����,Ĭ��Ϊ20,ʾ��ֵ(20)
				std::string m_strPages;                         // ҳ��
				std::string m_strCount;                         // ����
			};
			CBody m_oBody;
		};
	}
}

