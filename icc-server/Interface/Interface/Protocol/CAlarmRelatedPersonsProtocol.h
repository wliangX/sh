#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>
//#include <Protocol/AlarmStructDefine.h>

namespace ICC
{
    namespace PROTOCOL
    {
		const std::string Ex_Persons_create_time = "create_time";
		const std::string Ex_Persons_create_user = "create_user";
		const std::string Ex_Persons_update_time = "update_time";
		const std::string Ex_Persons_update_user = "update_user";

		const std::string Ex_Persons_JQDSRDBH = "id";

		class CAlarmRelatedPersonsInfoEx
		{
		public:
			void ParseString(const std::string& strPre, JsonParser::IJsonPtr p_pJson)
			{		
				m_mapInfo[Ex_Persons_JQDSRDBH] = p_pJson->GetNodeValue(strPre + Ex_Persons_JQDSRDBH, "");
				m_mapInfo["id"] = p_pJson->GetNodeValue(strPre + "id","");
				m_mapInfo["alarm_id"] = p_pJson->GetNodeValue(strPre + "alarm_id", "");
				m_mapInfo["process_id"] = p_pJson->GetNodeValue(strPre + "process_id", "");
				m_mapInfo["feedback_id"] = p_pJson->GetNodeValue(strPre + "feedback_id", "");
				m_mapInfo["name"] = p_pJson->GetNodeValue(strPre + "name", "");
				m_mapInfo["sex"] = p_pJson->GetNodeValue(strPre + "sex", "");
				m_mapInfo["identification_type"] = p_pJson->GetNodeValue(strPre + "identification_type", "");
				m_mapInfo["identification_id"] = p_pJson->GetNodeValue(strPre + "identification_id", "");
				m_mapInfo["identity"] = p_pJson->GetNodeValue(strPre + "identity", "");
				m_mapInfo["is_focus_pesron"] = p_pJson->GetNodeValue(strPre + "is_focus_pesron", "");
				m_mapInfo["focus_pesron_info"] = p_pJson->GetNodeValue(strPre + "focus_pesron_info", "");
				m_mapInfo["registered_address_division"] = p_pJson->GetNodeValue(strPre + "registered_address_division", "");
				m_mapInfo["registered_address"] = p_pJson->GetNodeValue(strPre + "registered_address", "");
				m_mapInfo["current_address_division"] = p_pJson->GetNodeValue(strPre + "current_address_division", "");
				m_mapInfo["current_address"] = p_pJson->GetNodeValue(strPre + "current_address", "");
				m_mapInfo["work_unit"] = p_pJson->GetNodeValue(strPre + "work_unit", "");
				m_mapInfo["occupation"] = p_pJson->GetNodeValue(strPre + "occupation", "");
				m_mapInfo["contact_number"] = p_pJson->GetNodeValue(strPre + "contact_number", "");
				m_mapInfo["other_certificates"] = p_pJson->GetNodeValue(strPre + "other_certificates", "");
				m_mapInfo["lost_item_information"] = p_pJson->GetNodeValue(strPre + "lost_item_information", "");
				m_mapInfo["date_of_birth"] = p_pJson->GetNodeValue(strPre + "date_of_birth", "");
				
				m_mapInfo["create_time"] = p_pJson->GetNodeValue(strPre + "create_time", "");
				m_mapInfo["update_time"] = p_pJson->GetNodeValue(strPre + "update_time", "");
				m_mapInfo["digital_signature"] = p_pJson->GetNodeValue(strPre + "digital_signature", "");
				m_mapInfo["person_tag"] = p_pJson->GetNodeValue(strPre + "person_tag", "");

				m_mapInfo["createTeminal"] = p_pJson->GetNodeValue(strPre + "createTeminal", "");
				m_mapInfo["updateTeminal"] = p_pJson->GetNodeValue(strPre + "updateTeminal", "");
			}

			void ToString(const std::string& strPre, JsonParser::IJsonPtr p_pJson)
			{
				std::map<std::string, std::string>::const_iterator itr_const;
				for (itr_const = m_mapInfo.begin(); itr_const != m_mapInfo.end(); ++itr_const)
				{
					p_pJson->SetNodeValue(strPre + itr_const->first, itr_const->second);
				}
			}

		public:
			std::map<std::string, std::string> m_mapInfo;
		};
		////////////////////////////////////////////////////////////////////
		//与VCS交互的 增加
		class CAlarmRelatedPersonsAddExRequest :	public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_alarmRelatedPersonsInfo.ParseString("/body/", p_pJson);
				
				return true;
			}			

		public:
			class CBody
			{
			public:
				CAlarmRelatedPersonsInfoEx m_alarmRelatedPersonsInfo;
			};
			CHeaderEx m_oHeader;
			CBody	m_oBody;
		};
		
		//删除
		class CAlarmRelatedPersonsDeleteExRequest :	public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_alarmRelatedPersonsInfo.ParseString("/body/", p_pJson);
				
				return true;
			}			

		public:

			class CBody
			{
			public:
				CAlarmRelatedPersonsInfoEx m_alarmRelatedPersonsInfo;
			};
			CHeaderEx m_oHeader;
			CBody	m_oBody;
		};

		//修改
		class CAlarmRelatedPersonsUpdateExRequest :public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_alarmRelatedPersonsInfo.ParseString("/body/", p_pJson);

				return true;
			}
		public:

			class CBody
			{
			public:
				CAlarmRelatedPersonsInfoEx m_alarmRelatedPersonsInfo;
			};
			CHeaderEx m_oHeader;
			CBody	m_oBody;
		};
		
		//增、删、改Respond 
		class CAlarmRelatedPersonsChangeRespond :  public IRespond
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

		//增、删、改同步
		class CAlarmRelatedPersonsChangeSync : public IRespond
		{
		public:

			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_oBody.m_strSyncType = p_pJson->GetNodeValue("/body/sync_type", "");
				m_oBody.m_alarmRelatedPersonsInfo.ParseString("/body/", p_pJson);

				return true;
			}

			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return "";
				}

				m_oHeader.SaveTo(p_pJson);
				if (!m_oBody.m_strSyncType.empty())
				{
					p_pJson->SetNodeValue("/body/sync_type", m_oBody.m_strSyncType);
				}
				m_oBody.m_alarmRelatedPersonsInfo.ToString("/body/", p_pJson);
				return p_pJson->ToString();
			}
		public:
			CHeaderEx m_oHeader;
			class CBody
			{
			public:
				CAlarmRelatedPersonsInfoEx m_alarmRelatedPersonsInfo;
				std::string m_strSyncType;
			};
			CBody m_oBody;
		};
		////////////////////////////////////////////////////////////////////////////
		//与VCS交互的,查询与查询反馈
		class CAlarmRelatedPersonsQueryExRequest :	public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				//m_oBody.m_alarmRelatedCarsInfo.ParseString("/body/", p_pJson);
				m_oBody.m_strAlarmID = p_pJson->GetNodeValue("/body/alarm_id", "");
				return true;
			}			

		public:

			class CBody
			{
			public:
				std::string m_strAlarmID;
			};
			CHeaderEx m_oHeader;
			CBody	m_oBody;
		};
		
		class CAlarmRelatedPersonsQueryExRespond :  public IRespond
        {
        public:
            virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
            {
				if (nullptr == p_pJson)
				{
					return "";
				}

                m_oHeader.SaveTo(p_pJson);
				if (!m_oBody.m_strResult.empty())
				{
					p_pJson->SetNodeValue("/body/result", m_oBody.m_strResult);
				}
				for (size_t i = 0; i < m_oBody.m_vecAlarmRelatedPersonsInfos.size(); i++)
				{
					std::string l_strValueNum = std::to_string(i);
					m_oBody.m_vecAlarmRelatedPersonsInfos[i].ToString("/body/data/" + l_strValueNum + "/", p_pJson);
				}
                return p_pJson->ToString();
            }

        public:
            CHeaderEx m_oHeader;
            class CBody
            {
            public:
                std::string m_strResult;
                std::vector<CAlarmRelatedPersonsInfoEx> m_vecAlarmRelatedPersonsInfos;
            };
            CBody m_oBody;
        };

		class CAlarmRelatedPersonsSearchExRequest : public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_oBody.m_strPageSize = p_pJson->GetNodeValue("/body/page_size", "");
				m_oBody.m_strPageIndex = p_pJson->GetNodeValue("/body/page_index", "");

				m_oBody.m_strStartTime = p_pJson->GetNodeValue("/body/start_time", "");
				m_oBody.m_strEndTime = p_pJson->GetNodeValue("/body/end_time", "");
				return true;
			}

		public:

			class CBody
			{
			public:
				std::string m_strPageSize;
				std::string m_strPageIndex;

				std::string m_strStartTime;
				std::string m_strEndTime;
			};
			CHeaderEx m_oHeader;
			CBody	m_oBody;
		};
    }
}

