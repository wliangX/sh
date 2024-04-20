#include "Boost.h"
#include <fstream>
#include "PriorityLevelDef.h"
#include "UploadFiles.h"
#include <boost/filesystem.hpp>

using namespace std;



TransPackage::TransPackage()
{

}

TransPackage::~TransPackage()
{

}

inline std::string TransPackage::GetBoundString()
{
	if (m_pRES != nullptr)
	{
		if (m_pRES->m_pString != nullptr)
		{
			std::string strGuid = m_pRES->m_pString->CreateGuid();
			return m_pRES->m_pString->ReplaceAll(strGuid, "-", "");
		}
	}
	return "4c98bf1e2c2f4dceaab56873d6502195";
}

void TransPackage::Init(boost::shared_ptr<PluginResource> theRES)
{
	this->m_pRES = theRES;
}


string TransPackage::PostContent(boost::shared_ptr<string> pFileContent, const std::string& fileName, const std::string fileServAddr, const std::string fileServPort)
{
	std::string boundaryStr = GetBoundString();

	string typeStr = "multipart/form-data; boundary=" + boundaryStr;
	std::map<std::string, std::string> mapHeaders;
	mapHeaders.insert( std::make_pair("Content-Type", typeStr) );

	std::string apiTarget = "/file/upload";
	std::string errMsg;

	boost::shared_ptr<string> pPostString = boost::make_shared<string>();

	*pPostString += "--";
	*pPostString += boundaryStr;
	*pPostString += "\r\n";

	*pPostString += "Content-Disposition: form-data; name=\"valid\"\r\n\r\n";

	*pPostString += "1";

	*pPostString += "\r\n--";
	*pPostString += boundaryStr;
	*pPostString += "\r\n";

	*pPostString += "Content-Disposition: form-data; name=\"files\"; filename=";
	*pPostString += fileName;
	*pPostString += "\r\nContent-Type: application/octet-stream\r\n\r\n";

	*pPostString += *pFileContent;

	*pPostString += "\r\n--";
	*pPostString += boundaryStr;
	*pPostString += "\r\n";

	*pPostString += "Content-Disposition: form-data; name=\"appName\"\r\n\r\n";

	*pPostString += "Record";

	*pPostString += "\r\n--";
	*pPostString += boundaryStr;
	*pPostString += "--\r\n";
	
	//string returnString = this->m_pRES->m_pHttpClient->PostEx("192.168.0.169", "28091", apiTarget, mapHeaders, "fihodsahgis", errMsg);

	string returnString = this->m_pRES->m_pHttpClient->PostEx(fileServAddr, fileServPort, apiTarget, mapHeaders, *pPostString, errMsg);

	// need error process codes here:

	return returnString;
}

//初始化大文件分片，获取分片ID
bool TransPackage::InitSlice(std::string& strSliceId, const std::string fileServAddr, const std::string fileServPort)
{
	std::string apiTarget = "/slice/init";
	std::string errMsg;
	std::map<std::string, std::string> mapHeaders;
	std::string strBody("");

	string returnString = this->m_pRES->m_pHttpClient->PostEx(fileServAddr, fileServPort, apiTarget, mapHeaders, strBody, errMsg);

	JsonParser::IJsonPtr pJson = this->m_pRES->m_pJsonFty->CreateJson();
	pJson->LoadJson(returnString);

	std::string codeStr = pJson->GetNodeValue("/code", "7020");
	if (codeStr == "200")
	{
		strSliceId = pJson->GetNodeValue("/data", "");
		return true;
	}
	return false;
}
//上传文件分片
bool TransPackage::UploadSlice(boost::shared_ptr<string> pFileContent, const std::string& strSliceId, int nIndex, const std::string fileServAddr, const std::string fileServPort)
{
	std::string boundaryStr = GetBoundString();

	string typeStr = "multipart/form-data; boundary=" + boundaryStr;
	std::map<std::string, std::string> mapHeaders;
	mapHeaders.insert(std::make_pair("Content-Type", typeStr));

	std::string apiTarget = "/slice/upload/"+ strSliceId +"/"+ std::to_string(nIndex);
	std::string errMsg;

	boost::shared_ptr<string> pPostString = boost::make_shared<string>();

	*pPostString += "--";
	*pPostString += boundaryStr;
	*pPostString += "\r\n";

	*pPostString += "Content-Disposition: form-data; name=\"file\"; filename=";
	*pPostString += std::to_string(nIndex)+".slice";
	*pPostString += "\r\nContent-Type: application/octet-stream\r\n\r\n";

	*pPostString += *pFileContent;

	*pPostString += "\r\n--";
	*pPostString += boundaryStr;
	*pPostString += "\r\n";

	*pPostString += "Content-Disposition: form-data; name=\"index\"\r\n\r\n";

	*pPostString += std::to_string(nIndex);

	*pPostString += "\r\n--";
	*pPostString += boundaryStr;
	*pPostString += "\r\n";

	*pPostString += "Content-Disposition: form-data; name=\"sliceId\"\r\n\r\n";

	*pPostString += strSliceId;

	*pPostString += "\r\n--";
	*pPostString += boundaryStr;
	*pPostString += "--\r\n";

	string returnString = this->m_pRES->m_pHttpClient->PostEx(fileServAddr, fileServPort, apiTarget, mapHeaders, *pPostString, errMsg);

	JsonParser::IJsonPtr pJson = this->m_pRES->m_pJsonFty->CreateJson();
	pJson->LoadJson(returnString);

	std::string codeStr = pJson->GetNodeValue("/code", "7020");
	ICC_LOG_INFO(m_pRES->m_pLog, "Upload Slice:%s Index:%d Result:%s", strSliceId.c_str(), nIndex, codeStr.c_str());
	if (codeStr == "200")
	{
		return true;
	}
	return false;
}
//合并文件分片
string TransPackage::ComposeSlice(const std::string& strFileName, const std::string& strSliceId, unsigned long long llFileSize, int nSliceCount, const std::string fileServAddr, const std::string fileServPort)
{
	std::string boundaryStr = GetBoundString();

	string typeStr = "multipart/form-data; boundary=" + boundaryStr;
	std::map<std::string, std::string> mapHeaders;
	mapHeaders.insert(std::make_pair("Content-Type", typeStr));

	std::string apiTarget = "/slice/compose";
	std::string errMsg;

	boost::shared_ptr<string> pPostString = boost::make_shared<string>();

	*pPostString += "--";
	*pPostString += boundaryStr;
	*pPostString += "\r\n";

	*pPostString += "Content-Disposition: form-data; name=\"fileName\"\r\n\r\n";

	*pPostString += strFileName;

	*pPostString += "\r\n--";
	*pPostString += boundaryStr;
	*pPostString += "\r\n";

	*pPostString += "Content-Disposition: form-data; name=\"fileSize\"\r\n\r\n";

	*pPostString += std::to_string(llFileSize);

	*pPostString += "\r\n--";
	*pPostString += boundaryStr;
	*pPostString += "\r\n";

	*pPostString += "Content-Disposition: form-data; name=\"sliceId\"\r\n\r\n";

	*pPostString += strSliceId;

	*pPostString += "\r\n--";
	*pPostString += boundaryStr;
	*pPostString += "\r\n";

	*pPostString += "Content-Disposition: form-data; name=\"sliceNum\"\r\n\r\n";

	*pPostString += std::to_string(nSliceCount);

	*pPostString += "\r\n--";
	*pPostString += boundaryStr;
	*pPostString += "\r\n";

	*pPostString += "Content-Disposition: form-data; name=\"valid\"\r\n\r\n";

	*pPostString += "1";

	*pPostString += "\r\n--";
	*pPostString += boundaryStr;
	*pPostString += "\r\n";

	*pPostString += "Content-Disposition: form-data; name=\"appName\"\r\n\r\n";

	*pPostString += "Record";

	*pPostString += "\r\n--";
	*pPostString += boundaryStr;
	*pPostString += "\r\n";

	*pPostString += "Content-Disposition: form-data; name=\"bucketId\"\r\n\r\n";

	*pPostString += "";

	*pPostString += "\r\n--";
	*pPostString += boundaryStr;
	*pPostString += "--\r\n";

	string returnString = this->m_pRES->m_pHttpClient->PostEx(fileServAddr, fileServPort, apiTarget, mapHeaders, *pPostString, errMsg);

	JsonParser::IJsonPtr pJson = this->m_pRES->m_pJsonFty->CreateJson();
	pJson->LoadJson(returnString);

	std::string codeStr = pJson->GetNodeValue("/code", "7020");
	ICC_LOG_INFO(m_pRES->m_pLog, "Compose Slice:%s fileName:%s fileSize:%llu SliceCount:%d Result:%s", strSliceId.c_str(), strFileName.c_str(), llFileSize, nSliceCount, codeStr.c_str());
	if (codeStr == "200")
	{
		std::string fileId = pJson->GetNodeValue("/data/id","");
		return fileId;
	}
	return "";
}

#if 0

string TransPackage::UploadFile(const std::string& filePath, const std::string fileServAddr, const std::string fileServPort)
{
	std::ifstream iFile(filePath.c_str(), std::ifstream::binary);

	if (!iFile)
	{
		return "ERROR:Can't Open File";
	}

	string theFileName;
	int lastPos = filePath.rfind("\\");
	if (lastPos == string::npos)
	{
		lastPos = filePath.rfind("/");
	}

	if (lastPos != string::npos)
	{
		theFileName = filePath.substr(lastPos);
	}
	else
	{
		theFileName = filePath;
	}

	boost::shared_ptr<string> pFileString = boost::make_shared<string>();

	char readBuf[1024];
	while(iFile.read(readBuf, sizeof(readBuf)))
	{
		pFileString->append(readBuf, iFile.gcount());
	}

	return this->UploadFileContent(pFileString, theFileName, fileServAddr, fileServPort);
}


bool TransPackage::UploadFile(const std::string& localPath)
{
	string serverURL = "http://";
	//serverURL += this->fileServiceAddress;
	serverURL += "192.168.0.169";
	serverURL += ":";
	//serverURL += this->fileServicePort;
	serverURL += "28091";
	serverURL += "/file/upload";

	JsonParser::IJsonPtr pJson = this->m_pRES->m_pJsonFty->CreateJson();


	pJson->SetNodeValue("/appName", "RecordServer");
	pJson->SetNodeValue("/byt", "ABCD");
	pJson->SetNodeValue("/fileName", "ChaosTest.txt");
	pJson->SetNodeValue("/fileSize", "4");

	string bodyStr = "[";
	bodyStr += pJson->ToString();
	bodyStr += "]";

	std::map<std::string, std::string> mapHeaders;
	mapHeaders.insert(std::make_pair("Content-Type", "multipart/form-data; boundary=------ijoighsiofhisa83294813ifgodshi571562175962628"));

	std::string target = "/file/upload";
	std::string err;

	string returnString = this->m_pRES->m_pHttpClient->PostEx("192.168.0.169", "28091", target, mapHeaders, "fihodsahgis", err);

	return true;
}
#endif


UploadManager::UploadManager():m_strFileServerName("commandcenter-file-service")
{

}

UploadManager::~UploadManager()
{

}

void UploadManager::Init(boost::shared_ptr<PluginResource> theRES)
{
	this->m_pRES = theRES;

	this->m_UP.Init(theRES);
	this->m_ADDR.Init(theRES);

	string atptCntStr = m_pRES->m_pConfig->GetValue("ICC/Plugin/Record/UploadAttemptCount", "");
	m_strFileServerName = m_pRES->m_pConfig->GetValue("ICC/Plugin/Record/FileServiceName","commandcenter-file-service");
	this->attemptCount = atoi( atptCntStr.c_str() );

	if (this->attemptCount < 1)
		this->attemptCount = 1;
}

bool UploadManager::RefreshHealthyFileService()
{
	string getFileAddress;
	string getFilePort;
	bool bRet = this->m_ADDR.GetHealthyServiceFromNacos(m_strFileServerName, getFileAddress, getFilePort);

	this->fileServiceMutex.lock();
	if (bRet)
	{
		this->fileServiceAddress = getFileAddress;
		this->fileServicePort = getFilePort;
	}
	if (!bRet)
	{// can't get healthy address
		this->fileServiceAddress.clear();
		this->fileServicePort.clear();
	}
	this->fileServiceMutex.unlock();

	ICC_LOG_INFO(m_pRES->m_pLog, "Refresh file URL: [%s : %s]", getFileAddress.c_str(), getFilePort.c_str());

	return bRet;
}

bool UploadManager::GetFileServiceAddress(string& address, string& port)
{
	this->fileServiceMutex.lock();

	address = this->fileServiceAddress;
	port = this->fileServicePort;

	this->fileServiceMutex.unlock();

	return (address.length() >= 7 && port.length() >= 3);

}

string UploadManager::UploadFile(const std::string& filePath, string& fileID)
{
	this->fileServiceMutex.lock();
	bool bEmpty = (this->fileServiceAddress.empty() || this->fileServicePort.empty());
	this->fileServiceMutex.unlock();

	if (bEmpty) // may call at first
	{
		if (!RefreshHealthyFileService())
		{
			return "7000";
		}
	}
	if (!boost::filesystem::exists(boost::filesystem::path(filePath))) //文件不存在时记录一条日志，不保存文件记录
	{
		ICC_LOG_DEBUG(m_pRES->m_pLog, "Upload file [%s] is not exists", filePath.c_str());
		return "7010";
	}
	unsigned long long ullFileLen = 0;
	ullFileLen = boost::filesystem::file_size(filePath.c_str());
	ICC_LOG_DEBUG(m_pRES->m_pLog, "Upload file Begin:[%s] filesize[%llu]", filePath.c_str(), ullFileLen);
	std::ifstream iFile(filePath.c_str(), std::ifstream::binary);
	if (!iFile)
	{
		ICC_LOG_ERROR(m_pRES->m_pLog, "Open file [%s] error", filePath.c_str());
		return "7050";
	}

	string theFileName;
	int lastPos = filePath.rfind("\\");
	if (lastPos == string::npos)
	{
		lastPos = filePath.rfind("/");
	}

	if (lastPos != string::npos)
	{
		theFileName = filePath.substr(lastPos+1);
	}
	else
	{
		theFileName = filePath;
	}

	std::string strSliceId("");
	unsigned int nSliceIndex = 0;
	this->fileServiceMutex.lock();
	string theServAddress = this->fileServiceAddress;
	string theServPort = this->fileServicePort;
	this->fileServiceMutex.unlock();
	
	unsigned long long ullMaxFileSize = 50;
	try
	{
		std::string strMaxFileSize = m_pRES->m_pConfig->GetValue("ICC/Plugin/Record/UpdateFileMaxSize", "50");
		ullMaxFileSize	= std::stoull(strMaxFileSize);
	}
	catch (std::exception ex)
	{
		ullMaxFileSize = 50;
	}
	
	if (ullFileLen > ullMaxFileSize * 1024 * 1024) //大文件分片处理
	{
		this->m_UP.InitSlice(strSliceId, theServAddress, theServPort);
		ICC_LOG_DEBUG(m_pRES->m_pLog, "file [%s] SliceId[%s]", filePath.c_str(), strSliceId.c_str());
	}

	boost::shared_ptr<string> pFileString = boost::make_shared<string>();

	char readBuf[1024];
	unsigned int nReadBuffCnt = 0;
	do
	{
		iFile.read(readBuf, sizeof(readBuf));

		if( iFile.gcount() > 0 )
		{
			pFileString->append(readBuf, iFile.gcount());
		}
		if (!strSliceId.empty())
		{
			if (++nReadBuffCnt > ullMaxFileSize * 1024)
			{
				this->m_UP.UploadSlice(pFileString, strSliceId, nSliceIndex++, theServAddress, theServPort);
				nReadBuffCnt = 0;
				pFileString = boost::make_shared<string>();
			}
		}
	} while (!iFile.eof());

	iFile.close();

	if (!strSliceId.empty())
	{
		if (!pFileString->empty())
		{
			this->m_UP.UploadSlice(pFileString, strSliceId, nSliceIndex++, theServAddress, theServPort);
			nReadBuffCnt = 0;
		}
		fileID = this->m_UP.ComposeSlice(theFileName, strSliceId, ullFileLen, nSliceIndex, theServAddress, theServPort);
		if (fileID.empty())
		{
			return "7020";
		}
		return "0";
	}

	return this->UploadFileContent(pFileString, theFileName, fileID);
}

string UploadManager::UploadFileContent(boost::shared_ptr<string> pFileContent, const std::string& fileName, string& fileID)
{
	int attempCnt = this->attemptCount;
	string codeStr;
	for( ; attempCnt > 0; attempCnt-- )
	{
		this->fileServiceMutex.lock();
		string theServAddress = this->fileServiceAddress;
		string theServPort = this->fileServicePort;
		this->fileServiceMutex.unlock();

		string responseStr = this->m_UP.PostContent(pFileContent, fileName, theServAddress, theServPort);

		// parse return JSON
		JsonParser::IJsonPtr pJson = this->m_pRES->m_pJsonFty->CreateJson();
		pJson->LoadJson(responseStr);

		codeStr = pJson->GetNodeValue("/code", "7020");
		if (codeStr == "200")
		{
			fileID = pJson->GetNodeValue("/data/0/id", "");

			if (fileID.length() < 2)
			{
				codeStr = "7020";
			}
		}

		ICC_LOG_DEBUG(m_pRES->m_pLog, "Upload file to [%s:%s] Return code:[%s], try Count left:[%d]", theServAddress.c_str(), theServPort.c_str(), codeStr.c_str(), attempCnt);

		if (codeStr == "200")
		{// upload and return content all valid, break out from attempt crcle
			break;
		}

		// not success, get new file service address and try again
		if (!RefreshHealthyFileService())
		{
			return "7000";
		}
	}

	return codeStr;
}


boost::shared_ptr<UploadManager> GetUploadManagerGlobalInstance()
{
	static boost::shared_ptr<UploadManager> S_pUM_Inst = NULL;
	if (!S_pUM_Inst)
	{
		S_pUM_Inst = boost::make_shared<UploadManager>();
	}
	return S_pUM_Inst;
}

