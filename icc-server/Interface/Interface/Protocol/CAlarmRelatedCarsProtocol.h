#pragma once
#include <Protocol/CHeader.h>
#include <Protocol/IRequest.h>
//#include <Protocol/AlarmStructDefine.h>

namespace ICC
{
    namespace PROTOCOL
    {
		const std::string Ex_Cars_JQSACLBH = "id";
		const std::string Ex_Cars_JJDBH = "alarm_id";
		const std::string Ex_Cars_car_type = "car_type";
		const std::string Ex_Cars_car_user_name = "car_user_name";
		const std::string Ex_Cars_car_user_phone = "car_user_phone";		
		const std::string Ex_Cars_car_user_number= "car_user_number";
		const std::string Ex_Cars_car_number = "car_number";
		const std::string Ex_Cars_create_time = "create_time";
		const std::string Ex_Cars_update_time = "update_time";

		const std::string Ex_Cars_createTeminal = "createTeminal";
		const std::string Ex_Cars_updateTeminal = "updateTeminal";
		class CAlarmRelatedCarsInfoEx
		{
		public:
			void ParseString(const std::string& strPre, JsonParser::IJsonPtr p_pJson)
			{
				m_mapInfo[Ex_Cars_JQSACLBH] = p_pJson->GetNodeValue(strPre + Ex_Cars_JQSACLBH, "");				
				m_mapInfo[Ex_Cars_JJDBH] = p_pJson->GetNodeValue(strPre + Ex_Cars_JJDBH, "");
	
				m_mapInfo["is_hazardous_vehicle"] = p_pJson->GetNodeValue(strPre + "is_hazardous_vehicle", "0");
				m_mapInfo["deleted"] = p_pJson->GetNodeValue(strPre + "deleted", "0");

				m_mapInfo[Ex_Cars_car_number] = p_pJson->GetNodeValue(strPre + Ex_Cars_car_number, "");				
				m_mapInfo[Ex_Cars_car_type] = p_pJson->GetNodeValue(strPre + Ex_Cars_car_type, "");				
				m_mapInfo[Ex_Cars_car_user_name] = p_pJson->GetNodeValue(strPre + Ex_Cars_car_user_name, "");				
				m_mapInfo[Ex_Cars_car_user_phone] = p_pJson->GetNodeValue(strPre + Ex_Cars_car_user_phone, "");
				m_mapInfo[Ex_Cars_car_user_number] = p_pJson->GetNodeValue(strPre + Ex_Cars_car_user_number, "");			
				m_mapInfo[Ex_Cars_create_time] = p_pJson->GetNodeValue(strPre + Ex_Cars_create_time, "");
				m_mapInfo[Ex_Cars_update_time] = p_pJson->GetNodeValue(strPre + Ex_Cars_update_time, "");

				m_mapInfo[Ex_Cars_createTeminal] = p_pJson->GetNodeValue(strPre + Ex_Cars_createTeminal, "");
				m_mapInfo[Ex_Cars_updateTeminal] = p_pJson->GetNodeValue(strPre + Ex_Cars_updateTeminal, "");

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
		class CAlarmRelatedCarsAddExRequest :	public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_alarmRelatedCarsInfo.ParseString("/body/", p_pJson);
				
				return true;
			}			

		public:
			class CBody
			{
			public:
				CAlarmRelatedCarsInfoEx m_alarmRelatedCarsInfo;
			};
			CHeaderEx m_oHeader;
			CBody	m_oBody;
		};
		
		//删除
		class CAlarmRelatedCarsDeleteExRequest :	public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_alarmRelatedCarsInfo.ParseString("/body/", p_pJson);
				
				return true;
			}			

		public:

			class CBody
			{
			public:
				CAlarmRelatedCarsInfoEx m_alarmRelatedCarsInfo;
			};
			CHeaderEx m_oHeader;
			CBody	m_oBody;
		};

		//修改
		class CAlarmRelatedCarsUpdateExRequest :public IRequest
		{
		public:
			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}

				m_oBody.m_alarmRelatedCarsInfo.ParseString("/body/", p_pJson);

				return true;
			}
		public:

			class CBody
			{
			public:
				CAlarmRelatedCarsInfoEx m_alarmRelatedCarsInfo;
			};
			CHeaderEx m_oHeader;
			CBody	m_oBody;
		};
		
		//增、删、改Respond 
		class CAlarmRelatedCarsChangeRespond :  public IRespond
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
		class CAlarmRelatedCarsChangeSync : public IRespond
		{
		public:

			virtual bool ParseString(std::string p_strReq, JsonParser::IJsonPtr p_pJson)
			{
				if (!m_oHeader.ParseString(p_strReq, p_pJson))
				{
					return false;
				}
				m_oBody.m_strSyncType = p_pJson->GetNodeValue("/body/sync_type", "");
				m_oBody.m_alarmRelatedCarsInfo.ParseString("/body/", p_pJson);

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
				m_oBody.m_alarmRelatedCarsInfo.ToString("/body/", p_pJson);
				return p_pJson->ToString();
			}
		public:
			CHeaderEx m_oHeader;
			class CBody
			{
			public:
				CAlarmRelatedCarsInfoEx m_alarmRelatedCarsInfo;
				std::string m_strSyncType;
			};
			CBody m_oBody;
		};
		////////////////////////////////////////////////////////////////////////////
		//与VCS交互的,查询与查询反馈
		class CAlarmRelatedCarsQueryExRequest :	public IRequest
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

		class CAlarmRelatedCarsSearchExRequest : public IRequest
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

				m_oBody.m_strStartTime = p_pJson->GetNodeValue("/body/begin_time", "");
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
		
		class CAlarmRelatedCarsQueryExRespond :  public IRespond
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
				for (size_t i = 0; i < m_oBody.m_vecAlarmRelatedCarsInfos.size(); i++)
				{
					std::string l_strValueNum = std::to_string(i);
					m_oBody.m_vecAlarmRelatedCarsInfos[i].ToString("/body/data/" + l_strValueNum + "/", p_pJson);
				}
                return p_pJson->ToString();
            }

        public:
            CHeaderEx m_oHeader;
            class CBody
            {
            public:
                std::string m_strResult;
                std::vector<CAlarmRelatedCarsInfoEx> m_vecAlarmRelatedCarsInfos;
            };
            CBody m_oBody;
        };

		class CAlarmRelatedCarsSearchExRespond : public IRespond
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
				for (size_t i = 0; i < m_oBody.m_vecAlarmRelatedCarsInfos.size(); i++)
				{
					std::string l_strValueNum = std::to_string(i);
					m_oBody.m_vecAlarmRelatedCarsInfos[i].ToString("/body/data/" + l_strValueNum + "/", p_pJson);
				}
				return p_pJson->ToString();
			}

		public:
			CHeaderEx m_oHeader;
			class CBody
			{
			public:
				std::string m_strResult;
				std::vector<CAlarmRelatedCarsInfoEx> m_vecAlarmRelatedCarsInfos;
			};
			CBody m_oBody;
		};
		
    }
}

