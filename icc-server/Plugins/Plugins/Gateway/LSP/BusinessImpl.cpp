#include "Boost.h"
#include "BusinessImpl.h"

#include <iostream>

#define TIMER_CMD_NAME "lsp_timer"
#define LSP_BUFF_SIZE 20000

CBusinessImpl* g_pBusinessImpl = nullptr;

CBusinessImpl::CBusinessImpl()
{
	g_pBusinessImpl = this;
}

CBusinessImpl::~CBusinessImpl()
{
}

void CBusinessImpl::OnInit()
{
	m_pObserverCenter = ICCGetIObserverFactory()->GetObserverCenter(GATEWAY_LSP_OBSERVER_CENTER);
	m_pLog = ICCGetILogFactory()->GetLogger(MODULE_NAME);
	m_pConfig = ICCGetIConfigFactory()->CreateConfig();
	m_pStaticConfig = ICCGetIConfigFactory()->CreateStaticConfig();
	m_pJsonFty = ICCGetIJsonFactory();
	m_pDateTime = ICCGetIDateTimeFactory()->CreateDateTime();
	m_pString = ICCGetIStringFactory()->CreateString();
	m_pHelpTool = ICCGetHelpToolFactory()->CreateHelpTool();
	m_pTimerMgr = ICCGetITimerFactory()->CreateTimerManager();
	m_pHttpClient = ICCGetIHttpClientFactory()->CreateHttpClient();	
}

void CBusinessImpl::OnStart()
{
	std::string l_strIsUsing = m_pConfig->GetValue("ICC/Plugin/LSP/IsUsing", "0");

	if (l_strIsUsing != "1")
	{
		ICC_LOG_DEBUG(m_pLog, "there is no need to load the plugin");
		return;
	}

	m_strSelfDir = m_pStaticConfig->GetValue("ICC/Component/Logger/TargetMidPath", "");
	m_strScanDir = m_pConfig->GetValue("ICC/Component/Logger/Target", "../../Tools/Logs");
	m_strLSPUrl = m_pConfig->GetValue("ICC/Plugin/LSP/LSPUrl", "http://127.0.0.1:22170/api/v2/events");
	std::string l_strInterval = m_pConfig->GetValue("ICC/Plugin/LSP/ScanInterval", "60");
	m_strSystem = m_pConfig->GetValue("ICC/Plugin/LSP/System", "ICC");
	m_iPackageSize = m_pString->ToInt(m_pConfig->GetValue("ICC/Plugin/LSP/PackageSize", "10"));
	std::string l_strBearerKey = m_pConfig->GetValue("ICC/Plugin/LSP/BearerKey", "");
	std::string l_strBearerKeySlave = m_pConfig->GetValue("ICC/Plugin/LSP/BearerKeySlave", "");

	if (l_strBearerKey.empty())
	{
		ICC_LOG_ERROR(m_pLog, "BearerKey is empty , Service stop !");

		return;
	}

	std::map<std::string, std::string> l_ParamsMap;
	l_ParamsMap["Content-Type"] = "application/json; charset = utf-8";
	l_ParamsMap["Accept"] = "application/json";
	l_ParamsMap["Authorization"] = std::string("Bearer ") + l_strBearerKey;
	l_ParamsMap["Connection"] = "Keep-Alive";

	if (!l_strBearerKeySlave.empty())
	{
		// 如果有多个 Header，配置的 URL 需与 Header 一一对应

		std::vector<std::map<std::string, std::string>> l_HeaderVec;
		l_HeaderVec.push_back(l_ParamsMap);

		l_ParamsMap["Authorization"] = std::string("Bearer ") + l_strBearerKeySlave;
		l_HeaderVec.push_back(l_ParamsMap);

		m_pHttpClient->SetHeadersEx(l_HeaderVec);
	}
	else
	{
		m_pHttpClient->SetHeaders(l_ParamsMap);
	}

	// 注册观察者
	ADDOBSERVER(m_pObserverCenter, CBusinessImpl, TIMER_CMD_NAME, OnTimer);
	m_pTimerMgr->AddTimer(TIMER_CMD_NAME, m_pString->ToUInt(l_strInterval), 60);

	ICC_LOG_DEBUG(m_pLog, "start lsp plugin success.");
}

void CBusinessImpl::OnStop()
{

}

void CBusinessImpl::OnDestroy()
{

}

void CBusinessImpl::OnTimer(ObserverPattern::INotificationPtr p_pNotifiRequest)
{	
	std::list<std::string> l_strFiles;
	
	m_pHelpTool->ScanFilesRecursive(m_strScanDir + "/CommandCenter.ICC.Server.Alarm", ".log", l_strFiles);
	l_strFiles.sort();
	if (!l_strFiles.empty())
	{
		l_strFiles.pop_back();
	}

	int l_size = l_strFiles.size();
	m_pHelpTool->ScanFilesRecursive(m_strScanDir + "/CommandCenter.ICC.Server.CTI", ".log", l_strFiles);	
	if (l_strFiles.size() > l_size)
	{
		l_strFiles.sort();
		if (!l_strFiles.empty())
		{
			l_strFiles.pop_back();
		}
	}

	ICC_LOG_DEBUG(m_pLog, "scan file, [%d]", l_strFiles.size());

	std::vector<std::string> l_vecRecords;
	for (std::string l_strFile : l_strFiles)
	{
		//自己服务的日志不发送
		if (m_pString->Find(l_strFile, m_strSelfDir))
		{
			ICC_LOG_DEBUG(m_pLog, "lsp log no need to send, [%s]", l_strFile.c_str());
			continue;
		}

		if (IsUsed(l_strFile))
		{
			ICC_LOG_DEBUG(m_pLog, "file is used, [%s]", l_strFile.c_str());

			GetLogRecordsIncrement(l_strFile, m_readCachMap[l_strFile], l_vecRecords);

			ICC_LOG_DEBUG(m_pLog, "get file increment record, [%d]", l_vecRecords.size());
		}
		else
		{
			std::map<std::string, unsigned int>::iterator it = m_readCachMap.find(l_strFile);
			if (it != m_readCachMap.end())
			{
				//如果在缓存中找到文件，说明文件已经写完
				//将文件最后一部分读出来，清掉缓存，并且将文件重命名，以免重复读取
				GetLogRecordsIncrement(l_strFile, it->second, l_vecRecords);	

				ICC_LOG_DEBUG(m_pLog, "get file last record, [%d]", l_vecRecords.size());

				RenameFile(l_strFile);
				m_readCachMap.erase(it);

				ICC_LOG_DEBUG(m_pLog, "rename file, [%s]", l_strFile.c_str());
			}
			else
			{
				GetLogRecords(l_strFile, l_vecRecords);

				ICC_LOG_DEBUG(m_pLog, "get file record, [%d]", l_vecRecords.size());
			}
		}

		//发送到日志平台
		SendLog(l_vecRecords);

		ICC_LOG_DEBUG(m_pLog, "send record, [%d]", l_vecRecords.size());

		l_vecRecords.clear();
	}
}

//判断文件是否被占用
bool CBusinessImpl::IsUsed(std::string p_strFile)
{
#if defined(WIN32) || defined(WIN64)
	std::string l_strOldFileName, l_strNewFileName;
	boost::filesystem::path l_filePath(p_strFile);
	try
	{
		l_strOldFileName = p_strFile;
		l_strNewFileName = m_pString->ReplaceLast(p_strFile, ".log", ".tmp");
		boost::filesystem::rename(l_filePath, l_strNewFileName);
	}
	catch (boost::filesystem::filesystem_error e)
	{
		std::string l_strErr = e.what();
		return true;
	}

	boost::filesystem::rename(l_strNewFileName, l_strOldFileName);
	return false;	
#else
	char line[1024] = {0};
	FILE *fp;
	std::string l_strCommand = "lsof -F p " + p_strFile;
	
	if ((fp = popen(l_strCommand.data(), "r")) == NULL) 
	{
		return true;
	}
	std::string l_result;
	while (fgets(line, sizeof(line) - 1, fp) != NULL)
	{
		l_result.append(line);
	}
	pclose(fp);

	return !l_result.empty();

#endif
}

bool CBusinessImpl::RenameFile(std::string p_strFile)
{
	std::string l_strOldFileName, l_strNewFileName;
	boost::filesystem::path l_filePath(p_strFile);
	try
	{
		l_strOldFileName = p_strFile;
		l_strNewFileName = m_pString->ReplaceLast(p_strFile, ".log", ".log.lsp");
		boost::filesystem::rename(l_filePath, l_strNewFileName);
	}
	catch (boost::filesystem::filesystem_error e)
	{
		std::string l_strErr = e.what();
		return false;
	}

	return true;
}

void CBusinessImpl::GetLogRecords(std::string p_strFile, std::vector<std::string>& p_vecRecords)
{	
	try
	{
		std::ifstream l_ifsStream(p_strFile.c_str(), std::ios_base::in | std::ios_base::out);
		if (!l_ifsStream.is_open())
		{
			return;
		}

		std::string l_strLine, l_strPreLine;
		std::vector<std::string> l_vecRecords;
		while (getline(l_ifsStream, l_strLine))
		{
			//以"@["开头的是一条新的日志
			if (m_pString->StartsWith(l_strLine, "@["))
			{
				if (m_pString->StartsWith(l_strPreLine, "@["))
				{
					//这是一条完成的日志
					p_vecRecords.push_back(l_strPreLine);
					l_strPreLine = l_strLine;
				}
				else
				{
					if (l_strPreLine.empty())
					{
						l_strPreLine = l_strLine;
					}
				}
			}
			else
			{
				l_strPreLine.append(l_strLine);
			}
		}

		if (!l_strPreLine.empty() && m_pString->StartsWith(l_strPreLine, "@["))
		{
			p_vecRecords.push_back(l_strPreLine);
		}

		l_ifsStream.close();
	}	
	catch (...)
	{
	}

	

	//重命名日志文件，以免重复获取
	RenameFile(p_strFile);
}

unsigned int CBusinessImpl::GetLogRecordsIncrement(std::string p_strFile, unsigned int p_startPos, std::vector<std::string>& p_vecRecords)
{
	std::ifstream l_ifsStream(p_strFile.c_str(), std::ios_base::in | std::ios_base::out);
	if (!l_ifsStream.is_open())
	{
		return 0;
	}

	std::string l_strLine, l_strPreLine;
	std::vector<std::string> l_vecRecords;

	l_ifsStream.seekg(0, std::ios_base::end);

	unsigned int l_iFileSize = l_ifsStream.tellg();

	for (unsigned int i = p_startPos; i < l_iFileSize; i++)
	{
		l_ifsStream.seekg(p_startPos, std::ios_base::beg);

		getline(l_ifsStream, l_strLine);

		//以"@["开头的是一条新的日志
		if (m_pString->StartsWith(l_strLine, "@["))
		{
			if (m_pString->StartsWith(l_strPreLine, "@["))
			{
				//这是一条完成的日志				
				p_vecRecords.push_back(l_strPreLine);
				l_strPreLine = l_strLine;
			}
			else
			{
				if (l_strPreLine.empty())
				{
					l_strPreLine = l_strLine;
				}
			}
		}
		else
		{
			l_strPreLine.append(l_strLine);
		}


		if (l_ifsStream.tellg() > 0)
		{
			p_startPos = l_ifsStream.tellg();
			i = p_startPos;
		}
	}

	if (!l_strPreLine.empty() && m_pString->StartsWith(l_strPreLine, "@[") && l_strPreLine != l_strLine)
	{
		p_vecRecords.push_back(l_strPreLine);
	}

	l_ifsStream.close();

	m_readCachMap[p_strFile] = p_startPos;

	return p_startPos;
}

std::string CBusinessImpl::BuildBody(const std::vector<std::string>& p_logVec)
{
	std::string l_strContent;
	l_strContent.append("[");
	int l_iCount = 0;
	for (std::string l_str : p_logVec)
	{
		++l_iCount;

		std::string l_strlog = m_pString->Right(l_str, l_str.length() - 1);//去掉开头的 @[ 2个字符
		//解析日志内容
		std::vector<std::string> l_splitVec;
		m_pString->Split(l_strlog, "][", l_splitVec, true);

		std::string l_strDateTime, l_strLogLevel, l_strThreadID, l_strModuleName, l_strFuncName, l_strFileLine;
		if (l_splitVec.size() < 11)
		{
			return "";
		}

		l_strDateTime = l_splitVec[1];
		l_strThreadID = l_splitVec[3];
		l_strLogLevel = l_splitVec[5];
		l_strModuleName = l_splitVec[7];
		l_strFuncName = l_splitVec[9];
		l_strFileLine = l_splitVec[11];

		JsonParser::IJsonPtr l_JsonPtr = m_pJsonFty->CreateJson();
		if (!l_JsonPtr)
		{
			return "";
		}

		l_JsonPtr->SetNodeValue("/source", m_strSystem);
		l_JsonPtr->SetNodeValue("/date", l_strDateTime);
		l_JsonPtr->SetNodeValue("/tags/0", l_strModuleName);
		l_JsonPtr->SetNodeValue("/tags/1", l_strFuncName);
		l_JsonPtr->SetNodeValue("/tags/2", l_strFileLine);
		l_JsonPtr->SetNodeValue("/message", l_strlog);
		l_JsonPtr->SetNodeValue("/data/@level", l_strLogLevel);
		l_JsonPtr->SetNodeValue("/data/@environment/thread_id", l_strThreadID);

		l_strContent.append(l_JsonPtr->ToString());
		
		if (l_iCount != p_logVec.size())
		{
			l_strContent.append(",");
		}		
	}

	l_strContent.append("]");
	return l_strContent;
}

void CBusinessImpl::SendLog(const std::vector<std::string>& p_vecRecords)
{
	std::string l_strContent;
	std::vector<std::string> l_LogVecTemp;
	for (std::string l_strlog : p_vecRecords)
	{
		// http 服务使用 utf-8 编码，转码后再发送
		std::string l_strUtfLog = m_pString->AnsiToUtf8(l_strlog);
		l_LogVecTemp.push_back(l_strUtfLog/*l_strlog*/);

		if (l_LogVecTemp.size() % m_iPackageSize == 0)
		{
			l_strContent = BuildBody(l_LogVecTemp);
			l_LogVecTemp.clear();
			if (!l_strContent.empty() && l_strContent.length() < LSP_BUFF_SIZE)
			{
				std::string l_strRespond = m_pHttpClient->Post(m_strLSPUrl, l_strContent);
				if (l_strRespond.empty())
				{
					ICC_LOG_ERROR(m_pLog, "send record failed, Url:[%s], [%d]:[%s]", m_strLSPUrl.c_str(), m_pHttpClient->GetLastErrorCode(), m_pHttpClient->GetLastError().c_str());
				}
			}
		}
	}

	if (!l_LogVecTemp.empty())
	{
		l_strContent = BuildBody(l_LogVecTemp);
		l_LogVecTemp.clear();
		if (!l_strContent.empty() && l_strContent.length() < LSP_BUFF_SIZE)
		{
			std::string l_strRespond = m_pHttpClient->Post(m_strLSPUrl, l_strContent);
			if (l_strRespond.empty())
			{
				ICC_LOG_ERROR(m_pLog, "send record failed, Url:[%s], [%d]:[%s]", m_strLSPUrl.c_str(), m_pHttpClient->GetLastErrorCode(), m_pHttpClient->GetLastError().c_str());
			}
		}
	}
}