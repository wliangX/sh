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
		void SetLogPtr(Log::ILogPtr p_pLog){ m_pLog = p_pLog; }
		void SetDateTimePtr(DateTime::IDateTimePtr p_pDateTime){ m_pDateTime = p_pDateTime; }
		void SetStringUtilPtr(StringUtil::IStringUtilPtr p_pStringUtil){ m_pStrUtil = p_pStringUtil; }
		void SetRedisClientPtr(Redis::IRedisClientPtr p_pRedisClient) { m_pRedisClient = p_pRedisClient; }
		void SetJsonFactoryPtr(JsonParser::IJsonFactoryPtr p_pJsonFty) { m_pJsonFty = p_pJsonFty; };
		//////////////////////////////////////////////////////////////////////////
		//load device------------------
		//装载所有设备
		void LoadAllDevice();
		void LoadACDDevice();		//装载ACD
		void LoadPhoneDevice();		//装载Phone
		void LoadVirDn();			//装载虚拟号码
		void DeleteAllDevice();		//删除所有设备
		//load device------------------

		std::list<boost::shared_ptr<CDevice>> GetDeviceList() const { return m_deviceQueue; }
		IGetDeviceListResultNotifPtr GetDeviceList(long p_lRequestId);

		bool DeviceIsFree(const std::string& p_strDeviceNum);
		std::string GetDeviceState(const std::string& p_strDeviceNum);
		std::string GetDeviceType(const std::string& p_strDeviceNum);
		std::string GetDnByLineHandle(HLINE p_dwLineId);
		boost::shared_ptr<CDevice> FindDeviceByDn(const std::string& p_strDeviceNum);
		boost::shared_ptr<CDevice> FindDeviceByMonitorId(long p_lMonitorId);
		boost::shared_ptr<CDevice> FindACDDevice(const std::string& p_strDeviceNum);
		boost::shared_ptr<CTapiLine> FindLineByDn(const std::string& p_strDeviceNum);
		boost::shared_ptr<CTapiLine> FindLineByHandle(HLINE p_dwLineId);

		void AddLogicalCallState(long p_lCSTACallRefId, const std::string& p_strDeviceNum, const std::string& p_strDeviceState);
		void GetLogicalCallState(long p_lCSTACallRefId, const std::string& p_strDeviceNum, std::string& p_strDeviceState);
		int UpdateLogicalCallState(long p_lCSTACallRefId, const std::string& p_strDeviceNum, const std::string& p_strDeviceState);
		void CopyLogicalCallState(long p_lDestCSTACallRefId, long p_lSrcCSTACallRefId);
		void DeleteAllLogicalCallState(long p_lCSTACallRefId);
		void DeleteLogicalCallState(long p_lCSTACallRefId, const std::string& p_strDeviceNum);
		std::string GetLogicalCallState(long p_lCSTACallRefId, const std::string& p_strDeviceNum);
		bool FindLogicalCallState(long p_lCSTACallRefId, const std::string& p_strDeviceNum);
		bool FindLogicalCallExcludeDevice(long p_lCSTACallRefId, const std::string& p_strDeviceNum);
		bool FindLogicalCall(long p_lCSTACallRefId);

		std::string GetDeptCodeByDeviceNum(const std::string& p_strDeviceNum);

		bool FindOtherLogicalCall(long p_lCSTACallRefId, const std::string& p_strDeviceNum);

		void GetDeviceCall(const std::string& p_strDeviceNum, std::vector<long>& CallIDVec);
		//////////////////////////////////////////////////////////////////////////
		//	TAPI
		bool InitTAPI();
		bool OpenAllAddresses();
		void ShutdownTAPI();
		void CloseAllAddresses();

		void TestOpenAllAddresses();

		static bool GetTapiString(std::string& p_strResult, void *p_pObj, DWORD p_dwSize, DWORD p_dwOffset);
		static HRESULT LoopLineGetDevCaps(HLINEAPP p_hLineApp, DWORD p_dwDeviceID, DWORD p_dwAPIVersion, DWORD p_dwExtVersion, LINEDEVCAPS*& p_pLineDevCaps);
		static HRESULT LoopLineGetCallInfo(HCALL p_hCall, LINECALLINFO*& p_pCallInfo);
		static HRESULT LoopLineGetID(DWORD& p_dwResult, HLINE p_hLine, DWORD p_dwAddressID, HCALL p_hCall, DWORD p_dwSelect, LPCSTR p_pszDeviceClass);
		static HRESULT LoopLineGetAddressCaps(HLINEAPP p_hLineApp, DWORD p_dwDeviceID, DWORD p_dwAddressID, DWORD p_dwAPIVersion, DWORD p_dwExtVersion, LINEADDRESSCAPS*& p_pAddressCaps);
		static HRESULT LoopLineGetAddressStatus(HLINE p_hLine, DWORD p_dwAddressID, LINEADDRESSSTATUS*& p_pAddressStatus);
		static HRESULT LoopLineGetCallStatus(HCALL p_hCall, LINECALLSTATUS*& p_pCallStatus);

		void ProcessCallHandle(HLINE p_dwLineId, HCALL p_hCall, E_CALL_HANDLE_TYPE p_nHandleType);
		void SetCallHandle(const std::string& p_strDeviceNum, HCALL p_hCall, E_CALL_HANDLE_TYPE p_nHandleType);
		void ResetCallHandle(const std::string& p_strDeviceNum, HCALL p_hCall);

	private:
		static boost::shared_ptr<CDeviceManager>	m_pInstance;
		Log::ILogPtr								m_pLog;
		DateTime::IDateTimePtr						m_pDateTime;
		StringUtil::IStringUtilPtr					m_pStrUtil;

		bool						m_bDeviceIsLoad;

		// TAPI 
		HLINEAPP					m_hLineApp;				// Application handle returned from TAPI
		DWORD						*m_pApiVersions;		// API versions supported by each device
		DWORD						m_dwDevicesCount;		// Number of devices (lines) available: 1 for INDeX.

		std::mutex					m_deviceQueueMutex;
		std::list<boost::shared_ptr<CDevice>> m_deviceQueue;

		Redis::IRedisClientPtr		m_pRedisClient;
		JsonParser::IJsonFactoryPtr m_pJsonFty;
	};
}	// end namespace
