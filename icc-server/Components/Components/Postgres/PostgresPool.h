#pragma once

namespace ICC
{
	namespace DataBase
	{		
		struct HostInfo
		{
			std::string strIp;
			std::string strPort;

			HostInfo(){}
			HostInfo(const std::string& strIp, const std::string& strPort)
			{
				this->strIp = strIp;
				this->strPort = strPort;
			}
			HostInfo(const HostInfo& other)
			{
				strIp = other.strIp;
				strPort = other.strPort;
			}
			HostInfo& operator=(const HostInfo& other)
			{
				if (&other != this)
				{
					strIp = other.strIp;
					strPort = other.strPort;
				}
				return *this;
			}
		};


		typedef boost::shared_ptr<boost::thread> ThreadPtr;

		class PooledConnection//辅助类
		{
		public:
			PooledConnection(PGconn* pConn);
			PooledConnection(const PooledConnection& oth);
			PooledConnection& operator=(const PooledConnection& oth);
			~PooledConnection();
		public:
			PGconn* Get();
			void Set(PGconn* pConn);
			
			void SetIsBusy(bool busy);
			bool GetIsBusy();

			void SetNeedFree(bool bNeedFree);			

		private:
			PGconn*	m_pConnection;
			bool m_isBusy;
			bool m_bIsNeedFree;
			
		};

		typedef boost::shared_ptr<PooledConnection> PooledConnectionPtr;

		class PostgresqlPool
		{
			friend class PooledConnection;
		public:
			//对外接口
			static PostgresqlPool* Instance();
			bool                   Init(IResourceManagerPtr p_pResourceManager);
			PooledConnectionPtr    GetFreeRWConnection();
			PooledConnectionPtr    GetFreeRConnection();			
			void                   ReturnConnection(PGconn* pConn);

			//bool IsConnected();
			void                   CloseConnectionPool();
			unsigned int           AvailableWriteConnNums();
		public:
			IResourceManagerPtr    GetResourceManager();
			std::string            CurrentMasterHostIp();
			std::string            MasterHostIp();
			void                   UpdateMasterHostIp(const std::string& strMasterHostIp);
			void                   UpdateRConnections();

		private:
			bool        _CreateConnections();
			void        _CloseConnections();
			bool        _CreateRWConnections();
			void        _CloseRWConnections();
			bool        _CreateRConnections();
			void        _CloseRConnections();
			void        _UpdateRConnections();
			void        _ProcessOnlyOneHost();
			void        _CloseConnection(PGconn* pConn);

			PGconn*     _NewRWConnection();
			PGconn*     _NewRConnection();
			PGconn*     _NewConnection(const std::string& strConnectionInfo);
			bool        _Reconnect(PGconn* pConn);
			void        _ReturnRWConnection(PGconn* pConn);
			void        _ReturnRConnection(PGconn* pConn);

			bool        _TestConnection(PGconn* pConn);

			PooledConnectionPtr	    _FindFreeRWConnection();
			PooledConnectionPtr	    _FindFreeRConnection();
			
			std::string _ComposeBaseConnectionInfo(const std::vector<HostInfo>& vecIpInfos, const std::string& strUser, const std::string& strPwd, const std::string strDbName, const std::string& strAttr);
			std::string _ComposeRWConnectionInfo(const std::vector<HostInfo>& vecIpInfos, const std::string& strUser, const std::string& strPwd, const std::string strDbName);
			std::string _ComposeRConnectionInfo(const std::vector<HostInfo>& vecIpInfos, const std::string& strUser, const std::string& strPwd, const std::string strDbName);

			std::string _QueryCurrentMasterHostIp(const std::vector<HostInfo>& vecIpInfos, const std::string& strUser, const std::string& strPwd, const std::string strDbName);
			std::string _QueryHostIP(PGconn* pConn);
			std::string _MasterHostIp();
			void        _SetMasterHostIp(const std::string& strMasterHostIp);
			
			int         _CurrentEnableHostNum(const std::vector<HostInfo>& vecIpInfos, const std::string& strUser, const std::string& strPwd, const std::string strDbName);
			void        _SetEnableHostNum(int iNum);
			int         _EnableHostNum();

			void        _TestPrintConnectionInfo();

			int         _RConnectionNums();
			int         _RWConnectionNums();

		private:
			bool CreateConnections();
			PGconn*	NewConnection();
			void CloseConnection(PGconn* pConn);
			PGconn*	FindFreeConnection();
			PGresult* ExecQuery(std::string strSql);
			bool TestConnection(PGconn* pConn);
			void SafePQClear(PGresult *ret);
		private:
			//检测连接池
			void CheckDBPool(PostgresqlPool* p_pThread);
		private:
			static void S_CheckDBPool(PostgresqlPool* p_pThread);

		private:
			PostgresqlPool();
			~PostgresqlPool(void);
		private:
			static PostgresqlPool* sm_instance;
			static boost::mutex sm_mutexInstance;

		private:
			Log::ILogPtr m_pLog;
			ThreadPtr m_pCheckMainDBThread;
			ThreadPtr m_pCheckSlaveDBThread;
			StringUtil::IStringUtilPtr m_pString;
			HelpTool::IHelpToolPtr m_pHelpTool;
			IResourceManagerPtr m_pResourceManager;

		private://属性字段
			std::string		m_strDBName;			//数据库名
			std::string		m_strUserName;			//数据库用户名
			std::string		m_strPassWord;			//数据库用户密码

			unsigned int    m_iCommonNum;
			unsigned int	m_iInitNum;				//连接池的初始化大小
			unsigned int	m_iIncreatmentNum;		//连接池自增长的大小
			unsigned int	m_iMaxNum;				//连接池的最大大小
			bool			m_bIsConnected;			//数据库是否连接
			bool			m_bIsInit;				//是否已初始化
			std::string     m_strStatementTimeout;  //语句执行超时时间

			Lock::ILockPtr m_lock;					//获取连接的锁			
			std::vector<PooledConnectionPtr>	m_Connections;	//主数据库连接数池		


			Lock::ILockPtr m_lockRW;
			std::vector<PooledConnectionPtr>	m_RWConnections;

			Lock::ILockPtr m_lockR;
			std::vector<PooledConnectionPtr>	m_RConnections;
			
			Lock::ILockPtr m_lockSaved;
			std::vector<PooledConnectionPtr>	m_vecSavedRConnections;

			std::vector<HostInfo>  m_vecHostInfos;

			Lock::ILockPtr         m_lockMasterHostIp;
			std::string            m_strMasterHostIp;

			Lock::ILockPtr         m_lockEnableHostNum;
			int                    m_iEnableHostNum;

			bool                   m_bRWConnectionInitFlag;
			bool                   m_bRConnectionInitFlag;
		};
	}
}

