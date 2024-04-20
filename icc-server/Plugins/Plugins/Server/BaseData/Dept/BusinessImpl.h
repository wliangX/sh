#pragma once
#include "DeptInfo.h"

#define SET_DEPT_RESPOND "set_dept_respond";
#define GET_DEPT_RESPOND "get_dept_respond";
#define DELETE_DEPT_RESPOND "delete_dept_respond";
#define DEPT_SYNC "dept_sync";

namespace ICC
{

    typedef std::map<std::string, PROTOCOL::CSetDeptRequest::CBody> mapDept;

	class CBusinessImpl :
        public CBusinessBase
    {
		enum eSyncType
		{
			ICC_ADD = 1,
			ICC_MODIFY,
			ICC_DELETE
		};
    public:
        CBusinessImpl();
        ~CBusinessImpl();

	public:
		
		virtual void OnInit();
		virtual void OnStart();
		virtual void OnStop();
		virtual void OnDestroy();

	public:        
        //��ȡ��λ��Ϣ
		void OnNotifiGetDeptRequest(ObserverPattern::INotificationPtr p_pNotify);
		//��ȡ�¼���λ��Ϣ
		void OnNotifiGetSubDeptRequest(ObserverPattern::INotificationPtr p_pNotify);

        //���õ�λ��Ϣ
		void OnNotifiSetDeptRequest(ObserverPattern::INotificationPtr p_pNotify);

        //ɾ����λ��Ϣ
		void OnNotifiDeleteDeptRequest(ObserverPattern::INotificationPtr p_pNotify);

	private:
		bool LoadDept();
		bool LoadDeptByUser(const std::string& p_strInUserCode, const std::string& p_strInPageSize, const std::string& p_strInPageIndex, 
			std::string& p_strAllCount, std::map<std::string, std::string>& p_mapOutDeptInfo);
		void SetCommandDept();
		bool SetRedisDetpInfo(CDeptInfo& p_oDeptInfo);

		void GenRespHeader(std::string p_strCmd, const PROTOCOL::CHeader& p_pRequestHeader, PROTOCOL::CHeader& p_pRespHeader);
		bool SyncDeptInfo(const CDeptInfo& p_oDept, eSyncType p_eSyncType);

	private:

		static const std::string DeptInfoKey;

		DateTime::IDateTimePtr m_pDateTime;
        StringUtil::IStringUtilPtr m_pString;
		JsonParser::IJsonFactoryPtr m_pJsonFty;
        Config::IConfigPtr m_pConfig;
        Log::ILogPtr m_pLog;
		DataBase::IDBConnPtr m_pDBConn;
		Redis::IRedisClientPtr m_pRedisClient;
        ObserverPattern::IObserverCenterPtr m_pObserverCenter;
    private:
		std::list<std::string>m_lstCommCode;		//���ʹ�����ʱ�������ͣ�Ĭ���о֡��־�ָ������
	};
}