#pragma once
#include <map>
#include <list>
#include <string>
#include <IObject.h>
#include "IResultSet.h"

namespace ICC
{
	namespace DataBase
	{
		class IPGClient :
			public IObject
		{
		public:
			// connect to database server
			virtual bool ConnectServer() = 0;

			// DEPRECATED
			virtual unsigned int ExecQuery(std::string p_strSql,
				std::map<std::string, std::string>& p_mapFieldInfo,
				std::list<std::list<std::string>>& p_strRes, bool bIsRead = false, std::string strTransGuid = "") = 0;
			// execute a query and return a result set
			virtual IResultSetPtr ExecQuery(std::string p_strSql, bool bIsRead = false, std::string strTransGuid = "") = 0;
			// execute a non-query sql(eg:create...,insert...,update...,delete...,drop...)
			virtual unsigned int ExecNoQuery(std::string p_strSql, bool bIsRead = false, std::string strTransGuid = "") = 0;
			// start a transaction block 
			virtual std::string BeginTransaction() = 0;
			// commit the transaction
			virtual unsigned int Commit(std::string p_strGuid) = 0;
			// save point
			virtual std::string Savepoint(std::string p_strSavePoint) = 0;
			// rollback the transaction
			virtual unsigned int Rollback(std::string p_strGuid, std::string p_strSavePoint = "") = 0;
			//
			virtual void Close() = 0;
		};

		typedef boost::shared_ptr<IPGClient> IPGClientPtr;
	}
}