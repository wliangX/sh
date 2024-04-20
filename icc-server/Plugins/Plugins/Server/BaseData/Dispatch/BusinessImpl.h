#pragma once

#define SYSTEMID								("icc_server")
#define SUBSYSTEMID								("icc_server_basedata_dispatch")

#define SELECT_ICC_T_CAN_DISPATCH_DEPT			"select_icc_t_can_dispatch_dept"
#define UPDATE_ICC_T_CAN_DISPATCH_DEPT			"update_icc_t_can_dispatch_dept"
#define DELETE_ICC_T_CAN_DISPATCH_DEPT			"delete_icc_t_can_dispatch_dept"
#define INSERT_ICC_T_CAN_DISPATCH_DEPT			"insert_icc_t_can_dispatch_dept"

#define GET_ALL_CAN_DISPATCH_DEPT_RESPOND		"get_all_can_dispatch_dept_respond"
#define GET_ALL_CAN_DISPATCH_DEPT_RESPOND		"get_all_can_dispatch_dept_respond"
#define SET_CAN_DISPATCH_DEPT_RESPOND			"set_can_dispatch_dept_respond"
#define CAN_DISPATCH_DEPT_SYNC					"can_dispatch_dept_sync"
#define DELETE_CAN_DISPATCH_DEPT_RESPOND		"delete_can_dispatch_dept_respond"
#define CAN_DISPATCH_DEPT_SYNC					"can_dispatch_dept_sync"
#define GET_CAN_DISPATCH_DEPT_REQUEST			"get_can_dispatch_dept_request"
#define GET_CAN_DISPATCH_DEPT_RESPOND			"get_can_dispatch_dept_respond"

namespace ICC
{    
	typedef std::map<std::string, PROTOCOL::CGetDispatchRespond::CDispatch> mapDispatchInfo;
	typedef std::map<std::string, PROTOCOL::CCanDispatchDept::CDispatch> mapCanDispatchDept;

	class CBusinessImpl :
        public CBusinessBase
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
		//��ȡ���е�λ�µ����пɵ��ɵ�λ��Ϣ
		void OnNotifiGetAllCanDispatchDept(ObserverPattern::INotificationPtr p_pNotify);
		//����ĳһ��λ��ĳһ���ɵ��ɵ�λ��Ϣ
		void OnNotifiSetDispatch4anDept(ObserverPattern::INotificationPtr p_pNotify);
		//ɾ��ĳһ��λ��ĳһ���ɵ��ɵ�λ��Ϣ
		void OnNotifiDelDeptCanDispatch(ObserverPattern::INotificationPtr p_pNotify);
		//��ȡĳһ��λ�µ����пɵ��ɵ�λ��Ϣ
		void OnNotifiGetAllCanDispatch4aDept(ObserverPattern::INotificationPtr p_pNotify);
    public:

		//���ؿɵ��ɵ�λ����
		bool LoadDispatchInfo();
		int GetAllCanDispatchDept(CanDispatchInfoMap& p_mapCanDispatchDept);
		//����: ���������cmd��������Ϣ��������Ӧ��Ϣ���Header
		void GenRespHeader(std::string p_strCmd, const PROTOCOL::CHeader& p_pRequestHeader, PROTOCOL::CHeader& p_pRespHeader);
    private:
		DataBase::IDBConnPtr m_pDBConn;

		Config::IConfigPtr m_pConfig;
        Log::ILogPtr m_pLog;

		JsonParser::IJsonFactoryPtr m_pJsonFty;
        StringUtil::IStringUtilPtr m_pString;
		DateTime::IDateTimePtr m_pDateTime;

        boost::shared_ptr<boost::thread> m_DBThread;
        bool m_isConnectDB;
        bool m_isConnectMQ;

		Lock::ILockPtr m_vDBConnLock;

		Redis::IRedisClientPtr m_pRedisClient;

		//mapDispatchInfo m_mapDispatchInfo;
		//Lock::ILockPtr m_mapDispatchInfoLock;

		//mapCanDispatchDept m_mapCanDispatchDept;
		//Lock::ILockPtr m_lCanDisLock;

	private:
		ObserverPattern::IObserverCenterPtr m_pObserverCenter;
	};
}