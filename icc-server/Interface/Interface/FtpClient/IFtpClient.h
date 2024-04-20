#pragma once

#include <IObject.h>
#include <boost/function.hpp>
//Connect:"127.0.0.1", "21","test", "123456"
//p_strName:ftphome/ftptest/filename
//p_strDir:ftphome/dirname

namespace ICC
{
	namespace Ftp
	{
		class IFtpClient :
			public IObject
		{
		public:
			typedef boost::function<void(bool, const std::string&, const std::string&, int)> NotifyFileCompletedFunc;
			//************************************
			// Method:    Connect
			// FullName:  ACS::IFtpClient::Connect
			// Access:    virtual public 
			// Returns:   void
			// Qualifier:
			// Parameter: std::string p_strServerIP
			// Parameter: std::string p_strPort
			// Parameter: std::string p_strUserName
			// Parameter: std::string p_strUserPasswd
			//************************************
			virtual bool Connect(const std::string& p_strServerIP, const std::string& p_strPort, const std::string& p_strUserName, const std::string& p_strUserPasswd) = 0;
			//************************************
			// Method:    DownLoadFile
			// FullName:  ACS::IFtpClient::DownLoadFile
			// Access:    virtual public 
			// Returns:   void
			// Qualifier:
			// Parameter: std::string p_strFileName
			// Parameter: bool bPrio = false
			//************************************
			virtual void DownLoadFile(const std::string& p_strFileName, bool bPrio = false) = 0;
			//************************************
			// Method:    DownLoadDirectory
			// FullName:  ACS::IFtpClient::DownLoadDirectory
			// Access:    virtual public 
			// Returns:   void
			// Qualifier:
			// Parameter: std::string p_strDirName
			//************************************
			virtual void DownLoadDirectory(const std::string& p_strDirName) = 0;
			//************************************
			// Method:    UpLoadFile
			// FullName:  ACS::IFtpClient::UpLoadFile
			// Access:    virtual public 
			// Returns:   void
			// Qualifier:
			// Parameter: std::string p_strFileName
			// Parameter: bool bPrio = false
			//************************************
			virtual void UpLoadFile(const std::string& p_strFileName, bool bPrio = false) = 0;
			//************************************
			// Method:    UpLoadDirectory
			// FullName:  ACS::IFtpClient::UpLoadDirectory
			// Access:    virtual public 
			// Returns:   void
			// Qualifier:
			// Parameter: std::string p_strDirName
			//************************************
			virtual void UpLoadDirectory(const std::string& p_strDirName) = 0;

			//************************************
			// Method:    SetDownLoadTmpDir
			// FullName:  IFtpClient::SetDownLoadTmpDir
			// Access:    virtual public 
			// Returns:   void
			// Qualifier:
			// Parameter: std::string p_strDirName
			//************************************
			virtual void SetDownLoadTmpDir(const std::string& p_strDirName) = 0;

			//************************************
			// Method:    删除远端文件
			// FullName:  IFtpClient::DeleteRemoteFile
			// Access:    virtual public 
			// Returns:   void
			// Qualifier:
			// Parameter: std::string p_strFileName
			//************************************
			virtual int DeleteRemoteFile(const std::string& strFileName) = 0;

			//************************************
			// Method:    设置上层调用函数
			// FullName:  IFtpClient::SetNotifyCompleteFunc
			// Access:    virtual public 
			// Returns:   void
			// Qualifier:
			// Parameter: NotifyFileCompletedFunc p_pFunc
			//************************************
			virtual void SetNotifyCompleteFunc(NotifyFileCompletedFunc p_pFunc) = 0;

		};

		typedef boost::shared_ptr<IFtpClient> IFtpClientPtr;
	}
}