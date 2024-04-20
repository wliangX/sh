#pragma once

namespace ICC
{
	enum eClientStatus
	{
		ICC_CLIENT_LOGOUT = 0,
		ICC_CLIENT_LOGIN 
	};
	typedef struct tMQClientInfo
	{
		std::string strConnectionID;
		std::string strClientID;
		std::string strClientIP;

		tMQClientInfo()
		{
			strConnectionID = "";
			strClientID = "";
			strClientIP = "";
		}
	}MQClientInfo, *PMQClientInfo;

	typedef  std::map<std::string, MQClientInfo> MQConnectMap;

	class CManagerMQClient
	{
	public:
		CManagerMQClient(IResourceManagerPtr p_pResourceManager);

	public:
		virtual void AddMQConnect(std::string p_strConnectionID, MQClientInfo &p_MQClientInfo);
		virtual void DeleteMQConnect(std::string p_strConnectionID);
		virtual bool GetMQConnectInfoByConnectionID(std::string p_strConnectionID, MQClientInfo &p_MQClientInfo);
		virtual bool GetMQConnectInfoByClientID(std::string p_strClientID, MQClientInfo &p_MQClientInfo);
		virtual bool GetMQConnectInfoByClientIP(std::string p_strClientIP, MQClientInfo &p_MQClientInfo);
		virtual bool IsConnect(std::string p_strClientID);
	public:
		MQConnectMap GetConnectList();

	public:
		virtual IResourceManagerPtr GetResourceManager();

	private:
		IResourceManagerPtr m_pResourceManager;

	private:
		Lock::ILockFactoryPtr		m_pLockFac;
		Lock::ILockPtr			m_pLock;
		MQConnectMap		m_mapMQList;
	};


	typedef boost::shared_ptr<CManagerMQClient> ManagerMQClientPtr;
	
}