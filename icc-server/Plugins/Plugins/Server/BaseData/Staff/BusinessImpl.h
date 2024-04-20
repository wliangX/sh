#pragma once
#include "StaffInfo.h"

#define STAFF_INFO_KEY "StaffInfo"

namespace ICC
{
	enum ESyncType
	{
		ADD = 1,
		UPDATE,
		DELETE
	};
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
		//获取警员信息
		void OnNotifiGetStaffRequest(ObserverPattern::INotificationPtr p_pNotify);

		//设置警员信息
		void OnNotifiSetStaffRequest(ObserverPattern::INotificationPtr p_pNotify);

		//删除警员信息
		void OnNotifiDeleteStaffRequest(ObserverPattern::INotificationPtr p_pNotify);

    public:       

        //加载警员数据
		bool LoadStaffInfo();

		//功能: 根据输入的cmd及请求消息体生成响应消息体的Header
		void GeneralHeader(std::string p_strCmd, const PROTOCOL::CHeader& p_pRequestHeader, PROTOCOL::CHeader& p_pRespHeader);

		//同步警员信息		
		bool SyncStaffInfo(const CStaffInfo &p_oStaff, ESyncType p_eSyncType);

    private:
        DataBase::IDBConnPtr m_pDBConn;		
		Log::ILogPtr m_pLog;
		Config::IConfigPtr m_pConfig; 
		JsonParser::IJsonPtr m_pIJson;
		DateTime::IDateTimePtr m_pDateTime; 
		StringUtil::IStringUtilPtr m_pString;
		JsonParser::IJsonFactoryPtr m_JsonFacPtr;
		ObserverPattern::IObserverCenterPtr m_pObserverCenter;
		Redis::IRedisClientPtr	m_pRedisClient;	//redis客户端
	};
}