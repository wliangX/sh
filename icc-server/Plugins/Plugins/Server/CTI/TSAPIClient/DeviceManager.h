#pragma once

#include "Device.h"

namespace ICC
{
	class CDeviceManager
	{
	public:
		CDeviceManager();
		virtual ~CDeviceManager();

		static boost::shared_ptr<CDeviceManager> Instance();
		void ExitInstance();

	public:
		void OnInit(IResourceManagerPtr p_pResourceManager);
		void OnStart();
		void OnStop();
		void SetSwitchConnect(bool p_bConnectFlag);

		void SetCTITestData();

		//load device------------------
		//装载所有设备
		void LoadAllDevice();
		void LoadACDDevice();		//装载ACD
		void LoadPhoneDevice();		//装载Phone
		void LoadVirDn();			//装载虚拟号码
		void DeleteAllDevice();		//删除所有设备
		//load device------------------

		//	设备监视
		void ResetMonitorDevice(const std::string& p_strDeviceNum);
		void ResetMonitorTime();
	//	long GetMonitorTimeSpan();
		bool MonitorIsTimeout();
		bool MonitorAllDevice();
		void ResetMonitorAllDevice();

		/*static */void DoMonitorDevice();

		//	CTI 与 AES 之间心跳检测
		void ResetHeartBeatTime();
		long GetHeartBeatTimeSpan();
		bool HeartBeatIsTimeout();
		void QueryDeviceInfo();
		/*static */void DoQueryDeviceInfo();

		std::list<boost::shared_ptr<CDevice>> GetDeviceList() const { return m_deviceQueue; }
	//	void GetDeviceList(long p_lRequestId);
		IGetDeviceListResultNotifPtr GetDeviceList(long p_lRequestId);
		IDeviceStateNotifPtr GetDeviceState(long p_lCSTACallRefId, const std::string& p_strDeviceNum);

		bool DeviceIsFree(const std::string& p_strDeviceNum);

		std::string GetDeviceType(const std::string& p_strDeviceNum);
		std::string GetDeviceState(const std::string& p_strDeviceNum);

		std::string GetDeptCodeByDeviceNum(const std::string& p_strDeviceNum);

		boost::shared_ptr<CDevice> FindDeviceByDn(const std::string& p_strDeviceNum);
		boost::shared_ptr<CDevice> FindDeviceByMonitorId(long p_lMonitorId);
		boost::shared_ptr<CDevice> FindACDDevice(const std::string& p_strDeviceNum);

	//	void PostDeviceState(long p_lCSTACallRefId, const std::string& p_strCTICallRefId, const std::string& p_strDeviceNum);

		void AddLogicalCallState(long p_lCSTACallRefId, const std::string& p_strDeviceNum, const std::string& p_strDeviceState);
		int UpdateLogicalCallState(long p_lCSTACallRefId, const std::string& p_strDeviceNum, const std::string& p_strDeviceState);
		void DeleteAllLogicalCallState(long p_lCSTACallRefId);
		void DeleteLogicalCallState(long p_lCSTACallRefId, const std::string& p_strDeviceNum);
		std::string GetLogicalCallState(long p_lCSTACallRefId, const std::string& p_strDeviceNum);
		bool FindLogicalCallState(long p_lCSTACallRefId, const std::string& p_strDeviceNum);	
		bool FindLogicalCall(long p_lCSTACallRefId);

		int GetDeviceCallCount(const std::string& strDeviceNum);

	private:
		static boost::shared_ptr<CDeviceManager>	m_pInstance;
		Log::ILogPtr								m_pLog;
		DateTime::IDateTimePtr						m_pDateTimePtr;
		Redis::IRedisClientPtr		m_pRedisClient;
		JsonParser::IJsonFactoryPtr m_pJsonFty;

		static int					m_nMonitorCount;

		bool						m_bMonitorThreadAlive;
		bool						m_bQueryDeviceInfoThreadAlive;
		bool						m_bDeviceIsLoad;
		DateTime::CDateTime			m_oLastHeartBeatTime;
		DateTime::CDateTime			m_oLastMonitorTime;

		std::mutex								m_deviceQueueMutex;
		std::list<boost::shared_ptr<CDevice>>	m_deviceQueue;

		boost::shared_ptr<boost::thread>		m_pMonitorDeviceThread;
		boost::shared_ptr<boost::thread>		m_pQueryDeviceInfoThread;

		bool m_bSwitchConnectFlag;
	};
}	// end namespace
