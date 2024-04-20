#pragma once

namespace ICC
{

	class CDeviceCallInfo
	{
	public:
		CDeviceCallInfo(const std::string &p_strCTICallId);
		~CDeviceCallInfo() {};
	public:
		void SetDeviceCallState(const std::string& p_strState, const std::string& p_strTime);
		void SaveRecordInfo(const std::string& p_strSwitchAddr, const std::string& p_strRecordFile);
		bool GetRecordInfo(std::string& p_strSwitchAddr, std::string& p_strRecordFile);

		std::string GetState() const { return m_strCurrCallState; };
		std::string GetStateTime() const { return m_strCurrStateTime; };
		std::string GetTalkTime() const { return m_strTalkTime; };
		std::string GetCallId() const { return m_strCTICallId; };
	public:
		std::string		m_strCTICallId;
		std::string		m_strCurrCallState;
		std::string		m_strCurrStateTime;

		std::string		m_strRecordFile;
		std::string		m_strSwitchAddr;
		std::string		m_strTalkTime;
		std::string		m_strRingTime;
	};

	typedef boost::shared_ptr<CDeviceCallInfo> IDeviceCallInfoPtr;
	

	class CDevice
	{
	public:
		CDevice(const std::string& p_strDeviceNum, int p_iDeviceType, const std::string& p_strAgentId = "", const std::string& p_strPsw = "");
		virtual ~CDevice();

	public:
		std::string GetDeviceNum() const { return m_strDeviceNum; }
		int GetDeviceType() const { return m_iDeviceType; }
		std::string GetDeviceTypeString();

		std::string GetCurrCallState();
		std::string GetCurrCallStateTime() ;
		std::string GetCurrCallTalkTime();
		std::string GetCurrCallId() const { return m_strCurrCallId; };

		std::string GetDeviceCallState(const std::string& p_strCTICallId);

		void SetIsAgent(bool p_bAgentFlag) {m_bAgentFlag = p_bAgentFlag;};

		std::string GetAgentId() const{ return m_strAgentId; }
		std::string GetAgentPsw() const { return m_strPsw; }

		bool SetDeviceCallState(std::string& p_strHcpCallId, const std::string& p_strState, const std::string& p_strTime, bool p_bUpdateCurrCallId = false);
		std::string GetNextCallState(std::string& p_strState);
		void ClearCallInfo();

		bool SaveCurrRecordInfo(const std::string& p_strHcpCallId,const std::string& p_strSwitchAddr, const std::string& p_strRecordFile);
		bool GetCurrRecordInfo(const std::string& p_strHcpCallId,std::string& p_strSwitchAddr, std::string& p_strRecordFile);

		int GetCallListSize();
		void DeleteCallInfo(const std::string& p_strHcpCallId);
	private:
		IDeviceCallInfoPtr _GetDeviceCallInfo(const std::string& p_strHcpCallId);
		IDeviceCallInfoPtr _GetDeviceNextCallInfo();
		void _DeleteDeviceCallInfo(const std::string& p_strHcpCallId);
	protected:
		std::string		m_strDeviceNum;
		int				m_iDeviceType;
		bool			m_bAgentFlag;
		std::string		m_strAgentId;
		std::string		m_strPsw;

		std::string		m_strCurrCallId;

		std::mutex	m_callInfoMutex;
		std::map<std::string, IDeviceCallInfoPtr>	m_mapDeviceCalls;
	};

}// end namespace ICC
