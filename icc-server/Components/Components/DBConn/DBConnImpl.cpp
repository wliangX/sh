#include "Boost.h"
#include "DBConnImpl.h"

CPGConnImpl::CPGConnImpl(IResourceManagerPtr p_pResourceManager):
	m_pResourceManager(p_pResourceManager)
{
	m_pDBClient = ICCGetIPGClientFactory()->CreatePGClient();
	m_pDateTime = ICCGetIDateTimeFactory()->CreateDateTime();
	m_pSQLBD = ICCGetISqlBuilderFactory()->CreateSqlBuilder();
	m_pRSetFty = ICCGetIResultSetFactory();
	m_pLog = ICCGetILogFactory()->GetLogger(MODULE_NAME);

	Connect();
}

CPGConnImpl::~CPGConnImpl()
{

}

bool CPGConnImpl::Connect()
{
	return m_pDBClient->ConnectServer();
}

void CPGConnImpl::Close()
{
	m_pDBClient->Close();
}

//************************************
// Method:    Exec
// FullName:  ICC::CPGConnImpl::Exec
// Access:    virtual public 
// Returns:   ICC::DataBase::IResultSetPtr
// Qualifier:
// Parameter: std::string p_strSql SQL语句字符串
//************************************
IResultSetPtr CPGConnImpl::Exec(std::string p_strSql, bool bIsRead /* = false */, std::string strTransGuid /* = "" */)
{
	boost::timer::cpu_timer l_oStart;

	DataBase::IResultSetPtr l_pRSet = m_pDBClient->ExecQuery(p_strSql, bIsRead, strTransGuid);

	//boost::timer::nanosecond_type l_dElapsed = l_oStart.elapsed().user; //得到的是ns的单位
	//const boost::timer::nanosecond_type oneSecond(1000000000LL); //默认SQL时间3s
	auto nanoseconds = boost::chrono::nanoseconds(l_oStart.elapsed().user);
	auto seconds = boost::chrono::duration_cast<boost::chrono::seconds>(nanoseconds);
	if (seconds.count() >= 1) //秒需要换成ns
	{//执行时间超过3s的SQL语句
		ICC_LOG_WARNING(m_pLog, "[SLOW SQL][%lld s][records:%d]sql:[%s]", seconds.count(), l_pRSet->RecordSize(), p_strSql.c_str());
	}
	else
	{
		//ICC_LOG_DEBUG(m_pLog, "[%.0f ms][records:%d]sql:[%s]", l_dElapsed * 1000, l_pRSet->RecordSize(), p_strSql.c_str());
	}
	return l_pRSet;
}

/*
 * 方法: CPGConnImpl::Exec
 * 功能: 根据用户传入的请求构造SQL语句执行并返回执行结果集
 * 参数: SQLRequest & p_oSqlReq
 * 返回: ICC::IResultSetPtr
 * 作者：[3/2/2018 t26150]
*/
IResultSetPtr CPGConnImpl::Exec(DataBase::SQLRequest& p_oSqlReq, bool bIsRead /* = false */, std::string strTransGuid /* = "" */)
{
	//将用户输入的SQLRequest类型的请求转为ISqlRequestPtr类型
	ISqlRequestPtr l_pSqlReq = ICCGetISqlRequestFactory()->CreateSqlRequest();
	l_pSqlReq->SetSqlID(p_oSqlReq.sql_id);
	l_pSqlReq->SetNewValParam(p_oSqlReq.set);
	l_pSqlReq->AddParam(p_oSqlReq.param);

	std::string l_strSQL;
	unsigned int l_uiRetCode = m_pSQLBD->BuildSql(l_pSqlReq, l_strSQL);	
	if (SQLBD_SUCCESS != l_uiRetCode)
	{//SQL语句构造失败，在结果集中设置相应错误消息
		IResultSetPtr l_pRSet = m_pRSetFty->CreateResultSet();
		l_pRSet->SetErrorMsg(m_pSQLBD->GetErrorMsg(l_uiRetCode) + "[sql_id:" + p_oSqlReq.sql_id + "]");
		return l_pRSet;
	}
	//SQL语句构造成功，执行并返回结果集
	return Exec(l_strSQL, bIsRead, strTransGuid);
}

std::string CPGConnImpl::BeginTransaction()
{
	return m_pDBClient->BeginTransaction();
}

unsigned int CPGConnImpl::Commit(std::string p_strGuid)
{
	return m_pDBClient->Commit(p_strGuid);
}

std::string CPGConnImpl::Savepoint(std::string p_strSavePoint)
{
	return m_pDBClient->Savepoint(p_strSavePoint);
}

unsigned int CPGConnImpl::Rollback(std::string p_strGuid, std::string p_strSavePoint /* = "" */)
{
	return m_pDBClient->Rollback(p_strGuid, p_strSavePoint);
}

ICC::IResourceManagerPtr CPGConnImpl::GetResourceManager()
{
	return m_pResourceManager;
}