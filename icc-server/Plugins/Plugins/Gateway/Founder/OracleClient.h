#pragma once

#include "occi.h"
using namespace oracle::occi;

#include <mutex>
#include "ResultSet.h"

namespace ICC
{
	class COracleClient
	{			
	public:
		//对外接口			
		
		bool                      Init(Log::ILogPtr pLog, StringUtil::IStringUtilPtr pString, Config::IConfigPtr pConfig, 
			HelpTool::IHelpToolPtr pHelpTool, DataBase::ISqlBuilderPtr	pSqlBuilder, DataBase::ISqlRequestFactoryPtr pSqlRequestFactory);
		bool                      ExecNoQuery(const std::string& strSql);
		ResultSetPtr              ExecQuery(const std::string& strSql);
		ResultSetPtr              Exec(DataBase::SQLRequest& p_oSqlReq);
		void                      UnInit();				

	public:
		COracleClient();
		~COracleClient(void);		

	private:
		std::string               _GenSql(DataBase::SQLRequest& p_oSqlReq);
		std::string               _StandardSql(const std::string& strSql);
	
	private:		
		Log::ILogPtr m_pLog;			
		StringUtil::IStringUtilPtr m_pString;		
		Config::IConfigPtr m_pConfig;
		HelpTool::IHelpToolPtr m_pHelpTool;
		DataBase::ISqlBuilderPtr	m_pSqlBuilder;
		DataBase::ISqlRequestFactoryPtr m_pSqlReqeustFactory;

	private://属性字段
		std::string		m_strIP;				//数据库IP
		std::string		m_strPort;				//数据库Port
		std::string		m_strDBName;			//数据库名
		std::string		m_strUserName;			//数据库用户名
		std::string		m_strPassWord;			//数据库用户密码

		unsigned int	m_iInitNum;				//连接池的初始化大小
		unsigned int	m_iIncreatmentNum;		//连接池自增长的大小
		unsigned int	m_iMaxNum;				//连接池的最大大小			
		bool			m_bIsInit;				//是否已初始化			
			
		oracle::occi::Environment* m_pEnvironment;
		oracle::occi::StatelessConnectionPool* m_pConnectPool;			
	};	

	typedef std::shared_ptr<COracleClient> OracleClientPtr;
}

