#pragma once
#include <string>
#include <acl_cpp/lib_acl.hpp>
#include <initializer_list>
#include <Redisclient/IRedisclient.h>

namespace ICC
{
	namespace Redis
	{
		typedef struct S_REDIS_CLIENT				//	�ڱ��ͻ���
		{
			std::string			m_strServerRole;
			acl::redis_client	*m_pSingleConn;
		}SRedisClient;

		class CRedisClientImpl : public IRedisClient
		{
		public:
			CRedisClientImpl(IResourceManagerPtr p_ResourceManagerPtr);
			virtual ~CRedisClientImpl();

		public:
			virtual bool Connect(const std::string& p_strAddress, bool p_bClusterMode, int p_iKeyTTL);

			virtual void Disconnect();

			virtual bool ServiceLock(const std::string& p_pcKey, std::string& p_strLockValue, int p_iLockTTL, int p_iTimeOut, std::string& p_strError);

			virtual bool Exists(const std::string& p_strKey);

			virtual int KeysPattern(const std::string& p_strKeyPattern, std::vector<std::string>* p_vecOutKeys);

			virtual int Del(const std::string& p_strKey);

			virtual int Expire(const std::string& p_strKey, const int p_iTTL);

			virtual int TTL(const std::string& p_strKey);
			//////////////////////////////////////////////////////////////////////////
			virtual bool Watch(const std::vector<std::string>& strkeys);
			
			virtual bool UnWatch(void);
			
			virtual bool Multi(void);
			
			virtual bool Exec(void);
			
			virtual bool Discard(void);
			
			virtual unsigned int GetSize(void);
			
			virtual bool GetChild(unsigned int i, std::string& strOut, std::string* strCmd = NULL);

			virtual std::vector<std::string> GetCommands(void);

			//////////////////////////////////////////////////////////////////////////

			virtual bool Set(const std::string& p_strKey, const std::string& strValue);
			virtual bool SetEx(const std::string& strKey, const std::string& strValue, unsigned int uTimeoutSeconds);

			virtual bool Get(const std::string& p_strKey, std::string& strValue);

			//////////////////////////////////////////////////////////////////////////

			virtual bool HSet(const char* p_pcKey, const char* p_strField, const char* p_strValue);

			virtual bool HSet(const std::string& p_pcKey, const std::string& p_strField, std::string& p_strValue);

			virtual bool HMSet(const std::string& p_pcKey, const std::map<std::string, std::string>& p_mapItem);

			virtual bool HGet(const std::string& p_pcKey, const std::string& p_strField, std::string& p_strValue);

			virtual bool HGetAll(const std::string& p_pcKey, std::map<std::string, std::string>& p_mapItem);
			virtual bool HGetAllEx(const std::string& p_strKey, std::map<std::string, std::string>& p_mapItem);

			virtual bool HDel(const std::string& p_strKey, const std::string& p_strField);

			virtual bool HDelFields(const std::string& p_strKey, const char* p_pcField, ...);

			virtual bool HDelFields(const std::string& p_strKey, const std::vector<std::string>& p_vecFields);

			virtual bool HExists(const std::string& p_pcKey, const std::string& p_pcField);

			virtual int HScan(const std::string& p_strKey, int p_iCursor, std::map<std::string, std::string>& p_mapOut, const std::string& p_strPattern = "", const unsigned int* p_uiCount = nullptr);

			virtual int LLen(const std::string& p_strKey);

			virtual int RPush(const std::string& p_strKey, const std::vector<std::string>& p_vecValues);

			virtual bool LIndex(const std::string& p_strKey, unsigned int p_uiIndex, std::string& p_strResult);

			virtual bool LRange(const std::string& p_strKey, int p_iStart, int p_iEnd, std::vector<std::string>& p_vesResult);

			virtual bool LSet(const std::string& p_strKey, unsigned int p_uiIndex, const std::string& p_strValue);

			virtual int LRem(const std::string& p_strKey, int p_iCount, const std::string& p_strValue);

			virtual int SAdd(const std::string& p_strKey, const std::vector<std::string>& p_vecMembers);

			virtual int SCard(const std::string& p_strKey);

			virtual int SMembers(const std::string& p_strKey, std::vector<std::string>& p_vecResult);

			virtual int SRem(const std::string& p_strKey, const std::vector<std::string>& p_vecMembers);

			virtual bool SIsMember(const std::string& p_strKey, const std::string& p_strMember);

			virtual int SDiffStore(const std::string& p_strDstKey, const std::string& p_strFirstKey, const std::string& p_strSecondKey);

			virtual int SInterStore(const std::string& p_strDstKey, const std::string& p_strFirstKey, const std::string& p_strSecondKey);

			virtual int SUnionStore(const std::string& p_strDstKey, const std::string& p_strFirstKey, const std::string& p_strSecondKey);

			//////////////////////////////////////////////////////////////////////////

			virtual int ZAdd(const std::string p_strKey, const std::vector<std::pair<const char*, double> >&vMembers);
			
			virtual int ZCard(const std::string p_strKey);
			
			virtual int ZCount(const std::string p_strKey, double dMin, double dMax);
			
			virtual bool ZIncrby(const std::string p_strKey, double dInc, const std::string strMember, double* pResult = NULL);

			virtual bool ZIncrby(const std::string p_strKey, double dInc, const std::string strMember, unsigned int sLen, double* pResult = NULL);
			
			virtual int ZRange(const std::string p_strKey, int nStart, int nStop, std::vector<std::string>* vResult);
			
			virtual int ZRangeWithScores(std::string p_strKey, int nStart, int nStop, std::vector<std::pair<std::string, double>>& vOut);
			
			virtual int ZRem(std::string p_strKey, const std::vector<std::string>& vMembers);
			
			virtual int ZRevrange(std::string p_strKey, int nStart, int nStop, std::vector<std::string>* pResult);
			
			virtual int ZRevrangeWithScores(std::string p_strKey, int nStart, int nStop, std::vector<std::pair<std::string, double>>& vOut);

			/*p_iLockTTL: ��λms*/
			virtual bool ServiceLockEx(const std::string& p_pcKey, std::string& p_strLockValue, int p_iLockTTL, int p_iTimeOut, bool p_bOperatorLock, std::string& p_strError);
			//////////////////////////////////////////////////////////////////////////
		private:
			virtual IResourceManagerPtr GetResourceManager();
		private:
			bool AcquireLock(const char* p_pcKey, acl::string& p_strLockValue, int p_iAcquireTimeOut = 10, int p_iLockTTL = 10);
			bool ReleaseLock(const char* p_pcKey, acl::string& p_strLockValue);
			void InitClient();
			bool ConnectRedis();

			std::string GetServerAddrs(std::map<std::string, std::string>& p_mapOutServers, const std::string& p_strInAddrList);
			void StringSplit(std::vector<std::string>& p_vecOutString, const std::string& p_strInString, const std::string& p_strInSplit);
			void InitSentinelClient(const std::string& p_strAddressList);
			void CloseSentinelClient();
			std::string AnalyzeResult(const std::string& p_strResult);
			bool GetServerRole(std::string& p_strOutResult, acl::redis_client* p_pSingleConn);
			bool CheckServerRole();
			void SetOperator(acl::redis_client* p_pSingleConn);

			std::string GetOperatorAddr(); //��ȡ��ǰ������redis��ַ��Ϣ
			void DoCheckRoleTask();
			void StartCheckRoleThread();
			void StopCheckRoleThread();


			//ȡ�ֲ�ʽ��
			bool _Lock(const std::string& p_pcKey, std::string& p_strLockValue, int p_iLockTTL, int p_iTimeOut, std::string& p_strError);

			//����ʱ��
			int _Renewal(const std::string& p_pcKey, std::string& p_strLockValue, int p_iLockTTL, int p_iTimeOut);
		private:
			IResourceManagerPtr m_pResourceManager;

		private:
			StringUtil::IStringUtilPtr	m_pString;
			Log::ILogPtr				m_pLog;
			Config::IConfigPtr			m_pConfig;
			HelpTool::IHelpToolPtr		m_HelpTool;
			acl::redis_client*			m_pSingleConn;	//�ͻ�������ͨ�������
			acl::redis_client_cluster*	m_pClusterConn;	//�ͻ��˼�Ⱥ�����

			acl::redis_transaction		m_transaction;			//����
			acl::redis_key				m_keyOperator;
			acl::redis_string			m_stringOperator;
			acl::redis_list				m_listOperator;
			acl::redis_hash				m_hashOperator;
			acl::redis_set				m_setOperator;
			acl::redis_zset				m_zsetOperator;			//����
			acl::redis_connection		m_pConnection;
			acl::redis_script           m_scriptOperator;   //ִ��lua�ű�ʹ��
			
			std::string m_strServerAddress;			//redis-server ������ַ��127.0.0.1:6379��
			int			m_iConnTimeOut;				//���� redis-server �ĳ�ʱʱ�䣨�룩
			int			m_iRWTimeOut;				//�� redis-server ����ͨ�ŵ� IO ��ʱʱ�䣨�룩
			bool		m_bClusterMode;				//�Ƿ�ʹ�ü�Ⱥ��Ĭ�� true
			//size_t	m_uiMaxConn;				//���ڲ���̬�������ӳض���ʱ����ֵָ��ÿ����̬���������ӳص������������
			int			m_iKeyTTL;					//Ĭ�ϴ��ʱ�䣨Time To Live��
			std::string m_strClientGuid;
			std::string m_strDefaultClassifyName;   //Ĭ�Ϸ�������
			std::string m_strServerMode;			//������ģʽ��0:�ڱ���1����Ⱥ				
			std::map<std::string, std::string> m_mapServers;		//redis-server/���� �б�

			bool							m_bCheckRoleThreadAlive;
			std::recursive_mutex			m_mutexCheckRole;
			std::vector<SRedisClient*>		m_vecRedisClients;		//�ڱ��ͻ��ˣ����������� Redis �ĵ�ַ
			std::shared_ptr<std::thread>	m_pCheckRoleThread;
		};
	}
}