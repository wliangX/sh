#pragma once

/**
@Copyright Copyright (C), 2018
@file		BlackListManager.h
@created	2018/4/2
@brief		黑名单管理类
1, 接收应用层发送的添加号码
2, 接收应用层发送的删除号码
3, 提供黑名单的查询

@author psy
*/

namespace ICC
{
	class CBlackListManager
	{
	public:
		CBlackListManager(void);
		virtual ~CBlackListManager(void);

		static boost::shared_ptr<CBlackListManager> Instance();
		void ExitInstance();

		void OnInit(IResourceManagerPtr p_pResourceManager);
		void OnStart();
		void OnStop();

	public:
		bool FindBlackNumber(const std::string& p_strLimitNum);

		void AddBlackList(const std::string& p_strLimitNum);
		void DeleteBlackList(const std::string& p_strLimitNum);
		void ClearBlackList();

	private:
		static boost::shared_ptr<CBlackListManager> m_pInstance;

		Log::ILogPtr			m_pLog;
		
		std::mutex				m_blackListMutex;
		std::list<std::string>	m_blackList;
	};

}

