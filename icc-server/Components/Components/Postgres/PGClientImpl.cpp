#include "Boost.h"
#include "pg_type.h"
#include "PGClientImpl.h"
#include "ResultSetImpl.h"

#define HAVE_NO_FREE_CONNECTION 100

/************************************************************************/
/* typedef enum
{
PGRES_EMPTY_QUERY = 0,      //empty query string was executed
PGRES_COMMAND_OK,           // a query command that doesn't return anything was executed properly by the backend
PGRES_TUPLES_OK,            // a query command that returns tuples was executed properly by the backend, PGresult contains the result tuples
PGRES_COPY_OUT,             // Copy Out data transfer in progress
PGRES_COPY_IN,              // Copy In data transfer in progress
PGRES_BAD_RESPONSE,         // an unexpected response was recv'd from the backend
PGRES_NONFATAL_ERROR,       // notice or warning message
PGRES_FATAL_ERROR,          // query failed
PGRES_COPY_BOTH,            // Copy In/Out data transfer in progress
PGRES_SINGLE_TUPLE          // single tuple from larger resultset
} ExecStatusType; */
/************************************************************************/

CPGClientImpl::CPGClientImpl(IResourceManagerPtr p_pResourceManager)
	: m_pResourceManager(p_pResourceManager)
{
	m_pLog = ICCGetILogFactory()->GetLogger(MODULE_NAME);
	m_pHelpTool = ICCGetHelpToolFactory()->CreateHelpTool();
	m_pString = ICCGetIStringFactory()->CreateString();

	int res = PQisthreadsafe();
	ICC_LOG_DEBUG(m_pLog, "PQisthreadsafe[%s]", (res == 1) ? "PQ is thread safe." : "PQ is not thread safe.");

	PostgresqlPool::Instance();
}

CPGClientImpl::~CPGClientImpl()
{

}

bool CPGClientImpl::ConnectServer()
{
	return PostgresqlPool::Instance()->Init(m_pResourceManager);
}

void CPGClientImpl::Close()
{
	PostgresqlPool::Instance()->CloseConnectionPool();
}

unsigned int CPGClientImpl::ExecQuery(std::string p_strSql, std::map<std::string, std::string>& p_mapFieldInfo, std::list<std::list<std::string>>& p_lstRes, bool bIsRead /* = false */, std::string strTransGuid /* = "" */)
{
	PooledConnectionPtr pPooledConnection = _GetConnection(bIsRead, strTransGuid);
	if (!pPooledConnection)
	{
		ICC_LOG_FATAL(m_pLog, "get freed db connection failed. not enable connection!");
		return HAVE_NO_FREE_CONNECTION;
	}
	
	PGconn* pConn = pPooledConnection->Get();
	if (pConn == nullptr)
	{
		ICC_LOG_FATAL(m_pLog, "get freed db connection failed.");
		return HAVE_NO_FREE_CONNECTION;
	}

	p_lstRes.clear();
	p_mapFieldInfo.clear();
	// Submits a command to the server and waits for the result.
	PGresult* l_pPgRes = PQexec(pConn, p_strSql.c_str());

	unsigned int l_uiExecStatusType = PQresultStatus(l_pPgRes);
	if (l_uiExecStatusType != PGRES_TUPLES_OK)
	{
		PQclear(l_pPgRes);
		return l_uiExecStatusType;
	}

	unsigned int l_uiRecordNum = PQntuples(l_pPgRes);
	unsigned int l_uiFieldNum = PQnfields(l_pPgRes);
	// p_mapFieldInfo is used to save fileld names and field type.
	std::list<std::string> l_lstFieldName;
	for (unsigned int j = 0; j < l_uiFieldNum; ++j)
	{
		const char* l_temp = PQfname(l_pPgRes, j);
		std::string l_strName = l_temp ? l_temp : "";
		Oid l_uiType = PQftype(l_pPgRes, j);
		p_mapFieldInfo[l_strName] = GetFieldType(l_uiType);
		l_lstFieldName.push_back(l_strName);
	}
	p_lstRes.push_back(l_lstFieldName);

	// p_strRes is used to save the query result,and its first element
	// is the field name of the result.
	for (unsigned int i = 0; i < l_uiRecordNum; ++i)
	{
		std::list<std::string> l_strResRecord;
		for (unsigned int j = 0; j < l_uiFieldNum; ++j)
		{
			const char* l_temp = PQgetvalue(l_pPgRes, i, j);
			std::string l_strFvalue = l_temp ? l_temp : "";
			l_strResRecord.push_back(l_strFvalue);
		}
		p_lstRes.push_back(l_strResRecord);
	}

	PQclear(l_pPgRes);	
	return l_uiExecStatusType;
}

IResultSetPtr CPGClientImpl::ExecQuery(std::string p_strSql, bool bIsRead /* = false */, std::string strTransGuid /* = "" */)
{
	PGresult* pRet = nullptr;
	PooledConnectionPtr pPooledConnection = _GetConnection(bIsRead, strTransGuid);
	if (!pPooledConnection)
	{
		std::string l_strErrorMsg = "get freed db connection failed. not enable connection";
		CResultSetImpl::PGResultPtr l_pPGReuslt(nullptr, PQclear);
		boost::shared_ptr<CResultSetImpl> l_pRSet = boost::make_shared<CResultSetImpl>(l_pPGReuslt);
		l_pRSet->SetErrorMsg(l_strErrorMsg);
		ICC_LOG_FATAL(m_pLog, l_strErrorMsg.c_str());
		return l_pRSet;
	}

	PGconn* pConn = pPooledConnection->Get();
	if (nullptr == pConn)
	{
		std::string l_strErrorMsg = "get freed db connection failed.";
		CResultSetImpl::PGResultPtr l_pPGReuslt(nullptr, PQclear);
		boost::shared_ptr<CResultSetImpl> l_pRSet = boost::make_shared<CResultSetImpl>(l_pPGReuslt);
		l_pRSet->SetErrorMsg(l_strErrorMsg);
		ICC_LOG_FATAL(m_pLog, l_strErrorMsg.c_str());

		return l_pRSet;
	}

	PGresult* l_pPgRes = PQexec(pConn, p_strSql.c_str());
	CResultSetImpl::PGResultPtr l_pPGReuslt(l_pPgRes, PQclear);
	boost::shared_ptr<CResultSetImpl> l_pRSet = boost::make_shared<CResultSetImpl>(l_pPGReuslt);
	l_pRSet->SetSQL(p_strSql);
	unsigned int l_uiExecStatusType = PQresultStatus(l_pPgRes);
	switch (l_uiExecStatusType)
	{
		case PGRES_TUPLES_OK:
		{
			l_pRSet->SetResultStatus(true);
			int l_iNTuples = PQntuples(l_pPgRes);
			if (l_iNTuples < 0)
			{
				ICC_LOG_ERROR(m_pLog, "PQ API PQntuples returned a negative value[%d],set recordsize = 0", l_iNTuples);
				l_iNTuples = 0;
			}
			unsigned int l_uiRecordNum = l_iNTuples;
			unsigned int l_uiFieldNum = PQnfields(l_pPgRes);
			l_pRSet->SetRecordSize(l_uiRecordNum);
			for (unsigned int j = 0; j < l_uiFieldNum; ++j)
			{
				const char* l_strName = PQfname(l_pPgRes, j);
				l_pRSet->AddFieldIndex(l_strName ? l_strName : "", j);
			}
			break;
		}
		case PGRES_COMMAND_OK:
		{
			l_pRSet->SetResultStatus(true);
			break;
		}
		case PGRES_FATAL_ERROR:
		{
			ICC_LOG_ERROR(m_pLog, "there is an fatal error with SQL or network connection, check the network connection by executing an absolutely correct SQL.");

			break;
		}
		case PGRES_BAD_RESPONSE:
		{
			break;
		}
		default:
		{
			l_pRSet->SetResultStatus(true);
			break;
		}
	}

	std::string l_strErrorMsg;
	const char* l_temp1 = PQerrorMessage(pConn);
	const char* l_temp2 = PQresultErrorMessage(l_pPgRes);
	l_strErrorMsg.append(l_temp1 ? l_temp1 : "no error").append(l_temp2 ? l_temp2 : "no result");
	l_pRSet->SetErrorMsg(l_strErrorMsg);	
	return l_pRSet;
}

unsigned int CPGClientImpl::ExecNoQuery(std::string p_strSql, bool bIsRead /* = false */, std::string strTransGuid /* = "" */)
{
	PGresult* pRet = nullptr;
	PooledConnectionPtr pPooledConnection = _GetConnection(bIsRead, strTransGuid);
	if (!pPooledConnection)
	{
		ICC_LOG_FATAL(m_pLog, "get freed db connection failed. not enable connection!");
		return HAVE_NO_FREE_CONNECTION;
	}

	PGconn* pConn = pPooledConnection->Get();
	if (nullptr == pConn)
	{
		ICC_LOG_FATAL(m_pLog, "get freed db connection failed.");
		return HAVE_NO_FREE_CONNECTION;
	}

	PGresult* l_pPgRes = PQexec(pConn, p_strSql.c_str());
	ExecStatusType rs = PQresultStatus(l_pPgRes);
	PQclear(l_pPgRes);	
	return rs;
}

std::string CPGClientImpl::BeginTransaction()
{
	PGresult* pRet = nullptr;
	PooledConnectionPtr pPooledConnection = PostgresqlPool::Instance()->GetFreeRWConnection();
	if (!pPooledConnection)
	{
		ICC_LOG_ERROR(m_pLog, "begin trans get freed rw db connection failed. not enable connection!");
		return "";
	}

	PGconn* pConn = pPooledConnection->Get();
	if (nullptr == pConn)
	{
		ICC_LOG_ERROR(m_pLog, "begin trans get freed rw db connection failed.");
		return "";
	}

	PGresult* l_pPgRes = PQexec(pConn, "begin;");
	ExecStatusType rs = PQresultStatus(l_pPgRes);
	if (PGRES_COMMAND_OK != rs)
	{
		PQclear(l_pPgRes);
		return "";
	}

	PQclear(l_pPgRes);
	std::string l_strGuid = m_pString->CreateGuid();
	{
		m_Mutex.lock();
		m_PooledConnectionMap[l_strGuid] = pPooledConnection;
		m_Mutex.unlock();
	}

	return l_strGuid;
}

unsigned int CPGClientImpl::Commit(std::string p_strGuid)
{
	PooledConnectionPtr pConn;
	{
		m_Mutex.lock();
		pConn = m_PooledConnectionMap[p_strGuid];
		m_Mutex.unlock();
	}

	if (nullptr == pConn)
	{
		ICC_LOG_FATAL(m_pLog, "commit transaction, get db connection failed.");
		return HAVE_NO_FREE_CONNECTION;
	}

	PGresult* l_pPgRes = PQexec(pConn->Get(), "commit;");
	ExecStatusType rs = PQresultStatus(l_pPgRes);
	PQclear(l_pPgRes);

	{
		m_Mutex.lock();
		m_PooledConnectionMap.erase(p_strGuid);
		m_Mutex.unlock();
	}

	ICC_LOG_DEBUG(m_pLog, "commit, current cash connections[%d].", m_PooledConnectionMap.size());
	return rs;
}

std::string CPGClientImpl::Savepoint(std::string p_strSavePoint)
{
	PGresult* pRet = nullptr;
	PooledConnectionPtr pPooledConnection = PostgresqlPool::Instance()->GetFreeRWConnection();
	if (!pPooledConnection)
	{
		ICC_LOG_FATAL(m_pLog, "get freed rw db connection failed. not enable connection!");
		return "";
	}

	PGconn* pConn = pPooledConnection->Get();
	if (nullptr == pConn)
	{
		ICC_LOG_FATAL(m_pLog, "get freed rw db connection failed.");
		return "";
	}

	std::string l_strExecSQL = "savepoint " + p_strSavePoint + ";";
	PGresult* l_pPgRes = PQexec(pConn, l_strExecSQL.c_str());
	ExecStatusType rs = PQresultStatus(l_pPgRes);
	if (PGRES_COMMAND_OK != rs)
	{
		PQclear(l_pPgRes);
		return "";
	}

	PQclear(l_pPgRes);
	std::string l_strGuid = m_pString->CreateGuid();
	{
		m_Mutex.lock();
		m_PooledConnectionMap[l_strGuid] = pPooledConnection;
		m_Mutex.unlock();
	}

	return l_strGuid;
}

unsigned int CPGClientImpl::Rollback(std::string p_strGuid, std::string p_strSavePoint /* = "" */)
{
	PooledConnectionPtr pConn;
	{
		m_Mutex.lock();
		pConn = m_PooledConnectionMap[p_strGuid];
		m_Mutex.unlock();
	}

	if (nullptr == pConn)
	{
		ICC_LOG_FATAL(m_pLog, "rollback transaction, get db connection failed.");
		return HAVE_NO_FREE_CONNECTION;
	}

	std::string l_strExecSQL = "rollback " + p_strSavePoint + ";";
	PGresult* l_pPgRes = PQexec(pConn->Get(), l_strExecSQL.c_str());
	ExecStatusType rs = PQresultStatus(l_pPgRes);
	PQclear(l_pPgRes);

	{
		m_Mutex.lock();
		m_PooledConnectionMap.erase(p_strGuid);
		m_Mutex.unlock();
	}

	ICC_LOG_DEBUG(m_pLog, "rollback, current cash connections[%d].", m_PooledConnectionMap.size());
	return rs;
}

std::string CPGClientImpl::GetFieldType(Oid p_uiType)
{
	std::string l_strType;
	switch (p_uiType)
	{
	case INT4OID:
		l_strType = "INT";
		break;
	case FLOAT4OID:
		l_strType = "FLOAT";
		break;
	case NUMERICOID:
		l_strType = "NUMER";
		break;
	case CHAROID:
		l_strType = "CHAR";
		break;
	case VARCHAROID:
		l_strType = "VARCHAR";
		break;
	case TEXTOID:
		l_strType = "TEXT";
		break;
	case BYTEAOID:
		l_strType = "BYTEA";
		break;
	case BOOLOID:
		l_strType = "BOOL";
		break;
	case TIMEOID:
		l_strType = "TIME";
		break;
	case DATEOID:
		l_strType = "DATE";
		break;
	case TIMESTAMPOID:
		l_strType = "TIMESTAMP";
		break;
	default:
		l_strType = "UNKNOWTYPE";
	}
	return l_strType;
}

IResourceManagerPtr CPGClientImpl::GetResourceManager()
{
	return m_pResourceManager;
}

PooledConnectionPtr CPGClientImpl::_GetConnection(bool bIsRead, const std::string& strTransGuid)
{
	PooledConnectionPtr pPooledConnection;
	if (bIsRead)
	{
		pPooledConnection = PostgresqlPool::Instance()->GetFreeRConnection();
	}
	else
	{
		if (strTransGuid.empty())
		{
			pPooledConnection = PostgresqlPool::Instance()->GetFreeRWConnection();
		}
		else
		{
			m_Mutex.lock();
			pPooledConnection = m_PooledConnectionMap[strTransGuid];
			m_Mutex.unlock();
		}
	}
	return pPooledConnection;
}
