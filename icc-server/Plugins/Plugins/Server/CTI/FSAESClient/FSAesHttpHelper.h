#pragma once
#include "Boost.h"
#include "CTIFSAesNotifaction.h"
namespace ICC
{
	//////////////////////////////////////////////////////////////////////////////
	//
	class CFSAesHttpHelper
	{
	public:
		CFSAesHttpHelper(Log::ILogPtr p_pLogPtr, 
			IResourceManagerPtr p_pResourceManager);
		virtual ~CFSAesHttpHelper();

		bool Start(const std::string& p_strHttpLocalAddress, const std::string& p_strHttpLocalPort,const std::string &p_strFSAesServerIP, const std::string& p_strFSAesServerPort);
		void Stop();
		virtual IResourceManagerPtr GetResourceManager()
		{
			return m_pResourceManager;
		}

		void SetNodeName(const std::string& p_strNodeName);
		bool GetLocalServerIp(std::string& p_strLocalServerIp);

		void OnHttpMessage(const std::string& p_strSrcIP, const std::string& p_strGuid, const std::string& p_strTarget, const std::string& p_strBody);

		int SendRequestToFSAes(const std::string& p_strMsgId,IFSAesCmdRequestPtr p_pAesCmdRequest, std::string& p_strResponse);

		void SetCTIClientID(const std::string& p_strCTIClientID) { m_strCTIClientID = p_strCTIClientID; };
		std::string GetCTIClientID() { return m_strCTIClientID; };
		void UpdateAESServerIp(const std::string &p_strAESServerIp);
	private:
		std::string _CreateFSAesHttpRequestBody(const std::string& p_strMsgId, IFSAesCmdRequestPtr p_pAesCmdRequest);
	protected:
		bool					m_bStart;
		IResourceManagerPtr		m_pResourceManager;
		DateTime::IDateTimePtr	m_pDateTimePtr;
		IHttpServerPtr			m_pHttpServer;
		IHttpClientPtr          m_pHttpRqstClient;
		Log::ILogPtr			m_pLog;

		std::string				m_strASEServerIP;
		std::string				m_strAESServerPort;

		std::string				m_strCTIClientID;
		std::string				m_strNodeName;
	};

	typedef boost::shared_ptr<CFSAesHttpHelper> CFSAesHttpHelperPtr;

}// end namespace ICC
