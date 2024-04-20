#pragma once

namespace ICC
{
	namespace Ftp
	{
		struct FileNode
		{
			std::string fileName;
			std::streampos filePos = 0;
		};
		struct FileInfo
		{
			FileInfo():bIsUpload(true), strFileName(""), nTryCount(3){}
			FileInfo(bool pIsUp, const std::string& pstrFileName):bIsUpload(pIsUp), strFileName(pstrFileName)
			{}
			bool bIsUpload;
			std::string strFileName;
			int nTryCount; //重试次数
		};
		typedef boost::shared_ptr<FileInfo> FileInfoPtr;


		class CCommonEvent
		{
		public:
			void reset()
			{
				boost::mutex::scoped_lock lk(m_lkRun);
				m_condition.notify_all();
			}
			void set()
			{
				boost::mutex::scoped_lock lk(m_lkRun);
				m_condition.notify_one();
			}
			//等待毫秒
			bool wait(unsigned long ulTimeExpire = ~0)
			{
				boost::mutex::scoped_lock lk(m_lkRun);
				return m_condition.timed_wait(m_lkRun, boost::posix_time::milliseconds(ulTimeExpire));
			}
		private:
			boost::mutex m_lkRun;
			boost::condition m_condition;
		};


		//连不上，或者获取文件失败会重连，重连上再重试，达到重试次数3次后，就从队列删除
		class CFtpClientImpl :
			public IFtpClient
		{
		public:
			CFtpClientImpl(IResourceManagerPtr pResourceManager);
			~CFtpClientImpl();

			//connect
			bool Connect(const std::string& host, const std::string& port, const std::string& user, const std::string& passwd);
			//upload file to ftp server
			void UpLoadFile(const std::string& strfilename, bool bPrio= false);
			
			//download file from ftp server
			void DownLoadFile(const std::string& strfilename, bool bPrio = false);

			//upload directory to ftp server
			void UpLoadDirectory(const std::string& strdirectname);
			//download directory from ftp server
			void DownLoadDirectory(const std::string& strdirectname);
			//设置下载临时目录
			void SetDownLoadTmpDir(const std::string& p_strDirName);

			//删除远端文件,加入删除队列，等待空闲时删除
			int DeleteRemoteFile(const std::string& strFileName);

			//设置完成调用函数
			void SetNotifyCompleteFunc(IFtpClient::NotifyFileCompletedFunc p_pFunc);

		private:
			void Start();

			//连接回调函数
			void HandleConnect(const boost::system::error_code& error);

			//读取服务端发出的命令和命令代码
			void ReadComplete(const boost::system::error_code& error, size_t bytes_transferred);

			//解析服务端的命令和命令代码
			void PaserResponse(size_t resp_code, std::string resp_arg);

			//重新连接
			void ReconnectCmdSock();

			//发送命令到服务端,异步处理结果, 此函数不可直接被public函数调度用
			size_t SendFtpMessage(std::string strMsg);

			//发送命令到服务器，需要立即得到结果, 此函数不可直接被public函数调度用
			size_t SendFtpMessage(const std::string& strFileName, std::string& strResponse);

			//解析主机IP和ID端口
			void ParserPasv(std::string strPasv);
			//检查DataSocket并关闭
			void CloseDataSocket();
			//检查并关闭命令套接字
			void CloseCmdSocket();
			std::pair<std::string, std::string> ParseHostPort(std::string str);

			//发送文件到服务端
			bool TransData(const std::string& strFilename);

			//接受从服务端下载的数据
			bool ReceiveData(const std::string& strFilename);

			//保存相关日志或错误到Log文件中
			void SaveInfo(std::string strInfo);

			//获取目录以及目录下的文件列表
			static const std::list<FileNode>& ScanFiles(const std::string&, std::list<FileNode>&,
				std::list<std::pair<std::string, std::list<FileNode>>>&);

			//创建多级目录
			void CreateMutiDirectory(boost::filesystem::path path);

			//对服务回复的命令处理函数
			//登录成功的处理函数
			void  LoginSuccess(const size_t &resp_code, const std::string& resp_arg);

            //切换工作目录
			void ChangeToDir(const std::string& strDir);

			bool FileListEmpty();
			void PushUpLoadFile(const std::string& filename, bool bPrio);
			void PushDownLoadFile(const std::string& filename, bool bPrio);

			//获取两个队列的数据
			FileInfoPtr GetFirstFile();
			void PopCurFile();

			//根据重试次数，重新放入文件列表, 返回 true:未达到重试上线，放入成功， false：达到重试次数，放入失败
			bool RePushCurFile();
			size_t FileListSize();

			//成功创建目录处理函数
			void CreatedDir(const size_t &resp_code, const std::string& resp_arg);

			//改变服务工作目录的处理函数
			void ChangedDir(const size_t &resp_code, const std::string& resp_arg);

			//服务处于数据传输状态
			void ReadyForData(const size_t &resp_code, const std::string& resp_arg);

			void RemoveFrontFromFileList(int nRet);

			//文件传输完毕
			void TranComplete(const size_t &resp_code, const std::string& resp_arg);

			//服务进入被动传输模式
			void EnterPassive(const size_t &resp_code, const std::string& resp_arg);

			//连接超时
			void TimeOut(const size_t &resp_code, const std::string& resp_arg);

			//获取文件大小
			size_t GetRemoteFileSize(const std::string& strFileName);
			//改变字符模式
			int ChangeCharMode(const std::string& strMode);

			int DoDeleteFtpFile();

			//服务器处理文件或目录目录失败
			void FileNotFound(const size_t &resp_code, const std::string& resp_arg);

			virtual IResourceManagerPtr GetResourceManager()
			{
				return m_pResourceManager;
			}

		private:
			IResourceManagerPtr m_pResourceManager;
			HelpTool::IHelpToolPtr  m_HelpTool;

			CCommonEvent        m_evtCondition;//判断连接是否成功的条件变量
			boost::thread       m_thread;   //与服务进行命令连接的线程句柄
			
			Lock::ILockPtr      m_lock;     //递归锁

			Xml::IXmlPtr m_Xml;

			boost::asio::streambuf			m_receive_msg;
			boost::asio::io_service			m_io_service;
			boost::asio::io_service::work	m_work;
			boost::mutex					m_iomutex;  //连接锁
			boost::asio::ip::tcp::socket	m_cmd_socket; //命令连接

			//数据连接相关定义
			boost::asio::ip::tcp::socket	 m_data_socket;//数据连接
			boost::shared_ptr<boost::thread> m_thrd_data_socket_monitor; //数据连接监控进程,因为数据连接是读数据是阻塞式的，当开始读数据时启动该线程进行监控
			boost::mutex					 m_mtDownLoadCnt;
			unsigned int                     m_downloadcnt;//数据块下载计数
			boost::mutex	                 m_mtDownloadError;
			bool                             m_bIsDownloadError;//下载是否出错
			bool GetDownLoadError();
			void SetDownLoadError(bool bDownLoadError);
			void ResetDownloadCnt();
			void IncreamDownloadCnt();
			unsigned int GetDownloadCnt();
			//数据连接监控线程的启停
			void MonitorDataSocket();
			void StartMonitorThread();
			void StopMonitorThread();

		private:
			std::string m_user;
			std::string m_passwd;
			std::string m_host;
			std::string m_port;
			std::string m_error_msg;
			std::string m_currentdir;//当前工作目录

			bool        m_IsCwd;
			bool		m_is_connected;
			bool		m_is_logined;
			bool        m_bloginTodir;//如果重连,目录上传，切换目录的一个标志


			boost::mutex	       m_filelstmutex;  //文件列表的锁
			std::list<FileInfoPtr> m_filelist;//上传与下载文件列表
			FileInfoPtr m_curFile;
			std::list<std::pair<std::string, std::list<FileNode>>>m_dirclist;//目录与文件的关系列表
			std::list<std::pair<std::string, std::list<FileNode>>>m_updirclist;//上传的目录，文件列表
			std::list<std::pair<std::string, std::list<FileNode>>>m_dirclistCache;//目录与文件的关系列表缓存
			std::list<std::pair<std::string, std::list<FileNode>>>m_Httplist;//断点续传文件列表

			std::list<std::string> m_loaddirlist;//下载目录列表

			int PushDeleteFile(const std::string& strFileName)
			{
				boost::mutex::scoped_lock lk(m_lkdelfile);
				m_delfilelist.push_back(strFileName);
				return 0;
			}
			std::string PopDeleteFile()
			{
				boost::mutex::scoped_lock lk(m_lkdelfile);
				std::string strDelFileName = m_delfilelist.front();
				m_delfilelist.pop_front();
				return strDelFileName;
			}
			size_t DeleteFileListSize()
			{
				boost::mutex::scoped_lock lk(m_lkdelfile);
				return m_delfilelist.size();
			}
			boost::mutex	    m_lkdelfile;  //文件列表的锁
			std::list<std::string> m_delfilelist;

			Log::ILogPtr m_pLog;

			IFtpClient::NotifyFileCompletedFunc m_pFtpFileCompletedFun;

		};
	}
}