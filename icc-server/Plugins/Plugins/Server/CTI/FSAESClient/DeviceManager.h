#pragma once
#include "Boost.h"
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

		void AddDevice(std::string& p_strDeviceNum, std::string& p_strDevicetype, std::string& p_strAgent, std::string& p_strPsw);
		std::string GetDeviceCurrState(const std::string& p_strDeviceNum);
		std::string GetDeviceCurrCallId(const std::string& p_strDeviceNum);

		std::string GetDeviceCallState(const std::string& p_strDeviceNum, const std::string& p_strCTICallId);

		std::string GetAgentID(const std::string& p_strDeviceNum);
		std::string GetAgentPswd(const std::string& p_strDeviceNum);
		std::string GetACDListString();

		bool IsACDDevice(const std::string& p_strNum);

		std::string GetAgentListString();
		std::string GetDeviceListString();

		bool IsExtension(const std::string& p_strDevice, bool p_bFindAesExtension = false);
		boost::shared_ptr<CDevice> FindACDDevice(const std::string& p_strDevice);

		bool GetDeviceList(IGetDeviceListResultNotifPtr p_pGetResultNotify);

		bool UpdateDeviceCallState(const std::string& p_strDeviceNum, const std::string& p_strHcpCallId, const std::string& p_strState, bool p_bUpdateCurrCallId = false);
		bool SetDeviceRecordInfo(const std::string& p_strDeviceNum, const std::string& p_strHcpCallId, const std::string& p_strSwitchAddr, const std::string& p_strRecordFile);
		bool GetDeviceRecordInfo(const std::string& p_strDeviceNum, const std::string& p_strHcpCallId, std::string& p_strSwitchAddr, std::string& p_strRecordFile);
		std::string GetCurrTalkTime(const std::string& p_strDevice);
		std::string GetDeviceNextCallState(const std::string& p_strDeviceNum, std::string& p_strState);

		void SaveAESExtensionInfo(const std::vector<std::string>& p_aesExtensions);

		void ClearAllDeviceCallInfo();
	private:
		void _LoadAllDevice();
		void _DeleteAllDevice();
		std::string _GetDeptCodeByDeviceNum(const std::string& p_strDeviceNum);
	private:
		static boost::shared_ptr<CDeviceManager>	m_pInstance;

		StringUtil::IStringUtilPtr					m_pStrUtil;

		Log::ILogPtr								m_pLog;
		DateTime::IDateTimePtr						m_pDateTimePtr;
		Redis::IRedisClientPtr						m_pRedisClient;
		JsonParser::IJsonFactoryPtr					m_pJsonFty;

		bool						m_bDeviceIsLoad;
		std::mutex					m_deviceTBMutex;
		std::map<std::string,boost::shared_ptr<CDevice>> m_mapDevicesTB;

		bool m_bLoadAesExtension;	
		std::map<std::string,std::string>	m_aesExtensionList;

	};
}	// end namespace
