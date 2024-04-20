#pragma once

namespace ICC
{
	class ISwitchNotif
	{
	public:
		ISwitchNotif(){}
		virtual ~ISwitchNotif(){}

	public:
		virtual void SetRequestId(long lRequestId) = 0;
		virtual long GetRequestId() const = 0;

		virtual void SetCSTACallRefId(long lCSTACallRefId) = 0;
		virtual long GetCSTACallRefId() const = 0;

		virtual void SetCTICallRefId(const std::string& strCTICallRefId) = 0;
		virtual std::string GetCTICallRefId() const = 0;
	};
	typedef boost::shared_ptr<ISwitchNotif> ISwitchNotifPtr;

	//	��������
	class CSwitchRequestNotif : public ISwitchNotif
	{
	public:
		CSwitchRequestNotif()
		{ 
			m_lRequestId = DEFAULT_TASKID;
			m_lCSTACallRefId = DEFAULT_CALLREFID;
			m_strCTICallRefId = "";
		}
		virtual ~CSwitchRequestNotif(){}

	public:
		virtual void SetRequestId(long lRequestId){ m_lRequestId = lRequestId; }
		virtual long GetRequestId() const { return m_lRequestId; }

		virtual void SetCSTACallRefId(long lCSTACallRefId){ m_lCSTACallRefId = lCSTACallRefId; }
		virtual long GetCSTACallRefId() const { return m_lCSTACallRefId; }

		virtual void SetCTICallRefId(const std::string& strCTICallRefId){ m_strCTICallRefId = strCTICallRefId; }
		virtual std::string GetCTICallRefId() const { return m_strCTICallRefId; }

	private:
		long			m_lRequestId;			//	�������� ID
		long			m_lCSTACallRefId;		//	�����������Ļ��� ID
		std::string		m_strCTICallRefId;		//	Ӧ�ò㻰�� ID
	};
	typedef boost::shared_ptr<CSwitchRequestNotif> ISwitchRequestNotifPtr;
	//	����Ӧ��
	class CSwitchResultNotif : public ISwitchNotif
	{
	public:
		CSwitchResultNotif(){
			m_lRequestId = DEFAULT_TASKID;
			m_lCSTACallRefId = DEFAULT_CALLREFID;		
			m_nErrorCode = ERROR_CMD_FAILED;
			m_bResult = false;
			m_strErrorMsg = "";
			m_strCTICallRefId = "";
		}
		virtual ~CSwitchResultNotif(){}

	public:
		virtual void SetRequestId(long lRequestId){ m_lRequestId = lRequestId; }
		virtual long GetRequestId() const { return m_lRequestId; }

		virtual void SetCSTACallRefId(long lCSTACallRefId){ m_lCSTACallRefId = lCSTACallRefId; }
		virtual long GetCSTACallRefId() const { return m_lCSTACallRefId; }

		virtual void SetCTICallRefId(const std::string& strCTICallRefId){ m_strCTICallRefId = strCTICallRefId; }
		virtual std::string GetCTICallRefId() const { return m_strCTICallRefId; }

		virtual void SetResult(bool bResult){ m_bResult = bResult; }
		virtual bool GetResult() const { return m_bResult; }

		virtual void SetErrorCode(int nErrorCode){ m_nErrorCode = nErrorCode; }
		virtual int GetErrorCode() { return m_nErrorCode; }

		virtual void SetErrorMsg(const std::string& strErrorMsg){ m_strErrorMsg = strErrorMsg; }
		virtual std::string GetErrorMsg() { return m_strErrorMsg; }

	private:
		long		m_lCSTACallRefId;		//	�����������Ļ��� ID
		long		m_lRequestId;			//	���� ID
		bool		m_bResult;				//	����ִ�н��
		long		m_nErrorCode;			//	�������
		std::string m_strErrorMsg;			//	������Ϣ
		std::string m_strCTICallRefId;		//	Ӧ�ò㻰�� ID
	};
	typedef boost::shared_ptr<CSwitchResultNotif> ISwitchResultNotifPtr;

	//////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////
	// ִ��ʧ���¼�֪ͨ
	class CFailedEventNotif : public CSwitchResultNotif
	{
	public:
		CFailedEventNotif(){
			m_strFailedDeviceNum = "";
			m_strCalledId = "";
		};
		virtual ~CFailedEventNotif(){};

	public:
		void SetFailedDeviceNum(const std::string& strFailedDeviceNum){ m_strFailedDeviceNum = strFailedDeviceNum; }
		std::string GetFailedDeviceNum() const { return m_strFailedDeviceNum; }

		void SetCalledId(const std::string& strCalledId){ m_strCalledId = strCalledId; }
		std::string GetCalledId() const { return m_strCalledId; }

	private:
		std::string m_strFailedDeviceNum;
		std::string m_strCalledId;
	};
	typedef boost::shared_ptr<CFailedEventNotif> IFailedEventNotifPtr;

	// ����������绰ת��ָ��������֪ͨ
	class CBlackTransferEventNotif : public CSwitchResultNotif
	{
	public:
		CBlackTransferEventNotif() {
			m_strCaller = "";
			m_strCalled = "";
			m_strTransferNumber = "";
		};
		virtual ~CBlackTransferEventNotif() {};

	public:
		void SetCaller(const std::string& strCaller) { m_strCaller = strCaller; }
		std::string GetCaller() const { return m_strCaller; }

		void SetCalled(const std::string& strCalled) { m_strCalled = strCalled; }
		std::string GetCalled() const { return m_strCalled; }

		void SetTransferNumber(const std::string& strTransferNumber) { m_strTransferNumber = strTransferNumber; }
		std::string GetTransferNumber() const { return m_strTransferNumber; }

	private:
		std::string m_strCaller;
		std::string m_strCalled;
		std::string m_strTransferNumber;
	};
	typedef boost::shared_ptr<CBlackTransferEventNotif> IBlackTransferEventNotifPtr;

	// AEServer ��������״̬
	class CCTIConnStateNotif : public CSwitchResultNotif
	{
	public:
		CCTIConnStateNotif(){
			m_bStreamOpen = false;
			m_bStreamException = false;
			m_strSwitchType = "0";
		};
		virtual ~CCTIConnStateNotif(){};

	public:
		void SetStreamOpen(bool bOpen){ m_bStreamOpen = bOpen; }
		bool GetStreamOpen() const { return m_bStreamOpen; }

		void SetSwitchType(std::string strSwitchType){ m_strSwitchType = strSwitchType; }
		std::string GetSwitchType() const{ return m_strSwitchType; }

		void SetStreamException(bool bException){ m_bStreamException = bException; }
		bool GetStreamException() const { return m_bStreamException; }

	private:
		bool m_bStreamOpen;		// AES �������Ƿ�ɹ�
		bool m_bStreamException;
		std::string m_strSwitchType;	// �������ӿ����ͣ�0 TSAPI��1 TAPI
	};
	typedef boost::shared_ptr<CCTIConnStateNotif> ICTIConnStateNotifPtr;

	// ACD Agent ״̬
	class CAgentStateNotif : public CSwitchResultNotif
	{
	public:
		CAgentStateNotif(){
			m_strAgentId = "";
			m_strACDGrp = "";
			m_strLoginMode = "";
			m_strReadyState = "";
			m_strTime = "";
			m_strMinOnlineNum = "";
			m_strCurrOnlineNum = "";

		};
		virtual ~CAgentStateNotif(){};

	public:
		void SetAgentId(const std::string& strAgentId){ m_strAgentId = strAgentId; }
		std::string GetAgentId() const { return m_strAgentId; }

		void SetACDGrp(const std::string& strACDGroup){ m_strACDGrp = strACDGroup; }
		std::string GetACDGrp() const { return m_strACDGrp; }

		void SetLoginMode(const std::string& strLoginMode){ m_strLoginMode = strLoginMode; }
		std::string GetLoginMode() const { return m_strLoginMode; }

		void SetReadyState(const std::string& strReadyState){ m_strReadyState = strReadyState; }
		std::string GetReadyState() const { return m_strReadyState; }

		void SetStateTime(const std::string& strStateTime){ m_strTime = strStateTime; }
		std::string GetStateTime() const { return m_strTime; }

		void SetMinOnlineNum(const std::string& strMainNum) { m_strMinOnlineNum = strMainNum; }
		std::string GetMinOnlineNum() const { return m_strMinOnlineNum; }

		void SetCurrOnlineNum(const std::string& strCurrOnlineNum) { m_strCurrOnlineNum = strCurrOnlineNum; }
		std::string GetCurrOnlineNum() const { return m_strCurrOnlineNum; }

	private:
		std::string		m_strAgentId;		// ��ϯ�ֻ���
		std::string		m_strACDGrp;		// ACD ��
		std::string		m_strLoginMode;		// Login/Logout
		std::string		m_strReadyState;	// Busy/Idel
		std::string		m_strTime;			// 
		std::string		m_strMinOnlineNum; //��С������ϯ��
		std::string		m_strCurrOnlineNum;//��ǰ������ϯ��
	};
	typedef boost::shared_ptr<CAgentStateNotif> IAgentStateNotifPtr;

	// �豸״̬
	class CDeviceStateNotif : public CSwitchResultNotif
	{
	public:
		CDeviceStateNotif(){
			m_bPostUp = false;
			m_strACDGrp = "";

			m_strDeviceNum = "";		//	�豸����
			m_strDeviceType = "";		//	�豸����
			m_strDeviceState = "";		//	�豸״̬
			m_strStateTime = "";		//	�豸״̬ʱ��

			m_strTalkTime = "";

			m_strCallDirection = "";	//	������з���
			m_strCallerId = "";			//	����
			m_strCalledId = "";			//	����
			m_strOriginalCallerId = "";	//	ԭʼ����
			m_strOriginalCalledId = "";	//	ԭʼ����

			m_strLoginMode = "";		//	login/logout ״̬
			m_strReadyState = "";

			m_strDeptCode = "";

			m_strRecordFilePath = "";

			m_strTransferType = "";

			/*m_strHangupDevice = "";		//	�һ��豸����
			m_strHangupType = "";		//	�һ�����*/
		};
		virtual ~CDeviceStateNotif(){};

	public:
		void SetACDGrp(const std::string& strACDGrp) { m_strACDGrp = strACDGrp; };
		std::string GetACDGrp() { return m_strACDGrp; };
		void SetPostUp(bool p_bPostUp){ m_bPostUp = p_bPostUp; }
		bool GetPostUp(){ return m_bPostUp; }

		void SetDeviceNum(const std::string& strDeviceNum){ m_strDeviceNum = strDeviceNum; }
		std::string GetDeviceNum() const { return m_strDeviceNum; }

		void SetDeviceType(const std::string& strDeviceType){ m_strDeviceType = strDeviceType; }
		std::string GetDeviceType() const { return m_strDeviceType; }

		void SetDeviceState(const std::string& strDeviceState){ m_strDeviceState = strDeviceState; }
		std::string GetDeviceState() const { return m_strDeviceState; }

		void SetStateTime(const std::string& strStateTime){ m_strStateTime = strStateTime; }
		std::string GetStateTime() const { return m_strStateTime; }

		void SetCallDirection(const std::string& strCallDirection){ m_strCallDirection = strCallDirection; }
		std::string GetCallDirection() const { return m_strCallDirection; }

		void SetCallerId(const std::string& strCallerId){ m_strCallerId = strCallerId; }
		std::string GetCallerId() const { return m_strCallerId; }

		void SetCalledId(const std::string& strCalledId){ m_strCalledId = strCalledId; }
		std::string GetCalledId() const { return m_strCalledId; }

		void SetOriginalCallerId(const std::string& strOriginalCallerId){ m_strOriginalCallerId = strOriginalCallerId; }
		std::string GetOriginalCallerId() const { return m_strOriginalCallerId; }

		void SetOriginalCalledId(const std::string& strOriginalCalledId){ m_strOriginalCalledId = strOriginalCalledId; }
		std::string GetOriginalCalledId() const { return m_strOriginalCalledId; }

		void SetLoginMode(const std::string& strLoginMode) { m_strLoginMode = strLoginMode; }
		std::string GetLoginMode() const { return m_strLoginMode; }

		void SetReadyState(const std::string& strReadyState) { m_strReadyState = strReadyState; }
		std::string GetReadyState() const { return m_strReadyState; }

		void SetDeptCode(const std::string& strDeptCode) { m_strDeptCode = strDeptCode; }
		std::string GetDeptCode() const { return m_strDeptCode; }

		void SetCallCount(int iCallCount) { m_iCallCount = iCallCount; }
		int  GetCallCount() { return m_iCallCount; }
		/*void SetHangupDevice(const std::string& strHangupDevice){ m_strHangupDevice = strHangupDevice; }
		std::string GetHangupDevice() const { return m_strHangupDevice; }

		void SetHangupType(const std::string& strHangupType){ m_strHangupType = strHangupType; }
		std::string GetHangupType() const { return m_strHangupType; }*/

		void SetTalkTime(const std::string& strTalkTime) { m_strTalkTime = strTalkTime; };
		std::string GetTalkTime() const { return m_strTalkTime; }

		void SetRecordFilePath(const std::string& strRecordFile) { m_strRecordFilePath = strRecordFile; };
		std::string GetRecordFilePath() const { return m_strRecordFilePath; }

		void SetSwitchAddress(const std::string& strSwitchAddr) { m_strSwitchAddr = strSwitchAddr; };
		std::string GetSwitchAddress() const { return m_strSwitchAddr; }

		void SetTransferType(const std::string& p_strTransferType) { m_strTransferType = p_strTransferType; };
		std::string GetTransferType() const { return m_strTransferType; }
		void SetSwitchAddress() {};

		void SetConferenceId(const std::string& p_strConferenceId) { m_strConferenceId = p_strConferenceId; };
		std::string GetConferenceId() const { return m_strConferenceId; }
		void SetConfCompere(const std::string& p_strConfCompere) { m_strConfCompere = p_strConfCompere; };
		std::string GetConfCompere() const { return m_strConfCompere; }
		void SetConfBargeinSponsor(const std::string& p_strConfBargeinSponsor) { m_strConfBargeinSponsor = p_strConfBargeinSponsor; };
		std::string GetConfBargeinSponsor() const { return m_strConfBargeinSponsor; }
	private:
		bool			m_bPostUp;				//	֪ͨӦ�ò�
		std::string		m_strACDGrp;
		std::string		m_strDeviceNum;			//	�豸����
		std::string		m_strDeviceType;		//	�豸����
		std::string		m_strDeviceState;		//	�豸״̬
		std::string		m_strStateTime;			//	�豸״̬ʱ��

		std::string		m_strTalkTime;

		std::string		m_strCallDirection;		//	������з���
		std::string		m_strCallerId;			//	����
		std::string		m_strCalledId;			//	����
		std::string		m_strOriginalCallerId;	//	ԭʼ����
		std::string		m_strOriginalCalledId;	//	ԭʼ����

		std::string		m_strLoginMode;		//	login/logout ״̬
		std::string		m_strReadyState;	//	busy/idle ״̬

		std::string		m_strDeptCode;

		std::string		m_strSwitchAddr;
		std::string		m_strRecordFilePath;

		int             m_iCallCount;           //  �豸�ϵĻ�������

		std::string		m_strTransferType;
		std::string		m_strConferenceId;
		std::string		m_strConfCompere;
		std::string		m_strConfBargeinSponsor;
		/*std::string		m_strHangupDevice;		//	�һ��豸����
		std::string		m_strHangupType;		//	�һ�����*/
	};
	typedef boost::shared_ptr<CDeviceStateNotif> IDeviceStateNotifPtr;

	// ����״̬
	class CCallStateNotif : public CSwitchResultNotif
	{
	public:
		CCallStateNotif(){
			m_strAgentId = "";							//	��������
			m_strACDGrp = "";

			m_bIsBlackCall = false;
		//	m_lRelateCSTACallRefId = DEFAULT_CALLREFID;
			m_strCallerId = "";							//	����
			m_strCalledId = "";							//	����
			m_strOriginalCallerId = "";
			m_strOriginalCalledId = "";
			m_strCallState = "";						//	����״̬
			m_strCallDirection = "";					//	������з���
			m_strStateTime = "";						//	����״̬ʱ��
			/*m_strHangupDevice = "";						//	�һ��豸����
			m_strHangupType = "";						//	�һ�����*/
		};
		virtual ~CCallStateNotif(){};

	public:
		void SetAgentId(const std::string& strAgentId){ m_strAgentId = strAgentId; }
		std::string GetAgentId() const { return m_strAgentId; }

		void SetACDGrp(const std::string& strACDGroup){ m_strACDGrp = strACDGroup; }
		std::string GetACDGrp() const { return m_strACDGrp; }

		void SetACDAlias(const std::string& strACDAlias){ m_strACDAlias = strACDAlias; }
		std::string GetACDAlias() const { return m_strACDAlias; }

		void SetACDDept(const std::string& strACDDept){ m_strACDDept = strACDDept; }
		std::string GetACDDept() const { return m_strACDDept; }

		void SetIsBlackCall(bool bBlackCall){ m_bIsBlackCall = bBlackCall; }
		bool GetIsBlackCall(){ return m_bIsBlackCall; }

		/*void SetRelateCSTACallRefId(long lRelateCSTACallRefId){ m_lRelateCSTACallRefId = lRelateCSTACallRefId; }
		long GetRelateCSTACallRefId() const { return m_lRelateCSTACallRefId; }*/

		void SetCallerId(const std::string& strCallerId){ m_strCallerId = strCallerId; }
		std::string GetCallerId() const { return m_strCallerId; }

		void SetCalledId(const std::string& strCalledId){ m_strCalledId = strCalledId; }
		std::string GetCalledId() const { return m_strCalledId; }

		void SetOriginalCallerId(const std::string& strOriginalCallerId){ m_strOriginalCallerId = strOriginalCallerId; }
		std::string GetOriginalCallerId() const { return m_strOriginalCallerId; }

		void SetOriginalCalledId(const std::string& strOriginalCalledId){ m_strOriginalCalledId = strOriginalCalledId; }
		std::string GetOriginalCalledId() const { return m_strOriginalCalledId; }

		void SetCallState(const std::string& strCallState){ m_strCallState = strCallState; }
		std::string GetCallState() const { return m_strCallState; }

		void SetCallDirection(const std::string& strCallDirection){ m_strCallDirection = strCallDirection; }
		std::string GetCallDirection() const { return m_strCallDirection; }

		void SetStateTime(const std::string& strStateTime){ m_strStateTime = strStateTime; }
		std::string GetStateTime() const { return m_strStateTime; }

		/*void SetHangupDevice(const std::string& strHangupDevice){ m_strHangupDevice = strHangupDevice; }
		std::string GetHangupDevice() const { return m_strHangupDevice; }

		void SetHangupType(const std::string& strHangupType){ m_strHangupType = strHangupType; }
		std::string GetHangupType() const { return m_strHangupType; }*/

	private:
		std::string		m_strAgentId;			//	��������(7001,7002)
		std::string		m_strACDGrp;			//	ACD ��
		std::string		m_strACDAlias;			//	ACD �����
		std::string		m_strACDDept;			//	ACD ��������

		bool			m_bIsBlackCall;
	//	long			m_lRelateCSTACallRefId;	//	�������������� ID�����顢ת�ơ��������õ���
		std::string		m_strCallerId;			//	����
		std::string		m_strCalledId;			//	����
		std::string		m_strOriginalCallerId;	//	ԭʼ����
		std::string		m_strOriginalCalledId;	//	ԭʼ����
		std::string		m_strCallDirection;		//	������з���
		std::string		m_strCallState;			//	����״̬
		std::string		m_strStateTime;			//	����״̬ʱ��
		/*std::string		m_strHangupDevice;		//	�һ��豸����
		std::string		m_strHangupType;		//	�һ�����*/	
	};
	typedef boost::shared_ptr<CCallStateNotif> ICallStateNotifPtr;

	class CCallOverNotif : public CCallStateNotif
	{
	public:
		CCallOverNotif(){
			m_strIncomingTime = "";		//	����ʱ��
			m_strAssignTime = "";		//	����ʱ��
			m_strRingTime = "";			//	����ʱ��
			m_strSignalBackTime = "";	//	����ʱ��
			m_strTalkTime = "";			//	��ʼͨ��ʱ��
			m_strHangupTime = "";		//	�һ�ʱ��
			m_strHangupDevice = "";		//	�һ��豸����
			m_strHangupType = "";		//	�һ�����
			m_strSwitchType = "0";		//	�������ӿ�����
		};
		virtual ~CCallOverNotif(){};

	public:
		void SetIncomingTime(const std::string& strIncomingTime){ m_strIncomingTime = strIncomingTime; }
		std::string GetIncomingTime() const { return m_strIncomingTime; }

		void SetAssignTime(const std::string& strAssignTime){ m_strAssignTime = strAssignTime; }
		std::string GetAssignTime() const { return m_strAssignTime; }

		void SetRingTime(const std::string& strRingTime){ m_strRingTime = strRingTime; }
		std::string GetRingTime() const { return m_strRingTime; }

		void SetSignalBackTime(const std::string& strSignalBackTime){ m_strSignalBackTime = strSignalBackTime; }
		std::string GetSignalBackTime() const { return m_strSignalBackTime; }

		void SetDialTime(const std::string& strDialTime){ m_strDialTime = strDialTime; }
		std::string GetDialTime() const { return m_strDialTime; }

		void SetReleaseTime(const std::string& strReleaseTime){ m_strReleaseTime = strReleaseTime; }
		std::string GetReleaseTime() const { return m_strReleaseTime; }

		void SetTalkTime(const std::string& strTalkTime){ m_strTalkTime = strTalkTime; }
		std::string GetTalkTime() const { return m_strTalkTime; }

		void SetHangupTime(const std::string& strHangupTime){ m_strHangupTime = strHangupTime; }
		std::string GetHangupTime() const { return m_strHangupTime; }

		void SetHangupDevice(const std::string& strHangupDevice){ m_strHangupDevice = strHangupDevice; }
		std::string GetHangupDevice() const { return m_strHangupDevice; }

		void SetHangupType(const std::string& strHangupType){ m_strHangupType = strHangupType; }
		std::string GetHangupType() const { return m_strHangupType; }

		void SetSwitchType(const std::string& strSwitchType){ m_strSwitchType = strSwitchType; }
		std::string GetSwitchType() const { return m_strSwitchType; }

		void SetRecordFilePath(const std::string& strFilePath) { m_strRecordFilePath = strFilePath; }
		std::string GetRecordFilePath() const { return m_strRecordFilePath; }
		
	private:
		std::string		m_strIncomingTime;		//	����ʱ��
		std::string		m_strAssignTime;		//	����ʱ��
		std::string		m_strRingTime;			//	����ʱ��
		std::string		m_strSignalBackTime;	//	����ʱ��
		std::string		m_strDialTime;			//	ժ��ʱ��
		std::string		m_strReleaseTime;		//	����ʱ��
		std::string		m_strTalkTime;			//	��ʼͨ��ʱ��
		std::string		m_strHangupTime;		//	�һ�ʱ��
		std::string		m_strHangupDevice;		//	�һ��豸����
		std::string		m_strHangupType;		//	�һ�����
		std::string		m_strSwitchType;		//	�������ӿ�����

		std::string		m_strRecordFilePath;
	};
	typedef boost::shared_ptr<CCallOverNotif> ICallOverNotifPtr;

	//	
	class CCallTimeoutEventNotif : public CSwitchResultNotif
	{
	public:
		CCallTimeoutEventNotif(){
		};
		virtual ~CCallTimeoutEventNotif(){};

	public:
		//

	private:
		//
	};
	typedef boost::shared_ptr<CCallTimeoutEventNotif> ICallTimeoutEventNotifPtr;


	//////////////////////////////////////////////////////////////////////////
	// ��ѯ�豸��Ϣ
	class CQueryDeviceInfoNotif : public CSwitchRequestNotif
	{
	public:
		CQueryDeviceInfoNotif(){
			m_strDeviceNum = "";
		};
		virtual ~CQueryDeviceInfoNotif(){};

	public:
		void SetDeviceNum(const std::string& strDeviceNum){ m_strDeviceNum = strDeviceNum; }
		std::string GetDeviceNum() const { return m_strDeviceNum; }

	private:
		std::string m_strDeviceNum;		// 
	};
	typedef boost::shared_ptr<CQueryDeviceInfoNotif> IQueryDeviceInfoNotifPtr;
	// �豸���ӽ��
	class CQueryDeviceInfoResultNotif : public CSwitchResultNotif
	{
	public:
		CQueryDeviceInfoResultNotif(){
			m_strDeviceNum = "";
			m_strDeviceClass = "";
			m_strDeviceType = "";
		};
		virtual ~CQueryDeviceInfoResultNotif(){};

	public:
		void SetDeviceNum(const std::string& strDeviceNum){ m_strDeviceNum = strDeviceNum; }
		std::string GetDeviceNum() const { return m_strDeviceNum; }

		void SetDeviceClass(const std::string& strDeviceClass){ m_strDeviceClass = strDeviceClass; }
		std::string GetDeviceClass() const { return m_strDeviceClass; }

		void SetDeviceType(const std::string& strDeviceType){ m_strDeviceType = strDeviceType; }
		std::string GetDeviceType() const { return m_strDeviceType; }

	private:
		std::string m_strDeviceNum;		// 
		std::string m_strDeviceClass;
		std::string m_strDeviceType;
	};
	typedef boost::shared_ptr<CQueryDeviceInfoResultNotif> IQueryDeviceInfoResultNotifPtr;

	// �豸����
	class CMonitorDeviceNotif : public CSwitchRequestNotif
	{
	public:
		CMonitorDeviceNotif(){
			m_strDeviceNum = "";
		};
		virtual ~CMonitorDeviceNotif(){};

	public:
		void SetDeviceNum(const std::string& strDeviceNum){ m_strDeviceNum = strDeviceNum; }
		std::string GetDeviceNum() const { return m_strDeviceNum; }

	private:
		std::string m_strDeviceNum;		// 
	};
	typedef boost::shared_ptr<CMonitorDeviceNotif> IMonitorDeviceNotifPtr;
	// �豸���ӽ��
	class CMonitorDeviceResultNotif : public CSwitchResultNotif
	{
	public:
		CMonitorDeviceResultNotif(){
			m_lMonitorId = 0;
		};
		virtual ~CMonitorDeviceResultNotif(){};

	public:
		void SetMonitorId(long lMonitorId){ m_lMonitorId = lMonitorId; }
		long GetMonitorId() const { return m_lMonitorId; }

	private:
		long m_lMonitorId;		// 
	};
	typedef boost::shared_ptr<CMonitorDeviceResultNotif> IMonitorDeviceResultNotifPtr;

	// RouteRegisterReq
	class CRouteRegisterReqNotif : public CSwitchRequestNotif
	{
	public:
		CRouteRegisterReqNotif(){
			m_strDeviceNum = "";
		};
		virtual ~CRouteRegisterReqNotif(){};

	public:
		void SetDeviceNum(const std::string& strDeviceNum){ m_strDeviceNum = strDeviceNum; }
		std::string GetDeviceNum() const { return m_strDeviceNum; }

	private:
		std::string m_strDeviceNum;		// 
	};
	typedef boost::shared_ptr<CRouteRegisterReqNotif> IRouteRegisterReqNotifPtr;
	class CRouteRegisterResultNotif : public CSwitchResultNotif
	{
	public:
		CRouteRegisterResultNotif(){
			m_lRouteRegId = 0;
		};
		virtual ~CRouteRegisterResultNotif(){};

	public:
		void SetRouteRegId(long lRouteRegId){ m_lRouteRegId = lRouteRegId; }
		long GetRouteRegId() const { return m_lRouteRegId; }

	private:
		long m_lRouteRegId;
	};
	typedef boost::shared_ptr<CRouteRegisterResultNotif> IRouteRegisterResultNotifPtr;


	// ���� ACD Agent loin/logout ready/notready
	class CSetAgentStateNotif : public CSwitchRequestNotif
	{
	public:
		CSetAgentStateNotif(){ 
			m_bInitAgentState = false;
			m_bPreLogin = false;
			m_strAgentId = "";
			m_strACDGrp = "";
			m_strLoginMode = "";
			m_strReadyState = "";
		}
		virtual ~CSetAgentStateNotif(){}

	public:
		void SetInitAgentState(bool bInitAgentState){ m_bInitAgentState = bInitAgentState; }
		bool GetInitAgentState() const { return m_bInitAgentState; }

		void SetPreLogin(bool bPreLogin){ m_bPreLogin = bPreLogin; }
		bool GetPreLogin() const { return m_bPreLogin; }

		void SetAgentId(const std::string& strAgentId){ m_strAgentId = strAgentId; }
		std::string GetAgentId() const { return m_strAgentId; }

		void SetACDGrp(const std::string& strACDGroup){ m_strACDGrp = strACDGroup; }
		std::string GetACDGrp() const { return m_strACDGrp; }

		void SetLoginMode(const std::string& strLoginMode){ m_strLoginMode = strLoginMode; }
		std::string GetLoginMode() const { return m_strLoginMode; }

		void SetReadyState(const std::string& strReadyState){ m_strReadyState = strReadyState; }
		std::string GetReadyState() const { return m_strReadyState; }

	private:
		bool			m_bInitAgentState;
		bool			m_bPreLogin;		//	ִ�� Busy/Idel ǰ��ִ�� Login
		std::string		m_strAgentId;		// ��ϯ�ֻ���
		std::string		m_strACDGrp;		// ACD ��
		std::string		m_strLoginMode;		// Login/Logout
		std::string		m_strReadyState;	// Busy/Idel
	};
	typedef boost::shared_ptr<CSetAgentStateNotif> ISetAgentStateNotifPtr;
	// ���� ACD Agent loin/logout ready/notready ���
	class CSetAgentStateResultNotif : public CSwitchResultNotif
	{
	public:
		CSetAgentStateResultNotif(){
			m_strAgentId = "";
			m_strACDGrp = "";
			m_strLoginMode = "";
			m_strReadyState = "";
		};
		virtual ~CSetAgentStateResultNotif(){};

	public:
		void SetAgentId(const std::string& strAgentId){ m_strAgentId = strAgentId; }
		std::string GetAgentId() const { return m_strAgentId; }

		void SetACDGrp(const std::string& strACDGroup){ m_strACDGrp = strACDGroup; }
		std::string GetACDGrp() const { return m_strACDGrp; }

		void SetLoginMode(const std::string& strLoginMode){ m_strLoginMode = strLoginMode; }
		std::string GetLoginMode() const { return m_strLoginMode; }

		void SetReadyState(const std::string& strReadyState){ m_strReadyState = strReadyState; }
		std::string GetReadyState() const { return m_strReadyState; }

	private:
		std::string		m_strAgentId;		// ��ϯ�ֻ���
		std::string		m_strACDGrp;		// ACD ��
		std::string		m_strLoginMode;		// Login/Logout
		std::string		m_strReadyState;	// Busy/Idel
	};
	typedef boost::shared_ptr<CSetAgentStateResultNotif> ISetAgentStateResultNotifPtr;

	// Login��Logout ���
	/*class CAgentLoginModeResultNotif : public CSwitchResultNotif
	{
	public:
		CAgentLoginModeResultNotif(){
			m_strAgentId = "";
			m_strACDGrp = "";
			m_strLoginMode = "";
		};
		virtual ~CAgentLoginModeResultNotif(){};

	public:
		void SetAgentId(const std::string& strAgentId){ m_strAgentId = strAgentId; }
		std::string GetAgentId() const { return m_strAgentId; }

		void SetACDGrp(const std::string& strACDGroup){ m_strACDGrp = strACDGroup; }
		std::string GetACDGrp() const { return m_strACDGrp; }

		void SetLoginMode(const std::string& strLoginMode){ m_strLoginMode = strLoginMode; }
		std::string GetLoginMode() const { return m_strLoginMode; }

	private:
		std::string		m_strAgentId;		// ��ϯ�ֻ���
		std::string		m_strACDGrp;		// ACD ��
		std::string		m_strLoginMode;		// Login/Logout
	};
	typedef boost::shared_ptr<CAgentLoginModeResultNotif> IAgentLoginModeResultNotifPtr;*/

	//��ѯ Agent ״̬
	class CQueryAgentStateNotif : public CSwitchRequestNotif
	{
	public:
		CQueryAgentStateNotif(){
			m_strAgentId = "";
		};
		virtual ~CQueryAgentStateNotif(){};

	public:
		void SetAgentId(const std::string& strAgentId){ m_strAgentId = strAgentId; }
		std::string GetAgentId() const { return m_strAgentId; }

	private:
		std::string		m_strAgentId;	// 
	};
	typedef boost::shared_ptr<CQueryAgentStateNotif> IQueryAgentStateNotifPtr;
	//��ѯ Agent ״̬���
	class CQueryAgentStateResultNotif : public CSwitchResultNotif
	{
	public:
		CQueryAgentStateResultNotif(){
			m_strAgentState = "";
		};
		virtual ~CQueryAgentStateResultNotif(){};

	public:
		void SetAgentState(const std::string& strAgentState){ m_strAgentState = strAgentState; }
		std::string GetAgentState() const { return m_strAgentState; }

	private:
		std::string		m_strAgentState;	// 
	};
	typedef boost::shared_ptr<CQueryAgentStateResultNotif> IQueryAgentStateResultNotifPtr;

	// ����
	class CMakeCallNotif : public CSwitchRequestNotif
	{
	public:
		CMakeCallNotif(){
			m_strCallerId = "";
			m_strCalledId = "";
			m_strTargetPhoneType = "";
			m_strCaseId = "";
		}
		virtual ~CMakeCallNotif(){}

	public:
		void SetCallerId(const std::string& strCallerId){ m_strCallerId = strCallerId; }
		std::string GetCallerId() const { return m_strCallerId; }

		void SetCalledId(const std::string& strCalledId){ m_strCalledId = strCalledId; }
		std::string GetCalledId() const { return m_strCalledId; }

		void SetTargetPhoneType(const std::string& strTargetPhoneType){ m_strTargetPhoneType = strTargetPhoneType; }
		std::string GetTargetPhoneType() const { return m_strTargetPhoneType; }

		void SetCaseId(const std::string& strCaseId){ m_strCaseId = strCaseId; }
		std::string GetCaseId() const { return m_strCaseId; }

	private:
		std::string		m_strCallerId;			// ��ϯ�ֻ���
		std::string		m_strCalledId;			// ACD ��
		std::string		m_strTargetPhoneType;	// Ŀ�껰������
		std::string		m_strCaseId;			// ���� ID
	};
	typedef boost::shared_ptr<CMakeCallNotif> IMakeCallNotifPtr;
	// ���� ���
	class CMakeCallResultNotif : public CSwitchResultNotif
	{
	public:
		CMakeCallResultNotif(){
			m_strCallerId = "";
			m_strCalledId = "";
			m_strCaseId = "";
		};
		virtual ~CMakeCallResultNotif(){};

	public:
		void SetCallerId(const std::string& strCallerId){ m_strCallerId = strCallerId; }
		std::string GetCallerId() const { return m_strCallerId; }

		void SetCalledId(const std::string& strCalledId){ m_strCalledId = strCalledId; }
		std::string GetCalledId() const { return m_strCalledId; }

		void SetCaseId(const std::string& strCaseId){ m_strCaseId = strCaseId; }
		std::string GetCaseId() const { return m_strCaseId; }

	private:
		std::string		m_strCallerId;		// ��ϯ�ֻ���
		std::string		m_strCalledId;		// ACD ��
		std::string		m_strCaseId;		// ���� ID
	};
	typedef boost::shared_ptr<CMakeCallResultNotif> IMakeCallResultNotifPtr;

	// Ӧ�𡢾ܽӡ��Ҷ�
	class CAnswerCallNotif : public CSwitchRequestNotif
	{
	public:
		CAnswerCallNotif(){
			m_strSponsor = "";
			m_strDeviceNum = "";
			m_strCallerId = "";
			m_strCalledId = "";
		};
		virtual ~CAnswerCallNotif(){};

	public:
		void SetSponsor(const std::string& strSponsor){ m_strSponsor = strSponsor; }
		std::string GetSponsor() const { return m_strSponsor; }

		void SetDeviceNum(const std::string& strDeviceNum){ m_strDeviceNum = strDeviceNum; }
		std::string GetDeviceNum() const { return m_strDeviceNum; }

		void SetCallerId(const std::string& strCallerId){ m_strCallerId = strCallerId; }
		std::string GetCallerId() const { return m_strCallerId; }

		void SetCalledId(const std::string& strCalledId){ m_strCalledId = strCalledId; }
		std::string GetCalledId() const { return m_strCalledId; }

	private:
		std::string		m_strSponsor;		// ����Ӧ�𡢾ܽӡ��Ҷϵ���ϯ�ֻ���
		std::string		m_strDeviceNum;		// ��ϯ�ֻ���
		std::string		m_strCallerId;		// ����
		std::string		m_strCalledId;		// ����
	};
	typedef boost::shared_ptr<CAnswerCallNotif> IAnswerCallNotifPtr;
	// Ӧ�� ���
	/*class CAnswerCallResultNotif : public CSwitchResultNotif
	{
	public:
		CAnswerCallResultNotif(){
			m_lCSTACallRefId = DEFAULT_CALLREFID;
			m_strDeviceNum = "";
		};
		virtual ~CAnswerCallResultNotif(){};

	public:
		void SetCSTACallRefId(long lCallRefId){ m_lCSTACallRefId = lCallRefId; }
		long GetCSTACallRefId() const { return m_lCSTACallRefId; }

		void SetDeviceNum(const std::string& strDeviceNum){ m_strDeviceNum = strDeviceNum; }
		std::string GetDeviceNum() const { return m_strDeviceNum; }

	private:
		long			m_lCSTACallRefId;	// 
		std::string		m_strDeviceNum;		// ��ϯ�ֻ���
	};
	typedef boost::shared_ptr<CAnswerCallResultNotif> IAnswerCallResultNotifPtr;*/

	// ����ͨ��
	class CClearCallNotif : public CSwitchRequestNotif
	{
	public:
		CClearCallNotif(){
			m_strSponsor = "";
			m_strTarget = "";
			m_strCTICallRefId = "";
		};
		virtual ~CClearCallNotif(){};

	public:
		void SetSponsor(const std::string& strSponsor){ m_strSponsor = strSponsor; }
		std::string GetSponsor() const { return m_strSponsor; }

		void SetTarget(const std::string& strTarget){ m_strTarget = strTarget; }
		std::string GetTarget() const { return m_strTarget; }

		void SetCTICallRefId(const std::string& strCTICallRefId){ m_strCTICallRefId = strCTICallRefId; }
		std::string GetCTICallRefId() const { return m_strCTICallRefId; }

	private:
		std::string		m_strSponsor;		// �������ͨ������ϯ�ֻ���
		std::string		m_strTarget;		// ��ǿ��ķֻ���
		std::string		m_strCTICallRefId;
	};
	typedef boost::shared_ptr<CClearCallNotif> IClearCallNotifPtr;

	// ����
	class CListenCallNotif : public CSwitchRequestNotif
	{
	public:
		CListenCallNotif(){
			m_strSponsor = "";
			m_strTarget = "";
		};
		virtual ~CListenCallNotif(){};

	public:
		void SetSponsor(const std::string& strSponsor){ m_strSponsor = strSponsor; }
		std::string GetSponsor() const { return m_strSponsor; }

		void SetTarget(const std::string& strTarget){ m_strTarget = strTarget; }
		std::string GetTarget() const { return m_strTarget; }

	private:
		std::string		m_strSponsor;		// ���������ߵķֻ���
		std::string		m_strTarget;		// �������ķֻ���
	};
	typedef boost::shared_ptr<CListenCallNotif> IListenCallNotifPtr;

	// ����
	class CPickupCallNotif : public CSwitchRequestNotif
	{
	public:
		CPickupCallNotif(){
			m_strSponsor = "";
			m_strTarget = "";
		};
		virtual ~CPickupCallNotif(){};

	public:
		void SetSponsor(const std::string& strSponsor){ m_strSponsor = strSponsor; }
		std::string GetSponsor() const { return m_strSponsor; }

		void SetTarget(const std::string& strTarget){ m_strTarget = strTarget; }
		std::string GetTarget() const { return m_strTarget; }

	private:
		std::string		m_strSponsor;		// �������ߵķֻ���
		std::string		m_strTarget;		// ������ķֻ���
	};
	typedef boost::shared_ptr<CPickupCallNotif> IPickupCallNotifPtr;

	// ǿ��
	class CBargeInCallNotif : public CSwitchRequestNotif
	{
	public:
		CBargeInCallNotif(){
			m_strSponsor = "";
			m_strTarget = "";
		};
		virtual ~CBargeInCallNotif(){};

	public:
		void SetSponsor(const std::string& strSponsor){ m_strSponsor = strSponsor; }
		std::string GetSponsor() const { return m_strSponsor; }

		void SetTarget(const std::string& strTarget){ m_strTarget = strTarget; }
		std::string GetTarget() const { return m_strTarget; }

	private:
		std::string		m_strSponsor;		// ǿ�巢���ߵķֻ���
		std::string		m_strTarget;		// ��ǿ��ķֻ���
	};
	typedef boost::shared_ptr<CBargeInCallNotif> IBargeInCallNotifPtr;

	// ǿ��
	class CForcePopCallNotif : public CSwitchRequestNotif
	{
	public:
		CForcePopCallNotif(){
			m_strSponsor = "";
			m_strTarget = "";
		};
		virtual ~CForcePopCallNotif(){};

	public:
		void SetSponsor(const std::string& strSponsor){ m_strSponsor = strSponsor; }
		std::string GetSponsor() const { return m_strSponsor; }

		void SetTarget(const std::string& strTarget){ m_strTarget = strTarget; }
		std::string GetTarget() const { return m_strTarget; }

	private:
		std::string		m_strSponsor;		// ǿ�����ߵķֻ���
		std::string		m_strTarget;		// ��ǿ��ķֻ���
	};
	typedef boost::shared_ptr<CForcePopCallNotif> IForcePopCallNotifPtr;

	//��ѯ
	class CConsultationCallNotif : public CSwitchRequestNotif
	{
	public:
		CConsultationCallNotif(){
			m_bPreConsultationCall = true;
			m_bIsTransferCall = false;
			m_ulCallHandle = 0;
			m_strSponsor = "";
			m_strTargetACDGrp = "";
			m_strTargetDevice = "";
			m_strTargetDeviceType = "";
		};
		virtual ~CConsultationCallNotif(){};

	public:
		void SetPreConsultationCall(bool bPreConsultationCall){ m_bPreConsultationCall = bPreConsultationCall; }
		bool GetPreConsultationCall(){ return m_bPreConsultationCall; }

		void SetIsTransferCall(bool bIsTransferCall){ m_bIsTransferCall = bIsTransferCall; }
		bool GetIsTransferCall(){ return m_bIsTransferCall; }

		void SetSponsor(const std::string& strSponsor){ m_strSponsor = strSponsor; }
		std::string GetSponsor() const { return m_strSponsor; }

		void SetTargetACDGrp(const std::string& strTargetACDGrp){ m_strTargetACDGrp = strTargetACDGrp; }
		std::string GetTargetACDGrp() const { return m_strTargetACDGrp; }

		void SetTargetDevice(const std::string& strTargetDevice){ m_strTargetDevice = strTargetDevice; }
		std::string GetTargetDevice() const { return m_strTargetDevice; }

		void SetTargetDeviceType(const std::string& strTargetDeviceType){ m_strTargetDeviceType = strTargetDeviceType; }
		std::string GetTargetDeviceType() const { return m_strTargetDeviceType; }

		void SetCallHandle(unsigned long ulCallHandle){ m_ulCallHandle = ulCallHandle; }
		unsigned long GetCallHandle() const { return m_ulCallHandle; }

	private:
		bool			m_bPreConsultationCall;	//	��ѯǰ�ò���
		bool			m_bIsTransferCall;
		unsigned long	m_ulCallHandle;			//	������������ IPO
		std::string		m_strSponsor;			//	��ѯ������
		std::string		m_strTargetACDGrp;		//	��ѯĿ�귽����ţ����ڰ���λת����
		std::string		m_strTargetDevice;		//	��ѯĿ�귽
		std::string		m_strTargetDeviceType;	//	��ѯĿ�귽����
	};
	typedef boost::shared_ptr<CConsultationCallNotif> IConsultationCallNotifPtr;
	// ��ѯ ���
	class CConsultationCallResultNotif : public CSwitchResultNotif
	{
	public:
		CConsultationCallResultNotif(){
			m_lActiveCSTACallRefId = DEFAULT_CALLREFID;
			m_lHeldCSTACallRefId = DEFAULT_CALLREFID;
			m_ulCallHandle = 0;
			m_strActiveCTICallRefId = "";
			m_strHeldCTICallRefId = "";
			m_strOriginalCallerId = "";
			m_strOriginalCalledId = "";
			m_strSponsor = "";
			m_strTargetDevice = "";
		};
		virtual ~CConsultationCallResultNotif(){};

	public:
		void SetActiveCSTACallRefId(long lCallRefId){ m_lActiveCSTACallRefId = lCallRefId; }
		long GetActiveCSTACallRefId() const { return m_lActiveCSTACallRefId; }

		void SetHeldCSTACallRefId(long lCallRefId){ m_lHeldCSTACallRefId = lCallRefId; }
		long GetHeldCSTACallRefId() const { return m_lHeldCSTACallRefId; }

		void SetActiveCTICallRefId(const std::string& strActiveCTICallRefId){ m_strActiveCTICallRefId = strActiveCTICallRefId; }
		std::string GetActiveCTICallRefId() const { return m_strActiveCTICallRefId; }

		void SetHeldCTICallRefId(const std::string& strHeldCTICallRefId){ m_strHeldCTICallRefId = strHeldCTICallRefId; }
		std::string GetHeldCTICallRefId() const { return m_strHeldCTICallRefId; }

		void SetOriginalCallerId(const std::string& strOriginalCallerId){ m_strOriginalCallerId = strOriginalCallerId; }
		std::string GetOriginalCallerId() const { return m_strOriginalCallerId; }

		void SetOriginalCalledId(const std::string& strOriginalCalledId){ m_strOriginalCalledId = strOriginalCalledId; }
		std::string GetOriginalCalledId() const { return m_strOriginalCalledId; }

		void SetSponsor(const std::string& strSponsor){ m_strSponsor = strSponsor; }
		std::string GetSponsor() const { return m_strSponsor; }

		void SetTargetDevice(const std::string& strTargetDevice){ m_strTargetDevice = strTargetDevice; }
		std::string GetTargetDevice() const { return m_strTargetDevice; }

		void SetCallHandle(unsigned long ulCallHandle){ m_ulCallHandle = ulCallHandle; }
		unsigned long GetCallHandle() const { return m_ulCallHandle; }

	private:
		long			m_lActiveCSTACallRefId;	// �»��� ID 
		long			m_lHeldCSTACallRefId;	// ���������� ID
		unsigned long	m_ulCallHandle;			//	������������ IPO
		std::string		m_strActiveCTICallRefId;
		std::string		m_strHeldCTICallRefId;
		std::string		m_strOriginalCallerId;	//	ԭʼ����
		std::string		m_strOriginalCalledId;	//	ԭʼ����
		std::string		m_strSponsor;			// ������ϯ�ֻ���
		std::string		m_strTargetDevice;		// Ŀ�귽��ϯ�ֻ���
	};
	typedef boost::shared_ptr<CConsultationCallResultNotif> IConsultationCallResultNotifPtr;

	// ת��
	class CTransferCallNotif : public CSwitchRequestNotif
	{
	public:
		CTransferCallNotif(){
			m_strActiveCTICallRefId = "";
			m_strHeldCTICallRefId = "";
			m_strSponsor = "";			//	����
			m_strTarget = "";			//	����
		};
		virtual ~CTransferCallNotif(){};

	public:
		void SetActiveCTICallRefId(const std::string& strActiveCTICallRefId){ m_strActiveCTICallRefId = strActiveCTICallRefId; }
		std::string GetActiveCTICallRefId() const { return m_strActiveCTICallRefId; }

		void SetHeldCTICallRefId(const std::string& strHeldCTICallRefId){ m_strHeldCTICallRefId = strHeldCTICallRefId; }
		std::string GetHeldCTICallRefId() const { return m_strHeldCTICallRefId; }

		void SetSponsor(const std::string& strSponsor){ m_strSponsor = strSponsor; }
		std::string GetSponsor() const { return m_strSponsor; }

		void SetTarget(const std::string& strTarget){ m_strTarget = strTarget; }
		std::string GetTarget() const { return m_strTarget; }

	private:
		std::string		m_strActiveCTICallRefId;	//	��ǰ���� ID
		std::string		m_strHeldCTICallRefId;		//	�������� ID
		std::string		m_strSponsor;				//	ƫת������
		std::string		m_strTarget;				//	ƫתĿ�귽
	};
	typedef boost::shared_ptr<CTransferCallNotif> ITransferCallNotifPtr;
	// ת�� ���
	class CTransferCallResultNotif : public CSwitchResultNotif
	{
	public:
		CTransferCallResultNotif(){
			m_lActiveCSTACallRefId = DEFAULT_CALLREFID;
			m_lHeldCSTACallRefId = DEFAULT_CALLREFID;
			m_strActiveCTICallRefId = "";
			m_strHeldCTICallRefId = "";
			m_strSponsor = "";
			m_strTargetDevice = "";
		};
		virtual ~CTransferCallResultNotif(){};

	public:
		void SetActiveCSTACallRefId(long lCallRefId){ m_lActiveCSTACallRefId = lCallRefId; }
		long GetActiveCSTACallRefId() const { return m_lActiveCSTACallRefId; }

		void SetHeldCSTACallRefId(long lCallRefId){ m_lHeldCSTACallRefId = lCallRefId; }
		long GetHeldCSTACallRefId() const { return m_lHeldCSTACallRefId; }

		void SetActiveCTICallRefId(const std::string& strActiveCTICallRefId){ m_strActiveCTICallRefId = strActiveCTICallRefId; }
		std::string GetActiveCTICallRefId() const { return m_strActiveCTICallRefId; }

		void SetHeldCTICallRefId(const std::string& strHeldCTICallRefId){ m_strHeldCTICallRefId = strHeldCTICallRefId; }
		std::string GetHeldCTICallRefId() const { return m_strHeldCTICallRefId; }

		void SetSponsor(const std::string& strSponsor){ m_strSponsor = strSponsor; }
		std::string GetSponsor() const { return m_strSponsor; }

		void SetTargetDevice(const std::string& strTargetDevice){ m_strTargetDevice = strTargetDevice; }
		std::string GetTargetDevice() const { return m_strTargetDevice; }

	private:
		long			m_lActiveCSTACallRefId;	// �»��� ID 
		long			m_lHeldCSTACallRefId;	// ���������� ID
		std::string		m_strActiveCTICallRefId;
		std::string		m_strHeldCTICallRefId;
		std::string		m_strSponsor;			// ������ϯ�ֻ���
		std::string		m_strTargetDevice;		// Ŀ�귽��ϯ�ֻ���
	};
	typedef boost::shared_ptr<CTransferCallResultNotif> ITransferCallResultNotifPtr;

	// ƫת����
	class CDeflectCallNotif : public CSwitchRequestNotif
	{
	public:
		CDeflectCallNotif(){
			m_strSponsor = "";			//	����
			m_strTarget = "";			//	����
			m_strOrgCaller = "";		//  �ⲿ�绰����
			m_strOrgCalled = "";		//  �ⲿ�绰���к���
		};
		virtual ~CDeflectCallNotif(){};

	public:
		void SetSponsor(const std::string& strSponsor){ m_strSponsor = strSponsor; }
		std::string GetSponsor() const { return m_strSponsor; }

		void SetTarget(const std::string& strTarget){ m_strTarget = strTarget; }
		std::string GetTarget() const { return m_strTarget; }

		void SetOrgCaller(const std::string& strOrgCaller) { m_strOrgCaller = strOrgCaller; }
		std::string GetOrgCaller() const { return m_strOrgCaller; }

		void SetOrgCalled(const std::string& strOrgCalled) { m_strOrgCalled = strOrgCalled; }
		std::string GetOrgCalled() const { return m_strOrgCalled; }

	private:
		std::string		m_strSponsor;		//	��ѯ������
		std::string		m_strTarget;		//	��ѯĿ�귽
		std::string		m_strOrgCaller;		//	�ⲿ�绰����
		std::string		m_strOrgCalled;		//	�ⲿ�绰���к���
	};
	typedef boost::shared_ptr<CDeflectCallNotif> IDeflectCallNotifPtr;

	class CHoldCallNotif : public CSwitchRequestNotif
	{
	public:
		CHoldCallNotif(){
			m_strSponsor = "";
			m_strDevice = "";
		};
		virtual ~CHoldCallNotif(){};

	public:
		void SetSponsor(const std::string& strSponsor){ m_strSponsor = strSponsor; }
		std::string GetSponsor() const { return m_strSponsor; }

		void SetDevice(const std::string& strDevice){ m_strDevice = strDevice; }
		std::string GetDevice() const { return m_strDevice; }

	private:
		std::string		m_strSponsor;
		std::string		m_strDevice;
	};
	typedef boost::shared_ptr<CHoldCallNotif> IHoldCallNotifPtr;

	class CRetriveCallNotif : public CSwitchRequestNotif
	{
	public:
		CRetriveCallNotif(){
			m_strSponsor = "";
			m_strDevice = "";
		};
		virtual ~CRetriveCallNotif(){};

	public:
		void SetSponsor(const std::string& strSponsor){ m_strSponsor = strSponsor; }
		std::string GetSponsor() const { return m_strSponsor; }

		void SetDevice(const std::string& strDevice){ m_strDevice = strDevice; }
		std::string GetDevice() const { return m_strDevice; }

	private:
		std::string		m_strSponsor;
		std::string		m_strDevice;
	};
	typedef boost::shared_ptr<CRetriveCallNotif> IRetriveCallNotifPtr;

	// ·�ɻ���
	class CRouteCallNotif : public CSwitchRequestNotif
	{
	public:
		CRouteCallNotif(){
			m_strCallerId = "";			//	����
			m_lRegisterReqId = 0;
			m_lCrossRefId = 0;
			m_bIsBlackCall = false;
		};
		virtual ~CRouteCallNotif(){};

	public:
		void SetRegisterReqId(long lRegisterReqId){ m_lRegisterReqId = lRegisterReqId; }
		long GetRegisterReqId() const { return m_lRegisterReqId; }

		void SetCrossRefId(long lCrossRefId){ m_lCrossRefId = lCrossRefId; }
		long GetCrossRefId() const { return m_lCrossRefId; }

		void SetCallerId(const std::string& strCallerId){ m_strCallerId = strCallerId; }
		std::string GetCallerId() const { return m_strCallerId; }

		void SetRouteDest(const std::string& strRouteDest){ m_strRouteDest = strRouteDest; }
		std::string GetRouteDest() const { return m_strRouteDest; }

		void SetIsBlackCall(bool bIsBlackCall){ m_bIsBlackCall = bIsBlackCall; }
		bool GetIsBlackCall(){ return m_bIsBlackCall; }

	private:
		long			m_lRegisterReqId;
		long			m_lCrossRefId;
		bool			m_bIsBlackCall;
		std::string		m_strCallerId;		//	����
		std::string		m_strRouteDest;		//	·��Ŀ���ַ
	};
	typedef boost::shared_ptr<CRouteCallNotif> IRouteCallNotifPtr;

	// �ܽӻ���
	class CRefuseCallNotif : public CSwitchRequestNotif
	{
	public:
		CRefuseCallNotif(){
			m_strCallerId = "";			//	����
			m_strCalledId = "";			//	����
			m_strStateTime = "";		//	����ʱ��
		};
		virtual ~CRefuseCallNotif(){};

	public:
		void SetCallerId(const std::string& strCallerId){ m_strCallerId = strCallerId; }
		std::string GetCallerId() const { return m_strCallerId; }

		void SetCalledId(const std::string& strCalledId){ m_strCalledId = strCalledId; }
		std::string GetCalledId() const { return m_strCalledId; }

		void SetRefuseReason(const std::string& strRefuseReason){ m_strRefuseReason = strRefuseReason; }
		std::string GetRefuseReason() const { return m_strRefuseReason; }

		void SetStateTime(const std::string& strStateTime){ m_strStateTime = strStateTime; }
		std::string GetStateTime() const { return m_strStateTime; }

	private:
		std::string		m_strCallerId;		//	����
		std::string		m_strCalledId;		//	����
		std::string		m_strRefuseReason;	//	�ܽ�ԭ��
		std::string		m_strStateTime;		//	����״̬ʱ��
	};
	typedef boost::shared_ptr<CRefuseCallNotif> IRefuseCallNotifPtr;

	//	��������
	class CReconnectCallNotif : public CSwitchRequestNotif
	{
	public:
		CReconnectCallNotif(){
			m_strActiveCTICallRefId = "";
			m_strHeldCTICallRefId = "";
			m_strSponsor = "";
			m_strDevice = "";
		};
		virtual ~CReconnectCallNotif(){};

	public:
		void SetActiveCTICallRefId(const std::string& strActiveCTICallRefId){ m_strActiveCTICallRefId = strActiveCTICallRefId; }
		std::string GetActiveCTICallRefId() const { return m_strActiveCTICallRefId; }

		void SetHeldCTICallRefId(const std::string& strHeldCTICallRefId){ m_strHeldCTICallRefId = strHeldCTICallRefId; }
		std::string GetHeldCTICallRefId() const { return m_strHeldCTICallRefId; }

		void SetSponsor(const std::string& strSponsor){ m_strSponsor = strSponsor; }
		std::string GetSponsor() const { return m_strSponsor; }

		void SetDevice(const std::string& strDevice){ m_strDevice = strDevice; }
		std::string GetDevice() const { return m_strDevice; }

	private:
		std::string		m_strActiveCTICallRefId;
		std::string		m_strHeldCTICallRefId;
		std::string		m_strSponsor;
		std::string		m_strDevice;
	};
	typedef boost::shared_ptr<CReconnectCallNotif> IReconnectCallNotifPtr;

	// ͨ���������
	class CConferenceCallNotif : public CSwitchRequestNotif
	{
	public:
		CConferenceCallNotif(){
			m_strActiveCTICallRefId = "";
			m_strHeldCTICallRefId = "";
			m_strTarget = "";
		};
		virtual ~CConferenceCallNotif(){};

	public:
		void SetActiveCTICallRefId(const std::string& strActiveCTICallRefId){ m_strActiveCTICallRefId = strActiveCTICallRefId; }
		std::string GetActiveCTICallRefId() const { return m_strActiveCTICallRefId; }

		void SetHeldCTICallRefId(const std::string& strHeldCTICallRefId){ m_strHeldCTICallRefId = strHeldCTICallRefId; }
		std::string GetHeldCTICallRefId() const { return m_strHeldCTICallRefId; }

		void SetTarget(const std::string& strTarget){ m_strTarget = strTarget; }
		std::string GetTarget() const { return m_strTarget; }

	private:
		std::string		m_strActiveCTICallRefId;
		std::string		m_strHeldCTICallRefId;
		std::string		m_strTarget;
	};
	typedef boost::shared_ptr<CConferenceCallNotif> IConferenceCallNotifPtr;
	// ���������
	class CConferenceCallResultNotif : public CSwitchResultNotif
	{
	public:
		CConferenceCallResultNotif(){
			m_ulCallHandle = 0;
			m_lCSTACallRefId = DEFAULT_CALLREFID;
			m_strCTICallRefId = "";
			m_strSponsor = "";
			m_strTargetDevice = "";
		};
		virtual ~CConferenceCallResultNotif(){};

	public:
		void SetCallHandle(long lCallHandle){ m_ulCallHandle = lCallHandle; }
		unsigned long GetCallHandle() const { return m_ulCallHandle; }

		void SetCSTACallRefId(long lCallRefId){ m_lCSTACallRefId = lCallRefId; }
		long GetCSTACallRefId() const { return m_lCSTACallRefId; }

		void SetCTICallRefId(const std::string& strCTICallRefId){ m_strCTICallRefId = strCTICallRefId; }
		std::string GetCTICallRefId() const { return m_strCTICallRefId; }

		void SetSponsor(const std::string& strSponsor){ m_strSponsor = strSponsor; }
		std::string GetSponsor() const { return m_strSponsor; }

		void SetTargetDevice(const std::string& strTargetDevice){ m_strTargetDevice = strTargetDevice; }
		std::string GetTargetDevice() const { return m_strTargetDevice; }

	private:
		unsigned long	m_ulCallHandle;		// ������������ IPO
		long			m_lCSTACallRefId;	// �»��� ID 
		std::string		m_strCTICallRefId;
		std::string		m_strSponsor;			// ������ϯ�ֻ���
		std::string		m_strTargetDevice;		// Ŀ�귽��ϯ�ֻ���
	};
	typedef boost::shared_ptr<CConferenceCallResultNotif> IConferenceCallResultNotifPtr;

	class CAddConferencePartyNotif : public CSwitchRequestNotif
	{
	public:
		CAddConferencePartyNotif(){
			m_strActiveCTICallRefId = "";
			m_strHeldCTICallRefId = "";
			m_strCompere = "";
			m_strTarget = "";
			m_strTargetDeviceType = "";
		};
		virtual ~CAddConferencePartyNotif(){};

	public:
		void SetHeldCTICallRefId(const std::string& strHeldCTICallRefId){ m_strHeldCTICallRefId = strHeldCTICallRefId; }
		std::string GetHeldCTICallRefId() const { return m_strHeldCTICallRefId; }

		void SetActiveCTICallRefId(const std::string& strActiveCTICallRefId){ m_strActiveCTICallRefId = strActiveCTICallRefId; }
		std::string GetActiveCTICallRefId() const { return m_strActiveCTICallRefId; }

		void SetCompere(const std::string& strCompere){ m_strCompere = strCompere; }
		std::string GetCompere() const { return m_strCompere; }

		void SetTarget(const std::string& strTarget){ m_strTarget = strTarget; }
		std::string GetTarget() const { return m_strTarget; }

		void SetTargetDeviceType(const std::string& strTargetDeviceType){ m_strTargetDeviceType = strTargetDeviceType; }
		std::string GetTargetDeviceType() const { return m_strTargetDeviceType; }

	private:
		std::string		m_strActiveCTICallRefId;
		std::string		m_strHeldCTICallRefId;
		std::string		m_strCompere;
		std::string		m_strTarget;
		std::string		m_strTargetDeviceType;
	};
	typedef boost::shared_ptr<CAddConferencePartyNotif> IAddConferencePartyNotifPtr;

	class CDeleteConferencePartyNotif : public CSwitchRequestNotif
	{
	public:
		CDeleteConferencePartyNotif(){
			m_strCTICallRefId = "";
			m_strCompere = "";
			m_strTarget = "";
		};
		virtual ~CDeleteConferencePartyNotif(){};

	public:
		void SetCTICallRefId(const std::string& strCTICallRefId){ m_strCTICallRefId = strCTICallRefId; }
		std::string GetCTICallRefId() const { return m_strCTICallRefId; }

		void SetCompere(const std::string& strCompere){ m_strCompere = strCompere; }
		std::string GetCompere() const { return m_strCompere; }

		void SetTarget(const std::string& strTarget){ m_strTarget = strTarget; }
		std::string GetTarget() const { return m_strTarget; }

	private:
		std::string		m_strCTICallRefId;
		std::string		m_strCompere;
		std::string		m_strTarget;
	};
	typedef boost::shared_ptr<CDeleteConferencePartyNotif> IDeleteConferencePartyNotifPtr;

	//	�ӹܻ���
	class CTakeOverCallNotif : public CSwitchRequestNotif
	{
	public:
		CTakeOverCallNotif(){
			m_lOriginalRequestId = DEFAULT_TASKID;
			m_strCTICallRefId = "";
			m_strSponsor = "";
			m_strTarget = "";
			m_strStep = Step_StopListenCall;
		};
		virtual ~CTakeOverCallNotif(){};

	public:
		void SetOriginalRequestId(const long lOriginalRequestId){ m_lOriginalRequestId = lOriginalRequestId; }
		long GetOriginalRequestId() const { return m_lOriginalRequestId; }

		void SetCTICallRefId(const std::string& strCTICallRefId){ m_strCTICallRefId = strCTICallRefId; }
		std::string GetCTICallRefId() const { return m_strCTICallRefId; }

		void SetSponsor(const std::string& strSponsor){ m_strSponsor = strSponsor; }
		std::string GetSponsor() const { return m_strSponsor; }

		void SetTarget(const std::string& strTarget){ m_strTarget = strTarget; }
		std::string GetTarget() const { return m_strTarget; }

		void SetStep(const std::string& strStep){ m_strStep = strStep; }
		std::string GetStep() const { return m_strStep; }

	private:
		long			m_lOriginalRequestId;	// �ӹܷ�����ִ�У������� RequestId ��ͬ�����Ľ���践��ԭ RequestId
		std::string		m_strCTICallRefId;
		std::string		m_strSponsor;
		std::string		m_strTarget;
		std::string		m_strStep;		//StopListenCall, BargeInCall, Hangup
	};
	typedef boost::shared_ptr<CTakeOverCallNotif> ITakeOverCallNotifPtr;

	// �ӹܻ��� ���
	class CTakeOverCallResultNotif : public CSwitchResultNotif
	{
	public:
		CTakeOverCallResultNotif(){
			m_strSponsor = "";
			m_strTarget = "";
		};
		virtual ~CTakeOverCallResultNotif(){};

	public:
		void SetSponsor(const std::string& strSponsor){ m_strSponsor = strSponsor; }
		std::string GetSponsor() const { return m_strSponsor; }

		void SetTarget(const std::string& strTarget){ m_strTarget = strTarget; }
		std::string GetTarget() const { return m_strTarget; }

	private:
		std::string		m_strSponsor;			// ������ϯ�ֻ���
		std::string		m_strTarget;			// Ŀ�귽��ϯ�ֻ���
	};
	typedef boost::shared_ptr<CTakeOverCallResultNotif> ITakeOverCallResultNotifPtr;

	//	���ú�����
	class CSetBlackListNotif : public CSwitchRequestNotif
	{
	public:
		CSetBlackListNotif(){
		};
		virtual ~CSetBlackListNotif(){};

	public:
		void SetLimitNum(const std::string& strLimitNum){ m_strLimitNum = strLimitNum; }
		std::string GetLimitNum(){ return m_strLimitNum; }

		void SetLimitType(const std::string& strLimitType){ m_strLimitType = strLimitType; }
		std::string GetLimitType(){ return m_strLimitType; }

	private:
		std::string m_strLimitNum;		//	����������
		std::string m_strLimitType;
	};
	typedef boost::shared_ptr<CSetBlackListNotif> ISetBlackListNotifPtr;


	//////////////////////////////////////////////////////////////////////////

	// ��ȡ CTI �� ����������������״̬
	class CGetCTIConnStateResultNotif : public CSwitchResultNotif
	{
	public:
		CGetCTIConnStateResultNotif(){
			m_bStreamOpen = false;
			m_strSwitchType = "0";
		};
		virtual ~CGetCTIConnStateResultNotif(){};

	public:
		void SetStreamOpen(bool bOpen){ m_bStreamOpen = bOpen; }
		bool GetStreamOpen() const { return m_bStreamOpen; }

		void SetSwitchType(std::string strSwitchType){ m_strSwitchType = strSwitchType; }
		std::string GetSwitchType() const{ return m_strSwitchType; }

	private:
		bool m_bStreamOpen;		// AES �������Ƿ�ɹ�
		std::string m_strSwitchType;	// �������ӿ����ͣ�0 TSAPI�� 1 TAPI
	};
	typedef boost::shared_ptr<CGetCTIConnStateResultNotif> IGetCTIConnStateResultNotifPtr;

	// ��ȡ�����豸״̬
	class CGetDeviceListResultNotif : public CSwitchResultNotif
	{
	public:
		CGetDeviceListResultNotif(){
		};
		virtual ~CGetDeviceListResultNotif(){};

	public:
		void AddDeviceList(const std::string& strDevice, const std::string& strDeviceType, 
			const std::string& strDeviceState, const std::string& strStateTime)
		{
			boost::shared_ptr<CDeviceStateNotif> pObj = boost::make_shared<CDeviceStateNotif>();
			pObj->SetDeviceNum(strDevice);
			pObj->SetDeviceType(strDeviceType);
			pObj->SetDeviceState(strDeviceState);
			pObj->SetStateTime(strStateTime);

			m_DeviceList.push_back(pObj);
		}
		void AddDeviceList(boost::shared_ptr<CDeviceStateNotif> pObj)
		{
			m_DeviceList.push_back(pObj);
		}

		int GetDeviceListSize(){ return m_DeviceList.size(); }

	//	std::list<boost::shared_ptr<CDeviceStateNotif>> GetDeviceList(){ return m_DeviceList; }

		std::list<boost::shared_ptr<CDeviceStateNotif>> m_DeviceList;
	};
	typedef boost::shared_ptr<CGetDeviceListResultNotif> IGetDeviceListResultNotifPtr;

	// ��ȡ���� ACD ���� Agent ��Ա����
	class CGetACDListRequestNotif : public CSwitchRequestNotif
	{
	public:
		CGetACDListRequestNotif(){
			//
		};
		virtual ~CGetACDListRequestNotif(){};
	};
	typedef boost::shared_ptr<CGetACDListRequestNotif> IGetACDListRequestNotifPtr;
	// ��ȡ���� ACD ���� Agent ��Ա���
	class CGetACDListResultNotif : public CSwitchResultNotif
	{
	public:
		CGetACDListResultNotif(){
		};
		virtual ~CGetACDListResultNotif(){};

	public:
		void AddAgentList(const std::string&  strAgentId, const std::string& strACDGrp, const std::string& strStateTime)
		{
			boost::shared_ptr<CAgentStateNotif> pObj = boost::make_shared<CAgentStateNotif>();
			pObj->SetAgentId(strAgentId);
			pObj->SetACDGrp(strACDGrp);
			pObj->SetStateTime(strStateTime);

			m_AgentList.push_back(pObj);
		}
		int GetAgentListSize(){ return m_AgentList.size(); }

		std::list<boost::shared_ptr<CAgentStateNotif>> GetAgentList(){ return m_AgentList; }

	private:
		std::list<boost::shared_ptr<CAgentStateNotif>> m_AgentList;
	};
	typedef boost::shared_ptr<CGetACDListResultNotif> IGetACDListResultNotifPtr;

	// ��ȡ���� Agent ״̬����
	class CGetAgentListRequestNotif : public CSwitchRequestNotif
	{
	public:
		CGetAgentListRequestNotif(){
			m_strACDGrp = "";
			m_strDeptCode = "";

		};
		virtual ~CGetAgentListRequestNotif(){};

	public:
		void SetACDGrp(const std::string& strACDGrp){ m_strACDGrp = strACDGrp; }
		std::string GetACDGrp() const { return m_strACDGrp; }

		void SetDeptCode(const std::string& strDeptCode) { m_strDeptCode = strDeptCode; }
		std::string GetDeptCode() const { return m_strDeptCode; }
	private:
		std::string m_strACDGrp;		// ACD Group
		std::string m_strDeptCode;
	};
	typedef boost::shared_ptr<CGetAgentListRequestNotif> IGetAgentListRequestNotifPtr;
	// ��ȡ���� Agent ״̬���
	class CGetAgentListResultNotif : public CSwitchResultNotif
	{
	public:
		CGetAgentListResultNotif(){
		};
		virtual ~CGetAgentListResultNotif(){};

	public:
		void AddAgentList(const std::string&  strAgentId, const std::string& strACDGrp,
			const std::string& strLoginMode, const std::string& strReadyState, const std::string& strStateTime)
		{
			boost::shared_ptr<CAgentStateNotif> pObj = boost::make_shared<CAgentStateNotif>();
			pObj->SetAgentId(strAgentId);
			pObj->SetACDGrp(strACDGrp);
			pObj->SetLoginMode(strLoginMode);
			pObj->SetReadyState(strReadyState);
			pObj->SetStateTime(strStateTime);

			m_AgentList.push_back(pObj);
		}
		int GetAgentListSize(){ return m_AgentList.size(); }

		std::list<boost::shared_ptr<CAgentStateNotif>> GetAgentList(){ return m_AgentList; }

	private:
		std::list<boost::shared_ptr<CAgentStateNotif>> m_AgentList;
	};
	typedef boost::shared_ptr<CGetAgentListResultNotif> IGetAgentListResultNotifPtr;

	// ��ȡ���� Agent ״̬���
	class CGetFreeAgentResultNotif : public CSwitchResultNotif
	{
	public:
		CGetFreeAgentResultNotif() {
			m_strDeptCode = "";
		};
		virtual ~CGetFreeAgentResultNotif() {};

	public:
		void AddAgentList(const std::string& strAgentId, const std::string& strACDGrp,
			const std::string& strLoginMode, const std::string& strReadyState, const std::string& strStateTime)
		{
			boost::shared_ptr<CAgentStateNotif> pObj = boost::make_shared<CAgentStateNotif>();
			pObj->SetAgentId(strAgentId);
			pObj->SetACDGrp(strACDGrp);
			pObj->SetLoginMode(strLoginMode);
			pObj->SetReadyState(strReadyState);
			pObj->SetStateTime(strStateTime);

			m_AgentList.push_back(pObj);
		}
		int GetAgentListSize() { return m_AgentList.size(); }
		std::list<boost::shared_ptr<CAgentStateNotif>> GetAgentList() { return m_AgentList; }

		void SetDeptCode(const std::string& p_strDeptCode) {m_strDeptCode = p_strDeptCode;};
		std::string GetDeptCode() {return m_strDeptCode;};
	private:
		std::list<boost::shared_ptr<CAgentStateNotif>> m_AgentList;
		std::string m_strDeptCode;
	};
	typedef boost::shared_ptr<CGetFreeAgentResultNotif> IGetFreeAgentResultNotifPtr;

	class CGetCallListResultNotif : public CSwitchResultNotif
	{
	public:
		CGetCallListResultNotif(){
		};
		virtual ~CGetCallListResultNotif(){};

	public:
		class CCallObj
		{
		public:
			long			m_lCSTACallRefId;			// �����������Ļ���ID
			std::string		m_strCTICallRefId;			//	Ӧ�ò㻰�� ID

			std::string		m_strCallerId;				// ���к���
			std::string		m_strCalledId;				// ���к���
			std::string		m_strCalledParty;			// ���к���

			std::string		m_strCallState;				// ����״̬(incoming, waiting, assing, ring,connect,hangup...)
			std::string		m_strStateTime;				// ��ǰ����״̬ʱ��

			std::string		m_strCallDierection;		// ����: Out  ����: In
			std::string		m_strCallMode;				// ��һ���У�single, ������У�Conference
		};

		int GetCallListSize(){ return m_CallList.size(); }

		//	std::list<boost::shared_ptr<CCallObj>> GetCallList(){ return m_CallList; }

		std::list<CCallObj> m_CallList;
	};
	typedef boost::shared_ptr<CGetCallListResultNotif> IGetCallListResultNotifPtr;

	// ��ȡ ACD ��һ�����е� Agent
	class CGetReadyAgentNotif : public CSwitchRequestNotif
	{
	public:
		CGetReadyAgentNotif(){
			m_strACDGrp = "";
		};
		virtual ~CGetReadyAgentNotif(){};

	public:
		void SetACDGrp(const std::string& strACDGrp){ m_strACDGrp = strACDGrp; }
		std::string GetACDGrp() const { return m_strACDGrp; }

	private:
		std::string		m_strACDGrp;
	};
	typedef boost::shared_ptr<CGetReadyAgentNotif> IGetReadyAgentNotifPtr;
	// ��ȡ ACD ��һ�����е� Agent ���
	class CGetReadyAgentResultNotif : public CSwitchResultNotif
	{
	public:
		CGetReadyAgentResultNotif(){
			m_strReadyAgent = "";
		};
		virtual ~CGetReadyAgentResultNotif(){};

	public:
		void SetReadyAgent(const std::string& strReadyAgent){ m_strReadyAgent = strReadyAgent; }
		std::string GetReadyAgent() const { return m_strReadyAgent; }

	private:
		std::string		m_strReadyAgent;
	};
	typedef boost::shared_ptr<CGetReadyAgentResultNotif> IGetReadyAgentResultNotifPtr;


	// ��ȡFS��չ���
	class CFSAesExEventNotif : public CSwitchResultNotif
	{
	public:
		CFSAesExEventNotif() {
			m_strEventData = "";
		};
		virtual ~CFSAesExEventNotif() {};

	public:
		void SetEventData(const std::string& strData) { m_strEventData = strData; }
		std::string GetEventData() const { return m_strEventData; }

	private:
		std::string	m_strEventData;
	};
	typedef boost::shared_ptr<CFSAesExEventNotif> IFSAesExEventNotifPtr;

}	// end namespace
