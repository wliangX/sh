#pragma once
#include "Boost.h"

/*
	ʹ�÷���
	1.ReadConfig����SQL����
	2.Connect�������ݿ������
	3.Eexcִ��SQL����
*/

namespace ICC
{
	class CPGConnExImpl :	public IDBConn
	{
	public:
		CPGConnExImpl(IResourceManagerPtr p_pIResourceManager);
		~CPGConnExImpl();
	
	public:
		// connect to database server
		virtual bool Connect();
		// close db connection
		virtual void Close();
		// execute a query and return a result set
		virtual IResultSetPtr Exec(std::string p_strSql, bool bIsRead = false, std::string strTransGuid = "");
		virtual IResultSetPtr Exec(DataBase::SQLRequest& p_oSqlReq, bool bIsRead = false, std::string strTransGuid = "");
		// start a transaction block 
		virtual std::string BeginTransaction();
		// commit the transaction
		virtual unsigned int Commit(std::string p_strGuid);
		// save point
		virtual std::string Savepoint(std::string p_strSavePoint);
		// rollback the transaction
		virtual unsigned int Rollback(std::string p_strGuid, std::string p_strSavePoint = "");
	
	public:
		IResourceManagerPtr GetResourceManager();
	
	private:
		Log::ILogPtr	m_pLog;
		IPGClientPtr	m_pDBClient;
		ISqlBuilderPtr	m_pSQLBD;
		DateTime::IDateTimePtr m_pDateTime;
		IResultSetFactoryPtr m_pRSetFty;
		IResourceManagerPtr m_pResourceManager;
	};
}