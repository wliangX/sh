#pragma once

namespace ICC
{
#define TAPI_LINE_BUFFER		64
#define MAX_SIZE				100

	class CLogicalCall
	{
	public:
		long			m_lCSTACallRefId;
		std::string		m_strDeviceState;
		std::string		m_strStateTime;
	};

	class CDevice
	{
	public:
		CDevice(const std::string& p_strDeviceNum, int p_iDeviceType, const std::string& p_strAgentId = "", const std::string& p_strPsw = "");
		virtual ~CDevice();

	public:
		void SetDeviceNum(const std::string& p_strDeviceNum){ m_strDeviceNum = p_strDeviceNum; }
		std::string GetDeviceNum() const { return m_strDeviceNum; }

		void SetDeviceType(int p_iDeviceType){ m_iDeviceType = p_iDeviceType; }
		void SetDeviceType(const std::string& p_strDeviceType);
		int GetDeviceType() const { return m_iDeviceType; }
		std::string GetDeviceTypeString();

		void SetDeviceState(long p_lCSTACallRefId, const std::string& p_strDeviceState, const std::string& p_strStateTime);
		std::string GetDeviceState(long &p_lCSTACallRefId);// const { return m_strDeviceState; }

		void SetStateTime();
		std::string GetStateTime(long p_lCSTACallRefId);

		void SetCSTACallRefId(long p_lCallRefId){ m_lCSTACallRefId = p_lCallRefId; }
		long GetCSTACallRefId(){ return m_lCSTACallRefId; }

		void GetAllCSTACallRefId(std::vector<long>& CallIDVec);// const { return m_strDeviceState; }

		void SetIsAgent(bool p_bIsAgent){ m_bIsAgent = p_bIsAgent; }
		std::string GetAgentId() const{ return m_strAgentId; }
		std::string GetAgentPsw() const { return m_strPsw; }

	public:
		void AddLogicalCall(long p_lCSTACallRefId, const std::string& p_strDeviceState, const std::string& p_strStateTime);
		bool UpdateLogicalCallState(long p_lCSTACallRefId, const std::string& p_strDeviceState, const std::string& p_strStateTime);
		bool CopyLogicalCallState(long p_lDestCSTACallRefId, long p_lSrcCSTACallRefId);
		bool DeleteLogicalCall(long p_lCSTACallRefId);
		
		bool FindLogicalCall(long p_lCSTACallRefId);
		int GetLogicalCallCount();
	//	bool GetLastLogicalCall(long &p_lCSTACallRefId);
		std::string GetLastLogicalCallState(long &p_lCSTACallRefId);
		std::string GetLogicalCallState(long p_lCSTACallRefId);
		std::string GetLogicalCallStateTime(long p_lCSTACallRefId);
		bool GetCurrentLogicalCall(long &p_lCSTACallRefId, std::string& p_strDeviceState, std::string& p_strStateTime);
		void SetLogPtr(Log::ILogPtr p_LogPtr) { m_pLog = p_LogPtr; };

	protected:
		Log::ILogPtr								m_pLog;
	protected:
		std::string		m_strDeviceNum;
		int				m_iDeviceType;

		long			m_lRouteRegId;
		long			m_lCSTACallRefId;
		bool			m_bIsMonitor;
		
		bool			m_bIsAgent;
		std::string		m_strAgentId;
		std::string		m_strPsw;

		std::map<long, boost::shared_ptr<CLogicalCall>>	m_mapLogicalCallQueue;
	};

	class CTapiLine : public CDevice
	{
	public:
		CTapiLine(const std::string& p_strDeviceNum, int p_iDeviceType, const std::string& p_strAgentId, const std::string& p_strPsw);
		CTapiLine();
		virtual ~CTapiLine();

	public:
		bool Open(HLINEAPP p_hLineApp, DWORD p_dwLineID, DWORD p_dwAPIVersion, DWORD p_dwCallPrivilege, DWORD p_dwMediaModes);
		bool Close();

		// Functions to support telephony commands
		HRESULT HytLineDevSpecific(HLINE p_hLine, DWORD p_dwAddressID, HCALL p_hCall, char* p_szBuffer, std::size_t p_nSize);

		HRESULT MakeCall(const std::string& p_strAddress);
		HRESULT DropCall(HCALL hCall = 0);
		HRESULT DropCallByHandleType(E_CALL_HANDLE_TYPE p_nHandleType);
		HRESULT ClearCall();
		HRESULT AnswerCall();
		HRESULT HoldCall();
		HRESULT UnholdCall();
		HRESULT ConferenceCall();
		HRESULT BlindTransferCall(const std::string& p_strAddress);
		HRESULT SetupTransfer();
		HRESULT Dial(const std::string& p_strAddress);
		HRESULT CompleteTransfer();
		HRESULT SwapHold();
		HRESULT Park(const std::string& p_strAddress);
		HRESULT Unpark(const std::string& p_strAddress);
		HRESULT Redirect(const std::string& p_strAddress);
		HRESULT AddToConference();
		HRESULT RemoveFromConference();
		HRESULT LogOn();
		HRESULT LogOff();
		HRESULT DivertDestination(const std::string& p_strAddress);
		HRESULT SetDivertSettings(bool p_bFwdAll, bool p_bFwdBusy, bool p_bFwdNoAnsw, bool p_bDND);
		HRESULT SetAppSpecific(DWORD p_dwNum);
		HRESULT SetMsgWaitLamp(DWORD p_dwNum);
		HRESULT GetDivertSettings(std::string& l_strReadyType);
		HRESULT SetInGroup(const std::string& p_strGroup);
		HRESULT SetOutGroup(const std::string& p_strGroup);
		HRESULT SetCallData(const std::string& p_strData);
		HRESULT Listen(const std::string& p_strAddress);
		HRESULT Intrude(const std::string& p_strAddress);
		HRESULT SetAccountCode(const std::string& p_strAddress);
		HRESULT GenerateDigits(const std::string& p_strAddress);

	public:

		DWORD GetLineId(){ return m_dwLineID; }

		void SetLineHandle(HLINE hLine) { m_hLine = hLine; }
		
		HLINE GetLineHandle(){ return m_hLine; }
		void SetExtension(DWORD p_dwExtension){ m_dwExtension = p_dwExtension; }

		void SetLineConnectState(bool p_bConnectState){ m_bLineConnectState = p_bConnectState; }
		bool GetLineConnectState(){ return m_bLineConnectState; }

		void ResetCallHandle(HCALL p_hCall);
		void ProcessCallHandle(HCALL p_hCall, E_CALL_HANDLE_TYPE p_nHandleType);
		void SetCallHandle(HCALL p_hCall, E_CALL_HANDLE_TYPE p_nHandleType);
		HCALL GetCallHandle(E_CALL_HANDLE_TYPE p_nHandleType);

	private:
		int m_nOffset;
		char m_szBuffer[TAPI_LINE_BUFFER * MAX_SIZE];

		bool m_bLineConnectState;	// true: Connect; false:DisConnect
		DWORD m_dwExtension;		// The IP Office Extension number that relates to this line
		DWORD m_dwLineID;			// My index according to my parent   //??ио?ж╠?
		HLINE m_hLine;				// My line handle according to TAPI

		// Call handles
		HCALL m_hConnectedCall;		 // My current call handle according to TAPI (0 if idle)
		HCALL m_hWaitingCall;		 // I am ringing this call (0 if none)
		HCALL m_hHeldCall;			 // My held call handle according to TAPI (0 if nothing on hold)
		HCALL m_hPendingCall;		 // This call is waiting to be answered according to TAPI (0 if none)
		HCALL m_hConferenceCall;	 // This is the conference call according to TAPI (0 if none)
		HCALL m_hConsultationCall;	 // This is the call created by lineSetupTransfer
		HCALL m_hLastCallIntoConf;	 // This is the last call added to a conference
	};

}// end namespace ICC
