#pragma once


namespace ICC
{
	class CBusinessImpl :
		public CBusinessBase, public CCommonTimerCore
    {
    public:
        CBusinessImpl();
        ~CBusinessImpl();

	public:
		
		virtual void OnInit();
		virtual void OnStart();
		virtual void OnStop();
		virtual void OnDestroy();

	public:
        // ������Ϣͬ����ϯ��������Ϣ
		void OnNotifiSeatDataSync(ObserverPattern::INotificationPtr p_pNotify);
		// ��ϯ�Ż���״̬����ͬ��
		void OnNotifiSeatDeviceStateSync(ObserverPattern::INotificationPtr p_pNotify);
		// ��ϯ�ŵ�½״̬����ͬ��
		void OnNotifiSeatLoginStateSync(ObserverPattern::INotificationPtr p_pNotify);
		// ��ϯ�Ž��վ�������ͬ��
		void OnNotifiAlarmCountSync(ObserverPattern::INotificationPtr p_pNotify);
		//��ȡ��ϯ������Ϣ
		void OnCNotifGetSeatInfoRequest(ObserverPattern::INotificationPtr p_pNotify);
		// ͬ����ϯ��Ϣ����
		void OnNotifiSeatInfoManagerSync(ObserverPattern::INotificationPtr p_pNotify);
		// ͬ��nacos �����IP
		void OnReceiveSynNacosParams(ObserverPattern::INotificationPtr p_pNotify);
		// CTI ���Ӹ�������
		void OnNotifiCtiConnectStateSync(ObserverPattern::INotificationPtr p_pNotify);

	private:
		// ���͸�ǰ��ͬ����Ϣ
		void SendSeatDataInfoSync(PROTOCOL::CSeatDataInfo SeatData);
		// ���͸�ǰ��ͬ����Ϣ
		void SyncSeatDataInfo(PROTOCOL::CSeatDataInfo SeatData, PROTOCOL::CSeatInfoManagerSync l_oSyncObj);
		// �޸� ��ϯ�ŵľ�������
		bool SetSeatAlarmCountBySeatNo(const std::string strSeatNo, const std::string strAlarmCount);

		bool SetStaffAlarmCountByStaff(const std::string strStaffCode, const std::string strAlarmCount);

		//�����е���ϯ��ַ
		bool _QueryTodayAlarmCount(std::vector<ICC::PROTOCOL::CSeatDataInfo>& vecAllSeatAddress);
		// ��������
		bool _LoadSeatDataFromDB();
		// �����賿����ˢ�µĶ�ʱ��
		void _LoadSeatDataToTimer();
		// ��ȡ�����賿����ʱ��
		std::string GetYestertodayTime();
		// ͳ�ƽ����賿����16�㵽����16��ĽӾ�������������redis
		void BuildAlarmCountByConfig();
		// ����Redis����
		void BuildSeatDataFromClientRegister(PROTOCOL::CClientRegisterSync l_oSyncObj, ICC::PROTOCOL::CSeatDataInfo& SeatData);
		// ��ʼ���豸״̬��Ϣ
		bool InitClientDevicesStateInfo();
		// ��ʼ�� �ͻ���ע����Ϣ
		bool InitClientRegisterInfo();
		// �೤ע����Ϣ
		bool InitSeatInfoFromRedis();
		// ��ʼ�����սӾ�����
		bool InitSeatAlarmCount();
		// ��ʼ���豸״̬
		bool InitSeatDeviceState(std::string& strSeatInfoReceive);
		// ��ʼ��ϯλ״̬���豸״̬
		bool InitSeatAgentState(std::string& strSeatInfoReceive);
		// ��ȡnacos������
		bool GetServiceInfo(std::string& p_strServiceName, std::string& p_strIp, std::string& p_strPort);
		// ����豸״̬
		void CheckDeviceState(std::string& strSeatDeptCode);
		// ��ȡ nacos params
		void SetNacosParams(const std::string& strNacosIp, const std::string& strNacosPort, const std::string& strNameSpace, const std::string& strGroupName);
		// �����ݿ��л�ȡ�����ַ�
		void GetSeatParamsFromConfig(const std::string strParamsKey, std::string& strParamsValue);
		// ���˲�����Ϣ����
		bool CheckDeptCodeData(const std::string strParamsKey, PROTOCOL::CSeatDataInfo data);

		bool _QueryStaffTodayAlarmCount(std::vector<ICC::PROTOCOL::CStaffDataInfo::CStaffInfo>& tmp_vecStaffCount);

	public:
		//��ʱ��������
		virtual void OnTimer(std::string uMsgId);
	private:
		Log::ILogPtr				m_pLog;
		StringUtil::IStringUtilPtr  m_pString;
		JsonParser::IJsonFactoryPtr m_pJsonFac;
		DateTime::IDateTimePtr		m_pDateTime;
		DataBase::IDBConnPtr					m_pIDBConn;
		Config::IConfigPtr			m_pConfig;
		MsgCenter::IMessageCenterPtr		m_pMsgCenter;
		ObserverPattern::IObserverCenterPtr  m_pObserverCenter;		
		Redis::IRedisClientPtr m_pRedisClient;
		Timer::ITimerManagerPtr		m_pTimerMgr;
		CCommonTimer				m_pTimer;
		IHttpClientPtr m_pHttpClient;
		std::string m_strAlarmCountConfig;
		std::string m_strCheckDeviceState;
		std::string m_strMinFreeCount;
		std::string m_strNacosServerIp;
		std::string m_strNacosServerPort;
		std::string m_strNacosServerNamespace;
		std::string m_strNacosServerGroupName;
		boost::mutex m_mutexNacosParams;

		std::string m_strCodeMode;                                   //��Ա����ģʽ��1����Ա���֤���� 2����Ա���
	};
}