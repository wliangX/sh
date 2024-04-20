#pragma once
#include "Boost.h"
namespace ICC
{
	////////////////////////////////////////////////////////////////////////////
	//
	class CFSAesRequestCmd : public ISwitchNotif
	{
	public:
		CFSAesRequestCmd() {};
		CFSAesRequestCmd(int p_nCmdType)
			:m_nRequestType(p_nCmdType)
		{
			m_nParamErrCode = ERROR_CMD_SUCCESS;
		};
		virtual ~CFSAesRequestCmd() 
		{
		};

		virtual void SetCSTACallRefId(long lCSTACallRefId) {};
		virtual long GetCSTACallRefId() const { return 0; }
	public:
		virtual void SetRequestId(long lRequestId) { m_nRequestTaskId = lRequestId; };
		virtual long GetRequestId() const { return m_nRequestTaskId; };

		virtual void SetCTICallRefId(const std::string& strCTICallRefId) { m_strCTICallRefId = strCTICallRefId; };
		virtual std::string GetCTICallRefId() const { return m_strCTICallRefId; };
	public:
		void SetRequestCmdType(int nRequestType) { m_nRequestType = nRequestType; };
		int GetRequestCmdType() { return m_nRequestType; };

		void SetParamError(int p_nErrorCode) { m_nParamErrCode = p_nErrorCode; };
		int GetParamError() { return m_nParamErrCode; };

		void SetParam(const std::string& p_strName, const std::string& p_strValue) 
		{
			m_mapRequestParams[p_strName] = p_strValue;
		};
		std::string GetRequestParam(const std::string& p_strRequestParamName)
		{
			if (m_mapRequestParams.find(p_strRequestParamName) != m_mapRequestParams.end())
			{
				return m_mapRequestParams[p_strRequestParamName];
			}
			return "";
		}

		int GetRequestParamCount()
		{
			return m_mapRequestParams.size();
		}
		void GetRequestParam(std::map<std::string, std::string>& l_mapRequestParams)
		{
			l_mapRequestParams = m_mapRequestParams;
		}
	protected:
		int m_nParamErrCode;
		long m_nRequestTaskId;
		int m_nRequestType;
		std::map<std::string, std::string>m_mapRequestParams; //请求参数

		std::string m_strCTICallRefId;
	};
	typedef boost::shared_ptr<CFSAesRequestCmd> IFSAesCmdRequestPtr;

	//同步请求结果
	class CHCPSyncResultMsg : public ISwitchNotif
	{
	public:
		CHCPSyncResultMsg() {};
		virtual ~CHCPSyncResultMsg() {};

		virtual void SetCSTACallRefId(long lCSTACallRefId) {};
		virtual long GetCSTACallRefId() const { return 0; }

		virtual void SetRequestId(long lRequestId) { m_nRequestTaskId = lRequestId; };
		virtual long GetRequestId() const { return m_nRequestTaskId; };

		virtual void SetCTICallRefId(const std::string& strCTICallRefId) { m_strCTICallRefId = strCTICallRefId; };
		virtual std::string GetCTICallRefId() const { return m_strCTICallRefId; };

		void SetResponse(const std::string& p_strResponse) { m_strResponse = p_strResponse; };
		std::string GetResponse() const { return m_strResponse; };

		void SetRequestCmdType(int nRequestType) { m_nRequestType = nRequestType; };
		int GetRequestCmdType() { return m_nRequestType; };
	protected:
		long m_nRequestTaskId;
		int m_nRequestType;

		std::string m_strCTICallRefId;

		std::string m_strResponse;
	};
	
	//异步请求结果
	class CHCPAsyncResultMsg : public ISwitchNotif
	{
	public:
		CHCPAsyncResultMsg() {};
		CHCPAsyncResultMsg(IFSAesCmdRequestPtr p_pRequest)
			:m_strCTICallRefId(""),
			m_strErrorMsg("")
		{
			p_pRequest->GetRequestParam(m_mapRequestParams);
			m_nRequestTaskId = p_pRequest->GetRequestId();
			m_nRequestType = p_pRequest->GetRequestCmdType();
			m_strCTICallRefId = p_pRequest->GetCTICallRefId();
		};

		virtual ~CHCPAsyncResultMsg() {};

		virtual void SetCSTACallRefId(long lCSTACallRefId) {};
		virtual long GetCSTACallRefId() const { return 0; }
	public:
		virtual void SetRequestId(long lRequestId) { m_nRequestTaskId = lRequestId; };
		virtual long GetRequestId() const { return m_nRequestTaskId; };

		virtual void SetCTICallRefId(const std::string& strCTICallRefId) { m_strCTICallRefId = strCTICallRefId; };
		virtual std::string GetCTICallRefId() const { return m_strCTICallRefId; };

		void SetRequestCmdType(int nRequestType) { m_nRequestType = nRequestType; };
		int GetRequestCmdType() { return m_nRequestType; };
	public:
		virtual void SetResult(bool bResult) { m_bResult = bResult; }
		virtual bool GetResult() const { return m_bResult; }
		virtual void SetResultParams(std::map<std::string, std::string>& l_mapParams) { m_mapResultParams = l_mapParams; };

		virtual void SetErrorCode(int nErrorCode) { m_nErrorCode = nErrorCode; }
		virtual int GetErrorCode() { return m_nErrorCode; }

		virtual void SetErrorMsg(const std::string& strErrorMsg) { m_strErrorMsg = strErrorMsg; }
		virtual std::string GetErrorMsg() { return m_strErrorMsg; }

		std::string GetRequestParam(const std::string& p_strRequestParamName)
		{
			if (m_mapRequestParams.find(p_strRequestParamName) != m_mapRequestParams.end())
			{
				return m_mapRequestParams[p_strRequestParamName];
			}
			return "";
		}

		std::string GetResultParam(const std::string& p_strParamName)
		{
			if (m_mapResultParams.find(p_strParamName) != m_mapResultParams.end())
			{
				return m_mapResultParams[p_strParamName];
			}
			return "";
		}

	private:
		long m_nRequestTaskId;
		int m_nRequestType;
		std::string m_strCTICallRefId;
		std::map<std::string, std::string>m_mapRequestParams; //请求参数

		std::map<std::string, std::string>m_mapResultParams;  //结果参数
		bool m_bResult;
		int m_nErrorCode;
		std::string m_strErrorMsg;

	};

	typedef boost::shared_ptr<CHCPSyncResultMsg> IHcpSyncResultMsgPtr;

	//HCP事件通知
	class CHCPNotifyEevent : public ISwitchNotif
	{
	public:
		CHCPNotifyEevent() {};
		CHCPNotifyEevent(const std::string &p_strEventName, const std::string& p_strEventData)
			:m_strEventName(p_strEventName),
			m_strEventData(p_strEventData)
		{

		};

		virtual void SetRequestId(long lRequestId) {};
		virtual long GetRequestId() const { return 0; };

		virtual void SetCSTACallRefId(long lCSTACallRefId) {};
		virtual long GetCSTACallRefId() const { return 0; };

		virtual void SetCTICallRefId(const std::string& strCTICallRefId) {};
		virtual std::string GetCTICallRefId() const { return ""; };

		std::string GetEventName() {return m_strEventName;};
		std::string GetEventData() { return m_strEventData; };
	private:
		std::string m_strEventName;
		std::string m_strEventData;
	};

	typedef boost::shared_ptr<CHCPSyncResultMsg> IHcpSyncResultMsgPtr;
	typedef boost::shared_ptr<CHCPAsyncResultMsg> IHcpAsyncResultMsgPtr;

	typedef boost::shared_ptr<CHCPNotifyEevent> ICHCPNotifyEeventPtr;

}