#include <Boost.h>
#include <RecordBusinessImpl.h>
#include <RecordFileManage.h>
#include <UploadFiles.h>
#include <boost/filesystem.hpp>

namespace ICC
{
	/************************************************************************/
	/*                                                                      */
	/************************************************************************/

	void RecordBusinessImpl::OnInit()
	{
		m_pLog = ICCGetILogFactory()->GetLogger(MODULE_NAME);
		if (!m_pLog)
		{
			return;
		}
		ICC_LOG_DEBUG(m_pLog, "%s", "RecordBusinessImpl::OnInit() begin.");
		m_pString = ICCGetIStringFactory()->CreateString();
		if (!m_pString)
		{
			return;
		}

		m_pConfig = ICCGetIConfigFactory()->CreateConfig();
		if (!m_pConfig)
		{					
			ICC_LOG_ERROR(m_pLog, "%s", "config is null!");
		}		

		m_pJsonFty = ICCGetIJsonFactory();
		if (!m_pJsonFty)
		{
			ICC_LOG_ERROR(m_pLog, "%s", "json factory is null!");
		}

		m_pInitDBConn = ICCGetIDBConnFactory()->CreateNewDBConn(DataBase::PostgreSQL);
		if (!m_pInitDBConn)
		{
			ICC_LOG_ERROR(m_pLog, "%s", "DB Connect is null!");
		}

		m_pObserverCenter = ICCGetIObserverFactory()->GetObserverCenter(RECORDSERVER_OBSERVER_NAME);
		if (!m_pObserverCenter)
		{
			ICC_LOG_ERROR(m_pLog, "%s", "ObserverCenter is null!");
		}

		m_pDateTime = ICCGetIDateTimeFactory()->CreateDateTime();
		if (!m_pDateTime)
		{
			ICC_LOG_ERROR(m_pLog, "%s", "DateTime is null!");
		}

		ICC_LOG_DEBUG(m_pLog, "%s", "RecordBusinessImpl::OnInit() end.");
	}

	void RecordBusinessImpl::OnStart()
	{		
		
		ICC_LOG_DEBUG(m_pLog, "%s", "RecordBusinessImpl::OnStart() begin.");
		//关注MQ消息
		ADDOBSERVER(m_pObserverCenter, RecordBusinessImpl, CMD_AesConnStateSync, OnCtiAesConnNotice);
		ADDOBSERVER(m_pObserverCenter, RecordBusinessImpl, CMD_AcdCallStateSync, OnCtiCallInfoNotice);
		ADDOBSERVER(m_pObserverCenter, RecordBusinessImpl, CMD_DeviceStateSync, OnCtiDeviceInfoNotice);
		ADDOBSERVER(m_pObserverCenter, RecordBusinessImpl, CMD_CallOverSync, OnCtiCallOverSyncNotice);
		ADDOBSERVER(m_pObserverCenter, RecordBusinessImpl, CMD_AlarmSync, OnCtiAlarmSyncNotice);
		ADDOBSERVER(m_pObserverCenter, RecordBusinessImpl, CMD_FileUploadSyncAck, OnCtiFileSyncMsgAck);
		ADDOBSERVER(m_pObserverCenter, RecordBusinessImpl, CMD_Sync_Server_Lock, OnSyncServerLock);

		StartConnectFSFtp();

		m_nEnableNetCap = m_pString->ToInt(m_pConfig->GetValue("ICC/Plugin/Record/CapNetDataProcEnable", "0"));
		
		//启动后就抓包，等待确定好主备后，如果是备再停止，防止在等待主备时导致网络包呼叫丢失
		if (m_nEnableNetCap !=0) 
		{
			if (!m_bIsStartAvayaCap)
			{
				if (CreateNetDataCap())
				{
					m_bIsStartAvayaCap=true;
				}
			}
		}
		unsigned long long ullFileUploadTimer = m_pString->ToInt(m_pConfig->GetValue("ICC/Plugin/Record/UploadFileTimer", "10"));
		m_ulTimerUploadFiles = CCommonTimerManager::GetInstance().AddTimer(this, ullFileUploadTimer*1000);//10秒检查上传一次文件

		ICC_LOG_DEBUG(m_pLog, "%s", "RecordBusinessImpl::OnStart() end.");	
	}

	void RecordBusinessImpl::StartConnectFSFtp()
	{
		std::string strFSFtpIP1 = m_pConfig->GetValue("ICC/Plugin/Record/FSFtpIP", "");
		std::string strFSFtpPort1 = m_pConfig->GetValue("ICC/Plugin/Record/FSFtpPort", "21");
		std::string strFSFtpUser1 = m_pConfig->GetValue("ICC/Plugin/Record/FSFtpUser", "recorder");
		std::string strFSFtpPwd1 = m_pConfig->GetValue("ICC/Plugin/Record/FSFtpPwd", "Command_123");

		std::string strFSFtpIP2 = m_pConfig->GetValue("ICC/Plugin/Record/FSFtp2IP", "");

		if (strFSFtpIP1.empty() && strFSFtpIP2.empty())
		{
			ICC_LOG_ERROR(m_pLog, "not Configured FSFtpIP and FSFtp2IP, do not connnect FreeSwitch Ftp.");
			return;
		}

		//创建FSFtp下载管理器
		m_pFtpDownLoadMgr = boost::make_shared<FtpDownLoadManage>(m_pLog);
		try
		{
			//设置是否删除远端文件
			bool bIsDeleteRemoteFile = std::stoi(m_pConfig->GetValue("ICC/Plugin/Record/FSFtpDeleteRemoteFile", "1"));
			m_pFtpDownLoadMgr->SetDeleteFileMode(bIsDeleteRemoteFile);
		}
		catch (std::exception & ex)
		{
			ICC_LOG_WARNING(m_pLog, "FtpClient SetDeleteFileMode failed %s!", ex.what());
		}

		//获取本地文件保存路径
		std::string strRecordFilePath = "";
		if (m_pConfig != nullptr)
		{
			strRecordFilePath = m_pConfig->GetValue("ICC/Plugin/Record/FileStoragePath", "");
		}
		if (strRecordFilePath[strRecordFilePath.length() - 1] != '/' && strRecordFilePath[strRecordFilePath.length() - 1] != '\\')
		{
			strRecordFilePath += "/";
		}
		m_strFSLocalRecordFileDir = strRecordFilePath;

		//创建Ftp客户端连接
		Ftp::IFtpClientPtr pFtpClient1 = ICCGetIFtpClientFactory()->CreateFtpClient();
		if (!pFtpClient1)
		{
			ICC_LOG_ERROR(m_pLog, "%s", "FtpClient is null!");
		}
		bool bFtpConnRet = m_pFtpDownLoadMgr->ConnectFtpServer(pFtpClient1, strFSFtpIP1, strFSFtpPort1, strFSFtpUser1, strFSFtpPwd1, m_strFSLocalRecordFileDir);
		ICC_LOG_DEBUG(m_pLog, "Connect FreeSwitch Ftp(%s) Ret %d", strFSFtpIP1.c_str(), bFtpConnRet);
		if (!strFSFtpIP2.empty())
		{
			std::string strFSFtpPort2 = m_pConfig->GetValue("ICC/Plugin/Record/FSFtp2Port", strFSFtpPort1);
			std::string strFSFtpUser2 = m_pConfig->GetValue("ICC/Plugin/Record/FSFtp2User", strFSFtpUser1);
			std::string strFSFtpPwd2 = m_pConfig->GetValue("ICC/Plugin/Record/FSFtp2Pwd", strFSFtpPwd1);
			Ftp::IFtpClientPtr pFtpClient2 = ICCGetIFtpClientFactory()->CreateFtpClient();
			if (!pFtpClient2)
			{
				ICC_LOG_ERROR(m_pLog, "FtpClient is null!");
			}
			bool bFtpConnRet = m_pFtpDownLoadMgr->ConnectFtpServer(pFtpClient2, strFSFtpIP2, strFSFtpPort2, strFSFtpUser2, strFSFtpPwd2, m_strFSLocalRecordFileDir);
			ICC_LOG_DEBUG(m_pLog, "Connect FreeSwitch Ftp(%s) Ret %d", strFSFtpIP2.c_str(), bFtpConnRet);
		}
	}

	int RecordBusinessImpl::CreateNetDataCap()
	{
		if (NULL == m_pConfig)
		{
			return -1;
		}
		std::string strNicName = m_pConfig->GetValue("ICC/Plugin/Record/CapNetDataPara/NicID", "eno1");
		std::string strG450IP1 = m_pConfig->GetValue("ICC/Plugin/Record/CapNetDataPara/MediaGwIp", "127.0.0.1");
		std::string strG450IP2 = m_pConfig->GetValue("ICC/Plugin/Record/CapNetDataPara/MediaGwIp2", "127.0.0.1");
		std::string strLocalFilePath = m_pConfig->GetValue("ICC/Plugin/Record/CapNetDataPara/LocalStoragePath", "./");
		std::string strAESServerIP = m_pConfig->GetValue("ICC/Plugin/Record/CapNetDataPara/AesServerIp", "127.0.0.1");
		std::string strAESServerPort = m_pConfig->GetValue("ICC/Plugin/Record/CapNetDataPara/AesServerPort", "443");
		std::string strAESServerUser = m_pConfig->GetValue("ICC/Plugin/Record/CapNetDataPara/AesServerUser", "");
		std::string strAESServerPwd = m_pConfig->GetValue("ICC/Plugin/Record/CapNetDataPara/AesServerPwd", "");
		std::string strVoiceInterval = m_pConfig->GetValue("ICC/Plugin/Record/CapNetDataPara/AudioPara/PkgInterval", "20");
		std::string strSampleRate = m_pConfig->GetValue("ICC/Plugin/Record/CapNetDataPara/AudioPara/SampleRate", "8000");
		std::string strFormat = m_pConfig->GetValue("ICC/Plugin/Record/CapNetDataPara/AudioPara/CodecFmt", "G711A");
		std::string strSampleBit = m_pConfig->GetValue("ICC/Plugin/Record/CapNetDataPara/AudioPara/SampleBits", "16");

		CRecordFileManage::GetInstance().SetLocalFilePath(strLocalFilePath);

		//开启网卡抓包
		m_pAvayaCap = std::make_shared<CCapDataProc>(m_pLog, strNicName, "udp", OPENMODE_LIVE);
		m_pAvayaCap->SetPaiDuiJiIp(strG450IP1);
		m_pAvayaCap->SetPaiDuiJiIp(strG450IP2);
		try
		{
			uint16_t usCodec = FORMAT_G711A;
			uint16_t usSampleBit = bitsPerSampleG711A;
			if (strFormat == "PCM")
			{
				usCodec = FORMAT_PCM;
				usSampleBit = std::stoi(strSampleBit);
			}
			m_pAvayaCap->SetAudioPara(std::stoi(strVoiceInterval), std::stoi(strSampleRate), usCodec, usSampleBit);
		}
		catch (std::exception & ex)
		{
			ICC_LOG_ERROR(m_pLog, "SetCaptureData AudioPara Error %s", ex.what());
		}

		m_pAvayaCap->StartCap();


		//读取XML配置的IP和AgentID的对应关系
		Config::IConfigPtr pConfigConfig = ICCGetIConfigFactory()->CreateConfigConfig();
		if (nullptr != pConfigConfig)
		{
			string strConfigFileDir = pConfigConfig->GetValue("ICC.ConfigFileDir", "./Config/");
			string strFilePath = strConfigFileDir + "agent-ip-config.xml";
			Xml::IXmlPtr pXmlIPAgentPtr = ICCGetIXmlFactory()->CreateXml();
			if (!pXmlIPAgentPtr->LoadFile(strFilePath))
			{
				ICC_LOG_ERROR(m_pLog, "load agent ip config(%s) file failed.", strFilePath.c_str());
			}
			else
			{
				unsigned int uiAgentCount = pXmlIPAgentPtr->GetChildCount("Record/Agents", "Agent");
				if (uiAgentCount > 0)
				{
					for (int i = 0; i < uiAgentCount; i++)
					{
						std::string strPath = m_pString->Format("Record/Agents/Agent[%d]", i);
						std::string strAgentID = pXmlIPAgentPtr->GetText(strPath + "/Ext", "");
						std::string strPhoneIP = pXmlIPAgentPtr->GetText(strPath + "/IP", "");
						CAgentIPTable::GetInstance().AddAgentPhoneIP(strAgentID, strPhoneIP);
						ICC_LOG_DEBUG(m_pLog, "load config AddAgentPhoneIP %s -- %s", strAgentID.c_str(), strPhoneIP.c_str());
					}
				}
			}
		}

		//创建连接AES客户端后先取一次
		m_pAvayaAesClt = std::make_shared<CAavayaAESClient>(m_pLog, strAESServerIP, strAESServerPort, 1, strAESServerUser, strAESServerPwd);
		DoGetAESPhoneIP();
		unsigned long long ullSyncAvayaConfigTimer = m_pString->ToInt(m_pConfig->GetValue("ICC/Plugin/Record/CapNetDataPara/SyncAvayaConfigTimer", "60"));
		m_ulTimerSyncAvayaConfig = CCommonTimerManager::GetInstance().AddTimer(this, ullSyncAvayaConfigTimer * 1000);//1分钟同步一次数据
		return 0;
	}

	int RecordBusinessImpl::StopNetDataCap()
	{
		CCommonTimerManager::GetInstance().RemoveTimer(m_ulTimerSyncAvayaConfig);
		if (m_pAvayaCap != nullptr)
		{
			m_pAvayaCap->StopCap();
			m_pAvayaCap->StopAllProc();
			m_pAvayaCap = nullptr;
		}
		if (m_pAvayaAesClt != nullptr)
		{
			m_pAvayaAesClt = nullptr;
		}
		return 0;
	}

	//循环上传未上传文件
	void RecordBusinessImpl::DoFileUpload()
	{
		ICC_LOG_DEBUG(m_pLog, "DoFileUpload NotUploadFileListSize(%u).", CRecordFileManage::GetInstance().NotUploadFileListSize());
		CRecordFilePtr pRecordFile = CRecordFileManage::GetInstance().GetANotUploadFile();
		while(nullptr != pRecordFile)
		{
			//1.上传文件
			int nRetUpload = UploadFileToServer(pRecordFile);

			//对上传失败的进行处理
			if (0 != nRetUpload)
			{
				if (7010 == nRetUpload)//文件不存在，则上传下一条记录,此时不再尝试上传，必须由存入方保证文件存在
				{
					ICC_LOG_DEBUG(m_pLog, "NotUploadFileListSize(%u).", CRecordFileManage::GetInstance().NotUploadFileListSize());
					pRecordFile = CRecordFileManage::GetInstance().GetANotUploadFile();
					if (nullptr != pRecordFile)
					{
						//有记录则等待20ms再上传一次，加快上传速度这样
						std::this_thread::sleep_for(std::chrono::milliseconds(20));
						continue;
					}
				}
				else//文件存在，上传出错，放入队列，后面继续上传
				{
					CRecordFileManage::GetInstance().AddARecordFile(pRecordFile);
				}
				return;//出错后返回
			}
			
			//2.发送消息
			if (!pRecordFile->m_setCallID.empty())
			{
				SendFileSyncMsg(pRecordFile, nRetUpload);
			}
			
			//3.更新数据库
			int nRetWriteDB = UpdateDBRecord(pRecordFile);
			if (0 != nRetWriteDB)
			{
				CRecordFileManage::GetInstance().AddARecordFile(pRecordFile);
				return;
			}
			
			//4.删除本地文件
			if (0 == nRetUpload && 0 == nRetWriteDB)
			{
				int bDeleleLocalFiles = true;
				if (m_pConfig != nullptr)
				{
					bDeleleLocalFiles = String2Int(m_pConfig->GetValue("ICC/Plugin/Record/DeleteLocalFiles", "1"));
				}
				ICC_LOG_INFO(m_pLog, "Delete File %s bDeleteLocalFiles(%d).", pRecordFile->m_strLocalFileName.c_str(), bDeleleLocalFiles);
				
				if (bDeleleLocalFiles)
				{
					bool bRetDelete = boost::filesystem::remove(pRecordFile->m_strLocalFileName);
					if (!bRetDelete)
					{
						ICC_LOG_INFO(m_pLog, "Delete File %s failed.", pRecordFile->m_strLocalFileName.c_str());
						return;
					}
				}

				//4.1删除Ftp远端文件，远端文件要等上传了minio文件服务后再删除，防止下载完删除，record还没上传minio就先退出了，再获取历史话单时取不到Ftp上的文件
				if (nullptr != m_pFtpDownLoadMgr)
				{
					m_pFtpDownLoadMgr->DeleteFtpFile(pRecordFile->m_strFSFtpIp, pRecordFile->m_strFSFtpFileName);
				}
			}
			
			//5.检查下一条记录
			ICC_LOG_DEBUG(m_pLog, "NotUploadFileListSize(%u).", CRecordFileManage::GetInstance().NotUploadFileListSize());
			pRecordFile = CRecordFileManage::GetInstance().GetANotUploadFile();
			if (nullptr != pRecordFile)
			{
				//有记录则等待20ms再上传一次，加快上传速度
				std::this_thread::sleep_for(std::chrono::milliseconds(20));
				continue;
			}
		};
	}

	void RecordBusinessImpl::OnStop()
	{
		ICC_LOG_DEBUG(m_pLog, "%s", "RecordBusinessImpl::OnStop() begin.");	
		if (m_bIsStartAvayaCap)
		{
			StopNetDataCap();
		}
		CCommonTimerManager::GetInstance().RemoveTimer(m_ulTimerUploadFiles);
		ICC_LOG_DEBUG(m_pLog, "%s", "RecordBusinessImpl::OnStop() end.");		
	}

	void RecordBusinessImpl::OnDestroy()
	{
		ICC_LOG_DEBUG(m_pLog, "%s", "RecordBusinessImpl::OnDestroy() begin.");		

		

		ICC_LOG_DEBUG(m_pLog, "%s", "RecordBusinessImpl::OnDestroy() end.");		
	}

	void RecordBusinessImpl::OnCtiAesConnNotice(ObserverPattern::INotificationPtr p_pNotifiRequest)
	{
		JsonParser::IJsonPtr l_pJson = ICCGetIJsonFactory()->CreateJson();
		PROTOCOL::CCTIConnStateEvent cticonnstate;
		std::string l_strRecvMsg(p_pNotifiRequest->GetMessages());
		if (!cticonnstate.ParseString(l_strRecvMsg, l_pJson))
		{
			ICC_LOG_ERROR(m_pLog, "[OnCtiAesConnNotice]Parse request error.[%s]", l_strRecvMsg.c_str());
			return;
		}
		ICC_LOG_DEBUG(m_pLog, "receive message[%s].", l_strRecvMsg.c_str());

		m_thrdGetHistoryCallList.reset(new boost::thread(boost::bind(&RecordBusinessImpl::ThreadGetHistoryCallList, this)));
	}

	void RecordBusinessImpl::ThreadGetHistoryCallList()
	{
		if (GetHistoryCallState(true))
		{
			ICC_LOG_WARNING(m_pLog, "already get history call list.");
			return;
		}
		bool retflag = DoProcHistoryCallList();
		if (!retflag)
		{
			//获取历史话务数据失败则等待1秒后再试一次，一般情况是由于cti服务重启导致获取不到服务，才会出现此错误
			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
			retflag = DoProcHistoryCallList();
			if (!retflag)
			{
				ICC_LOG_ERROR(m_pLog, "DoProcHistoryCallList false");
			}
		}
		SetHistoryCallState(false);
	}


	//存储CallRefID和BeginTalkTime的关系

	inline std::string RecordBusinessImpl::GetBeginTalkTimeByCallRefId(const std::string& strCallRefId, const std::string& strAgentID)
	{
		std::lock_guard<std::mutex> lk(m_lkMap);
		std::map < std::string, std::map<std::string, std::string> >::iterator itrCallRefId = m_mapCallRefIdBeginTalkTime.find(strCallRefId);
		if (itrCallRefId != m_mapCallRefIdBeginTalkTime.end())
		{
			if (itrCallRefId->second.find(strAgentID) != itrCallRefId->second.end())
			{
				return itrCallRefId->second[strAgentID];
			}

		}
		return "";
	}

	inline void RecordBusinessImpl::SetCallBeginTalkTime(const std::string& strCallRefId, const std::string& strAgentID, const std::string& strBeginTalkTime)
	{
		std::lock_guard<std::mutex> lk(m_lkMap);
		std::map < std::string, std::map<std::string, std::string> >::iterator itrCallRefId = m_mapCallRefIdBeginTalkTime.find(strCallRefId);
		if (itrCallRefId == m_mapCallRefIdBeginTalkTime.end())
		{
			std::map<std::string, std::string> mapAgentBeginTalk;
			mapAgentBeginTalk[strAgentID] = strBeginTalkTime;
			m_mapCallRefIdBeginTalkTime[strCallRefId] = mapAgentBeginTalk;
		}
		else
		{
			itrCallRefId->second.insert(std::make_pair(strAgentID, strBeginTalkTime));
		}

	}

	inline void RecordBusinessImpl::RmvCallBeginTalkTime(const std::string& strCallRefId, const std::string& strAgentID)
	{
		std::lock_guard<std::mutex> lk(m_lkMap);
		std::map < std::string, std::map<std::string, std::string> >::iterator itrCallRefId = m_mapCallRefIdBeginTalkTime.find(strCallRefId);
		if (itrCallRefId != m_mapCallRefIdBeginTalkTime.end())
		{
			itrCallRefId->second.erase(strAgentID);
			if (itrCallRefId->second.size() <= 0)
			{
				m_mapCallRefIdBeginTalkTime.erase(strCallRefId);
			}
		}
	}

	inline void RecordBusinessImpl::RmvCallBeginTalkTime(const std::string& strCallRefId)
	{
		std::lock_guard<std::mutex> lk(m_lkMap);
		m_mapCallRefIdBeginTalkTime.erase(strCallRefId);
	}

	inline size_t RecordBusinessImpl::CallBeginTalkTimeSize()
	{
		std::lock_guard<std::mutex> lk(m_lkMap);
		return m_mapCallRefIdBeginTalkTime.size();
	}

	bool RecordBusinessImpl::DoProcHistoryCallList()
	{
		std::string strCurCTIAddr;
		std::string strCurCTIPort;
		std::string strCTIServiceName = m_pConfig->GetValue("ICC/Plugin/Record/CTIServiceName", "icc-cti-service");
		std::string strHistoryFileAccessApi = m_pConfig->GetValue("ICC/Plugin/Record/HistoryFileAccessAPI", "/icc/cti/get_history_callList");

		if (!GetAddressInteractionGlobalInstance()->GetHealthyServiceFromNacos(strCTIServiceName, strCurCTIAddr, strCurCTIPort))
		{
			ICC_LOG_ERROR(m_pLog, "[DoProcHistoryCallList]Get Healthy CTI Service Failed.[%s]", strCTIServiceName.c_str());
			return false;
		}
		//http://ip:port/icc/cti/get_history_callList

		std::string strUrl = "http://" + strCurCTIAddr + ":" + strCurCTIPort + strHistoryFileAccessApi;

		int nAllCount = 0;
		int nPageSize = 200;
		int nPageIndex = 1;
		int nGetCount = 0;
		int nRet = GetHistoryCallList(strUrl, nPageSize, nPageIndex, nAllCount);
		if (nRet > 0)
		{
			nGetCount += nRet;
			while (nGetCount = nPageSize)
			{
				nPageIndex++;
				int nRet = GetHistoryCallList(strUrl, nPageSize, nPageIndex, nAllCount);
				if (nRet <= 0)
				{
					break;
				}
				nGetCount += nRet;
			}
		}
		return true;
	}

	//负数：错误 0 或者正数：记录的条数
	int RecordBusinessImpl::GetHistoryCallList(std::string& strUrl, int nPageSize, int nPageIndex, int& nAllCount)
	{
		JsonParser::IJsonPtr pRequestJson = ICCGetIJsonFactory()->CreateJson();
		pRequestJson->SetNodeValue("/header/cmd", strUrl);
		pRequestJson->SetNodeValue("/header/seat_no", "0");
		pRequestJson->SetNodeValue("/header/code", "");
		pRequestJson->SetNodeValue("/header/token", "");
		pRequestJson->SetNodeValue("/body/page_size", std::to_string(nPageSize));
		pRequestJson->SetNodeValue("/body/page_index", std::to_string(nPageIndex));

		std::string strRequestJson = pRequestJson->ToString();

		ICC::IHttpClientPtr pHttpClient = ICCGetIHttpClientFactory()->CreateHttpClient();
		if (!pHttpClient)
		{// create http client
			ICC_LOG_ERROR(m_pLog, "%s", "init http client failed!");
			return -1;
		}
		std::map<std::string, std::string> header;
		header.insert(std::make_pair("content-type","application/json"));
		header.insert(std::make_pair("Accept", "*/*"));
		header.insert(std::make_pair("Connection", "close"));
		header.insert(std::make_pair("Accept-Encoding", "gzip, deflate, br"));
		pHttpClient->SetHeaders(header);
		std::string strResponse = pHttpClient->Post(strUrl, strRequestJson);

		ICC_LOG_INFO(m_pLog, " GetHistoryCallList url %s ", strUrl.c_str());
		ICC_LOG_DEBUG(m_pLog, " GetHistoryCallList request %s ", strRequestJson.c_str());
		ICC_LOG_DEBUG(m_pLog, " GetHistoryCallList response %s ", strResponse.c_str());
		ICC_LOG_INFO(m_pLog, " GetHistoryCallList code %d msg %s ", pHttpClient->GetLastErrorCode(), pHttpClient->GetLastError().c_str());

		int pos = strResponse.find('{');
		int lastPos = strResponse.rfind('}');

		if (pos == string::npos || lastPos == string::npos)
		{// can't parse invalid json string
			ICC_LOG_ERROR(m_pLog, "%s", "response from Nacos is invalid!!");
			return -1;
		}

		std::string midString = strResponse.substr(pos, lastPos - pos + 1);
		JsonParser::IJsonPtr pResponseJson = ICCGetIJsonFactory()->CreateJson();
		if (!pResponseJson->LoadJson(midString))
		{
			ICC_LOG_ERROR(m_pLog, "[GetHistoryCallList]Parse request error.[%s]", midString.c_str());
			return -1;
		}

		std::string strCount = pResponseJson->GetNodeValue("/body/count", "0");
		std::string strAllCount = pResponseJson->GetNodeValue("/body/all_count", "0");

		int nDataCount = pResponseJson->GetCount("/body/data");
		ICC_LOG_INFO(m_pLog, " strCount %s strAllCount %s nDataCount %d ", strCount.c_str(), strAllCount.c_str(), nDataCount);
		for (int nIndex = 0; nIndex < nDataCount; nIndex++)
		{
			std::string strAgentID = pResponseJson->GetNodeValue("/body/data/" + std::to_string(nIndex) + "/agent", "");
			std::string strRecordFileList = pResponseJson->GetNodeValue("/body/data/" + std::to_string(nIndex) + "/record_files", "");
			std::string strTalkTime = pResponseJson->GetNodeValue("/body/data/" + std::to_string(nIndex) + "/talk_time", "");
			std::string strRingTime = pResponseJson->GetNodeValue("/body/data/" + std::to_string(nIndex) + "/ring_time", "");
			std::string strHangupTime = pResponseJson->GetNodeValue("/body/data/" + std::to_string(nIndex) + "/hangup_time", "");
			std::string strCallRefId = pResponseJson->GetNodeValue("/body/data/" + std::to_string(nIndex) + "/callref_id", "");
			std::string strCallerId = pResponseJson->GetNodeValue("/body/data/" + std::to_string(nIndex) + "/caller_id", "");
			std::string strCalledId = pResponseJson->GetNodeValue("/body/data/" + std::to_string(nIndex) + "/called_id", "");

			//获取开始通话时间
			std::string strBeginTalkTime = strTalkTime;
			if (strBeginTalkTime.empty())
			{
				strBeginTalkTime = strRingTime;
			}
			GetFileRecordFromProtocol("", strRecordFileList, strBeginTalkTime, strHangupTime,
				strAgentID, strCallRefId, strCallerId, strCalledId, true);

		}
		nAllCount = String2Int(strAllCount);
		return String2Int(strCount);
	}

	void RecordBusinessImpl::OnCtiCallInfoNotice(ObserverPattern::INotificationPtr p_pNotifiRequest)
	{
		JsonParser::IJsonPtr l_pJson = ICCGetIJsonFactory()->CreateJson();
		PROTOCOL::CCallStateEvent request;
		std::string l_strRecvMsg(p_pNotifiRequest->GetMessages());
		if (!request.ParseString(l_strRecvMsg, l_pJson))
		{
			ICC_LOG_ERROR(m_pLog, "[OnCtiCallInfoNotice]Parse request error.[%s]", l_strRecvMsg.c_str());
			return;
		}
		ICC_LOG_DEBUG(m_pLog, "receive message[%s].", l_strRecvMsg.c_str());

		if (request.m_oBody.m_strState.compare(CTI_ASSIGN) != 0 || request.m_oBody.m_strAgent.empty())
		{// 其他状态未分配分机，交换机不录音
			return;
		}

		if (m_pAvayaCap != nullptr)
		{
			CallInfo pCallInfo;
			pCallInfo.m_strAgentID = request.m_oBody.m_strAgent;
			pCallInfo.m_strCallID = request.m_oBody.m_strCallRefId;
			pCallInfo.m_strCaller = request.m_oBody.m_strCallerId;
			pCallInfo.m_strCalled = request.m_oBody.m_strCalledId;
			m_pAvayaCap->BindCallInfo(pCallInfo);
			ICC_LOG_DEBUG(m_pLog, "Bind call info, device[%s], callrefid[%s], caller[%s], called[%s].",
				pCallInfo.m_strAgentID.c_str(), pCallInfo.m_strCallID.c_str(), pCallInfo.m_strCaller.c_str(), pCallInfo.m_strCalled.c_str());
		}
	}
	void RecordBusinessImpl::OnCtiDeviceInfoNotice(ObserverPattern::INotificationPtr p_pNotifiRequest)
	{
		JsonParser::IJsonPtr l_pJson = ICCGetIJsonFactory()->CreateJson();
		PROTOCOL::CDeviceStateEvent request;
		std::string l_strRecvMsg(p_pNotifiRequest->GetMessages());
		if (!request.ParseString(l_strRecvMsg, l_pJson))
		{
			ICC_LOG_ERROR(m_pLog, "[OnCtiDeviceInfoNotice]Parse request error.[%s]", l_strRecvMsg.c_str());
			return;
		}
		ICC_LOG_DEBUG(m_pLog, "receive message[%s].", l_strRecvMsg.c_str());

		if (request.m_oBody.m_strDeviceType.compare("ipphone") != 0
			|| (request.m_oBody.m_strState.compare(CTI_RINGSTATE) != 0
				&& request.m_oBody.m_strState.compare(CTI_RINGBACKSTATE) != 0
				&& request.m_oBody.m_strState.compare(CTI_DIALSTATE) != 0
				&& request.m_oBody.m_strState.compare(CTI_HANGUPSTATE) != 0
				&& request.m_oBody.m_strState.compare(CTI_WAITHANGUPSTATE) != 0
				&& request.m_oBody.m_strState.compare(CTI_TALKSTATE) !=0
				&& request.m_oBody.m_strState.compare(CTI_CONFERENCESTATE) != 0
				&& request.m_oBody.m_strState.compare(CTI_SILENTMONITOR) != 0
				&& request.m_oBody.m_strState.compare(CTI_FREESTATE) !=0) )
		{//不处理非话机的消息，仅处理上面状态的消息。
			ICC_LOG_DEBUG(m_pLog, "[OnCtiDeviceInfoNotice] does not process message .[device type %s device state %s]", request.m_oBody.m_strDeviceType.c_str(), request.m_oBody.m_strState.c_str());
			return;
		}

		if (request.m_oBody.m_strDevice.empty())
		{//其他状态未分配分机，交换机不录音
			request.m_oBody.m_strDevice = (request.m_oBody.m_strCallDirection.compare("in") == 0) ? request.m_oBody.m_strCalledId : request.m_oBody.m_strCallerId;
		}


		/*freestate（空闲态）ringstate（振铃态）ringbackstate（回铃态）dialstate（拨号态）talkstate（通话态）
	conferencestate（会议态）holdstate（保留态）hangupstate（挂机态）waithangupstate（等待挂机态） silentmonitorstate(监听态) */
		if (request.m_oBody.m_strState.compare(CTI_RINGSTATE) == 0
			|| request.m_oBody.m_strState.compare(CTI_TALKSTATE) == 0
			|| request.m_oBody.m_strState.compare(CTI_CONFERENCESTATE) == 0
			|| request.m_oBody.m_strState.compare(CTI_SILENTMONITOR) == 0)
		{
			//绑定呼叫关联信息
			if (m_pAvayaCap != nullptr)
			{
			    CallInfo pCallInfo;
			    pCallInfo.m_strAgentID = request.m_oBody.m_strDevice;
			    pCallInfo.m_strCallID = request.m_oBody.m_strCallRefId;
			    pCallInfo.m_strCaller = request.m_oBody.m_strCallerId;
			    pCallInfo.m_strCalled = request.m_oBody.m_strCalledId;
			    m_pAvayaCap->BindCallInfo(pCallInfo);
			}
			//取开始通话时间
			if (request.m_oBody.m_strState.compare(CTI_TALKSTATE) == 0 
				|| request.m_oBody.m_strState.compare(CTI_CONFERENCESTATE) == 0 
				|| request.m_oBody.m_strState.compare(CTI_SILENTMONITOR) == 0)
			{
				if (!request.m_oBody.m_strBeginTalkTime.empty())
				{
					SetCallBeginTalkTime(request.m_oBody.m_strCallRefId, request.m_oBody.m_strDevice, request.m_oBody.m_strBeginTalkTime);
				}
				else if (!request.m_oBody.m_strTime.empty())
				{
					SetCallBeginTalkTime(request.m_oBody.m_strCallRefId, request.m_oBody.m_strDevice, request.m_oBody.m_strTime);
				}
			}
		}
		else if (request.m_oBody.m_strState.compare(CTI_HANGUPSTATE) == 0)
		{
			if (m_pAvayaCap != nullptr)
			{
			    CallInfo pCallInfo;
			    pCallInfo.m_strAgentID = request.m_oBody.m_strDevice;
			    pCallInfo.m_strCallID = request.m_oBody.m_strCallRefId;
			    pCallInfo.m_strCaller = request.m_oBody.m_strCallerId;
			    pCallInfo.m_strCalled = request.m_oBody.m_strCalledId;
			    m_pAvayaCap->StartMediaStorageByCallInfo(pCallInfo);
			}
			std::string strBeginTalkTime = request.m_oBody.m_strBeginTalkTime;
			if (strBeginTalkTime.empty())
			{
				strBeginTalkTime = GetBeginTalkTimeByCallRefId(request.m_oBody.m_strCallRefId, request.m_oBody.m_strDevice);
				size_t mapSize = CallBeginTalkTimeSize();
				ICC_LOG_DEBUG(m_pLog, "[OnCtiDeviceInfoNotice] get begin talk time of (callid:%s deviceid:%s) time:%s map size(%u)", request.m_oBody.m_strCallRefId.c_str(), request.m_oBody.m_strDevice.c_str(), strBeginTalkTime.c_str(), mapSize);
			}
			RmvCallBeginTalkTime(request.m_oBody.m_strCallRefId, request.m_oBody.m_strDevice);
			GetFileRecordFromProtocol(request.m_oBody.m_strSwitchAddress, request.m_oBody.m_strRecordFilePath, strBeginTalkTime, request.m_oBody.m_strTime,
				request.m_oBody.m_strDevice, request.m_oBody.m_strCallRefId, request.m_oBody.m_strCallerId, request.m_oBody.m_strCalledId, false);
		}
	}
	void RecordBusinessImpl::OnCtiCallOverSyncNotice(ObserverPattern::INotificationPtr p_pNotifiRequest)
	{
		JsonParser::IJsonPtr l_pJson = ICCGetIJsonFactory()->CreateJson();
		PROTOCOL::CCallOverEvent request;
		std::string l_strRecvMsg(p_pNotifiRequest->GetMessages());
		if (!request.ParseString(l_strRecvMsg, l_pJson))
		{
			ICC_LOG_ERROR(m_pLog, "[OnCtiCallOverSyncNotice]Parse request error.[%s]", l_strRecvMsg.c_str());
			return;
		}
		ICC_LOG_DEBUG(m_pLog, "receive message[%s].", l_strRecvMsg.c_str());
		//RmvCallBeginTalkTime(request.m_oBody.m_strCallRefId);
		//GetFileRecordFromProtocol("", request.m_oBody.m_strRecordFilePath, request.m_oBody.m_strRingTime, request.m_oBody.m_strHangupTime,
		//	request.m_oBody.m_strAgent, request.m_oBody.m_strCallRefId, request.m_oBody.m_strCallerId, request.m_oBody.m_strCalledId);

	}
	void RecordBusinessImpl::GetFileRecordFromProtocol(const std::string& paraFSIp, const std::string& paraRecordFileList, const std::string& paraIncomingTime, const std::string& paraHangupTime,
		const std::string& paraAgentId, const std::string& paraCallRefId, const std::string& paraCallerId, const std::string& paraCalledId, bool bIsHistory/* = false*/)
	{

		ICC_LOG_DEBUG(m_pLog, "GetFileRecordFromProtocol paraFSIp:%s paraRecordFileList:%s paraIncomingTime:%s paraHangupTime:%s paraCallRefId:%s paraCallerId:%s paraCalledId:%s", paraFSIp.c_str(), paraRecordFileList.c_str(),
			paraIncomingTime.c_str(), paraHangupTime.c_str(), paraCallRefId.c_str(), paraCallerId.c_str(), paraCalledId.c_str());

		if (nullptr == m_pFtpDownLoadMgr)//FtpDownLoadMgr不为空指针代表FreeSwitch录音有效，进行FreeSwitch的录音处理
		{
			ICC_LOG_ERROR(m_pLog, "GetFileRecordFromProtocol DownLoadManager is null, may be not configure ICC/Plugin/Record/FSFtpIP or ICC/Plugin/Record/FSFtp2IP in configure file.");
			return;
		}

		if (paraRecordFileList.empty())	//不处理文件列表为空的消息，可能是通过抓包获取录音的消息
		{
			return;
		}

		std::string strRecordFileList = paraRecordFileList;
		std::string strFSIP = paraFSIp;

		std::vector<std::string> vecSwitchAddrs;
		std::vector<std::string> vecRecordFiles;
		str2list(strRecordFileList, ";", vecRecordFiles);
		if (!strFSIP.empty())
		{
			str2list(strFSIP, ";", vecSwitchAddrs);
		}
		ICC_LOG_DEBUG(m_pLog, "GetFileRecordFromProtocol vecSwitchAddrs size(%u), vecRecordFiles size(%u)", vecSwitchAddrs.size(), vecRecordFiles.size());
		for (size_t indx = 0;indx < vecRecordFiles.size();indx++)
		{
			std::string strRecordFile = vecRecordFiles[indx];
			std::string strFSIp = "";
			if (vecSwitchAddrs.size() > indx)
			{
				strFSIp = vecSwitchAddrs[indx];
			}
			std::string strRecordFileFullPath = m_strFSLocalRecordFileDir + strRecordFile;
			CRecordFilePtr pRecordFile = std::make_shared<CRecordFile>(strRecordFileFullPath, paraIncomingTime, paraHangupTime, "", 0);
			if (pRecordFile != nullptr)
			{
				pRecordFile->m_strAgentPhoneID = paraAgentId;
				pRecordFile->m_setCallID.insert(paraCallRefId);
				pRecordFile->m_strCaller = paraCallerId;
				pRecordFile->m_strCalled = paraCalledId;
				pRecordFile->m_strStartTime = paraIncomingTime;
				pRecordFile->m_strStopTime = paraHangupTime;
				pRecordFile->m_strFSFtpFileName = strRecordFile;
				pRecordFile->m_bIsHistory = bIsHistory;
			}
			m_pFtpDownLoadMgr->AddDownloadFile(strFSIp, strRecordFile, pRecordFile);
		}
	}

	void RecordBusinessImpl::OnCtiAlarmSyncNotice(ObserverPattern::INotificationPtr p_pNotifiRequest)
	{
		JsonParser::IJsonPtr l_pJson = ICCGetIJsonFactory()->CreateJson();
		PROTOCOL::CAlarmSync request;
		std::string l_strRecvMsg(p_pNotifiRequest->GetMessages());
		if (!request.ParseString(l_strRecvMsg, l_pJson))
		{
			ICC_LOG_ERROR(m_pLog, "[OnCtiAlarmSyncNotice]Parse request error.[%s]", l_strRecvMsg.c_str());
			return;
		}
		ICC_LOG_DEBUG(m_pLog, "receive message[%s].", l_strRecvMsg.c_str());
	}

	void RecordBusinessImpl::OnSyncServerLock(ObserverPattern::INotificationPtr p_pNotifiRequest)
	{
		JsonParser::IJsonPtr l_pJson = ICCGetIJsonFactory()->CreateJson();
		PROTOCOL::CSyncServerLock request;
		std::string l_strRecvMsg(p_pNotifiRequest->GetMessages());
		if (!request.ParseString(l_strRecvMsg, l_pJson))
		{
			ICC_LOG_ERROR(m_pLog, "[OnSyncServerLock]Parse request error.[%s]", l_strRecvMsg.c_str());
			return;
		}
		ICC_LOG_DEBUG(m_pLog, "receive message[%s].", l_strRecvMsg.c_str());
		if (request.m_oBody.m_strServiceState == "master")
		{
			//只有主处理业务,升主后获取历史话单
			m_thrdGetHistoryCallList.reset(new boost::thread(boost::bind(&RecordBusinessImpl::ThreadGetHistoryCallList, this)));
			if (m_nEnableNetCap != 0)
			{
				if (!m_bIsStartAvayaCap)
				{
					if (0 == CreateNetDataCap())
					{
						m_bIsStartAvayaCap = true;
					}
				}
			}
		}
		else
		{
			//备停止处理业务
			if (m_nEnableNetCap != 0)
			{
				if (m_bIsStartAvayaCap)
				{
					if (0 == StopNetDataCap())
					{
						m_bIsStartAvayaCap = false;
					}
				}
			}
		}
	}

	int RecordBusinessImpl::UpdateDBRecord(CRecordFilePtr pRecordFile)
	{
		if (NULL == pRecordFile)
		{
			return 0;
		}

		//是否更新数据库配置, 
		int bUpdateDBRecord = true;
		if (m_pConfig != nullptr)
		{
			bUpdateDBRecord = String2Int(m_pConfig->GetValue("ICC/Plugin/Record/UpdateDBRecord", "1"));
		}
		if (!bUpdateDBRecord)
		{
			return 0;
		}

		//通过 RecordFile 的DBGuid判断是否保存到了数据库，如果不为空则表示存过数据库了，再次调用时就不再保存数据库
		std::string strDBGuid = pRecordFile->m_strDBGuid;
		if (!pRecordFile->m_strDBGuid.empty())
		{
			return 0;
		}
		strDBGuid = m_pString->CreateGuid();
		DataBase::SQLRequest m_Query;
		m_Query.sql_id = "insert_into_record_file_item";
		m_Query.param.insert(std::pair<std::string, std::string>("guid", strDBGuid));
		m_Query.param.insert(std::pair<std::string, std::string>("file_id", pRecordFile->m_strFileID));
		m_Query.param.insert(std::pair<std::string, std::string>("record_file_name", pRecordFile->m_strLocalFileName));
		m_Query.param.insert(std::pair<std::string, std::string>("start_time", (pRecordFile->m_strStartTime.empty()? m_pDateTime->CurrentDateTimeStr(): pRecordFile->m_strStartTime))); //开始时间为空，取当前系统时间
		m_Query.param.insert(std::pair<std::string, std::string>("stop_time", (pRecordFile->m_strStopTime.empty() ? m_pDateTime->CurrentDateTimeStr() : pRecordFile->m_strStopTime))); //结束时间为空，取当前系统时间
		m_Query.param.insert(std::pair<std::string, std::string>("record_file_url", pRecordFile->m_strFileUrl));
		m_Query.param.insert(std::pair<std::string, std::string>("agent_phone_ip", pRecordFile->m_strAgentPhoneIP));
		m_Query.param.insert(std::pair<std::string, std::string>("agent_phone_no", pRecordFile->m_strAgentPhoneID));
		m_Query.param.insert(std::pair<std::string, std::string>("call_id", pRecordFile->GetCallRefID()));
		m_Query.param.insert(std::pair<std::string, std::string>("caller_no", pRecordFile->m_strCaller));
		m_Query.param.insert(std::pair<std::string, std::string>("called_no", pRecordFile->m_strCalled));
		m_Query.param.insert(std::pair<std::string, std::string>("cti_refreshed", std::to_string(pRecordFile->m_bCTIRefreshed)));

		//	使用主线程的数据库链接
		DataBase::IResultSetPtr l_pResult = m_pInitDBConn->Exec(m_Query);
		if (!l_pResult->IsValid())
		{
			ICC_LOG_ERROR(m_pLog, "Add Record File Item failed , SQL: [%s], Error: [%s]",
				l_pResult->GetSQL().c_str(),
				l_pResult->GetErrorMsg().c_str());
			return -1;
		}
		ICC_LOG_DEBUG(m_pLog, "Add Record File Item success, SQL: [%s]",
			l_pResult->GetSQL().c_str());
		pRecordFile->m_strDBGuid = strDBGuid;
		return 0;
	}

	void RecordBusinessImpl::SendFileSyncMsg(CRecordFilePtr pRecordFile,int nResult)
	{
		PROTOCOL::CRecordFileUploadSync msgFileUploadSync;
		msgFileUploadSync.m_oHeader.m_strCmd = CMD_FileUploadSync;
		msgFileUploadSync.m_oHeader.m_strSystemID = CMD_System_Id_Record;
		msgFileUploadSync.m_oHeader.m_strSubsystemID = CMD_SubSystemId_RecordServer;
		msgFileUploadSync.m_oHeader.m_strRelatedID = "";
		msgFileUploadSync.m_oHeader.m_strSendTime = m_pDateTime->CurrentDateTimeStr();
		msgFileUploadSync.m_oHeader.m_strRequest = CMD_TopicName_FileUploadSync;
		msgFileUploadSync.m_oHeader.m_strRequestType = CMD_SendType_Topic;
		msgFileUploadSync.m_oHeader.m_strResponse = "";
		msgFileUploadSync.m_oHeader.m_strResponseType = "";
		msgFileUploadSync.m_oBody.m_strFileID = pRecordFile->m_strFileID;
		msgFileUploadSync.m_oBody.m_strStartTime = pRecordFile->m_strStartTime;
		msgFileUploadSync.m_oBody.m_strAgentID = pRecordFile->m_strAgentPhoneID;
		
		msgFileUploadSync.m_oBody.m_strFileName = GetFileName(pRecordFile->m_strLocalFileName);
		msgFileUploadSync.m_oBody.m_strResult = std::to_string(nResult);

		//针对每一个CallRefID发送一条消息
		std::set<std::string>::iterator iter = pRecordFile->m_setCallID.begin();
		for (;iter != pRecordFile->m_setCallID.end(); iter++)
		{
			msgFileUploadSync.m_oBody.m_strCallRefID = *iter;
			msgFileUploadSync.m_oHeader.m_strMsgid = m_pString->CreateGuid();
			std::string l_strMsg = msgFileUploadSync.ToString(ICCGetIJsonFactory()->CreateJson());
			m_pObserverCenter->Notify(boost::make_shared<CNotifiSendRequest>(l_strMsg));
			ICC_LOG_DEBUG(m_pLog, "SendFileSyncMsg send message:[%s]", l_strMsg.c_str());
			this_thread::sleep_for(std::chrono::milliseconds(10));
		}

		pRecordFile->m_bCTIRefreshed = true;
		

	}

	void RecordBusinessImpl::OnCtiFileSyncMsgAck(ObserverPattern::INotificationPtr p_pNotifiReponse)
	{
		;
	}

	int RecordBusinessImpl::UploadFileToServer(CRecordFilePtr pRecordFile)
	{
		if (NULL == pRecordFile)
		{
			return 0;
		}
		//FileID不为空代表已经上传过文件了。直接返回成功
		if (!pRecordFile->m_strFileID.empty())
		{
			return 0;
		}
		std::string strRet = GetUploadManagerGlobalInstance()->UploadFile(pRecordFile->m_strLocalFileName, pRecordFile->m_strFileID);
		ICC_LOG_ERROR(m_pLog, "UploadFile %s result %s", pRecordFile->m_strLocalFileName.c_str(), strRet.c_str());
		if (strRet == "0" || strRet == "200")
		{
			std::string strFileServerIP("");
			std::string strFileServerPort("");
			GetUploadManagerGlobalInstance()->GetFileServiceAddress(strFileServerIP, strFileServerPort);
			pRecordFile->m_strFileUrl = "http://" + strFileServerIP + ":" + strFileServerPort + "/file/down?fileId=" + pRecordFile->m_strFileID;
			return 0;
		}
		int nRet = -1;
		try
		{
			nRet = std::stoi(strRet);
		}
		catch (...)
		{
			ICC_LOG_ERROR(m_pLog, "UploadFileToServer: convert upload result %s error.", strRet.c_str());
		}
		return nRet;
	}

	void RecordBusinessImpl::OnTimer(unsigned long ulTimerID)
	{
		if (ulTimerID == m_ulTimerSyncAvayaConfig)
		{
			DoGetAESPhoneIP();
		}
		else if (ulTimerID == m_ulTimerUploadFiles)
		{
			DoFileUpload();
		}
	}

	void RecordBusinessImpl::DoGetAESPhoneIP()
	{
		std::map<std::string, std::string> mapStationIP;
		if (m_pAvayaAesClt != nullptr)
		{
			m_pAvayaAesClt->getAllStationIP(mapStationIP);
			Xml::IXmlPtr pXmlIPAgentPtr = ICCGetIXmlFactory()->CreateXml();

			int iIndexPhoneIndex =0;
			for (std::map<std::string, std::string>::iterator itr = mapStationIP.begin();itr != mapStationIP.end(); itr++)
			{
				CAgentIPTable::GetInstance().AddAgentPhoneIP(itr->first, itr->second);
				if (nullptr != pXmlIPAgentPtr)
				{
					std::string strPath = m_pString->Format("Record/Agents/Agent[%d]", iIndexPhoneIndex);
					pXmlIPAgentPtr->SetText(strPath + "/Ext", itr->first);
					pXmlIPAgentPtr->SetText(strPath + "/IP", itr->second);
				}
				iIndexPhoneIndex++;
			}

			Config::IConfigPtr pConfigConfig = ICCGetIConfigFactory()->CreateConfigConfig();
			if (nullptr != pConfigConfig)
			{
				string strConfigFileDir = pConfigConfig->GetValue("ICC.ConfigFileDir", "./Config/");
				string strFilePath = strConfigFileDir + "agent-ip-config.xml";
				pXmlIPAgentPtr->SaveAs(strFilePath);
			}
		}
	}

	/************************************************************************/
	/*                                                                      */
	/************************************************************************/
	RecordBusinessImpl::RecordBusinessImpl():m_ulTimerSyncAvayaConfig(0), m_ulTimerUploadFiles(0), m_bIsStartAvayaCap(false), 
		m_nEnableNetCap(0), m_pFtpDownLoadMgr(NULL), m_pAvayaCap(NULL),m_pAvayaAesClt(NULL), m_strFSLocalRecordFileDir(""), m_bIsGetHistoryCall(false)
	{
	}

	RecordBusinessImpl::~RecordBusinessImpl()
	{
	}
}
