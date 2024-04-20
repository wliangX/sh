#pragma once

/*!
 * \file AvayaResult.h
 *	@Copyright Copyright (C), 2012-2013
 * \author PSY
 * \date 一月 2018
 *@brief 交换机事件格式化类
 *	负责将对应的事件，格式化成相应的字符串,供程序log显示和调试使用
 *	该类与Avaya交换机相匹配,与AvayaEvent类配合使用
 *
 */

namespace ICC
{
	class CAvayaResult
	{
	public:
		CAvayaResult(void);
		virtual ~CAvayaResult(void);

		static boost::shared_ptr<CAvayaResult> Instance();
		void ExitInstance();

		void Init(IResourceManagerPtr pResourceManager);

	private:
		static boost::shared_ptr<CAvayaResult> m_pResultInstance;

		StringUtil::IStringUtilPtr	m_pStrUtil;

	public:
		std::string GetEventClassString(EventClass_t eventClass);
		std::string GetConnectionID_tString(ConnectionID_t conn);
		std::string GetAgentModeString(AgentMode_t agentmode);

	//	std::string GetActionTypeString(E_ACTION_TYPE actionType);
		std::string GetEventTypeString(EventType_t eventType);
		std::string GetACSUnsoliEventString(ACSUniversalFailure_t error);
		std::string GetCSTAUnsoliEventString(CSTAUniversalFailure_t error);
		std::string GetConnectionStateString(LocalConnectionState_t state);
		std::string GetCSTAEventCauseString(CSTAEventCause_t eventCause);
		std::string GetForwardTypeString(ForwardingType_t forwardType);
		std::string GetACSERRStr(int iErrCode);
		std::string GetErrorString(std::string strErrorModule, int iErrCode);
		std::string GetAgentStateString(CSTAQueryAgentStateConfEvent_t queryAgentState);

		int GetCSTAUnsoliEventErrorCode(CSTAUniversalFailure_t error);
	};

}	// end namespace


