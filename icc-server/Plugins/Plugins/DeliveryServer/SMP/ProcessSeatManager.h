#pragma once
#include <Json/IJsonFactory.h>
#include "IBasicDataManager.h"

namespace ICC
{
	namespace Data
	{

		class CProcessSeat : public IBasicData
		{
		public:
			virtual std::string ToString(JsonParser::IJsonPtr p_pJson)
			{
				p_pJson->SetNodeValue("/guid", m_strGuid);
				p_pJson->SetNodeValue("/seats_guid", m_strSeatsGuid);
				p_pJson->SetNodeValue("/org_code", m_strOrgCode);
				p_pJson->SetNodeValue("/org_name", m_strOrgName);
				p_pJson->SetNodeValue("/telephone", m_strTelephone);
				p_pJson->SetNodeValue("/ip", m_strIp);

				return p_pJson->ToString();
			}

			virtual bool Parse(const std::string& strJson, JsonParser::IJsonPtr pJson)
			{
				return true;
			}

		public:
			CProcessSeat(){}
			~CProcessSeat(){}

		public:			
			std::string m_strSeatsGuid;
			std::string m_strOrgCode;
			std::string m_strOrgName;
			std::string m_strTelephone;
			std::string m_strIp;
		};	
		typedef boost::shared_ptr<CProcessSeat> ProcessSeatPtr;

		class CProcessSeatManager : public IBasicDataManager
		{
		public:
			CProcessSeatManager(Log::ILogPtr pLog, StringUtil::IStringUtilPtr  pString, DataBase::IDBConnPtr pDBConn, 
				JsonParser::IJsonFactoryPtr pJsonFty, DateTime::IDateTimePtr pDateTime);
			virtual ~CProcessSeatManager();

		protected:
			virtual IBasicDataPtr _CreateBasicDataPtr();
			virtual bool _LoadDataFromDb(std::map<std::string, IBasicDataPtr>& mapSources);
			virtual bool _AddData(const std::vector<IBasicDataPtr>& vecAdds);
			virtual bool _UpdateData(const std::vector<IBasicDataPtr>& vecUpdates);
			virtual bool _DeleteData(const std::vector<IBasicDataPtr>& vecDeletes);
			virtual bool _ParseData(const std::string& strData, std::map<std::string, IBasicDataPtr>& mapInputs);

		private:			
			Log::ILogPtr m_pLog;
			StringUtil::IStringUtilPtr  m_pString;			
			DataBase::IDBConnPtr m_pDBConn;
			JsonParser::IJsonFactoryPtr m_pJsonFty;		
			DateTime::IDateTimePtr    m_pDateTime;
		};

		
	}

};