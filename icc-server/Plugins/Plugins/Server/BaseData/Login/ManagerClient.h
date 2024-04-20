#pragma once

namespace ICC
{
	typedef struct tClientInfo
	{
		std::string strClientType;
		std::string strClientID;
		std::string strClientName;

		tClientInfo()
		{
			strClientID = "";
			strClientName = "";
		}
	}ClientInfo, *LPClientInfo;
	typedef  std::map<std::string, ClientInfo> ClientInfoMAP;
	typedef  std::map<std::string, ClientInfo>::iterator ClientMapIter;

	class CManagerClient
	{
	public:
		CManagerClient(IResourceManagerPtr p_pResourceManager);

	public:
		virtual void AddClient(std::string p_strClientID, ClientInfo &p_ClientInfo);
		virtual bool DeleteClient(std::string p_strClientID);
		virtual ClientInfoMAP GetAllClientInfo();
		virtual bool IsConnected(std::string p_strClientID);
		virtual bool GetClientInfoByClientID(std::string p_strClientID, ClientInfo &p_ClientInfo);
		virtual bool GetClientInfoByUserName(std::string p_strUserName, ClientInfo &p_ClientInfo);

		virtual std::string GetUserNameByClientID(std::string p_strClientID);

	public:
		virtual IResourceManagerPtr GetResourceManager();

	private:
		IResourceManagerPtr m_pResourceManager;

	private:
		Lock::ILockFactoryPtr		m_pLockFac;
		Lock::ILockPtr			m_pLock;
		ClientInfoMAP		m_mapClientInfo;		
	};

	typedef boost::shared_ptr<CManagerClient> ManagerClientPtr;
	
}