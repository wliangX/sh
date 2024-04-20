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
		//����ӿ�			
		
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

	private://�����ֶ�
		std::string		m_strIP;				//���ݿ�IP
		std::string		m_strPort;				//���ݿ�Port
		std::string		m_strDBName;			//���ݿ���
		std::string		m_strUserName;			//���ݿ��û���
		std::string		m_strPassWord;			//���ݿ��û�����

		unsigned int	m_iInitNum;				//���ӳصĳ�ʼ����С
		unsigned int	m_iIncreatmentNum;		//���ӳ��������Ĵ�С
		unsigned int	m_iMaxNum;				//���ӳص�����С			
		bool			m_bIsInit;				//�Ƿ��ѳ�ʼ��			
			
		oracle::occi::Environment* m_pEnvironment;
		oracle::occi::StatelessConnectionPool* m_pConnectPool;			
	};	

	typedef std::shared_ptr<COracleClient> OracleClientPtr;
}

