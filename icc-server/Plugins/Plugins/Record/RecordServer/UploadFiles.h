#pragma once

#include "Boost.h"
#include <mutex>

#include "PluginResource.h"

#include "HttpAddress.h"

using namespace std;

namespace ICC
{
	class LargeFile
	{

	};

	class TransPackage
	{
	public:
		void Init(boost::shared_ptr<PluginResource> theRES);

		//string UploadFile(const std::string& filePath, const std::string fileServAddr, const std::string fileServPort);
		string PostContent(boost::shared_ptr<string> pFileContent, const std::string& fileName, const std::string fileServAddr, const std::string fileServPort);

		//初始化大文件分片，获取分片ID
		bool InitSlice(std::string& strSliceId, const std::string fileServAddr, const std::string fileServPort);
		//上传文件分片
		bool UploadSlice(boost::shared_ptr<string> pFileContent, const std::string& strSliceId, int nIndex, const std::string fileServAddr, const std::string fileServPort);
		//合并文件分片
		string ComposeSlice(const std::string& strFileName, const std::string& strSliceId, unsigned long long llFileSize, int nSliceCount, const std::string fileServAddr, const std::string fileServPort);

	public:
		TransPackage();
		virtual ~TransPackage();

	protected:
		std::string GetBoundString();
		boost::shared_ptr<PluginResource> m_pRES;

	};


	class UploadManager
	{
	public:
		void Init(boost::shared_ptr<PluginResource> theRES);

		bool RefreshHealthyFileService();

		bool GetFileServiceAddress(string& address, string& port);

		// return code:
		// 200: success
		// 201: Created
		// 401: Unauthorized
		// 403: Forbidden
		// 404: Not Found
		// 7000: can't get healthy fileservice
		// 7010：file does not exists
		// 7020: return JSON parse failed
		// 7050: open file error
		string UploadFile(const std::string& filePath, string& fileID);

		string UploadFileContent(boost::shared_ptr<string> pFileContent, const std::string& fileName, string& fileID);

	public:
		UploadManager();
		virtual ~UploadManager();


	protected:
		std::recursive_mutex fileServiceMutex;
		std::string fileServiceAddress;
		std::string fileServicePort;

		int			attemptCount;


	protected:
		boost::shared_ptr<PluginResource> m_pRES;

		TransPackage m_UP;
		AddressInteraction m_ADDR;
		std::string m_strFileServerName;

	};

}

extern boost::shared_ptr<UploadManager> GetUploadManagerGlobalInstance();
