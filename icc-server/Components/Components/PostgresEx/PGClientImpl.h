#pragma once

#include <PGClient/IPGClient.h>
#include <PGClient/IResultSet.h>
#include "PostgresPool.h"

namespace ICC
{
	namespace DataBase
	{
		class CPGClientExImpl :
			public IPGClient
		{
		public:
			CPGClientExImpl(IResourceManagerPtr p_pResourceManager);
			~CPGClientExImpl();

		public:
			virtual bool ConnectServer();
			// [DEPRECATED] execute a query sql(eg:select...)
			virtual unsigned int ExecQuery(std::string p_strSql,
				std::map<std::string, std::string>& p_mapFieldInfo,
				std::list<std::list<std::string>>& p_lstRes, bool bIsRead = false, std::string strTransGuid = "");
			virtual IResultSetPtr ExecQuery(std::string p_strSql, bool bIsRead = false, std::string strTransGuid = "");
			// execute a non-query sql(eg:create...,insert...,update...,delete...,drop...)
			virtual unsigned int ExecNoQuery(std::string p_strSql, bool bIsRead = false, std::string strTransGuid = "");
			// start a transaction block 
			virtual std::string BeginTransaction();
			// commit the transaction
			virtual unsigned int Commit(std::string p_strGuid);
			// save point
			virtual std::string Savepoint(std::string p_strSavePoint);
			// rollback the transaction
			virtual unsigned int Rollback(std::string p_strGuid, std::string p_strSavePoint = "");
			//
			virtual void Close();
		public:
			IResourceManagerPtr GetResourceManager();
		private:
			std::string GetFieldType(Oid p_uiType);
			PooledConnectionPtr _GetConnection(bool bIsRead, const std::string& strTransGuid);

		private:			
			Log::ILogPtr m_pLog;
			HelpTool::IHelpToolPtr m_pHelpTool;
			StringUtil::IStringUtilPtr m_pString;
			IResourceManagerPtr m_pResourceManager;
		private:
			boost::recursive_mutex m_Mutex;
			std::map<std::string, PooledConnectionPtr> m_PooledConnectionMap;
		};
	}
}