#include "Boost.h"
#include "PostgresPool.h"

PostgresqlPool* PostgresqlPool::sm_instance = nullptr;
boost::mutex PostgresqlPool::sm_mutexInstance;

PostgresqlPool* PostgresqlPool::Instance()
{
	if (nullptr == sm_instance)
	{
		boost::lock_guard<boost::mutex> lock(sm_mutexInstance);
		if (nullptr == sm_instance)
		{
			sm_instance = new PostgresqlPool();
		}
	}
	return sm_instance;

}

bool PostgresqlPool::Init(IResourceManagerPtr p_pResourceManager)
{
	if (m_bIsInit)
	{
		return true;
	}

	m_pResourceManager = p_pResourceManager;
	m_pLog = ICCGetILogFactory()->GetLogger(MODULE_NAME);
	m_lock = ICCGetILockFactory()->CreateLock(Lock::TypeRecursiveMutex);
	m_lockRW = ICCGetILockFactory()->CreateLock(Lock::TypeMutex);
	m_lockR = ICCGetILockFactory()->CreateLock(Lock::TypeMutex);
	m_lockMasterHostIp = ICCGetILockFactory()->CreateLock(Lock::TypeMutex);
	m_lockEnableHostNum = ICCGetILockFactory()->CreateLock(Lock::TypeMutex);
	m_pString = ICCGetIStringFactory()->CreateString();
	m_pHelpTool = ICCGetHelpToolFactory()->CreateHelpTool();

	Config::IConfigPtr l_pConfig = ICCGetIConfigFactory()->CreateConfig();
	m_strDBName = l_pConfig->GetValue("ICC/Component/DBConnEx/dbname", "icc");
	m_iInitNum = m_pString->ToUInt(l_pConfig->GetValue("ICC/Component/DBConnEx/connnum", "10"));
	m_iIncreatmentNum = m_pString->ToUInt(l_pConfig->GetValue("ICC/Component/DBConnEx/increasenum", "2"));
	m_iMaxNum = m_pString->ToUInt(l_pConfig->GetValue("ICC/Component/DBConnEx/maxnum", "20"));

	//先从公共配置中读取
	int nPubHostCount = l_pConfig->GetPubicNodeCount("ICC/Component/DBConnEx/Servers", "Server");
	bool bReadDBPubCfgFlag = false;
	if (nPubHostCount > 0)
	{
		for (int i = 0; i < nPubHostCount; ++i)
		{
			std::string l_strPath = m_pString->Format("PublicConfig/Database/PG[%d]", i);
			std::string strHost = l_pConfig->GetPublicValue(0, l_strPath + "/Host", "");
			std::string strPort = l_pConfig->GetPublicValue(0, l_strPath + "/Port", "");
			m_strUserName = l_pConfig->GetPublicValue(0, l_strPath + "/User", "");
			m_strPassWord = l_pConfig->GetPublicValue(0, l_strPath + "/Password", "");
			ICC_LOG_DEBUG(m_pLog, "postgresql read public config,Host=%s,Port=%s,nPubHostCount=%d", strHost.c_str(), strPort.c_str(), nPubHostCount);
			HostInfo info;
			if (!strHost.empty() && !strPort.empty())
			{
				bReadDBPubCfgFlag = true;
				info.strIp = strHost;
				info.strPort = strPort;
				m_vecHostInfos.push_back(info);
			}
		}

		ICC_LOG_DEBUG(m_pLog, "postgresql read public config,UserName=%s,PassWord=%s", m_strUserName.c_str(), m_strPassWord.c_str());

		if (m_strUserName.empty() || m_strPassWord.empty())
		{
			m_strUserName = l_pConfig->GetValue("ICC/Component/DBConn/username", "postgres");
			m_strPassWord = l_pConfig->GetValue("ICC/Component/DBConn/password", "Command_123");
		}
	}

	//公共配置没有读到从原配置文件读取
	if (!bReadDBPubCfgFlag)
	{
		int iHostCount = l_pConfig->GetNodeCount("ICC/Component/DBConnEx/Servers", "Server");
		for (int i = 0; i < iHostCount; ++i)
		{
			std::string strPath = m_pString->Format("ICC/Component/DBConnEx/Servers/Server[%d]", i);
			std::string strHost = l_pConfig->GetValue(strPath + "/dbhost", "");
			std::string strPort = l_pConfig->GetValue(strPath + "/dbport", "");

			ICC_LOG_DEBUG(m_pLog, "postgresql read config,Host=%s,Port=%s", strHost.c_str(), strPort.c_str());
			HostInfo info;
			if (!strHost.empty() && !strPort.empty())
			{
				info.strIp = strHost;
				info.strPort = strPort;
				m_vecHostInfos.push_back(info);
			}
		}

		m_strUserName = l_pConfig->GetValue("ICC/Component/DBConn/username", "postgres");
		m_strPassWord = l_pConfig->GetValue("ICC/Component/DBConn/password", "Command_123");
	}


	DateTime::CDateTime dt = ICCGetIDateTimeFactory()->CreateDateTime()->CurrentDateTime();
	m_bIsConnected = CreateConnections();//初始化连接对象
	if (!m_bIsConnected)
	{
		ICC_LOG_DEBUG(m_pLog, "postgresql pool init failed.");
		int iCount = 0;
		while (1)
		{
			ICC_LOG_DEBUG(m_pLog, "postgresql pool init failed. will reconnect. reconnect count [%d]", ++iCount);
			boost::this_thread::sleep(boost::posix_time::millisec(3000));
			_CloseConnections();
			m_bIsConnected = CreateConnections();
			if (m_bIsConnected)
			{
				break;
			}
		}

	}

	std::string strCurrentMasterHostIp = _QueryCurrentMasterHostIp(m_vecHostInfos, m_strUserName, m_strPassWord, m_strDBName);
	_SetMasterHostIp(strCurrentMasterHostIp);
	int iEnableNum = _CurrentEnableHostNum(m_vecHostInfos, m_strUserName, m_strPassWord, m_strDBName);
	_SetEnableHostNum(iEnableNum);

	DateTime::CDateTime dtcur = ICCGetIDateTimeFactory()->CreateDateTime()->CurrentDateTime();

	m_pCheckMainDBThread = boost::make_shared<boost::thread>(boost::bind(&PostgresqlPool::S_CheckDBPool, this));

	ICC_LOG_DEBUG(m_pLog, "postgresql pool init complete. createtime = %d", dtcur - dt);

	m_bIsInit = true;

	return true;
}

PostgresqlPool::PostgresqlPool() : m_bIsConnected(false), m_bIsInit(false), m_iInitNum(5), m_iIncreatmentNum(2), m_iMaxNum(20)
{
	m_iEnableHostNum = 0;
	m_bRWConnectionInitFlag = false;
	m_bRConnectionInitFlag = false;
}

//bool PostgresqlPool::IsConnected()
//{
//	Lock::AutoLock l_lock(m_lock);
//	return m_bIsConnected;
//}

//线程入口
void PostgresqlPool::S_CheckDBPool(PostgresqlPool* p_pThread)
{
	p_pThread->CheckDBPool(p_pThread);
}

void PostgresqlPool::CheckDBPool(PostgresqlPool* p_pThread)
{
	ICC_LOG_DEBUG(m_pLog, "CheckDBPool begin!!!");

	PostgresqlPool* pthis = (PostgresqlPool*)p_pThread;
	while (1)
	{
		ICC_LOG_LOWDEBUG(m_pLog, "query master host ip begin!!!");
		std::string strCurrentMasterHostIp = pthis->_QueryCurrentMasterHostIp(pthis->m_vecHostInfos, pthis->m_strUserName, pthis->m_strPassWord, pthis->m_strDBName);
		ICC_LOG_LOWDEBUG(m_pLog, "get master host ip begin!!! current master ip = %s", strCurrentMasterHostIp.c_str());
		std::string strSavedMasterHostIp = pthis->_MasterHostIp();
		if (strCurrentMasterHostIp != strSavedMasterHostIp)
		{
			ICC_LOG_DEBUG(m_pLog, "master host ip is modify!!! current master host is %s, old master host is %s", strCurrentMasterHostIp.c_str(), strSavedMasterHostIp.c_str());

			pthis->_SetMasterHostIp(strCurrentMasterHostIp);

			ICC_LOG_LOWDEBUG(m_pLog, "set master host ip complete, will update r connection!!!");

			pthis->_UpdateRConnections();
		}
		else
		{
			ICC_LOG_LOWDEBUG(m_pLog, "master host ip is not modify!!! current master host is %s", strCurrentMasterHostIp.c_str());

			int iEnableNum = pthis->_CurrentEnableHostNum(pthis->m_vecHostInfos, pthis->m_strUserName, pthis->m_strPassWord, pthis->m_strDBName);
			ICC_LOG_LOWDEBUG(m_pLog, "current host num is %d, old num is %d !!!", iEnableNum, pthis->_EnableHostNum());
			if (iEnableNum == 1 && iEnableNum != pthis->_EnableHostNum())
			{				
				pthis->_SetEnableHostNum(iEnableNum);
				pthis->_UpdateRConnections();
			}
			else if (pthis->_EnableHostNum() == 1 && iEnableNum > 1)
			{				
				pthis->_SetEnableHostNum(iEnableNum);
				pthis->_UpdateRConnections();
			}
			else
			{
				pthis->_SetEnableHostNum(iEnableNum);
			}
		}

		ICC_LOG_LOWDEBUG(m_pLog, "current connection ogject:[r = %d, rw = %d]!!!", pthis->_RConnectionNums(), pthis->_RWConnectionNums());

		//pthis->_TestPrintConnectionInfo();
		
		boost::this_thread::sleep(boost::posix_time::millisec(5000));
	}

	ICC_LOG_DEBUG(m_pLog, "CheckDBPool end!!!");
}

void PostgresqlPool::_TestPrintConnectionInfo()
{
	{
		Lock::AutoLock l_lock(m_lockRW);
		PooledConnection* pConn = nullptr;
		for (auto var : m_RWConnections)
		{
			std::string strHost = _QueryHostIP(var->Get());
			ICC_LOG_DEBUG(m_pLog, "rw connection host info = %s", strHost.c_str());
		}
	}

	{
		Lock::AutoLock l_lock(m_lockR);
		PooledConnection* pConn = nullptr;
		for (auto var : m_RConnections)
		{
			std::string strHost = _QueryHostIP(var->Get());
			ICC_LOG_DEBUG(m_pLog, "r connection host info = %s", strHost.c_str());
		}
	}
}

PostgresqlPool::~PostgresqlPool(void)
{

}

unsigned int PostgresqlPool::AvailableWriteConnNums()
{
	Lock::AutoLock l_lock(m_lockRW);
	int l_iSize = 0;
	for (auto var : m_RWConnections)
	{
		PGconn* pConn = var->Get();
		if (!var->GetIsBusy() && _TestConnection(pConn))
		{
			++l_iSize;
		}
	}
	return l_iSize;
}

bool PostgresqlPool::CreateConnections()
{	
	return _CreateConnections();
}

PooledConnectionPtr	PostgresqlPool::GetFreeRWConnection()
{
	Lock::AutoLock l_lock(m_lockRW);
	PooledConnectionPtr ptrReturnConn = nullptr;
	PGconn*	pConn = _FindFreeRWConnection();
	if (pConn == nullptr)//目没有可用的连接
	{
		if (m_RWConnections.size() < m_iMaxNum)
		{
			pConn = _NewRWConnection();
			if (pConn)
			{
				ptrReturnConn = boost::make_shared<PooledConnection>(pConn);
				ptrReturnConn->SetIsBusy(true);
				m_RWConnections.push_back(ptrReturnConn);
			}
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "current rw connection size[%d] has reached limit...", m_RWConnections.size());
		}
	}
	else
	{
		ptrReturnConn = boost::make_shared<PooledConnection>(pConn);
	}

	//如果 ptrReturnConn == nullptr是和数据库的网络中断了
	return ptrReturnConn;
}

PooledConnectionPtr	PostgresqlPool::GetFreeRConnection()
{
	Lock::AutoLock l_lock(m_lockR);
	PooledConnectionPtr ptrReturnConn = nullptr;
	PGconn*	pConn = _FindFreeRConnection();
	if (pConn == nullptr)//目没有可用的连接
	{
		if (m_RConnections.size() < m_iMaxNum)
		{
			pConn = _NewRConnection();
			if (pConn)
			{
				ptrReturnConn = boost::make_shared<PooledConnection>(pConn);
				ptrReturnConn->SetIsBusy(true);
				m_RConnections.push_back(ptrReturnConn);
			}
		}
		else
		{
			ICC_LOG_ERROR(m_pLog, "current r connection size[%d] has reached limit...", m_RConnections.size());
		}
	}
	else
	{
		ptrReturnConn = boost::make_shared<PooledConnection>(pConn);
	}

	//如果 ptrReturnConn == nullptr是和数据库的网络中断了
	return ptrReturnConn;
}

void PostgresqlPool::ReturnConnection(PGconn* pConn)
{	
	_ReturnRConnection(pConn);
	_ReturnRWConnection(pConn);
}

void PostgresqlPool::CloseConnectionPool()
{
	_CloseRWConnections();
	_CloseRConnections();
}

//PGresult* PostgresqlPool::ExecQuery(std::string strSql)
//{
//	PGresult* pRet = nullptr;
//	PooledConnectionPtr pPooledConnection = PostgresqlPool::Instance()->GetFreeConnection();
//	PGconn* pConn = pPooledConnection->Get();
//	if (nullptr == pConn)
//	{
//		ICC_LOG_FATAL(m_pLog, "get freed db connection failed.");
//		return pRet;
//	}
//
//	pRet = PQexec(pConn, strSql.c_str());
//	if (PQresultStatus(pRet) != PGRES_TUPLES_OK)
//	{
//		std::string l_strErrorMsg;
//		const char* l_temp1 = PQerrorMessage(pConn);
//		const char* l_temp2 = PQresultErrorMessage(pRet);
//		l_strErrorMsg.append(l_temp1 ? l_temp1 : "no error").append(l_temp2 ? l_temp2 : "no result");
//		
//		PQclear(pRet);
//		pRet = nullptr;
//		ICC_LOG_ERROR(m_pLog, "ExecQuery failed %s:", l_strErrorMsg.c_str());
//		return pRet;
//	}
//
//	return pRet;
//}

void PostgresqlPool::SafePQClear(PGresult *ret)
{
	PQclear(ret);
}

IResourceManagerPtr PostgresqlPool::GetResourceManager()
{
	return m_pResourceManager;
}

/************************************************************************/
/*                                                                      */
/************************************************************************/
int PostgresqlPool::_EnableHostNum()
{
	Lock::AutoLock l_lock(m_lockMasterHostIp);
	return m_iEnableHostNum;
}

void PostgresqlPool::_SetEnableHostNum(int iNum)
{
	Lock::AutoLock l_lock(m_lockMasterHostIp);
	m_iEnableHostNum = iNum;
}

int PostgresqlPool::_RConnectionNums()
{
	Lock::AutoLock l_lock(m_lockR);
	return m_RConnections.size();
}

int PostgresqlPool::_RWConnectionNums()
{
	Lock::AutoLock l_lock(m_lockRW);
	return m_RWConnections.size();
}

std::string PostgresqlPool::_MasterHostIp()
{
	Lock::AutoLock l_lock(m_lockMasterHostIp);
	return m_strMasterHostIp;
}

void PostgresqlPool::_SetMasterHostIp(const std::string& strMasterHostIp)
{
	Lock::AutoLock l_lock(m_lockMasterHostIp);
	m_strMasterHostIp = strMasterHostIp;
}


std::string PostgresqlPool::_ComposeBaseConnectionInfo(const std::vector<HostInfo>& vecIpInfos, const std::string& strUser, const std::string& strPwd, const std::string strDbName, const std::string& strAttr)
{
	std::string strConnectionInfo;
	std::string strIpInfo;
	std::map<std::string, std::string>::iterator itr;
	for (int i = 0; i < vecIpInfos.size(); ++i)
	{
		strIpInfo += vecIpInfos[i].strIp;
		strIpInfo += ":";
		strIpInfo += vecIpInfos[i].strPort;

		if (i != vecIpInfos.size() - 1)
		{
			strIpInfo += ",";
		}
	}

	char szTmp[1024] = { 0 };
	sprintf(szTmp, "postgresql://%s/%s?target_session_attrs=%s&user=%s&password=%s", strIpInfo.c_str(), strDbName.c_str(), strAttr.c_str(), strUser.c_str(), strPwd.c_str());
	return szTmp;
}

std::string PostgresqlPool::_QueryHostIP(PGconn* pConn)
{
	std::string strSql = "select inet_server_addr();";
	PGresult* l_pPgRes = PQexec(pConn, strSql.c_str());

	unsigned int l_uiExecStatusType = PQresultStatus(l_pPgRes);
	if (l_uiExecStatusType != PGRES_TUPLES_OK)
	{
		PQclear(l_pPgRes);
		PQreset(pConn);
		return "";
	}

	unsigned int l_uiRecordNum = PQntuples(l_pPgRes);
	unsigned int l_uiFieldNum = PQnfields(l_pPgRes);

	std::string strResult;
	const char* pszMastIp = PQgetvalue(l_pPgRes, 0, 0);
	strResult = pszMastIp;
	PQclear(l_pPgRes);
	return strResult;
}

std::string PostgresqlPool::_QueryCurrentMasterHostIp(const std::vector<HostInfo>& vecIpInfos, const std::string& strUser, const std::string& strPwd, const std::string strDbName)
{
	std::string strConnectionInfo = _ComposeRWConnectionInfo(vecIpInfos, strUser, strPwd, strDbName);
	PGconn* pConn = _NewConnection(strConnectionInfo);
	if (pConn)
	{
		std::string strResult =  _QueryHostIP(pConn);
		_CloseConnection(pConn);
		return strResult;
	}

	return "";
}

std::string PostgresqlPool::_ComposeRWConnectionInfo(const std::vector<HostInfo>& vecIpInfos, const std::string& strUser, const std::string& strPwd, const std::string strDbName)
{
	return _ComposeBaseConnectionInfo(vecIpInfos, strUser, strPwd, strDbName, "read-write");
}

std::string PostgresqlPool::_ComposeRConnectionInfo(const std::vector<HostInfo>& vecIpInfos, const std::string& strUser, const std::string& strPwd, const std::string strDbName)
{
	int iEnableNum = _EnableHostNum();//_CurrentEnableHostNum(vecIpInfos, strUser, strPwd, strDbName);
	if (iEnableNum == 0)
	{
		iEnableNum = _CurrentEnableHostNum(vecIpInfos, strUser, strPwd, strDbName);
		//_SetEnableHostNum(iEnableNum);
	}

	ICC_LOG_DEBUG(m_pLog, "enable host num = %d", iEnableNum);

	if (iEnableNum == 1)
	{
		ICC_LOG_WARNING(m_pLog, "enable host only one!!!!!!!!!!!!!!");
		return _ComposeBaseConnectionInfo(vecIpInfos, strUser, strPwd, strDbName, "any");
	}
	else if (iEnableNum == 0)
	{
		ICC_LOG_WARNING(m_pLog, "not enable host!!!!!!!!!!!!!!");
		return "";
	}

	std::string strMastIp = _MasterHostIp();
	if (strMastIp.empty())
	{
		strMastIp = _QueryCurrentMasterHostIp(vecIpInfos, strUser, strPwd, strDbName);
	}

	ICC_LOG_DEBUG(m_pLog, "current master ip = %s", strMastIp.c_str());

	if (strMastIp.empty())
	{		
		return _ComposeBaseConnectionInfo(vecIpInfos, strUser, strPwd, strDbName, "any");
	}
	else
	{		
		std::vector<HostInfo> vecTmpInfos;
		for (int i = vecIpInfos.size() - 1; i >= 0; i--)
		{
			if (strMastIp != vecIpInfos[i].strIp)
			{
				vecTmpInfos.push_back(vecIpInfos[i]);
			}
		}

		return _ComposeBaseConnectionInfo(vecTmpInfos, strUser, strPwd, strDbName, "any");		
	}	
}


PGconn* PostgresqlPool::_NewConnection(const std::string& strConnectionInfo)
{
	PGconn* pConn = nullptr;
	pConn = PQconnectdb(strConnectionInfo.c_str());

	if (PQstatus(pConn) == CONNECTION_BAD)
	{
		ICC_LOG_ERROR(m_pLog, "ConnectDB Error: %s, connection info = %s", PQerrorMessage(pConn), strConnectionInfo.c_str());
		PQfinish(pConn);
		pConn = nullptr;
		return pConn;
	}

	if (0 != PQsetClientEncoding(pConn, "utf8"))
	{
		ICC_LOG_ERROR(m_pLog, "set db[%s] encodeing failed failed: %s", m_strDBName.c_str(), PQerrorMessage(pConn));
		PQfinish(pConn);
		pConn = nullptr;
		return pConn;
	}

	ICC_LOG_LOWDEBUG(m_pLog, "ConnectDB Success, connection info[%s]", strConnectionInfo.c_str());
	return pConn;
}

void PostgresqlPool::_ReturnRWConnection(PGconn* pConn)
{
	Lock::AutoLock l_lock(m_lockRW);
	if (m_RWConnections.size() == 0)
	{
		return;
	}
	for (auto var : m_RWConnections)
	{
		if (pConn == var->Get())//比较地址
		{
			var->SetIsBusy(false);
			//ICC_LOG_DEBUG(m_pLog, "return db connection, current available connections[%d]", AvailableConnNums());
			break;
		}
	}
}

void PostgresqlPool::_ReturnRConnection(PGconn* pConn)
{
	Lock::AutoLock l_lock(m_lockR);
	if (m_RConnections.size() == 0)
	{
		return;
	}
	for (auto var : m_RConnections)
	{
		if (pConn == var->Get())//比较地址
		{
			var->SetIsBusy(false);
			//ICC_LOG_DEBUG(m_pLog, "return db connection, current available connections[%d]", AvailableConnNums());
			break;
		}
	}
}

PGconn* PostgresqlPool::_NewRWConnection()
{
	std::string strConnectionInfo = _ComposeRWConnectionInfo(m_vecHostInfos, m_strUserName, m_strPassWord, m_strDBName);
	return _NewConnection(strConnectionInfo);
}

PGconn* PostgresqlPool::_NewRConnection()
{
	std::string strConnectionInfo;
	strConnectionInfo = _ComposeRConnectionInfo(m_vecHostInfos, m_strUserName, m_strPassWord, m_strDBName);	
	return _NewConnection(strConnectionInfo);
}

bool PostgresqlPool::_CreateRWConnections()
{
	Lock::AutoLock l_lock(m_lockRW);
	for (int i = 0; i < m_iInitNum; ++i)//没有空的情况 那么就说明没有可用的连接了 需要增加
	{
		PGconn* pConn = _NewRWConnection();
		if (pConn == nullptr)
		{
			continue;
		}
		m_RWConnections.push_back(boost::make_shared<PooledConnection>(pConn));
	}
	m_bRWConnectionInitFlag = (m_RWConnections.size() == m_iInitNum);
	return m_bRWConnectionInitFlag;
}


bool PostgresqlPool::_CreateRConnections()
{
	Lock::AutoLock l_lock(m_lockR);
	for (int i = 0; i < m_iInitNum; ++i)//没有空的情况 那么就说明没有可用的连接了 需要增加
	{
		PGconn* pConn = _NewRConnection();
		if (pConn == nullptr)
		{
			continue;
		}
		m_RConnections.push_back(boost::make_shared<PooledConnection>(pConn));
	}
	m_bRConnectionInitFlag = (m_RConnections.size() == m_iInitNum);
	return m_bRConnectionInitFlag;

	//Lock::AutoLock l_lock(m_lockR);
	//bool success = false;
	//for (auto var : m_RConnections)
	//{
	//	if (var->Get() == nullptr)
	//	{
	//		PGconn* pConn = _NewRConnection();
	//		if (pConn == nullptr)
	//		{
	//			return false;
	//		}

	//		m_RConnections.push_back(boost::make_shared<PooledConnection>(pConn));
	//		success = true;
	//	}
	//}

	//if (success)//如果含的有空的 并且重新创建了对象 那么就可以返回了
	//{
	//	return success;
	//}

	//for (int i = 0; i < m_iInitNum; ++i)//没有空的情况 那么就说明没有可用的连接了 需要增加
	//{
	//	if (m_iMaxNum > 0 && m_RConnections.size() >= m_iMaxNum)
	//	{
	//		ICC_LOG_ERROR(m_pLog, "current r connection size[%d] has reached limit...", m_RConnections.size());
	//		return false;
	//	}

	//	PGconn* pConn = _NewRConnection();
	//	if (pConn == nullptr)
	//	{
	//		continue;
	//	}
	//	m_RConnections.push_back(boost::make_shared<PooledConnection>(pConn));
	//}
	//return true;
}

bool PostgresqlPool::_CreateConnections()
{
	bool bRw = _CreateRWConnections();
	bool bR = _CreateRConnections();

	return (bRw && bR);
}

bool PostgresqlPool::_TestConnection(PGconn* pConn)
{
	PGresult* pRet = PQexec(pConn, "select 1;");
	ExecStatusType l_tTestResultStatus = PQresultStatus(pRet);
	PQclear(pRet);
	if (l_tTestResultStatus == PGRES_FATAL_ERROR)
	{
		return false;
	}
	return true;
}

bool PostgresqlPool::_Reconnect(PGconn* pConn)
{
	PQreset(pConn);//close the connection and establish a new connection to the database
	if (PQstatus(pConn) != CONNECTION_OK)
	{
		ICC_LOG_ERROR(m_pLog, "lost DB connection...[%s]", PQerrorMessage(pConn));
		return false;
	}
	return true;
}

void PostgresqlPool::_CloseConnection(PGconn* pConn)
{
	PQfinish(pConn);
}

void PostgresqlPool::_CloseRWConnections()
{
	Lock::AutoLock l_lock(m_lockRW);
	PooledConnection* pConn = nullptr;
	for (auto var : m_RWConnections)
	{
		_CloseConnection(var->Get());
		var->Set(nullptr);
		var->SetNeedFree(false);
	}

	m_RWConnections.clear();
}

void PostgresqlPool::_CloseRConnections()
{
	ICC_LOG_DEBUG(m_pLog, "_CloseRConnections begin !!!!");
	{
		Lock::AutoLock l_lock(m_lockR);
		ICC_LOG_DEBUG(m_pLog, "_CloseRConnections size = %d !!!!", m_RConnections.size());
		PooledConnection* pConn = nullptr;
		for (auto var : m_RConnections)
		{
			_CloseConnection(var->Get());
			var->Set(nullptr);
			var->SetNeedFree(false);
			ICC_LOG_DEBUG(m_pLog, "_CloseConnection complete !!!!");
		}

		m_RConnections.clear();
	}
	ICC_LOG_DEBUG(m_pLog, "_CloseRConnections end !!!!");
}

void PostgresqlPool::_CloseConnections()
{
	_CloseRWConnections();
	_CloseRConnections();
}

PGconn* PostgresqlPool::_FindFreeRWConnection()
{
	//Lock::AutoLock l_lock(m_lockRW);
	PGconn* pConn = nullptr;
	for (auto var : m_RWConnections)
	{
		if (var->Get() == nullptr)
		{
			continue;
		}

		if (!var->GetIsBusy())
		{
			var->SetIsBusy(true);
			pConn = var->Get();
			if (!_TestConnection(pConn))
			{				
				if (!_Reconnect(pConn))
				{
					var->SetIsBusy(false);
					continue;
				}				
			}
			break;
		}
	}
	return pConn;
}

PGconn* PostgresqlPool::_FindFreeRConnection()
{
	//Lock::AutoLock l_lock(m_lockR);
	PGconn* pConn = nullptr;
	for (auto var : m_RConnections)
	{
		if (var->Get() == nullptr)
		{
			continue;
		}

		if (!var->GetIsBusy())
		{
			var->SetIsBusy(true);
			pConn = var->Get();
			if (!_TestConnection(pConn))
			{
				if (!_Reconnect(pConn))
				{
					var->SetIsBusy(false);
					continue;
				}
			}
			break;
		}
	}
	return pConn;
}

int PostgresqlPool::_CurrentEnableHostNum(const std::vector<HostInfo>& vecIpInfos, const std::string& strUser, const std::string& strPwd, const std::string strDbName)
{
	int iEnableHostNum = 0;
	for (int i = 0; i < vecIpInfos.size(); ++i)
	{
		std::vector<HostInfo> vecTmp;
		vecTmp.push_back(vecIpInfos[i]);
		std::string strConnectionInfo = _ComposeBaseConnectionInfo(vecTmp, strUser, strPwd, strDbName, "any");
		PGconn* pConn = _NewConnection(strConnectionInfo);
		if (pConn)
		{
			iEnableHostNum++;
			_CloseConnection(pConn);
		}
	}
	return iEnableHostNum;
}

void PostgresqlPool::_ProcessOnlyOneHost()
{
	
}

void PostgresqlPool::_UpdateRConnections()
{
	ICC_LOG_DEBUG(m_pLog, "_UpdateRConnections begin !!!!");
	_CloseRConnections();
	ICC_LOG_DEBUG(m_pLog, "_CloseRConnections complete !!!!");
	_CreateRConnections();
	ICC_LOG_DEBUG(m_pLog, "_UpdateRConnections end !!!!");
}

/************************************************************************/
PooledConnection::PooledConnection(PGconn* pConn) : m_pConnection(pConn), m_isBusy(false), m_bIsNeedFree(true)
{

}

PooledConnection::PooledConnection(const PooledConnection& oth)
{
	this->m_pConnection = oth.m_pConnection;
	this->m_isBusy = oth.m_isBusy;
	this->m_bIsNeedFree = oth.m_bIsNeedFree;
}

PooledConnection& PooledConnection::operator=(const PooledConnection& oth)
{
	if (this != &oth)
	{
		this->m_pConnection = oth.m_pConnection;
		this->m_isBusy = oth.m_isBusy;
		this->m_bIsNeedFree = oth.m_bIsNeedFree;
	}
	return *this;
}

PooledConnection::~PooledConnection()
{
	if (m_bIsNeedFree)
	{
		PostgresqlPool::Instance()->ReturnConnection(m_pConnection);
	}	
}

void PooledConnection::Set(PGconn* pConn)
{
	m_pConnection = pConn;
}

PGconn*	PooledConnection::Get()
{
	return m_pConnection;
}

void PooledConnection::SetIsBusy(bool busy)
{
	m_isBusy = busy;
}

bool PooledConnection::GetIsBusy()
{
	return this->m_isBusy;
}

void PooledConnection::SetNeedFree(bool bNeedFree)
{
	m_bIsNeedFree = bNeedFree;
}
