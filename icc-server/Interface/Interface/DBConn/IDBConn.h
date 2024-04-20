#pragma once
#include <map>
#include <string>
#include <IObject.h>
#include <PGClient/IResultSet.h>
namespace ICC
{
	namespace DataBase
	{
		typedef struct SQLRequest_t
		{
			std::string sql_id;
			std::map<std::string, std::string> set;	//update set子句参数
			std::map<std::string, std::string> param;
		} SQLRequest;

		class IDBConn :
			public IObject
		{
		public:
			// connect to database server
			virtual bool Connect() = 0;
			// close db connection
			virtual void Close() = 0;
			// execute a query and return a result set
			virtual IResultSetPtr Exec(std::string p_strSql, bool bIsRead = false, std::string strTransGuid = "") = 0;
			virtual IResultSetPtr Exec(SQLRequest& p_oSqlReq, bool bIsRead = false, std::string strTransGuid = "") = 0;
			// start a transaction block 
			virtual std::string BeginTransaction() = 0;
			// commit the transaction
			virtual unsigned int Commit(std::string p_strGuid) = 0;
			// save point
			virtual std::string Savepoint(std::string p_strSavePoint) = 0;
			// rollback the transaction
			virtual unsigned int Rollback(std::string p_strGuid, std::string p_strSavePoint = "") = 0;
		};
		typedef boost::shared_ptr<IDBConn> IDBConnPtr;
	}
}