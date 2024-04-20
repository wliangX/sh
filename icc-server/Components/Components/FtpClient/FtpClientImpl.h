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
			int nTryCount; //���Դ���
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
			//�ȴ�����
			bool wait(unsigned long ulTimeExpire = ~0)
			{
				boost::mutex::scoped_lock lk(m_lkRun);
				return m_condition.timed_wait(m_lkRun, boost::posix_time::milliseconds(ulTimeExpire));
			}
		private:
			boost::mutex m_lkRun;
			boost::condition m_condition;
		};


		//�����ϣ����߻�ȡ�ļ�ʧ�ܻ������������������ԣ��ﵽ���Դ���3�κ󣬾ʹӶ���ɾ��
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
			//����������ʱĿ¼
			void SetDownLoadTmpDir(const std::string& p_strDirName);

			//ɾ��Զ���ļ�,����ɾ�����У��ȴ�����ʱɾ��
			int DeleteRemoteFile(const std::string& strFileName);

			//������ɵ��ú���
			void SetNotifyCompleteFunc(IFtpClient::NotifyFileCompletedFunc p_pFunc);

		private:
			void Start();

			//���ӻص�����
			void HandleConnect(const boost::system::error_code& error);

			//��ȡ����˷�����������������
			void ReadComplete(const boost::system::error_code& error, size_t bytes_transferred);

			//��������˵�������������
			void PaserResponse(size_t resp_code, std::string resp_arg);

			//��������
			void ReconnectCmdSock();

			//������������,�첽������, �˺�������ֱ�ӱ�public����������
			size_t SendFtpMessage(std::string strMsg);

			//�����������������Ҫ�����õ����, �˺�������ֱ�ӱ�public����������
			size_t SendFtpMessage(const std::string& strFileName, std::string& strResponse);

			//��������IP��ID�˿�
			void ParserPasv(std::string strPasv);
			//���DataSocket���ر�
			void CloseDataSocket();
			//��鲢�ر������׽���
			void CloseCmdSocket();
			std::pair<std::string, std::string> ParseHostPort(std::string str);

			//�����ļ��������
			bool TransData(const std::string& strFilename);

			//���ܴӷ�������ص�����
			bool ReceiveData(const std::string& strFilename);

			//���������־�����Log�ļ���
			void SaveInfo(std::string strInfo);

			//��ȡĿ¼�Լ�Ŀ¼�µ��ļ��б�
			static const std::list<FileNode>& ScanFiles(const std::string&, std::list<FileNode>&,
				std::list<std::pair<std::string, std::list<FileNode>>>&);

			//�����༶Ŀ¼
			void CreateMutiDirectory(boost::filesystem::path path);

			//�Է���ظ����������
			//��¼�ɹ��Ĵ�����
			void  LoginSuccess(const size_t &resp_code, const std::string& resp_arg);

            //�л�����Ŀ¼
			void ChangeToDir(const std::string& strDir);

			bool FileListEmpty();
			void PushUpLoadFile(const std::string& filename, bool bPrio);
			void PushDownLoadFile(const std::string& filename, bool bPrio);

			//��ȡ�������е�����
			FileInfoPtr GetFirstFile();
			void PopCurFile();

			//�������Դ��������·����ļ��б�, ���� true:δ�ﵽ�������ߣ�����ɹ��� false���ﵽ���Դ���������ʧ��
			bool RePushCurFile();
			size_t FileListSize();

			//�ɹ�����Ŀ¼������
			void CreatedDir(const size_t &resp_code, const std::string& resp_arg);

			//�ı������Ŀ¼�Ĵ�����
			void ChangedDir(const size_t &resp_code, const std::string& resp_arg);

			//���������ݴ���״̬
			void ReadyForData(const size_t &resp_code, const std::string& resp_arg);

			void RemoveFrontFromFileList(int nRet);

			//�ļ��������
			void TranComplete(const size_t &resp_code, const std::string& resp_arg);

			//������뱻������ģʽ
			void EnterPassive(const size_t &resp_code, const std::string& resp_arg);

			//���ӳ�ʱ
			void TimeOut(const size_t &resp_code, const std::string& resp_arg);

			//��ȡ�ļ���С
			size_t GetRemoteFileSize(const std::string& strFileName);
			//�ı��ַ�ģʽ
			int ChangeCharMode(const std::string& strMode);

			int DoDeleteFtpFile();

			//�����������ļ���Ŀ¼Ŀ¼ʧ��
			void FileNotFound(const size_t &resp_code, const std::string& resp_arg);

			virtual IResourceManagerPtr GetResourceManager()
			{
				return m_pResourceManager;
			}

		private:
			IResourceManagerPtr m_pResourceManager;
			HelpTool::IHelpToolPtr  m_HelpTool;

			CCommonEvent        m_evtCondition;//�ж������Ƿ�ɹ�����������
			boost::thread       m_thread;   //���������������ӵ��߳̾��
			
			Lock::ILockPtr      m_lock;     //�ݹ���

			Xml::IXmlPtr m_Xml;

			boost::asio::streambuf			m_receive_msg;
			boost::asio::io_service			m_io_service;
			boost::asio::io_service::work	m_work;
			boost::mutex					m_iomutex;  //������
			boost::asio::ip::tcp::socket	m_cmd_socket; //��������

			//����������ض���
			boost::asio::ip::tcp::socket	 m_data_socket;//��������
			boost::shared_ptr<boost::thread> m_thrd_data_socket_monitor; //�������Ӽ�ؽ���,��Ϊ���������Ƕ�����������ʽ�ģ�����ʼ������ʱ�������߳̽��м��
			boost::mutex					 m_mtDownLoadCnt;
			unsigned int                     m_downloadcnt;//���ݿ����ؼ���
			boost::mutex	                 m_mtDownloadError;
			bool                             m_bIsDownloadError;//�����Ƿ����
			bool GetDownLoadError();
			void SetDownLoadError(bool bDownLoadError);
			void ResetDownloadCnt();
			void IncreamDownloadCnt();
			unsigned int GetDownloadCnt();
			//�������Ӽ���̵߳���ͣ
			void MonitorDataSocket();
			void StartMonitorThread();
			void StopMonitorThread();

		private:
			std::string m_user;
			std::string m_passwd;
			std::string m_host;
			std::string m_port;
			std::string m_error_msg;
			std::string m_currentdir;//��ǰ����Ŀ¼

			bool        m_IsCwd;
			bool		m_is_connected;
			bool		m_is_logined;
			bool        m_bloginTodir;//�������,Ŀ¼�ϴ����л�Ŀ¼��һ����־


			boost::mutex	       m_filelstmutex;  //�ļ��б����
			std::list<FileInfoPtr> m_filelist;//�ϴ��������ļ��б�
			FileInfoPtr m_curFile;
			std::list<std::pair<std::string, std::list<FileNode>>>m_dirclist;//Ŀ¼���ļ��Ĺ�ϵ�б�
			std::list<std::pair<std::string, std::list<FileNode>>>m_updirclist;//�ϴ���Ŀ¼���ļ��б�
			std::list<std::pair<std::string, std::list<FileNode>>>m_dirclistCache;//Ŀ¼���ļ��Ĺ�ϵ�б���
			std::list<std::pair<std::string, std::list<FileNode>>>m_Httplist;//�ϵ������ļ��б�

			std::list<std::string> m_loaddirlist;//����Ŀ¼�б�

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
			boost::mutex	    m_lkdelfile;  //�ļ��б����
			std::list<std::string> m_delfilelist;

			Log::ILogPtr m_pLog;

			IFtpClient::NotifyFileCompletedFunc m_pFtpFileCompletedFun;

		};
	}
}