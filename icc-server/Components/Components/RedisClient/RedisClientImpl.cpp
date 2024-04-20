#include "Boost.h"
#include "RedisClientImpl.h"

#define SLEEP_TIMESPAN	1
#define SERVER_ROLE_MASTER	"master"
#define SERVER_ROLE_SLAVE	"slave"

#define MODE_SENTINEL		"0"
#define MODE_CLUSTER		"1"
#define MODE_SINGLE			"2"

#define	SAFE_LOCK(lk) std::lock_guard<std::recursive_mutex> l_guard(lk);

ICC::Redis::CRedisClientImpl::CRedisClientImpl(IResourceManagerPtr p_ResourceManagerPtr) 
	: m_pResourceManager(p_ResourceManagerPtr)
	, m_pString(nullptr)
	, m_pLog(nullptr)
	, m_pSingleConn(nullptr)
	, m_pClusterConn(nullptr)
	, m_iConnTimeOut(10)
	, m_iRWTimeOut(10)
	, m_bClusterMode(false)
	, m_iKeyTTL(0)
{	
	m_bCheckRoleThreadAlive = false;
	m_pCheckRoleThread = nullptr;
	m_pLog = ICCGetResourceEx(Log::ILogFactory, ICCILogFactoryResourceName, p_ResourceManagerPtr)->GetLogger(MODULE_NAME);
	m_pString = ICCGetResourceEx(StringUtil::IStringFactory, ICCIStringFactoryResourceName, p_ResourceManagerPtr)->CreateString();
	m_pConfig = ICCGetIConfigFactory()->CreateConfig();
	m_strClientGuid = m_pString->CreateGuid();
	m_HelpTool = ICCGetHelpToolFactory()->CreateHelpTool();
	ConnectRedis();	
}

CRedisClientImpl::~CRedisClientImpl()
{
	this->Disconnect();
}

void CRedisClientImpl::InitClient()
{	
	acl::acl_cpp_init();
	acl::log::stdout_open(true);

	if (m_bClusterMode)
	{
		if (m_strServerMode.compare(MODE_SENTINEL) == 0)
		{
			//	哨兵模式
			InitSentinelClient(m_strServerAddress);
		}
		else
		{
			//	集群模式
			if (m_pClusterConn == nullptr)
			{
				m_pClusterConn = new acl::redis_client_cluster();
				for (auto l_objAddr : m_mapServers)
				{
					m_pClusterConn->set_password(l_objAddr.first.c_str(), l_objAddr.second.c_str());
				}
			}
		}
	}
	else
	{
		// 单点模式
		if (m_pSingleConn == nullptr)
		{
			m_pSingleConn = new acl::redis_client(m_strServerAddress.c_str(), m_iConnTimeOut, m_iRWTimeOut);
			m_pSingleConn->set_password(m_mapServers[m_strServerAddress].c_str());
		}
	}	
}
bool CRedisClientImpl::ConnectRedis()
{	
	//连接Redis服务器
	std::string l_strRedisList = m_pConfig->GetValue("ICC/Component/RedisClient/Servers", "127.0.0.1:6379#Command_123");//默认Redis服务部署于本机	
	std::string l_strIsCluster = m_pConfig->GetValue("ICC/Component/RedisClient/Cluster", "0");//默认非集群部署
	std::string l_strTTL = m_pConfig->GetValue("ICC/Component/RedisClient/TTL", "3628800");//默认生存时间一周
	m_strDefaultClassifyName = m_pConfig->GetValue("ICC/Component/RedisClient/ClassifyName", "icc");//默认分类名称
	m_strServerMode = m_pConfig->GetValue("ICC/Component/RedisClient/Mode", "0");	//服务部署模式，0:哨兵，1：集群	
	m_strServerAddress = GetServerAddrs(m_mapServers, l_strRedisList);
	m_iConnTimeOut = m_pString->ToInt(m_pConfig->GetValue("ICC/Component/RedisClient/ConnTimeOut", "10"));//连接超时时间
	m_iRWTimeOut = m_pString->ToInt(m_pConfig->GetValue("ICC/Component/RedisClient/RWTimeOut", "10"));//读写超时时间

	bool l_bIsCluster = (l_strIsCluster == "1");
	int l_iKeyTTL = std::atoi(l_strTTL.c_str());
	m_bClusterMode = l_bIsCluster;
	m_iKeyTTL = l_iKeyTTL;

	InitClient();

	while (!Connect(m_strServerAddress, l_bIsCluster, l_iKeyTTL))
	{
		ICC_LOG_ERROR(m_pLog, "Plugin connect redis server failed, server:[%s], IsCluster: [%d], Mode: [%s], KeyTTL: [%d]"
			, m_strServerAddress.c_str(), l_bIsCluster, m_strServerMode.c_str(), l_iKeyTTL);

		std::this_thread::sleep_for(std::chrono::seconds(SLEEP_TIMESPAN * 3));
	}	
	

	ICC_LOG_DEBUG(m_pLog, "Plugin connect redis server success, server:[%s], IsCluster: [%d], Mode: [%s], KeyTTL: [%d]"
		, m_strServerAddress.c_str(), l_bIsCluster, m_strServerMode.c_str(), l_iKeyTTL);

	if (l_bIsCluster && m_strServerMode.compare(MODE_SENTINEL) == 0)
	{
		// 哨兵模式
		StartCheckRoleThread();
	}	

	return true;
}

//////////////////////////////////////////////////////////////////////////
std::string CRedisClientImpl::GetServerAddrs(std::map<std::string, std::string>& p_mapOutServers, const std::string& p_strInAddrList)
{
	p_mapOutServers.clear();

	int l_nCount = 0;
	std::string l_strServerList;

	std::vector<std::string> l_vecAddrs;
	StringSplit(l_vecAddrs, p_strInAddrList, ";");
	for (auto l_strTemp : l_vecAddrs)
	{
		size_t l_nIndex = l_strTemp.find("#");
		if (l_nIndex != std::string::npos)
		{
			std::string l_strServ = l_strTemp.substr(0, l_nIndex);
			std::string l_strPwd = l_strTemp.substr(l_nIndex + 1);
			
			l_strServerList += l_strServ;
			p_mapOutServers[l_strServ] = l_strPwd;
		}

		if (++l_nCount < l_vecAddrs.size())
		{
			l_strServerList += ";";
		}
	}

	return l_strServerList;
}
void CRedisClientImpl::StringSplit(std::vector<std::string>& p_vecOutString, const std::string& p_strInString, const std::string& p_strInSplit)
{
	std::string l_strStringList = p_strInString;
	p_vecOutString.clear();

	int l_nIndex = l_strStringList.find(p_strInSplit);
	while (l_nIndex != std::string::npos)
	{
		std::string l_strSubString = l_strStringList.substr(0, l_nIndex);
		p_vecOutString.push_back(l_strSubString);

		l_strStringList = l_strStringList.substr(l_nIndex + 1, l_strStringList.length() - l_nIndex - 1);
		l_nIndex = l_strStringList.find(p_strInSplit);
	}

	if (!l_strStringList.empty())
	{
		p_vecOutString.push_back(l_strStringList);
	}
}
void CRedisClientImpl::InitSentinelClient(const std::string& p_strAddressList)
{
	std::vector<std::string> l_vecAddr;
	StringSplit(l_vecAddr, p_strAddressList, ";");

	for (auto l_strServerAddr : l_vecAddr)
	{
		acl::redis_client *l_pSingleConn = new acl::redis_client(l_strServerAddr.c_str(), m_iConnTimeOut, m_iRWTimeOut);
		l_pSingleConn->set_password(m_mapServers[l_strServerAddr].c_str());
		SRedisClient *l_pClientObj = new SRedisClient();
		l_pClientObj->m_strServerRole = SERVER_ROLE_SLAVE;
		l_pClientObj->m_pSingleConn = l_pSingleConn;

		m_vecRedisClients.push_back(l_pClientObj);
	}
}
void CRedisClientImpl::CloseSentinelClient()
{
	for (auto l_pClientObj : m_vecRedisClients)
	{
		if (l_pClientObj)
		{
			acl::redis_client *l_pSingleConn = l_pClientObj->m_pSingleConn;
			if (l_pSingleConn)
			{
				l_pSingleConn->close();

				delete l_pSingleConn;
				l_pSingleConn = nullptr;
			}

			delete l_pClientObj;
			l_pClientObj = nullptr;
		}
	}

	m_vecRedisClients.clear();
}
std::string CRedisClientImpl::AnalyzeResult(const std::string& p_strResult)
{
	std::string l_strServerRole = "";
	std::vector<std::string> l_vecResult;

	StringSplit(l_vecResult, p_strResult, "\r\n");
	if (l_vecResult.size() > 0)
	{
		l_strServerRole = l_vecResult[0];
	}

	return l_strServerRole;
}
bool CRedisClientImpl::GetServerRole(std::string& p_strOutResult, acl::redis_client* p_pSingleConn)
{
	//SAFE_LOCK(m_mutexCheckRole);  //2022-04-19为啥要锁
	std::string tmp_strPingRedis(p_pSingleConn->get_addr());
	ICC_LOG_DEBUG(m_pLog, "enter to ping: %s", tmp_strPingRedis.c_str());
	//如果是当前所有命令操作的redis需要锁，否则不需要
	if (tmp_strPingRedis == GetOperatorAddr())
	{
		SAFE_LOCK(m_mutexCheckRole);
		
		acl::redis_connection l_pConnection;
		acl::redis_string l_stringOperator;
		l_pConnection.set_client(p_pSingleConn);

		l_stringOperator.set_client(p_pSingleConn);
		ICC_LOG_DEBUG(m_pLog, "start to ping");
		if (l_pConnection.ping())
		{
			acl::string l_strAclOut;
			std::vector<acl::string> l_vecArgs;
			l_vecArgs.push_back("role");

			const acl::redis_result* l_pResult = l_stringOperator.request(l_vecArgs);
			if (l_pResult)
			{
				l_pResult->to_string(l_strAclOut);
				p_strOutResult = l_strAclOut.c_str();
				//	ICC_LOG_LOWDEBUG(m_pLog, "Get Server Role: [%s]", l_strAclOut.c_str());

				return true;
			}
			else
			{
				ICC_LOG_ERROR(m_pLog, "Get Server Role Failed");
			}
		}
		ICC_LOG_DEBUG(m_pLog, "end to ping");
		return false;
	}
	else
	{
		ICC_LOG_DEBUG(m_pLog, "enter to ping");
		acl::redis_connection l_pConnection;
		acl::redis_string l_stringOperator;
		l_pConnection.set_client(p_pSingleConn);

		l_stringOperator.set_client(p_pSingleConn);
		ICC_LOG_DEBUG(m_pLog, "start to ping");
		if (l_pConnection.ping())
		{
			acl::string l_strAclOut;
			std::vector<acl::string> l_vecArgs;
			l_vecArgs.push_back("role");

			const acl::redis_result* l_pResult = l_stringOperator.request(l_vecArgs);
			if (l_pResult)
			{
				l_pResult->to_string(l_strAclOut);
				p_strOutResult = l_strAclOut.c_str();
				//	ICC_LOG_LOWDEBUG(m_pLog, "Get Server Role: [%s]", l_strAclOut.c_str());

				return true;
			}
			else
			{
				ICC_LOG_ERROR(m_pLog, "Get Server Role Failed");
			}
		}
		ICC_LOG_DEBUG(m_pLog, "end to ping");
		return false;
	}
}
bool CRedisClientImpl::CheckServerRole()
{
	for (auto l_pClientObj : m_vecRedisClients)
	{
		if (l_pClientObj && l_pClientObj->m_pSingleConn)
		{
			std::string l_strRoleInfo;
		
			if (GetServerRole(l_strRoleInfo, l_pClientObj->m_pSingleConn))
			{
				std::string l_strAddress = l_pClientObj->m_pSingleConn->get_addr();
				std::string l_strOldRole = l_pClientObj->m_strServerRole;

				std::string l_strCurrentRole = AnalyzeResult(l_strRoleInfo);
				//l_pClientObj->m_strServerRole = l_strCurrentRole; //当前redis状态，需要每次都设置,否则下面的判断条件从备到主可能不能进去

				if (l_strCurrentRole.compare(SERVER_ROLE_MASTER) == 0)
				{
					std::string l_strOldMastAddress(GetOperatorAddr());
					
					//通过地址信息判断
					if (l_strOldMastAddress != l_strAddress)
					{
						SetOperator(l_pClientObj->m_pSingleConn);

						ICC_LOG_DEBUG(m_pLog, "Reset Master Server, Address: [%s], older addr info:[%s], role: %s, Server Info: [%s]",
							l_strAddress.c_str(), l_strOldMastAddress.c_str(), l_strCurrentRole.c_str(), l_strRoleInfo.c_str());

						return true;
					}
				}
				
			}
		}
	}

	return false;
}
void CRedisClientImpl::SetOperator(acl::redis_client* p_pSingleConn)
{
	SAFE_LOCK(m_mutexCheckRole);

	if (p_pSingleConn)
	{
		m_transaction.set_client(p_pSingleConn);
		m_keyOperator.set_client(p_pSingleConn);
		m_stringOperator.set_client(p_pSingleConn);
		m_listOperator.set_client(p_pSingleConn);
		m_setOperator.set_client(p_pSingleConn);
		m_zsetOperator.set_client(p_pSingleConn);
		m_hashOperator.set_client(p_pSingleConn);
		m_pConnection.set_client(p_pSingleConn);
	
		m_scriptOperator.set_client(p_pSingleConn);
	}
}

std::string CRedisClientImpl::GetOperatorAddr()
{
	SAFE_LOCK(m_mutexCheckRole);

	const char*tmp_pAddr = m_stringOperator.get_client_addr();
	
	if (NULL != tmp_pAddr)
	{
		return tmp_pAddr;
	}

	return "";
}

void CRedisClientImpl::DoCheckRoleTask()
{
	ICC_LOG_DEBUG(m_pLog, "===================== Sentinel Mode, Check Role Thread Start!! =====================");

	while (m_bCheckRoleThreadAlive)
	{
		CheckServerRole();

		std::this_thread::sleep_for(std::chrono::seconds(SLEEP_TIMESPAN));
	}

	ICC_LOG_DEBUG(m_pLog, "------------------ Sentinel Mode, Check Role Thread Exit!! ------------------");
}
void CRedisClientImpl::StartCheckRoleThread()
{
	if (!m_bCheckRoleThreadAlive)
	{
		m_bCheckRoleThreadAlive = true;

		m_pCheckRoleThread = std::make_shared<std::thread>(std::bind(&CRedisClientImpl::DoCheckRoleTask, this));
	}
}
void CRedisClientImpl::StopCheckRoleThread()
{
	if (m_bCheckRoleThreadAlive)
	{
		m_bCheckRoleThreadAlive = false;

		if (m_pCheckRoleThread)
		{
			m_pCheckRoleThread->join();
		}
	}
}
//////////////////////////////////////////////////////////////////////////

bool CRedisClientImpl::Connect(const std::string& p_strAddress, bool p_bClusterMode, int p_iKeyTTL)
{	
	this->m_strServerAddress = p_strAddress;
	this->m_bClusterMode = p_bClusterMode;
	this->m_iKeyTTL = p_iKeyTTL;

	if (m_bClusterMode)
	{
		if (m_strServerMode.compare(MODE_SENTINEL) == 0)
		{
			//	哨兵模式
			CheckServerRole();
		}
		else
		{
			//	集群模式
			if (m_pClusterConn == nullptr)
			{
				m_pClusterConn = new acl::redis_client_cluster();
			}

			m_pClusterConn->init(nullptr, m_strServerAddress.c_str(), 0, m_iConnTimeOut, 0);
			for (auto l_objAddr : m_mapServers)
			{
				m_pClusterConn->set_password(l_objAddr.first.c_str(), l_objAddr.second.c_str());
			}

			m_transaction.set_cluster(m_pClusterConn, 0);
			m_keyOperator.set_cluster(m_pClusterConn, 0);
			m_stringOperator.set_cluster(m_pClusterConn, 0);
			m_listOperator.set_cluster(m_pClusterConn, 0);
			m_setOperator.set_cluster(m_pClusterConn, 0);
			m_zsetOperator.set_cluster(m_pClusterConn, 0);
			m_hashOperator.set_cluster(m_pClusterConn, 0);
			m_pConnection.set_cluster(m_pClusterConn, 0);

			m_scriptOperator.set_cluster(m_pClusterConn, 0);
		}
	}
	else
	{
		// 单点模式
		if (m_pSingleConn == nullptr)
		{
			m_pSingleConn = new acl::redis_client(m_strServerAddress.c_str(), m_iConnTimeOut, m_iRWTimeOut);
		}
		
		m_pSingleConn->set_password(m_mapServers[m_strServerAddress].c_str());
		m_transaction.set_client(m_pSingleConn);
		m_keyOperator.set_client(m_pSingleConn);
		m_stringOperator.set_client(m_pSingleConn);
		m_listOperator.set_client(m_pSingleConn);
		m_setOperator.set_client(m_pSingleConn);
		m_zsetOperator.set_client(m_pSingleConn);
		m_hashOperator.set_client(m_pSingleConn);
		m_pConnection.set_client(m_pSingleConn);

		m_scriptOperator.set_client(m_pSingleConn);
	}	

	return m_pConnection.ping();
}
void CRedisClientImpl::Disconnect()
{
	if (m_pSingleConn)
	{
		m_pSingleConn->close();

		delete m_pSingleConn;
		m_pSingleConn = nullptr;
	}

	if (m_pClusterConn)
	{
		delete m_pClusterConn;
		m_pClusterConn = nullptr;
	}

	StopCheckRoleThread();
	CloseSentinelClient();
}


//取分布式锁
bool CRedisClientImpl::_Lock(const std::string& p_pcKey, std::string& p_strLockValue, int p_iLockTTL, int p_iTimeOut, std::string& p_strError)
{
	acl::string l_strLuaScript("if redis.call('setNx',KEYS[1],ARGV[1]) then if redis.call('get',KEYS[1])==ARGV[1] then return redis.call('pexpire',KEYS[1],ARGV[2]) else return 0 end end");

	ICC_LOG_DEBUG(m_pLog, "begin server lock1");

	std::vector<const char*> tmp_keys;
	tmp_keys.push_back(p_pcKey.c_str());

	std::vector<const char*> tmp_Args;
	tmp_Args.push_back(p_strLockValue.c_str()); //键值
	tmp_Args.push_back(std::to_string(p_iLockTTL).c_str()); //超时时间

	SAFE_LOCK(m_mutexCheckRole);
	m_scriptOperator.clear();

	const acl::redis_result* tmp_pResult = m_scriptOperator.eval(l_strLuaScript.c_str(),
		tmp_keys,
		tmp_Args);

	int nRet = -1;

	if (NULL != tmp_pResult)
	{
		nRet = tmp_pResult->get_integer();

		acl::string tmp_strResult;
		ICC_LOG_DEBUG(m_pLog, "excute script, ret: %d, errorinfo: %s", nRet, tmp_pResult->to_string(tmp_strResult).c_str());

		//成功 1，失败
		if (1 == nRet)
		{
			return true;
		}

		p_strError = "lock failed, key: " + p_pcKey+", result: " + tmp_strResult.c_str();
	}
	else
	{
		ICC_LOG_ERROR(m_pLog, "redis result is NULL");
	}

	return false;
}

//延期时间
int CRedisClientImpl::_Renewal(const std::string& p_pcKey, std::string& p_strLockValue, int p_iLockTTL, int p_iTimeOut)
{
	acl::string l_strLuaScript("if redis.call('get', KEYS[1]) == ARGV[1] then return redis.call('pexpire', KEYS[1], ARGV[2]) else return -100 end");

	ICC_LOG_DEBUG(m_pLog, "begin Renewal lock");

	std::vector<const char*> tmp_keys;
	tmp_keys.push_back(p_pcKey.c_str());

	std::vector<const char*> tmp_Args;
	tmp_Args.push_back(p_strLockValue.c_str()); //键值
	tmp_Args.push_back(std::to_string(p_iLockTTL).c_str()); //超时时间

	SAFE_LOCK(m_mutexCheckRole);
	m_scriptOperator.clear();

	const acl::redis_result* tmp_pResult = m_scriptOperator.eval(l_strLuaScript.c_str(),
		tmp_keys,
		tmp_Args);

	int nRet = -1;

	if (NULL != tmp_pResult)
	{
		nRet = tmp_pResult->get_integer();

		acl::string tmp_strResult;
		ICC_LOG_DEBUG(m_pLog, "excute script, ret: %d,  errorinfo: %s, [-100 key is the other]", nRet, tmp_pResult->to_string(tmp_strResult).c_str());
	}
	else
	{
		ICC_LOG_ERROR(m_pLog, "redis result is NULL");
	}
   
	return nRet;
}

bool CRedisClientImpl::ServiceLockEx(const std::string& p_pcKey, std::string& p_strLockValue, int p_iLockTTL, int p_iTimeOut, bool p_bOperatorLock, std::string& p_strError)
{
	ICC_LOG_DEBUG(m_pLog, "begin server lock");

	std::string l_strKey(m_strDefaultClassifyName + ":master_slave_lock:" + p_pcKey);
	int tmp_iRet = -1;
	//是否操作取锁
	if (p_bOperatorLock)
	{
		bool bLockRes = _Lock(l_strKey, p_strLockValue, p_iLockTTL, p_iTimeOut, p_strError);
		if (!bLockRes)
		{
			std::string l_strTmpCheckKey("master_slave_lock:" + p_pcKey);
			int nTmpTTL = TTL(l_strTmpCheckKey);
			nTmpTTL = nTmpTTL * 1000;
			if (nTmpTTL > p_iLockTTL * 2)
			{
				int nDelRes = Del(l_strTmpCheckKey);
				ICC_LOG_DEBUG(m_pLog, "Key: %s, del key: currentTTL=%d,p_iLockTTL=%d,res=%d", l_strKey.c_str(), nTmpTTL, p_iLockTTL, nDelRes);
			}
			else
			{
				ICC_LOG_DEBUG(m_pLog, "Key: %s, get ttl:%d,p_iLockTTL=%d", l_strKey.c_str(), nTmpTTL, p_iLockTTL);
			}
		}
		return bLockRes;
	}
	else
	{
		//操作延期
		tmp_iRet = _Renewal(l_strKey, p_strLockValue, p_iLockTTL, p_iTimeOut);
	}

	//成功为1
	if (1 == tmp_iRet)
	{
		ICC_LOG_DEBUG(m_pLog, "Key: %s, reset ttl: %d", l_strKey.c_str(), p_iLockTTL);
		return true;
	}
	else if (-100 == tmp_iRet) //表示被别人锁了
	{
		p_strError = l_strKey + " is locked by other client";
		ICC_LOG_DEBUG(m_pLog, "server is locked by other client, my keyvalue: %s", p_strLockValue.c_str());
		return false;
	}
	
	p_strError = l_strKey + "lock refresh expire time, failed, keyvalue: %s" + p_strLockValue;
	return false;

}

bool CRedisClientImpl::ServiceLock(const std::string& p_pcKey, std::string& p_strLockValue, int p_iLockTTL, int p_iTimeOut, std::string& p_strError)
{
	SAFE_LOCK(m_mutexCheckRole);

	CClockTimer clock;
	std::string l_strKey = m_strDefaultClassifyName + ":master_slave_lock:" + p_pcKey;

	while ((clock.ElapsedSec() * 1000) < p_iTimeOut) //允许3s的超时时间
	{
		m_stringOperator.clear();
		int nRet = m_stringOperator.setnx(l_strKey.c_str(), p_strLockValue.c_str());
		if (nRet == 1)//设置成功，加锁成功
		{
			m_keyOperator.clear();
			return (m_keyOperator.pexpire(l_strKey.c_str(), p_iLockTTL) > 0);
		}
		else if (nRet == 0)//锁已经存在
		{
			acl::string strTempBuffer;
			m_stringOperator.clear();
			if (!m_stringOperator.get(l_strKey.c_str(), strTempBuffer))
			{
				//key 不存在
				ICC_LOG_ERROR(m_pLog, "Exec get failed, redis no exists key: %s", l_strKey.c_str());

				continue;
			}

			std::string l_strValue = strTempBuffer.c_str();
			m_keyOperator.clear();
			//当锁设置没有过期时间时，为其设置过期时间，以防崩溃的客户端永远持有锁
			long long int l_iTTL = m_keyOperator.pttl(l_strKey.c_str());
			if (l_iTTL == -1)//存在，但是没有设置生存时间
			{
				//key存在但没有设置剩余时间，设置生存时间
				m_keyOperator.clear();
				m_keyOperator.pexpire(l_strKey.c_str(), p_iLockTTL);
				ICC_LOG_DEBUG(m_pLog, "Key: %s ttl = -1, reset ttl: %d", l_strKey.c_str(), p_iLockTTL);

				if (l_strValue == p_strLockValue)//如果是自己锁住了
				{
					return true;
				}
				else
				{
					p_strError = l_strKey + " is locked by: " + l_strValue + "; TTL = -1";
					return false;
				}
			}
			else if (l_iTTL > 0)
			{
				//如果是自己锁住了，更新生存时间
				if (l_strValue == p_strLockValue)
				{
					p_strError = l_strKey + " is locked by myself: " + l_strValue;
					m_keyOperator.clear();
					m_keyOperator.pexpire(l_strKey.c_str(), p_iLockTTL);
					return true;
				}

				p_strError = l_strKey + " is locked by: " + l_strValue;
				return false;
			}
			else if (l_iTTL == -2)//key 不存在
			{
				ICC_LOG_ERROR(m_pLog, "Exec pttl failed, redis no exists key: %s", l_strKey.c_str());
				continue;
			}
			else
			{
				p_strError = p_strLockValue + " exec pttl failed";
				return false;
			}
		}
		else
		{
			p_strError = p_strLockValue + " exec setnx failed, error code: " + std::to_string(nRet);
			return false;
		}
	}

	p_strError = p_strLockValue + " locked failed, execute timeout";

	return false;
}


bool CRedisClientImpl::Exists(const std::string& p_strKey)
{
	SAFE_LOCK(m_mutexCheckRole);

	acl::string strMsg;
	acl::string strLockValue;
	std::string l_strKey = m_strDefaultClassifyName + ":" + p_strKey;
	m_keyOperator.clear();
	bool bRet = m_keyOperator.exists(l_strKey.c_str());
	if (!bRet)
	{
		acl::string aclOut;
		const acl::redis_result* pResult = m_keyOperator.get_result();
		if (pResult)
		{
			pResult->to_string(aclOut);
		}
		ICC_LOG_DEBUG(m_pLog, "Redis[no exists key: %s, code:%s]", l_strKey.c_str(), aclOut.c_str());
	}
	else
	{
		ICC_LOG_DEBUG(m_pLog, "Redis[exists key: %s]", l_strKey.c_str());
	}

	return bRet;
}

int ICC::Redis::CRedisClientImpl::KeysPattern(const std::string& p_strKeyPattern, std::vector<std::string>* p_vecOutKeys)
{
	SAFE_LOCK(m_mutexCheckRole);

	m_keyOperator.clear();
	std::string l_strKey = m_strDefaultClassifyName + ":" + p_strKeyPattern;
	std::vector<acl::string> l_vecOut;
	int l_iRet = m_keyOperator.keys_pattern(l_strKey.c_str(), &l_vecOut);
	if (l_iRet > 0)
	{
		for (auto& it : l_vecOut)
		{
			if (p_vecOutKeys)
			{
				p_vecOutKeys->push_back(it.c_str());
			}
		}
	}
	else if (l_iRet < 0)
	{
		acl::string aclOut;
		const acl::redis_result* pResult = m_keyOperator.get_result();
		if (pResult)
		{
			pResult->to_string(aclOut);
		}
		ICC_LOG_ERROR(m_pLog, "Redis[%s]", aclOut.c_str());
	}

	return l_iRet;
}

int CRedisClientImpl::Del(const std::string& p_strKey)
{
	SAFE_LOCK(m_mutexCheckRole);

	acl::string strMsg;
	acl::string strLockValue;
	std::string l_strKey = m_strDefaultClassifyName + ":" + p_strKey;
	m_keyOperator.clear();
	int nRet = m_keyOperator.del(l_strKey.c_str());
	if (nRet < 0)
	{
		acl::string aclOut;
		const acl::redis_result* pResult = m_keyOperator.get_result();
		if (pResult)
		{
			pResult->to_string(aclOut);
		}
		ICC_LOG_ERROR(m_pLog, "del key: %s error: %s", l_strKey.c_str(), aclOut.c_str());
	}
	else
	{
		ICC_LOG_DEBUG(m_pLog, "Redis[del key: %s ok]", l_strKey.c_str());
	}

	return nRet;
}

int CRedisClientImpl::Expire(const std::string& p_strKey, const int p_iTTL)
{
	SAFE_LOCK(m_mutexCheckRole);

	acl::string strMsg;
	acl::string strLockValue;
	std::string l_strKey = m_strDefaultClassifyName + ":" + p_strKey;
	m_keyOperator.clear();
	int nRet = m_keyOperator.expire(l_strKey.c_str(), p_iTTL);
	if (nRet <= 0)
	{
		acl::string aclOut;
		const acl::redis_result* pResult = m_keyOperator.get_result();
		if (pResult)
		{
			pResult->to_string(aclOut);
		}
		ICC_LOG_ERROR(m_pLog, "Redis[set key: %s ttl error:%s]", l_strKey.c_str(), aclOut.c_str());
	}
	else
	{
		ICC_LOG_DEBUG(m_pLog, "Redis[set key: %s ttl: %d s ok]", l_strKey.c_str(), p_iTTL);
	}

	return nRet;
}

int CRedisClientImpl::TTL(const std::string& p_strKey)
{
	SAFE_LOCK(m_mutexCheckRole);

	acl::string strMsg;
	acl::string strLockValue;
	std::string l_strKey = m_strDefaultClassifyName + ":" + p_strKey;
	m_keyOperator.clear();
	int nRet = m_keyOperator.ttl(l_strKey.c_str());
	if (nRet < 0)
	{
		acl::string aclOut;
		const acl::redis_result* pResult = m_keyOperator.get_result();
		if (pResult)
		{
			pResult->to_string(aclOut);
		}
		ICC_LOG_WARNING(m_pLog, "Redis[get key: %s ttl error: %s]", l_strKey.c_str(), aclOut.c_str());
	}
	else
	{
		ICC_LOG_DEBUG(m_pLog, "Redis[get key: %s ttl: %d s ok]", l_strKey.c_str(), nRet);
	}

	return nRet;
}

bool CRedisClientImpl::Watch(const std::vector<std::string>& strkeys)
{
	SAFE_LOCK(m_mutexCheckRole);

	std::vector<acl::string> aclKeys;
	std::vector<std::string>::const_iterator iter = strkeys.begin();
	while (iter != strkeys.end())
	{
		std::string l_strKey = m_strDefaultClassifyName + ":" + *iter;
		aclKeys.push_back(l_strKey.c_str());
		iter++;
	}

	m_transaction.clear();
	if (!m_transaction.watch(aclKeys))
	{
		acl::string aclOut;
		const acl::redis_result* pResult = m_transaction.get_result();
		if (pResult)
		{
			pResult->to_string(aclOut);
		}
		ICC_LOG_ERROR(m_pLog, "Redis[%s]", aclOut.c_str());
		return false;
	}
	
	return true;
}

bool CRedisClientImpl::UnWatch(void)
{
	SAFE_LOCK(m_mutexCheckRole);

	m_transaction.clear();
	if (!m_transaction.unwatch())
	{
		acl::string aclOut;
		const acl::redis_result* pResult = m_transaction.get_result();
		if (pResult)
		{
			pResult->to_string(aclOut);
		}
		ICC_LOG_ERROR(m_pLog, "Redis[%s]", aclOut.c_str());
		return false;
	}

	return true;
}

bool CRedisClientImpl::Multi(void)
{
	SAFE_LOCK(m_mutexCheckRole);

	m_transaction.clear();
	if (!m_transaction.multi())
	{
		acl::string aclOut;
		const acl::redis_result* pResult = m_transaction.get_result();
		if (pResult)
		{
			pResult->to_string(aclOut);
		}
		ICC_LOG_ERROR(m_pLog, "Redis[%s]", aclOut.c_str());
		return false;
	}

	return true;
}

bool CRedisClientImpl::Exec(void)
{
	SAFE_LOCK(m_mutexCheckRole);

	m_transaction.clear();
	if (!m_transaction.exec())
	{
		acl::string aclOut;
		const acl::redis_result* pResult = m_transaction.get_result();
		if (pResult)
		{
			pResult->to_string(aclOut);
		}
		ICC_LOG_ERROR(m_pLog, "Redis[%s]", aclOut.c_str());
		return false;
	}

	return true;
}

bool CRedisClientImpl::Discard(void)
{
	SAFE_LOCK(m_mutexCheckRole);

	m_transaction.clear();
	if (!m_transaction.discard())
	{
		acl::string aclOut;
		const acl::redis_result* pResult = m_transaction.get_result();
		if (pResult)
		{
			pResult->to_string(aclOut);
		}
		ICC_LOG_ERROR(m_pLog, "Redis[%s]", aclOut.c_str());
		return false;
	}

	return true;
}

unsigned int CRedisClientImpl::GetSize(void)
{
	SAFE_LOCK(m_mutexCheckRole);

	return m_transaction.get_size();
}

bool CRedisClientImpl::GetChild(unsigned int i, std::string& strOut, std::string* strCmd/* = NULL*/)
{
	SAFE_LOCK(m_mutexCheckRole);

	acl::string aclCmd = nullptr == strCmd ? nullptr : acl::string(strCmd->c_str());
	const acl::redis_result* pResult = m_transaction.get_child(i, &aclCmd);
	if (NULL != pResult)
	{
		acl::string aclOut;
		acl::string aclResult = pResult->to_string(aclOut).c_str();

		strOut = aclResult.c_str();

		return true;
	}

	return false;
}

std::vector<std::string> CRedisClientImpl::GetCommands(void)
{
	SAFE_LOCK(m_mutexCheckRole);

	const std::vector<acl::string> vResult = m_transaction.get_commands();

	std::vector<std::string> vCmds;
	if (!vResult.empty())
	{
		std::vector<acl::string>::const_iterator iter = vResult.begin();
		while (iter != vResult.end())
		{
			vCmds.push_back(iter->c_str());

			iter++;
		}
	}

	return vCmds;
}

//////////////////////////////////////////////////////////////////////////

bool CRedisClientImpl::Set(const std::string& p_strKey, const std::string& strValue)
{
	SAFE_LOCK(m_mutexCheckRole);

	std::string l_strKey = m_strDefaultClassifyName + ":" + p_strKey;

	m_stringOperator.clear();
	bool bRet = m_stringOperator.set(l_strKey.c_str(), strValue.c_str());

	if (!bRet)
	{
		acl::string aclOut;
		const acl::redis_result* pResult = m_stringOperator.get_result();
		if (pResult)
		{
			pResult->to_string(aclOut);
		}
		ICC_LOG_ERROR(m_pLog, "Redis[set string key: %s error: %s]", l_strKey.c_str(), aclOut.c_str());
	}
	else
	{
		ICC_LOG_DEBUG(m_pLog, "Redis[set string key: %s ok, value: %s]", l_strKey.c_str(), strValue.c_str());
	}

	return bRet;
}

bool CRedisClientImpl::SetEx(const std::string& strKey, const std::string& strValue, unsigned int uTimeoutSeconds)
{
	SAFE_LOCK(m_mutexCheckRole);

	std::string l_strKey = m_strDefaultClassifyName + ":" + strKey;

	m_stringOperator.clear();
	bool bRet = m_stringOperator.setex(l_strKey.c_str(), strValue.c_str(), uTimeoutSeconds);

	if (!bRet)
	{
		acl::string aclOut;
		const acl::redis_result* pResult = m_stringOperator.get_result();
		if (pResult)
		{
			pResult->to_string(aclOut);
		}
		ICC_LOG_ERROR(m_pLog, "Redis[set string key: %s error: %s]", l_strKey.c_str(), aclOut.c_str());		
	}
	else
	{
		ICC_LOG_DEBUG(m_pLog, "Redis[set string key: %s ok, value: %s]", l_strKey.c_str(), strValue.c_str());
	}

	return bRet;
}

bool CRedisClientImpl::Get(const std::string& p_strKey, std::string& strValue)
{
	SAFE_LOCK(m_mutexCheckRole);

	acl::string strTempBuffer;
	std::string l_strKey = m_strDefaultClassifyName + ":" + p_strKey;
	strValue.clear();

	m_stringOperator.clear();
	bool bRet = m_stringOperator.get(l_strKey.c_str(), strTempBuffer);
	if (!bRet)
	{
		acl::string aclOut;
		const acl::redis_result* pResult = m_stringOperator.get_result();
		if (pResult)
		{
			pResult->to_string(aclOut);
		}
		ICC_LOG_ERROR(m_pLog, "Redis[get string key: %s error: %s]", l_strKey.c_str(), aclOut.c_str());
	}
	else
	{
		strValue = std::string(strTempBuffer.c_str());
		ICC_LOG_DEBUG(m_pLog, "Redis[get string key : %s ok, value : %s]", l_strKey.c_str(), strValue.c_str());
	}

	return bRet;
}

//////////////////////////////////////////////////////////////////////////
bool CRedisClientImpl::HSet(const char* p_pcKey, const char* p_strField, const char* p_strValue)
{
	SAFE_LOCK(m_mutexCheckRole);
	std::string l_strKey = m_strDefaultClassifyName + ":" + std::string(p_pcKey);

	m_hashOperator.clear();
	int l_iRet = m_hashOperator.hset(l_strKey.c_str(), p_strField, p_strValue);
	if (-1 == l_iRet)
	{
		acl::string aclOut;
		const acl::redis_result* pResult = m_hashOperator.get_result();
		if (pResult)
		{
			pResult->to_string(aclOut);
		}
		ICC_LOG_ERROR(m_pLog, "Redis[hset key:%s, error: %s]", l_strKey.c_str(), aclOut.c_str());
	}
	bool l_bRet = (l_iRet == -1) ? false : true;

	return l_bRet;
}

bool CRedisClientImpl::HSet(const std::string& p_strKey, const std::string& p_strField, std::string& p_strValue)
{
	SAFE_LOCK(m_mutexCheckRole);

	std::string l_strKey = m_strDefaultClassifyName + ":" + p_strKey;
	acl::string l_strValueBuf = p_strValue.c_str();

	m_hashOperator.clear();
	int l_iRet = m_hashOperator.hset(l_strKey.c_str(), p_strField.c_str(), l_strValueBuf);
	if (-1 == l_iRet)
	{
		acl::string aclOut;
		const acl::redis_result* pResult = m_hashOperator.get_result();
		if (pResult)
		{
			pResult->to_string(aclOut);
		}
		ICC_LOG_ERROR(m_pLog, "Redis[hset key:%s, error: %s]", l_strKey.c_str(), aclOut.c_str());
	}
	bool l_bRet = (l_iRet != -1);

	return l_bRet;
}

bool CRedisClientImpl::HMSet(const std::string& p_strKey, const std::map<std::string, std::string>& p_mapItem)
{
	SAFE_LOCK(m_mutexCheckRole);

	std::string l_strKey = m_strDefaultClassifyName + ":" + p_strKey;

	std::map<acl::string, acl::string> l_mapAclItem;
	for (auto it = p_mapItem.begin(); it != p_mapItem.end(); ++it)
	{
		l_mapAclItem.insert(std::make_pair(it->first.c_str(), it->second.c_str()));
	}

	m_hashOperator.clear();
	if (!m_hashOperator.hmset(l_strKey.c_str(), l_mapAclItem))
	{
		acl::string aclOut;
		const acl::redis_result* pResult = m_hashOperator.get_result();
		if (pResult)
		{
			pResult->to_string(aclOut);
		}
		ICC_LOG_ERROR(m_pLog, "Redis[hmset key:%s, error: %s]", l_strKey.c_str(), aclOut.c_str());
		return false;
	}
	
	return true;
}

bool CRedisClientImpl::HGet(const std::string& p_strKey, const std::string& p_strField, std::string& p_strValue)
{
	SAFE_LOCK(m_mutexCheckRole);

	std::string l_strKey = m_strDefaultClassifyName + ":" + p_strKey;
	acl::string l_strValueBuf;

	m_hashOperator.clear();
	bool l_bRet = m_hashOperator.hget(l_strKey.c_str(), p_strField.c_str(), l_strValueBuf);
	if (l_bRet)
	{
		p_strValue = std::string(l_strValueBuf.c_str());

		if (l_strValueBuf.empty())
		{
			l_bRet = false;
		}
	}
	else
	{
		acl::string aclOut;
		const acl::redis_result* pResult = m_hashOperator.get_result();
		if (pResult)
		{
			pResult->to_string(aclOut);
		}
		ICC_LOG_ERROR(m_pLog, "Redis[hget key:%s, error: %s]", l_strKey.c_str(), aclOut.c_str());
	}

	return l_bRet;
}

bool CRedisClientImpl::HGetAll(const std::string& p_strKey, std::map<std::string, std::string>& p_mapItem)
{
	SAFE_LOCK(m_mutexCheckRole);

	std::string l_strKey = m_strDefaultClassifyName + ":" + p_strKey;
	std::map<acl::string, acl::string> l_mapAclItem;

	m_hashOperator.clear();
	bool l_bRet = m_hashOperator.hgetall(l_strKey.c_str(), l_mapAclItem);
	if (l_bRet)
	{
		for (auto it = l_mapAclItem.begin(); it != l_mapAclItem.end(); ++it)
		{
			p_mapItem[it->first.c_str()] = it->second.c_str();
		}

		if (l_mapAclItem.size() == 0)
		{
			l_bRet = false;
		}
	}
	else
	{
		acl::string aclOut;
		const acl::redis_result* pResult = m_hashOperator.get_result();
		if (pResult)
		{
			pResult->to_string(aclOut);
		}
		ICC_LOG_ERROR(m_pLog, "Redis[hgetall key:%s, error: %s]", l_strKey.c_str(), aclOut.c_str());
	}

	return l_bRet;
}

bool CRedisClientImpl::HGetAllEx(const std::string& p_strKey, std::map<std::string, std::string>& p_mapItem)
{
	SAFE_LOCK(m_mutexCheckRole);

	std::string l_strKey = m_strDefaultClassifyName + ":" + p_strKey;
	std::map<acl::string, acl::string> l_mapAclItem;

	m_hashOperator.clear();
	bool l_bRet = m_hashOperator.hgetall(l_strKey.c_str(), l_mapAclItem);
	if (l_bRet)
	{
		for (auto it = l_mapAclItem.begin(); it != l_mapAclItem.end(); ++it)
		{
			p_mapItem[it->first.c_str()] = it->second.c_str();
		}

		if (l_mapAclItem.size() == 0)
		{
			l_bRet = true;
		}
	}
	else
	{
		acl::string aclOut;
		const acl::redis_result* pResult = m_hashOperator.get_result();
		if (pResult)
		{
			pResult->to_string(aclOut);
		}
		ICC_LOG_ERROR(m_pLog, "Redis[hgetall key:%s, error: %s]", l_strKey.c_str(), aclOut.c_str());
	}

	return l_bRet;
}

bool CRedisClientImpl::HDel(const std::string& p_strKey, const std::string& p_pcField)
{
	SAFE_LOCK(m_mutexCheckRole);

	std::string l_strKey = m_strDefaultClassifyName + ":" + p_strKey;

	m_hashOperator.clear();
	int l_iRet = m_hashOperator.hdel(l_strKey.c_str(), p_pcField.c_str());
	if (-1 == l_iRet)
	{
		acl::string aclOut;
		const acl::redis_result* pResult = m_hashOperator.get_result();
		if (pResult)
		{
			pResult->to_string(aclOut);
		}
		ICC_LOG_ERROR(m_pLog, "Redis[hdel key:%s, error: %s]", l_strKey.c_str(), aclOut.c_str());
	}

	return l_iRet != -1;
}

bool CRedisClientImpl::HDelFields(const std::string& p_strKey, const char* p_pcField, ...)
{
	SAFE_LOCK(m_mutexCheckRole);

	const char* l_pcName;
	std::vector<const char*> l_vecFields;
	l_vecFields.push_back(p_pcField);
	va_list ap;
	va_start(ap, p_pcField);
	while ((l_pcName = va_arg(ap, const char*)) != 0)
		l_vecFields.push_back(l_pcName);
	va_end(ap);

	std::string l_strKey = m_strDefaultClassifyName + ":" + p_strKey;

	m_hashOperator.clear();
	int l_nRet = m_hashOperator.hdel_fields(l_strKey.c_str(), l_vecFields);
	if (-1 == l_nRet)
	{
		acl::string aclOut;
		const acl::redis_result* pResult = m_hashOperator.get_result();
		if (pResult)
		{
			pResult->to_string(aclOut);
		}
		ICC_LOG_ERROR(m_pLog, "Redis[hdel_fields key:%s, error: %s]", l_strKey.c_str(), aclOut.c_str());
	}

	return l_nRet == l_vecFields.size();
}

bool ICC::Redis::CRedisClientImpl::HDelFields(const std::string& p_strKey, const std::vector<std::string>& p_vecFields)
{
	SAFE_LOCK(m_mutexCheckRole);

	std::string l_strKey = m_strDefaultClassifyName + ":" + p_strKey;
	std::vector<acl::string> l_vecFields;
	for (const std::string field : p_vecFields)
	{
		l_vecFields.push_back(field.c_str());
	}

	m_hashOperator.clear();
	int l_nRet = m_hashOperator.hdel_fields(l_strKey.c_str(), l_vecFields);
	if (-1 == l_nRet)
	{
		acl::string aclOut;
		const acl::redis_result* pResult = m_hashOperator.get_result();
		if (pResult)
		{
			pResult->to_string(aclOut);
		}
		ICC_LOG_ERROR(m_pLog, "Redis[hdel_fields key:%s, error: %s]", l_strKey.c_str(), aclOut.c_str());
	}

	return l_nRet == l_vecFields.size();
}

bool CRedisClientImpl::HExists(const std::string& p_strKey, const std::string& p_pcField)
{
	SAFE_LOCK(m_mutexCheckRole);

	std::string l_strKey = m_strDefaultClassifyName + ":" + p_strKey;

	m_hashOperator.clear();
	if (!m_hashOperator.hexists(l_strKey.c_str(), p_pcField.c_str()))
	{
		acl::string aclOut;
		const acl::redis_result* pResult = m_hashOperator.get_result();
		if (pResult)
		{
			pResult->to_string(aclOut);
		}
		ICC_LOG_DEBUG(m_pLog, "Redis[no hexists key:%s, fild:%s, code: %s]", l_strKey.c_str(), p_pcField.c_str(), aclOut.c_str());
		return false;
	}

	return true;
}

int CRedisClientImpl::HScan(const std::string& p_strKey, int p_iCursor, std::map<std::string, std::string>& p_mapOut,
	const std::string& p_strPattern, const unsigned int* p_uiCount)
{
	SAFE_LOCK(m_mutexCheckRole);

	const char* l_pPattern = p_strPattern.empty() ? nullptr : p_strPattern.c_str();
	std::map<acl::string, acl::string> l_mapOut;
	std::string l_strKey = m_strDefaultClassifyName + ":" + p_strKey;

	m_hashOperator.clear();
	int l_iRet = m_hashOperator.hscan(l_strKey.c_str(), p_iCursor, l_mapOut, l_pPattern, (size_t*)p_uiCount);
	if (l_iRet != -1)
	{
		for (auto it = l_mapOut.begin(); it != l_mapOut.end(); ++it)
		{
			p_mapOut[std::string(it->first)] = it->second;
		}
	}
	else
	{
		acl::string aclOut;
		const acl::redis_result* pResult = m_hashOperator.get_result();
		if (pResult)
		{
			pResult->to_string(aclOut);
		}
		ICC_LOG_DEBUG(m_pLog, "Redis[hscan key:%s, error: %s]", l_strKey.c_str(), aclOut.c_str());
	}

	return l_iRet;
}

//////////////////////////////////////////////////////////////////////////

int CRedisClientImpl::LLen(const std::string& p_strKey)
{
	SAFE_LOCK(m_mutexCheckRole);

	acl::string strMsg;
	acl::string strLockValue;
	std::string l_strKey = m_strDefaultClassifyName + ":" + p_strKey;

	m_listOperator.clear();
	int nRet = m_listOperator.llen(l_strKey.c_str());
	if (nRet < 0)
	{
		acl::string aclOut;
		const acl::redis_result* pResult = m_listOperator.get_result();
		if (pResult)
		{
			pResult->to_string(aclOut);
		}
		ICC_LOG_ERROR(m_pLog, "Redis[get list key: %s len error: %s]", l_strKey.c_str(), aclOut.c_str());
	}
	else
	{
		ICC_LOG_DEBUG(m_pLog, "Redis[get list key: %s len: %d ok]", l_strKey.c_str(), nRet);
	}

	return nRet;
}

int CRedisClientImpl::RPush(const std::string& p_strKey, const std::vector<std::string>& p_vecValues)
{
	SAFE_LOCK(m_mutexCheckRole);

	acl::string strMsg;
	acl::string strLockValue;

	std::vector<acl::string> strTempValues;
	std::vector<std::string>::const_iterator citr = p_vecValues.begin();
	for (; citr != p_vecValues.end(); ++citr)
	{
		strTempValues.push_back(acl::string((*citr).c_str()));
	}

	std::string l_strKey = m_strDefaultClassifyName + ":" + p_strKey;

	m_listOperator.clear();
	int nRet = m_listOperator.rpush(l_strKey.c_str(), strTempValues);
	if (-1 == nRet)
	{
		acl::string aclOut;
		const acl::redis_result* pResult = m_listOperator.get_result();
		if (pResult)
		{
			pResult->to_string(aclOut);
		}
		ICC_LOG_ERROR(m_pLog, "Redis[push list key: %s error: %s]", l_strKey.c_str(), aclOut.c_str());
	}
	else
	{
		ICC_LOG_DEBUG(m_pLog, "Redis[push list key: %s ok, values size: %d]", l_strKey.c_str(), nRet);
	}

	return nRet;
}

bool CRedisClientImpl::LIndex(const std::string& p_strKey, unsigned int p_uiIndex, std::string& p_strResult)
{
	SAFE_LOCK(m_mutexCheckRole);

	acl::string strMsg;
	acl::string strTempBuffer;
	acl::string strLockValue;
	std::string l_strKey = m_strDefaultClassifyName + ":" + p_strKey;

	m_listOperator.clear();
	bool bRet = m_listOperator.lindex(l_strKey.c_str(), p_uiIndex, strTempBuffer);
	if (!bRet)
	{
		acl::string aclOut;
		const acl::redis_result* pResult = m_listOperator.get_result();
		if (pResult)
		{
			pResult->to_string(aclOut);
		}
		ICC_LOG_ERROR(m_pLog, "Redis[get list index: %d key: %s error: %s]", p_uiIndex, l_strKey.c_str(), aclOut.c_str());
	}
	else
	{
		p_strResult = std::string(strTempBuffer.c_str());
		ICC_LOG_DEBUG(m_pLog, "Redis[get list index: %d key: %s ok, value: %s]", p_uiIndex, l_strKey.c_str(), p_strResult.c_str());
	}

	return bRet;
}

bool CRedisClientImpl::LRange(const std::string& p_strKey, int p_iStart, int p_iEnd, std::vector<std::string>& p_vesResult)
{
	SAFE_LOCK(m_mutexCheckRole);

	acl::string strMsg;
	acl::string strLockValue;
	std::vector<acl::string> strTempResult;
	std::string l_strKey = m_strDefaultClassifyName + ":" + p_strKey;

	m_listOperator.clear();
	bool bRet = m_listOperator.lrange(l_strKey.c_str(), p_iStart, p_iEnd, &strTempResult);
	if (!bRet)
	{
		acl::string aclOut;
		const acl::redis_result* pResult = m_listOperator.get_result();
		if (pResult)
		{
			pResult->to_string(aclOut);
		}
		ICC_LOG_ERROR(m_pLog, "Redis[get list rang: %d-%d key: %s error: %s]", p_iStart, p_iEnd, l_strKey.c_str(), aclOut.c_str());
	}
	else
	{
		std::vector<acl::string>::const_iterator citr = strTempResult.begin();
		for (; citr != strTempResult.end(); ++citr)
		{
			p_vesResult.push_back(std::string((*citr).c_str()));
		}
		ICC_LOG_DEBUG(m_pLog, "Redis[get list rang: %d-%d key: %s ok]", p_iStart, p_iEnd, l_strKey.c_str());
	}

	return bRet;
}

bool CRedisClientImpl::LSet(const std::string& p_strKey, unsigned int p_uiIndex, const std::string& p_strValue)
{
	SAFE_LOCK(m_mutexCheckRole);

	acl::string strMsg;
	acl::string strLockValue;
	std::string l_strKey = m_strDefaultClassifyName + ":" + p_strKey;

	m_listOperator.clear();
	bool bRet = m_listOperator.lset(l_strKey.c_str(), p_uiIndex, p_strValue.c_str());
	if (!bRet)
	{
		acl::string aclOut;
		const acl::redis_result* pResult = m_listOperator.get_result();
		if (pResult)
		{
			pResult->to_string(aclOut);
		}
		ICC_LOG_ERROR(m_pLog, "Redis[set list index: %d key: %s error: %s]", p_uiIndex, l_strKey.c_str(), aclOut.c_str());
	}
	else
	{
		ICC_LOG_DEBUG(m_pLog, "Redis[set list index: %d key: %s value: %s ok]", p_uiIndex, l_strKey.c_str(), p_strValue.c_str());
	}

	return bRet;
}

int CRedisClientImpl::LRem(const std::string& p_strKey, int p_iCount, const std::string& p_strValue)
{
	SAFE_LOCK(m_mutexCheckRole);

	acl::string strMsg;
	acl::string strLockValue;
	std::string l_strKey = m_strDefaultClassifyName + ":" + p_strKey;

	m_listOperator.clear();
	int nRet = m_listOperator.lrem(l_strKey.c_str(), p_iCount, p_strValue.c_str());
	if (-1 == nRet)
	{
		acl::string aclOut;
		const acl::redis_result* pResult = m_listOperator.get_result();
		if (pResult)
		{
			pResult->to_string(aclOut);
		}
		ICC_LOG_ERROR(m_pLog, "Redis[remove list key: %s error: %s]", l_strKey.c_str(), aclOut.c_str());
	}
	else
	{
		ICC_LOG_DEBUG(m_pLog, "Redis[remove list key: %s value: %s ret: %d ok]", l_strKey.c_str(), p_strValue.c_str(), nRet);
	}

	return nRet;
}

//////////////////////////////////////////////////////////////////////////

int CRedisClientImpl::SAdd(const std::string& p_strKey, const std::vector<std::string>& p_vecMembers)
{
	SAFE_LOCK(m_mutexCheckRole);

	acl::string strMsg;
	acl::string strLockValue;

	std::vector<acl::string> strTempMembers;
	std::vector<std::string>::const_iterator citr = p_vecMembers.begin();
	for (; citr != p_vecMembers.end(); ++citr)
	{
		strTempMembers.push_back(acl::string((*citr).c_str()));
	}

	std::string l_strKey = m_strDefaultClassifyName + ":" + p_strKey;
	m_setOperator.clear();
	int nRet = m_setOperator.sadd(l_strKey.c_str(), strTempMembers);
	if (-1 == nRet)
	{
		acl::string aclOut;
		const acl::redis_result* pResult = m_setOperator.get_result();
		if (pResult)
		{
			pResult->to_string(aclOut);
		}
		ICC_LOG_ERROR(m_pLog, "Redis[add set key: %s error: %s]", l_strKey.c_str(), aclOut.c_str());
	}
	else
	{
		ICC_LOG_DEBUG(m_pLog, "Redis[add set key: %s ok, values size: %d]", l_strKey.c_str(), nRet);
	}

	return nRet;
}

int CRedisClientImpl::SCard(const std::string& p_strKey)
{
	SAFE_LOCK(m_mutexCheckRole);

	acl::string strMsg;
	acl::string strLockValue;
	std::string l_strKey = m_strDefaultClassifyName + ":" + p_strKey;

	m_setOperator.clear();
	int nRet = m_setOperator.scard(l_strKey.c_str());
	if (-1 == nRet)
	{
		acl::string aclOut;
		const acl::redis_result* pResult = m_setOperator.get_result();
		if (pResult)
		{
			pResult->to_string(aclOut);
		}
		ICC_LOG_ERROR(m_pLog, "Redis[get set card key: %s error: %s]", l_strKey.c_str(), aclOut.c_str());
	}
	else
	{
		ICC_LOG_DEBUG(m_pLog, "Redis[get set card key: %s ok, size: %d]", l_strKey.c_str(), nRet);
	}

	return nRet;
}

int CRedisClientImpl::SMembers(const std::string& p_strKey, std::vector<std::string>& p_vecResult)
{
	SAFE_LOCK(m_mutexCheckRole);

	acl::string strMsg;
	acl::string strLockValue;
	std::vector<acl::string> strTempMembers;

	std::string l_strKey = m_strDefaultClassifyName + ":" + p_strKey;

	m_setOperator.clear();
	int nRet = m_setOperator.smembers(l_strKey.c_str(), &strTempMembers);
	if (-1 == nRet)
	{
		acl::string aclOut;
		const acl::redis_result* pResult = m_setOperator.get_result();
		if (pResult)
		{
			pResult->to_string(aclOut);
		}
		ICC_LOG_ERROR(m_pLog, "Redis[get set members key: %s error: %s]", l_strKey.c_str(), aclOut.c_str());
	}
	else
	{
		std::vector<acl::string>::const_iterator citr = strTempMembers.begin();
		for (; citr != strTempMembers.end(); ++citr)
		{
			p_vecResult.push_back(std::string((*citr).c_str()));
		}

		ICC_LOG_DEBUG(m_pLog, "Redis[get set members key: %s, members size: %d]", l_strKey.c_str(), nRet);
	}

	return nRet;
}

int CRedisClientImpl::SRem(const std::string& p_strKey, const std::vector<std::string>& p_vecMembers)
{
	SAFE_LOCK(m_mutexCheckRole);

	acl::string strMsg;
	acl::string strLockValue;

	std::vector<acl::string> strTempMembers;
	std::vector<std::string>::const_iterator citr = p_vecMembers.begin();
	for (; citr != p_vecMembers.end(); ++citr)
	{
		strTempMembers.push_back(acl::string((*citr).c_str()));
	}

	std::string l_strKey = m_strDefaultClassifyName + ":" + p_strKey;

	m_setOperator.clear();
	int nRet = m_setOperator.srem(l_strKey.c_str(), strTempMembers);
	if (-1 == nRet)
	{
		acl::string aclOut;
		const acl::redis_result* pResult = m_setOperator.get_result();
		if (pResult)
		{
			pResult->to_string(aclOut);
		}
		ICC_LOG_ERROR(m_pLog, "Redis[remove set key: %s error: %s]", l_strKey.c_str(), aclOut.c_str());
	}
	else
	{
		ICC_LOG_DEBUG(m_pLog, "Redis[remove set key: %s ret: %d]", l_strKey.c_str(), nRet);
	}

	return nRet;
}

bool CRedisClientImpl::SIsMember(const std::string& p_strKey, const std::string& p_strMember)
{
	SAFE_LOCK(m_mutexCheckRole);

	acl::string l_strMsg;
	acl::string l_strLockValue;
	std::string l_strKey = m_strDefaultClassifyName + ":" + p_strKey;

	m_setOperator.clear();
	bool bRet = m_setOperator.sismember(l_strKey.c_str(), p_strMember.c_str());
	if (false == bRet)
	{
		acl::string aclOut;
		const acl::redis_result* pResult = m_setOperator.get_result();
		if (pResult)
		{
			pResult->to_string(aclOut);
		}
		ICC_LOG_ERROR(m_pLog, "%s is not a member of the %s,error:%s", p_strMember.c_str(), l_strKey.c_str(), aclOut.c_str());
	}
	else
	{
		ICC_LOG_DEBUG(m_pLog, "%s is a member of the %s", p_strMember.c_str(), l_strKey.c_str());
	}

	return bRet;
}

int CRedisClientImpl::SDiffStore(const std::string& p_strKey, const std::string& p_strFirstKey, const std::string& p_strSecondKey)
{
	SAFE_LOCK(m_mutexCheckRole);

	acl::string l_strMsg;
	acl::string l_strFirstLockValue;
	acl::string l_strSecondLockValue;
	std::string l_strKey = m_strDefaultClassifyName + ":" + p_strKey;
	std::string l_strFirstKey = m_strDefaultClassifyName + ":" + p_strFirstKey;
	std::string l_strSecondKey = m_strDefaultClassifyName + ":" + p_strSecondKey;

	m_setOperator.clear();
	int l_iRet = m_setOperator.sdiffstore(l_strKey.c_str(), l_strFirstKey.c_str(), l_strSecondKey.c_str(), nullptr);
	if (-1 == l_iRet)
	{
		acl::string aclOut;
		const acl::redis_result* pResult = m_setOperator.get_result();
		if (pResult)
		{
			pResult->to_string(aclOut);
		}
		ICC_LOG_ERROR(m_pLog, "Redis[get set diff between %s and %s error: %s]", l_strFirstKey.c_str(), p_strSecondKey.c_str(), aclOut.c_str());
	}
	else
	{
		ICC_LOG_DEBUG(m_pLog, "Redis[get set diff between %s and %s success, members size: %d]", l_strFirstKey.c_str(), p_strSecondKey.c_str(), l_iRet);
	}
	
	return l_iRet;
}

int CRedisClientImpl::SInterStore(const std::string& p_strKey, const std::string& p_strFirstKey, const std::string& p_strSecondKey)
{
	SAFE_LOCK(m_mutexCheckRole);

	acl::string l_strMsg;
	acl::string l_strFirstLockValue;
	acl::string l_strSecondLockValue;

	std::string l_strKey = m_strDefaultClassifyName + ":" + p_strKey;
	std::string l_strFirstKey = m_strDefaultClassifyName + ":" + p_strFirstKey;
	std::string l_strSecondKey = m_strDefaultClassifyName + ":" + p_strSecondKey;

	m_setOperator.clear();
	int l_iRet = m_setOperator.sinterstore(l_strKey.c_str(), l_strFirstKey.c_str(), l_strSecondKey.c_str(), nullptr);
	if (-1 == l_iRet)
	{
		acl::string aclOut;
		const acl::redis_result* pResult = m_setOperator.get_result();
		if (pResult)
		{
			pResult->to_string(aclOut);
		}
		ICC_LOG_ERROR(m_pLog, "Redis[get set inter between %s and %s error: %s]", l_strFirstKey.c_str(), l_strSecondKey.c_str(), aclOut.c_str());
	}
	else
	{
		ICC_LOG_DEBUG(m_pLog, "Redis[get set inter between %s and %s success, members size: %d]", l_strFirstKey.c_str(), l_strSecondKey.c_str(), l_iRet);
	}
	
	return l_iRet;
}

int CRedisClientImpl::SUnionStore(const std::string& p_strKey, const std::string& p_strFirstKey, const std::string& p_strSecondKey)
{
	SAFE_LOCK(m_mutexCheckRole);

	acl::string l_strMsg;
	acl::string l_strFirstLockValue;
	acl::string l_strSecondLockValue;

	std::string l_strKey = m_strDefaultClassifyName + ":" + p_strKey;
	std::string l_strFirstKey = m_strDefaultClassifyName + ":" + p_strFirstKey;
	std::string l_strSecondKey = m_strDefaultClassifyName + ":" + p_strSecondKey;

	m_setOperator.clear();
	int l_iRet = m_setOperator.sunionstore(l_strKey.c_str(), l_strFirstKey.c_str(), l_strSecondKey.c_str(), nullptr);
	if (-1 == l_iRet)
	{
		acl::string aclOut;
		const acl::redis_result* pResult = m_setOperator.get_result();
		if (pResult)
		{
			pResult->to_string(aclOut);
		}
		ICC_LOG_ERROR(m_pLog, "Redis[get set union between %s and %s error: %s]", l_strFirstKey.c_str(), l_strSecondKey.c_str(), aclOut.c_str());
	}
	else
	{
		ICC_LOG_DEBUG(m_pLog, "Redis[get set union between %s and %s success, members size: %d]", l_strFirstKey.c_str(), l_strSecondKey.c_str(), l_iRet);
	}
	
	return l_iRet;
}

IResourceManagerPtr ICC::Redis::CRedisClientImpl::GetResourceManager()
{
	return m_pResourceManager;
}

int ICC::Redis::CRedisClientImpl::ZAdd(const std::string p_strKey, const std::vector<std::pair<const char*, double> >&vMembers)
{
	SAFE_LOCK(m_mutexCheckRole);

	std::string l_strKey = m_strDefaultClassifyName + ":" + p_strKey;

	m_zsetOperator.clear();
	int nRet = m_zsetOperator.zadd(l_strKey.c_str(), vMembers);
	if (-1 == nRet)
	{
		acl::string aclOut;
		const acl::redis_result* pResult = m_zsetOperator.get_result();
		if (pResult)
		{
			pResult->to_string(aclOut);
		}
		ICC_LOG_ERROR(m_pLog, "ZAdd[%s],error[%s]", l_strKey.c_str(), aclOut.c_str());
	}

	return nRet;
}

int ICC::Redis::CRedisClientImpl::ZCard(std::string p_strKey)
{
	SAFE_LOCK(m_mutexCheckRole);

	std::string l_strKey = m_strDefaultClassifyName + ":" + p_strKey;

	m_zsetOperator.clear();
	int nRet = m_zsetOperator.zcard(l_strKey.c_str());

	if (-1 == nRet)
	{
		acl::string aclOut;
		const acl::redis_result* pResult = m_zsetOperator.get_result();
		if (pResult)
		{
			pResult->to_string(aclOut);
		}
		ICC_LOG_ERROR(m_pLog, "Redis[%s]", aclOut.c_str());
	}

	return nRet;
}

int ICC::Redis::CRedisClientImpl::ZCount(const std::string p_strKey, double dMin, double dMax)
{
	SAFE_LOCK(m_mutexCheckRole);

	std::string l_strKey = m_strDefaultClassifyName + ":" + p_strKey;

	m_zsetOperator.clear();
	int nRet = m_zsetOperator.zcount(l_strKey.c_str(), dMin, dMax);
	if (-1 == nRet)
	{
		acl::string aclOut;
		const acl::redis_result* pResult = m_zsetOperator.get_result();
		if (pResult)
		{
			pResult->to_string(aclOut);
		}
		ICC_LOG_ERROR(m_pLog, "Redis[%s]", aclOut.c_str());
	}

	return nRet;
}

bool ICC::Redis::CRedisClientImpl::ZIncrby(const std::string p_strKey, double dInc, const std::string strMember, double* pResult /*= NULL*/)
{
	SAFE_LOCK(m_mutexCheckRole);

	std::string l_strKey = m_strDefaultClassifyName + ":" + p_strKey;

	m_zsetOperator.clear();
	bool bRet = m_zsetOperator.zincrby(l_strKey.c_str(), dInc, strMember.c_str(), pResult);
	if (!bRet)
	{
		acl::string aclOut;
		const acl::redis_result* pResult = m_zsetOperator.get_result();
		if (pResult)
		{
			pResult->to_string(aclOut);
		}
		ICC_LOG_ERROR(m_pLog, "Redis[%s]", aclOut.c_str());
	}

	return bRet;
}

bool ICC::Redis::CRedisClientImpl::ZIncrby(const std::string p_strKey, double dInc, const std::string strMember, unsigned int sLen, double* pResult /*= NULL*/)
{
	SAFE_LOCK(m_mutexCheckRole);

	std::string l_strKey = m_strDefaultClassifyName + ":" + p_strKey;

	m_zsetOperator.clear();
	bool bRet = m_zsetOperator.zincrby(l_strKey.c_str(), dInc, strMember.c_str(), sLen, pResult);
	if (!bRet)
	{
		acl::string aclOut;
		const acl::redis_result* pResult = m_zsetOperator.get_result();
		if (pResult)
		{
			pResult->to_string(aclOut);
		}
		ICC_LOG_ERROR(m_pLog, "Redis[%s]", aclOut.c_str());
	}

	return bRet;
}

int ICC::Redis::CRedisClientImpl::ZRange(const std::string p_strKey, int nStart, int nStop, std::vector<std::string>* pResult)
{
	SAFE_LOCK(m_mutexCheckRole);

	std::string l_strKey = m_strDefaultClassifyName + ":" + p_strKey;

	m_zsetOperator.clear();
	std::vector<acl::string> vResult;
	vResult.clear();
	int nRet = m_zsetOperator.zrange(l_strKey.c_str(), nStart, nStop, &vResult);

	if (-1 != nRet)
	{
		std::vector<acl::string>::iterator iter = vResult.begin();
		while (iter != vResult.end())
		{
			acl::string strTmp = *iter;
			pResult->push_back(strTmp.c_str());

			iter++;
		}
	}
	else
	{
		acl::string aclOut;
		const acl::redis_result* pResult = m_zsetOperator.get_result();
		if (pResult)
		{
			pResult->to_string(aclOut);
		}
		ICC_LOG_ERROR(m_pLog, "Redis[%s]", aclOut.c_str());
	}

	return nRet;
}

int ICC::Redis::CRedisClientImpl::ZRangeWithScores(std::string p_strKey, int nStart, int nStop, std::vector<std::pair<std::string, double>>& vOut)
{
	SAFE_LOCK(m_mutexCheckRole);

	std::string l_strKey = m_strDefaultClassifyName + ":" + p_strKey;

	m_zsetOperator.clear();
	std::vector<std::pair<acl::string, double>> result;
	result.clear();
	int nRet = m_zsetOperator.zrange_with_scores(l_strKey.c_str(), nStart, nStop, result);

	if (-1 != nRet)
	{
		std::vector<std::pair<acl::string, double>>::iterator iter = result.begin();
		while (iter != result.end())
		{
			std::pair<acl::string, double> pairTmp = *iter;
			vOut.push_back(std::make_pair(pairTmp.first.c_str(), pairTmp.second));

			iter++;
		}
	}
	else
	{
		acl::string aclOut;
		const acl::redis_result* pResult = m_zsetOperator.get_result();
		if (pResult)
		{
			pResult->to_string(aclOut);
		}
		ICC_LOG_ERROR(m_pLog, "Redis[%s]", aclOut.c_str());
	}

	return nRet;
}

int ICC::Redis::CRedisClientImpl::ZRem(std::string p_strKey, const std::vector<std::string>& vMembers)
{
	SAFE_LOCK(m_mutexCheckRole);

	std::string l_strKey = m_strDefaultClassifyName + ":" + p_strKey;

	m_zsetOperator.clear();
	std::vector<acl::string> members;
	std::vector<std::string>::const_iterator iter = vMembers.begin();
	while (iter != vMembers.end())
	{
		members.push_back(iter->c_str());
		iter++;
	}

	int nRet = m_zsetOperator.zrem(l_strKey.c_str(), members);
	if (-1 == nRet)
	{
		acl::string aclOut;
		const acl::redis_result* pResult = m_zsetOperator.get_result();
		if (pResult)
		{
			pResult->to_string(aclOut);
		}
		ICC_LOG_ERROR(m_pLog, "Redis[%s]", aclOut.c_str());
	}

	return nRet;
}

int ICC::Redis::CRedisClientImpl::ZRevrange(std::string p_strKey, int nStart, int nStop, std::vector<std::string>* pResult)
{
	SAFE_LOCK(m_mutexCheckRole);

	std::string l_strKey = m_strDefaultClassifyName + ":" + p_strKey;

	m_zsetOperator.clear();
	std::vector<acl::string> vResult;
	vResult.clear();
	int nRet = m_zsetOperator.zrange(l_strKey.c_str(), nStart, nStop, &vResult);

	if (-1 != nRet)
	{
		std::vector<acl::string>::iterator iter = vResult.begin();
		while (iter != vResult.end())
		{
			acl::string strTmp = *iter;
			pResult->push_back(strTmp.c_str());

			iter++;
		}
	}
	else
	{
		acl::string aclOut;
		const acl::redis_result* pResult = m_zsetOperator.get_result();
		if (pResult)
		{
			pResult->to_string(aclOut);
		}
		ICC_LOG_ERROR(m_pLog, "Redis[%s]", aclOut.c_str());
	}

	return nRet;
}

int ICC::Redis::CRedisClientImpl::ZRevrangeWithScores(std::string p_strKey, int nStart, int nStop, std::vector<std::pair<std::string, double>>& vOut)
{
	SAFE_LOCK(m_mutexCheckRole);

	std::string l_strKey = m_strDefaultClassifyName + ":" + p_strKey;

	m_zsetOperator.clear();
	std::vector<std::pair<acl::string, double>> result;
	result.clear();
	int nRet = m_zsetOperator.zrange_with_scores(l_strKey.c_str(), nStart, nStop, result);

	if (-1 != nRet)
	{
		std::vector<std::pair<acl::string, double>>::iterator iter = result.begin();
		while (iter != result.end())
		{
			std::pair<acl::string, double> pairTmp = *iter;
			vOut.push_back(std::make_pair(pairTmp.first.c_str(), pairTmp.second));

			iter++;
		}
	}
	else
	{
		acl::string aclOut;
		const acl::redis_result* pResult = m_zsetOperator.get_result();
		if (pResult)
		{
			pResult->to_string(aclOut);
		}
		ICC_LOG_ERROR(m_pLog, "Redis[%s]", aclOut.c_str());
	}

	return nRet;
}

bool ICC::Redis::CRedisClientImpl::AcquireLock(const char* p_pcKey, acl::string& p_strLockValue, int p_iAcquireTimeOut /*= 10*/, int p_iLockTTL /*= 10*/)
{
	SAFE_LOCK(m_mutexCheckRole);

	std::string strMsg;
	std::string strLockKey, strLockValue;
	strLockKey = std::string("lock_key:") + p_pcKey;
	strLockValue = std::string("lock_value:") + m_strClientGuid.c_str() + "-" + p_strLockValue.c_str();

	CClockTimer clock;
	while (clock.ElapsedMillionSec() < p_iAcquireTimeOut)
	{
		m_stringOperator.clear();
		int nRet = m_stringOperator.setnx(strLockKey.c_str(), strLockValue.c_str());
		if (nRet > 0)
		{	//设置成功，加锁成功
			m_keyOperator.clear();
			m_keyOperator.pexpire(strLockKey.c_str(), p_iLockTTL);

			return true;
		}
		else if (nRet == 0)
		{	//失败
			//Set TTL 
			m_keyOperator.clear();
			if (m_keyOperator.pttl(strLockKey.c_str()) == -1)
			{//当锁没有过期时间设置时，为其设置过期时间，以防获得锁却崩溃的客户端永远持有锁
				m_keyOperator.clear();
				m_keyOperator.pexpire(strLockKey.c_str(), p_iLockTTL);
			}
		}
		else
		{
			acl::string aclOut;
			const acl::redis_result* pResult = m_stringOperator.get_result();
			if (pResult)
			{
				pResult->to_string(aclOut);
			}
			ICC_LOG_ERROR(m_pLog, "Redis[setnx lock key: %s error: %s]", strLockKey.c_str(), aclOut.c_str());
			return false;
		}

		m_HelpTool->Sleep(1);
	}

	return false;
}

bool ICC::Redis::CRedisClientImpl::ReleaseLock(const char* p_pcKey, acl::string& p_strLockValue)
{
	SAFE_LOCK(m_mutexCheckRole);

	acl::string strTemp;
	std::string strLockKey, strLockValue;
	strLockKey = std::string("lock_key:") + p_pcKey;
	strLockValue = std::string("lock_value:") + m_strClientGuid.c_str() + "-" + p_strLockValue.c_str();

	m_stringOperator.clear();
	if (m_stringOperator.get(strLockKey.c_str(), strTemp))
	{
		if (strTemp.compare(strLockValue.c_str()) == 0)
		{
			m_keyOperator.clear();
			m_keyOperator.del(strLockKey.c_str());

			ICC_LOG_DEBUG(m_pLog, "Redis[release lock key: %s ok]", strLockKey.c_str());
		}
	}
	else
	{
		acl::string aclOut;
		const acl::redis_result* pResult = m_stringOperator.get_result();
		if (pResult)
		{
			pResult->to_string(aclOut);
		}
		ICC_LOG_ERROR(m_pLog, "Redis[release lock key: %s error: %s]", strLockKey.c_str(), aclOut.c_str());

		return false;
	}

	return true;
}