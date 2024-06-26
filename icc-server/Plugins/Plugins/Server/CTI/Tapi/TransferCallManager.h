#pragma once

/**
@Copyright Copyright (C), 2018
@file		TransferCallManager.h
@created	2018/01/23
@brief		转移话务管理类,负责转移话务的控制及实现

1、转警分为转指定坐席和转部门两类。
2、转警流程：
1）转部门，CTI 根据 ACD 查询该分组的空闲坐席，如无空闲坐席，转警失败；
2）CTI 咨询指定的坐席或转部门查询到的目标空闲坐席，如咨询失败，取回原话务，转警失败；
3）咨询成功，开始振铃超时检测，如振铃超时，取回原话务，转警失败；
4）转警目标方振铃过程中，如报警人或转警方挂机，话务结束，转警失败；
	如转警目标方拒绝接收转警，取回原话务，转警失败。

@author psy
*/

namespace ICC
{
	class CTransferCall
	{
	public:
		CTransferCall();
		virtual ~CTransferCall();

	public:
		bool				m_bTargetIsACD;				//	是否为转机构
		long				m_lGetReadyAgentTaskId;		//	查询空闲坐席的任务 ID
		long				m_lConsultationCallTaskId;	//	咨询产生的任务 ID
		long				m_lTransferCallTaskId;		//	转移产生的任务 ID

		/*std::string			m_strOriginalCallerId;		//	原始主叫
		std::string			m_strOriginalCalledId;		//	原始被叫*/
		std::string			m_strRelatedId;				//	请求协议的消息 ID
		std::string			m_strHeldCallRefId;			//	保留的话务 ID
		std::string			m_strActiveCallRefId;		//	咨询产生的话务 ID
		std::string			m_strDialTime;				//	开始咨询时间
		std::string			m_strRingTime;				//	被咨询方开始振铃时间
		std::string			m_strTargetState;			//	被咨询方的状态，振铃、通话、挂机
		std::string			m_strSponsor;				//	发起咨询的坐席号
		std::string			m_strTarget;				//	咨询目标坐席号
		std::string			m_strTargetDeviceType;		//	咨询目标的设备类型
	};

	//////////////////////////////////////////////////////////////////////////
	class CTransferCallManager
	{
	public:
		CTransferCallManager(void);
		virtual ~CTransferCallManager(void);

		static boost::shared_ptr<CTransferCallManager> Instance();
		void ExitInstance();

	public:
		void AddTransferCall(long p_lCSTACallRefId, long p_lRelatedCSTACallRefId);

		void DeleteTransferCall(long p_lCSTACallRefId);

		long GetRelatedCSTACallRefId(long p_lCSTACallRefId);

		long GetCSTACallRefId(long p_lRelatedCSTACallRefId);

		void ClearTransferCallList();
		void SetRingTimeout(int p_nTimeSpan){ m_nRingTimeout = p_nTimeSpan; }
		int GetRingTimeout(){ return m_nRingTimeout; }

		void SetLogPtr(Log::ILogPtr p_pLog){ m_pLog = p_pLog; }
		void SetDateTimePtr(DateTime::IDateTimePtr p_pDateTime){ m_pDateTime = p_pDateTime; }

		void AddNewTransferCall(const std::string& p_strHeldCallRefId, const std::string& p_strSponsor, const std::string& p_strTarget, 
			const std::string& p_strTargetDeviceType, const std::string& p_strMsgId, bool p_bIsACD);
		void CancelTransferCall(const std::string& p_strHeldCallRefId, const std::string& p_strSponsor, const std::string& p_strTarget);
		void DeleteTransferCall(const std::string& p_strHeldCallRefId);

		bool FindTransferCallByHeldCallRefId(const std::string& p_strHeldCallRefId);
		bool FindTransferCallByActiveCallRefId(const std::string& p_strActiveCallRefId);
		bool FindTransferCallByGetReadyAgentTaskId(long p_lTaskId);
		bool FindTransferCallByConsultationCallTaskId(long p_lTaskId);
		bool FindTransferCallByTransferCallTaskId(long p_lTaskId);

		void SetActiveCallRefId(const std::string& p_strHeldCallRefId, const std::string& p_strActiveCallRefId);
		bool GetCTICallRefIdByActiveCallRefId(const std::string& p_strActiveCTICallRefId, std::string& p_strCTICallRefId);

		//	转警结果
		void SendTransferCallResult(const std::string& p_strHeldCallRefId, int p_nResult);

		//	按 ACD 组查询空闲坐席
		long GetReadyAgent(const std::string& p_strACDGrp);
		void ProcessGetReadyAgentResult(long p_lTaskId, const std::string& p_strTarget, bool p_bResult);

		//	咨询转移目标
		long ConsultationCall(const std::string& p_strHeldCallRefId, const std::string& p_strSponsor, const std::string& p_strTarget, const std::string& p_strTargetDeviceType);
		void ProcessConsultationCallResult(long p_lTaskId, bool p_bResult, int p_nErrorCode);
		void ProcessConsultationCallResult(long p_lTaskId, const std::string& p_strHeldCallRefId, const std::string& p_strActiveCallRefId, bool p_bResult, int p_nErrorCode);
		
		void ProcessRingState(const std::string& p_strDeviceNum, const std::string& p_strActiveCallRefId);
		void ProcessTalkState(const std::string& p_strDeviceNum, const std::string& p_strActiveCallRefId);
		void ProcessHangupState(const std::string& p_strDeviceNum, const std::string& p_strCTICallRefId);
		void DeviceStateNotif(const std::string& p_strDeviceNum, const std::string& p_strActiveCallRefId, const std::string& p_strDeviceState);

		//	转移话务
		long TransferCall(const std::string& p_strActiveCallRefId, const std::string& p_strHeldCallRefId,
			const std::string& p_strSponsor, const std::string& p_strTarget);
		void ProcessTransferCallResult(long p_lTaskId, bool p_bResult);
		void ProcessTransferCallResult(long p_lTaskId, const std::string& p_strHeldCallRefId, const std::string& p_strActiveCallRefId, bool p_bResult);
		
		//	转移目标振铃超时 或 取消转移，重连原话务
		void HangupCall(const std::string& p_strCTICallRefId, const std::string& p_strSponsor, const std::string& p_strTarget);
		long RetrieveCall(const std::string& p_strHeldCTICallRefId, const std::string& p_strSponsor, const std::string& p_strTarget);
		void ReconnectCall(const std::string& p_strActiveCallRefId, const std::string& p_strHeldCallRefId,
			const std::string& p_strSponsor, const std::string& p_strTarget);

		//	原主叫挂机，挂断咨询话务，转移失败
		void ForcePopCall(const std::string& p_strActiveCallRefId, const std::string& p_strTarget);

		void ProcessFailedEvent(const std::string& p_strDeviceNum, const std::string& p_strCalledId, const std::string& p_strCTICallRefId);

		void ProcessDialTimeout(const std::string& p_strHeldCallRefId);
		bool FindDialTimeout(std::string& p_strHeldCallRefId);
		void ProcessRingTimeout(const std::string& p_strHeldCallRefId);
		bool FindRingTimeout(std::string& p_strHeldCallRefId);



	private:
		static boost::shared_ptr<CTransferCallManager> m_pManagerInstance;

		std::mutex	            m_transferCallMutex;
		std::map<long, long>    m_mTransferCallMap;
		std::map<std::string, boost::shared_ptr<CTransferCall>> m_mTransferCallList;

		int									m_nRingTimeout;
		Log::ILogPtr						m_pLog;
		DateTime::IDateTimePtr				m_pDateTime;
	};

}// end namespace