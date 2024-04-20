#pragma once 
#include <Protocol/IRequest.h>
#include <Protocol/CHeader.h>
#include <Protocol/CAlarmInfo.h>
#include <Protocol/CAddOrUpdateProcessRequest.h>
#include <Protocol/CAlarmLogSync.h>
namespace ICC
{
	namespace PROTOCOL
	{
		class CVcsSyncDataRequset : public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_strStartTime = p_pJson->GetNodeValue("/body/start_time", "");
				m_oBody.m_strSyncUpdateFlag = p_pJson->GetNodeValue("/body/sync_update_flag", "");
				m_oBody.m_strPackageSize = p_pJson->GetNodeValue("/body/package_size", "");
				return true;
			}

		public:
			class CBody
			{
			public:
				std::string m_strStartTime;
				std::string m_strSyncUpdateFlag;
				std::string m_strPackageSize;
			};

			CHeaderEx m_oHeader;
			CBody	m_oBody;
		};

		class CVcsSyncDataRespond :public IRespond
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);
				p_pJson->SetNodeValue("/body/result", m_oBody.m_strResult);
				return p_pJson->ToString();
			}

		public:
			CHeaderEx m_oHeader;

			class CBody
			{
			public:
				std::string m_strResult;
			};
			CBody m_oBody;
		};
		////////////////////////////////////////

		class CVcsAlarmSynDataRespond :public IRequest, IRespond
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_strCode = p_pJson->GetNodeValue("/code", "");
				m_strMessage = p_pJson->GetNodeValue("/message", "");

				int l_iCount = p_pJson->GetCount("/data");
				for (int i = 0; i < l_iCount; i++)
				{
					std::string p_strPrefix("/data/" + std::to_string(i) + "/alarm");
					CData l_oData;
					l_oData.m_oAlarm.ParseString(p_strPrefix, p_pJson);

					int l_iProcessCount = p_pJson->GetCount("/data/" + std::to_string(i) + "/process_data");
					for (int j = 0; j < l_iProcessCount; j++)
					{
						std::string p_strProcessPrefix("/data/" + std::to_string(i) + "/process_data/" +std::to_string(j) + "/");
						CAddOrUpdateProcessRequest::CProcessData l_oProcessData;
						if (l_oProcessData.ParseString(p_strProcessPrefix, p_pJson))
						{
							l_oData.m_vecProcessData.push_back(l_oProcessData);
						}
					}

					m_vecData.push_back(l_oData);


				}

				return true;
			}

			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}
				return "";
			}
		public:
			class CData
			{
			public:
				CAlarmInfo m_oAlarm;
				std::vector<CAddOrUpdateProcessRequest::CProcessData> m_vecProcessData;
			};
			CHeaderEx m_oHeader;
			std::string m_strEndTime;
			std::string m_strCode;
			std::string m_strMessage;

			std::vector<CData> m_vecData;

			
			
		};
		///////////////////////////////////////////////////////////////////////
		class CVcsProcessSynDataRespond :
			public IRequest, IRespond
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_strCode = p_pJson->GetNodeValue("/code", "");
				m_strMessage = p_pJson->GetNodeValue("/message", "");

				int l_iCount = p_pJson->GetCount("/data");
				for (int i = 0; i < l_iCount; i++)
				{
					std::string p_strPrefix("/data/" + std::to_string(i) + "/");
					CAddOrUpdateProcessRequest::CProcessData l_oData;
					if (l_oData.ParseString(p_strPrefix, p_pJson))
					{
						m_vecData.push_back(l_oData);
					}				
				}

				return true;
			}

			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}
				return "";
			}
		public:
			CHeaderEx m_oHeader;
			std::string m_strEndTime;
			std::string m_strCode;
			std::string m_strMessage;

			std::vector<CAddOrUpdateProcessRequest::CProcessData> m_vecData;
		};

		/////////////////////////////////////////////////
		class CVcsLogSynDataRespond :public IRequest, IRespond
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_strCode = p_pJson->GetNodeValue("/code", "");
				m_strMessage = p_pJson->GetNodeValue("/message", "");

				int l_iCount = p_pJson->GetCount("/data");
				for (int i = 0; i < l_iCount; i++)
				{
					std::string p_strPrefix("/data/" + std::to_string(i) + "/");
					PROTOCOL::CAlarmLogSync::CBody l_oData;

					l_oData.m_strID = p_pJson->GetNodeValue(p_strPrefix + "id", "");
					l_oData.m_strAlarmID = p_pJson->GetNodeValue(p_strPrefix + "alarm_id", "");
					l_oData.m_strProcessID = p_pJson->GetNodeValue(p_strPrefix + "process_id", "");
					l_oData.m_strFeedbackID = p_pJson->GetNodeValue(p_strPrefix + "feedback_id", "");
					l_oData.m_strOperate = p_pJson->GetNodeValue(p_strPrefix + "operate", "");
					l_oData.m_strOperateContent = p_pJson->GetNodeValue(p_strPrefix + "operate_content", "");
					l_oData.m_strFromType = p_pJson->GetNodeValue(p_strPrefix + "from_type", "");
					l_oData.m_strFromObject = p_pJson->GetNodeValue(p_strPrefix + "from_object", "");
					l_oData.m_strFromObjectName = p_pJson->GetNodeValue(p_strPrefix + "from_object_name", "");
					l_oData.m_strFromObjectOrgName = p_pJson->GetNodeValue(p_strPrefix + "from_object_org_name", "");
					l_oData.m_strFromObjectOrgCode = p_pJson->GetNodeValue(p_strPrefix + "from_object_org_code", "");

					l_oData.m_strToType = p_pJson->GetNodeValue(p_strPrefix + "to_type", "");
					l_oData.m_strToObject = p_pJson->GetNodeValue(p_strPrefix + "to_object", "");
					l_oData.m_strToObjectName = p_pJson->GetNodeValue(p_strPrefix + "to_object_name", "");
					l_oData.m_strToObjectOrgCode = p_pJson->GetNodeValue(p_strPrefix + "to_object_org_code", "");
					l_oData.m_strToObjectOrgName = p_pJson->GetNodeValue(p_strPrefix + "to_object_org_name", "");

					l_oData.m_strCreateUser = p_pJson->GetNodeValue(p_strPrefix + "create_user", "");
					l_oData.m_strCreateTime = p_pJson->GetNodeValue(p_strPrefix + "create_time", "");
					l_oData.m_strSourceName = p_pJson->GetNodeValue(p_strPrefix + "source_name", "");
					l_oData.m_strOperateAttachDesc = p_pJson->GetNodeValue(p_strPrefix + "operate_attach_desc", "");

					l_oData.m_strFromOrgIdentifier = p_pJson->GetNodeValue(p_strPrefix + "from_object_org_identifier", "");//new
					l_oData.m_strToObjectOrgIdentifier = p_pJson->GetNodeValue(p_strPrefix + "to_object_org_identifier", "");//new
					l_oData.m_strDescription = p_pJson->GetNodeValue(p_strPrefix + "description", "");  //new
					l_oData.m_strCreateOrg = p_pJson->GetNodeValue(p_strPrefix + "create_org", "");		//new

					l_oData.m_strDeptOrgCode = p_pJson->GetNodeValue(p_strPrefix + "dept_org_code", ""); //delete
					l_oData.m_strSeatNo = p_pJson->GetNodeValue(p_strPrefix + "seat_no", "");	//delete

					m_vecData.push_back(l_oData);
				}

				return true;
			}

			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}
				return "";
			}
		public:
			CHeaderEx m_oHeader;
			std::string m_strEndTime;
			std::string m_strCode;
			std::string m_strMessage;

			std::vector<PROTOCOL::CAlarmLogSync::CBody> m_vecData;
		};
		//////////////////////////////////////////////////////////////////////////////
		class CVcsBllStatusSynDataRespond :public IRequest, IRespond
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_strCode = p_pJson->GetNodeValue("/code", "");
				m_strMessage = p_pJson->GetNodeValue("/message", "");

				int l_iCount = p_pJson->GetCount("/data");
				for (int i = 0; i < l_iCount; i++)
				{
					std::string p_strPrefix("/data/" + std::to_string(i) + "/");
					CData l_oData;

					l_oData.m_strID = p_pJson->GetNodeValue(p_strPrefix + "id", "");

					l_oData.m_strAlarmID = p_pJson->GetNodeValue(p_strPrefix + "alarm_id", "");
					l_oData.m_strProcessID = p_pJson->GetNodeValue(p_strPrefix + "process_id", "");
					l_oData.m_strOwner = p_pJson->GetNodeValue(p_strPrefix + "owner", "");
					l_oData.m_strOwnerType = p_pJson->GetNodeValue(p_strPrefix + "owner_type", "");
					l_oData.m_strStatus = p_pJson->GetNodeValue(p_strPrefix + "status", "");
					l_oData.m_strUpdateTime = p_pJson->GetNodeValue(p_strPrefix + "update_time", "");
					l_oData.m_strCreateTime = p_pJson->GetNodeValue(p_strPrefix + "create_time", "");

					//l_oData.m_oAlarm.ParseString(p_strPrefix, p_pJson);
					m_oBody.m_vecData.push_back(l_oData);
				}

				return true;
			}

			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}
				return "";
			}
		public:
			CHeaderEx m_oHeader;
			std::string m_strCode;
			std::string m_strMessage;

			class CData
			{
			public:
				std::string m_strID;			//主键
				std::string m_strAlarmID;		//接警单编号
				std::string m_strProcessID;		//派警单编码
				std::string m_strOwner;			//状态所有者（警号或者单位代码）
				std::string m_strOwnerType;		//状态所有者类型Org、staff
				std::string m_strStatus;		//业务状态
				std::string m_strUpdateTime;	//更新时间
				std::string m_strCreateTime;	//创建时间
			};

			class CBody
			{
			public:
				std::vector<CData> m_vecData;
			};
			CBody m_oBody;
		};
	}
}
