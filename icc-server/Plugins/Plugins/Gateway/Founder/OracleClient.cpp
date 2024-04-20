#include "Boost.h"
#include "OracleClient.h"
#include "ResultSetCounter.h"

bool COracleClient::Init(Log::ILogPtr pLog, StringUtil::IStringUtilPtr pString, Config::IConfigPtr pConfig, HelpTool::IHelpToolPtr pHelpTool, 
	DataBase::ISqlBuilderPtr	pSqlBuilder, DataBase::ISqlRequestFactoryPtr pSqlRequestFactory)
{	
	m_pLog = pLog;
	m_pString = pString;
	m_pConfig = pConfig;
	m_pHelpTool = pHelpTool;
	m_pSqlBuilder = pSqlBuilder;
	m_pSqlReqeustFactory = pSqlRequestFactory;

	ICC_LOG_DEBUG(m_pLog, "init begin!");

	if (m_bIsInit)
	{
		ICC_LOG_DEBUG(m_pLog, "inited, not init again!");
		return true;
	}
	
	m_strIP = m_pConfig->GetValue("ICC/Component/DBConnEx/dbhost", "127.0.0.1");
	m_strPort = m_pConfig->GetValue("ICC/Component/DBConnEx/dbport", "5432");
	m_strDBName = m_pConfig->GetValue("ICC/Component/DBConnEx/dbname", "icc");
	m_strUserName = m_pConfig->GetValue("ICC/Component/DBConnEx/username", "icc");
	m_strPassWord = m_pConfig->GetValue("ICC/Component/DBConnEx/password", "icc");
	m_iInitNum = m_pString->ToUInt(m_pConfig->GetValue("ICC/Component/DBConnEx/connnum", "10"));
	m_iIncreatmentNum = m_pString->ToUInt(m_pConfig->GetValue("ICC/Component/DBConnEx/increasenum", "2"));
	m_iMaxNum = m_pString->ToUInt(m_pConfig->GetValue("ICC/Component/DBConnEx/maxnum", "20"));	

	ICC_LOG_DEBUG(m_pLog, "inited param: ip:%s; port:%s; dbname:%s; user:%s; maxnum:%d; initnum:%d; incnum:%d; ", m_strIP.c_str(), m_strPort.c_str(),
		m_strDBName.c_str(), m_strUserName.c_str(), m_iMaxNum, m_iInitNum, m_iIncreatmentNum);
	
	try
	{
		m_pEnvironment = Environment::createEnvironment("UTF8", "UTF8");
		if (m_pEnvironment == nullptr)
		{
			ICC_LOG_ERROR(m_pLog, "m_pEnvironment is null!!!");
			return false;
		}

		std::string strLang = m_pEnvironment->getNLSLanguage();

		ICC_LOG_ERROR(m_pLog, "nls language: %s", strLang.c_str());

		m_pConnectPool = m_pEnvironment->createStatelessConnectionPool(m_strUserName, m_strPassWord, m_pString->Format("%s:%s/%s", m_strIP.c_str(), m_strPort.c_str(), m_strDBName.c_str()), m_iMaxNum, m_iInitNum, m_iIncreatmentNum);
		if (m_pConnectPool == nullptr)
		{
			ICC_LOG_ERROR(m_pLog, "m_pConnectPool is null!!!");
			return false;
		}
		
	}	
	catch (SQLException ea)
	{
		int iCode = ea.getErrorCode();
		std::string strMessage = ea.getMessage();
		ICC_LOG_ERROR(m_pLog, "init failed. error = %s", ea.getMessage().c_str());
		return false;
	}

	m_bIsInit = true;

	ICC_LOG_DEBUG(m_pLog, "init complete!");

	return true;
}

void COracleClient::UnInit()
{
	ICC_LOG_DEBUG(m_pLog, "uninit begin! counter is %u!", CResultSetCounter::Instance()->GetCounter());
	try
	{	
		while (CResultSetCounter::Instance()->GetCounter() > 0)
		{
			m_pHelpTool->Sleep(100);
		}

		if (m_pEnvironment && m_pConnectPool)
		{
			m_pEnvironment->terminateStatelessConnectionPool(m_pConnectPool);
			m_pConnectPool = nullptr;
		}

		if (m_pEnvironment)
		{
			Environment::terminateEnvironment(m_pEnvironment);
			m_pEnvironment = nullptr;
		}		
	}
	catch (SQLException ea)
	{
		int iCode = ea.getErrorCode();
		std::string strMessage = ea.getMessage();
		ICC_LOG_ERROR(m_pLog, "uninit failed. error = %s", ea.getMessage().c_str());		
	}

	ICC_LOG_DEBUG(m_pLog, "uninit complete!");
}

bool COracleClient::ExecNoQuery(const std::string& strSql)
{
	if (m_pEnvironment == nullptr || m_pConnectPool == nullptr)
	{
		ICC_LOG_ERROR(m_pLog, "no init");
		return false;
	}

	oracle::occi::Connection* pConnection = nullptr;
	oracle::occi::Statement* pStmt = nullptr;
	try
	{
		pConnection = m_pConnectPool->getConnection();
		if (pConnection == nullptr)
		{
			ICC_LOG_ERROR(m_pLog, "not enough connection");
			return false;
		}

		pStmt = pConnection->createStatement();
		if (pStmt == nullptr)
		{
			ICC_LOG_ERROR(m_pLog, "create Statement failed!!");
			return false;
		}

		pStmt->setAutoCommit(true);

		int iRs = pStmt->executeUpdate(strSql);

		ICC_LOG_DEBUG(m_pLog, "update success! rs:[%d] sql:[%s]", iRs, strSql.c_str());

		pConnection->terminateStatement(pStmt);
		m_pConnectPool->releaseConnection(pConnection);			

		return true;
	}
	catch (SQLException ea)
	{
		int iCode = ea.getErrorCode();
		std::string strMessage = ea.getMessage();
		ICC_LOG_ERROR(m_pLog, "exec sql failed. error = %s, sql = [%s]", ea.getMessage().c_str(), strSql.c_str());

		if (pStmt)
		{
			pConnection->terminateStatement(pStmt);
		}

		if (pConnection)
		{
			m_pConnectPool->releaseConnection(pConnection);
		}
	}

	return false;
}

ResultSetPtr COracleClient::ExecQuery(const std::string& strSql)
{
	ResultSetPtr pReset = std::make_shared<CResultSet>();
	pReset->SetSQL(strSql);

	if (m_pEnvironment == nullptr || m_pConnectPool == nullptr)
	{
		ICC_LOG_ERROR(m_pLog, "no init");
		return pReset;
	}

	oracle::occi::Connection* pConnection = nullptr;
	oracle::occi::Statement* pStmt = nullptr;
	try
	{
		pConnection = m_pConnectPool->getConnection();
		if (pConnection == nullptr)
		{
			ICC_LOG_ERROR(m_pLog, "not enough connection");
			return pReset;
		}
		
		pReset->SetConnection(m_pConnectPool, pConnection);

		pStmt = pConnection->createStatement();
		if (pStmt == nullptr)
		{
			ICC_LOG_ERROR(m_pLog, "create Statement failed!!");
			return pReset;
		}

		pReset->SetStatement(pStmt);

		pStmt->setAutoCommit(true);

		pStmt->setSQL(strSql);
		oracle::occi::ResultSet* pRs = pStmt->executeQuery();
		if (pRs == nullptr)
		{
			ICC_LOG_DEBUG(m_pLog, "exec sql failed sql:[%s]", strSql.c_str());
			return pReset;
		}
		pReset->SetValid(true);
		pReset->SetResultSet(pRs);			

		return pReset;
	}
	catch (SQLException ea)
	{
		int iCode = ea.getErrorCode();
		std::string strMessage = ea.getMessage();
		ICC_LOG_ERROR(m_pLog, "exec sql failed. error = %s, sql = [%s]", ea.getMessage().c_str(), strSql.c_str());		

		return pReset;
	}

	return pReset;
}

ResultSetPtr COracleClient::Exec(DataBase::SQLRequest& p_oSqlReq)
{
	ResultSetPtr pReset = std::make_shared<CResultSet>();

	std::string strSql = _GenSql(p_oSqlReq);
	if (strSql.empty())
	{
		return pReset;
	}

	pReset->SetSQL(strSql);

	if (m_pEnvironment == nullptr || m_pConnectPool == nullptr)
	{
		ICC_LOG_ERROR(m_pLog, "no init");
		return pReset;
	}

	oracle::occi::Connection* pConnection = nullptr;
	oracle::occi::Statement* pStmt = nullptr;
	try
	{
		pConnection = m_pConnectPool->getConnection();
		if (pConnection == nullptr)
		{
			ICC_LOG_ERROR(m_pLog, "not enough connection");
			return pReset;
		}

		pReset->SetConnection(m_pConnectPool, pConnection);

		pStmt = pConnection->createStatement();
		if (pStmt == nullptr)
		{
			ICC_LOG_ERROR(m_pLog, "create Statement failed!!");
			return pReset;
		}

		pReset->SetStatement(pStmt);

		pStmt->setAutoCommit(true);

		oracle::occi::Statement::Status st = pStmt->execute(strSql);
		if (st == oracle::occi::Statement::RESULT_SET_AVAILABLE)
		{
			oracle::occi::ResultSet* pRs = pStmt->getResultSet();
			if (pRs == nullptr)
			{
				ICC_LOG_DEBUG(m_pLog, "exec sql failed sql:[%s]", strSql.c_str());
				return pReset;
			}
			pReset->SetValid(true);
			pReset->SetResultSet(pRs);
			
		}
		else if (oracle::occi::Statement::UPDATE_COUNT_AVAILABLE)
		{
			pReset->SetValid(true);
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "exec sql unknown result!! sql:[%s]", strSql.c_str());
		}	

		return pReset;
	}
	catch (SQLException ea)
	{
		int iCode = ea.getErrorCode();
		std::string strMessage = ea.getMessage();
		ICC_LOG_ERROR(m_pLog, "exec sql failed. error = %s, sql = [%s]", ea.getMessage().c_str(), strSql.c_str());		

		return pReset;
	}

	return pReset;
}

COracleClient::COracleClient() : m_iInitNum(2), m_iIncreatmentNum(1), m_iMaxNum(16),m_bIsInit(false)
{
	m_pEnvironment = nullptr;
	m_pConnectPool = nullptr;

}

COracleClient::~COracleClient(void)
{

}

std::string COracleClient::_GenSql(DataBase::SQLRequest& p_oSqlReq)
{
	if (!m_pSqlReqeustFactory)
	{
		return "";
	}

	DataBase::ISqlRequestPtr l_pSqlReq = m_pSqlReqeustFactory->CreateSqlRequest();
	l_pSqlReq->SetSqlID(p_oSqlReq.sql_id);
	l_pSqlReq->SetNewValParam(p_oSqlReq.set);
	l_pSqlReq->AddParam(p_oSqlReq.param);

	std::string l_strSQL;
	unsigned int l_uiRetCode = m_pSqlBuilder->BuildSql(l_pSqlReq, l_strSQL);
	if (DataBase::SQLBD_SUCCESS != l_uiRetCode)
	{//SQL语句构造失败，在结果集中设置相应错误消息
		ICC_LOG_ERROR(m_pLog, "build sql failed!! errcode:%u, sql_id:[%s]", l_uiRetCode, p_oSqlReq.sql_id.c_str());
		return "";
	}
	return _StandardSql(l_strSQL);
}

std::string COracleClient::_StandardSql(const std::string& strSql)
{
	std::string strTmp;
	int iIndex = strSql.size() - 1;
	for (int i = strSql.size() - 1; i >= 0; i--)
	{
		if (strSql.at(i) == ';')
		{
			iIndex = i;
		}
		else
		{
			break;
		}
	}

	if (iIndex > 0)
	{
		return strSql.substr(0, iIndex);
	}
	return "";
}
