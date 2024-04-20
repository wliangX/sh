#pragma once
#include <Boost.h>
#include <RecordFileManage.h>

namespace ICC
{
    class FtpDownLoadManage
    {
    public:
        FtpDownLoadManage(Log::ILogPtr pLog);
        ~FtpDownLoadManage();
        bool ConnectFtpServer(Ftp::IFtpClientPtr p_pFtpClient, const std::string& pstrFSFtpIp, 
            const std::string& pstrFSFtpPort, const std::string& pstrFSFtpUser, const std::string& pstrFSFtpPwd, const std::string& pstrDownloadDir);
        void SetDeleteFileMode(bool bIsDelete);
        void NotifyFileCompleted(bool isUpLoad, const std::string& pstrFtpIp, const std::string& fileName, int nResult);
        void DeleteFtpFile(const std::string& pstrFtpIp, const std::string& pstrFileName);
        void AddDownloadFile(const std::string& pstrFSFtpIp, const std::string& pstrFileName, CRecordFilePtr pRecordFilePtr);
    private:

        void AddRecordFileMap(const std::string& pstrFileName, CRecordFilePtr pRecordFilePtr)
        {
            lock_guard<mutex> autolock(lkRecordFile);
            m_mapRecordFile[pstrFileName] = pRecordFilePtr;
        }
        CRecordFilePtr GetRecordFileMap(const std::string& pstrFileName)
        {
            lock_guard<mutex> autolock(lkRecordFile);
            if (m_mapRecordFile.find(pstrFileName) != m_mapRecordFile.end())
            {
                return m_mapRecordFile[pstrFileName];
            }
            return CRecordFilePtr((CRecordFile*)NULL);
        }
        void RmvRecordFileMap(const std::string& pstrFileName)
        {
            lock_guard<mutex> autolock(lkRecordFile);
            m_mapRecordFile.erase(pstrFileName);
        }
        size_t RecordFileMapSize()
        {
            lock_guard<mutex> autolock(lkRecordFile);
            return m_mapRecordFile.size();
        }
        void ClearRecordFileMap()
        {
            lock_guard<mutex> autolock(lkRecordFile);
            m_mapRecordFile.clear();
        }
        void AddFtpClient(const std::string& pstrFtpIP, Ftp::IFtpClientPtr pRecordFilePtr)
        {
            lock_guard<mutex> autolock(lkFtpClient);
            m_mapFtpClient[pstrFtpIP] = pRecordFilePtr;
        }
        Ftp::IFtpClientPtr GetFtpClient(const std::string& pstrFtpIP)
        {
            lock_guard<mutex> autolock(lkFtpClient);
            if (m_mapFtpClient.find(pstrFtpIP) != m_mapFtpClient.end())
            {
                return m_mapFtpClient[pstrFtpIP];
            }
            return nullptr;
        }
        Ftp::IFtpClientPtr GetFtpClient(int nIndex, std::string& pstrFtpIP)
        {
            lock_guard<mutex> autolock(lkFtpClient);
            std::map<std::string, Ftp::IFtpClientPtr>::iterator it = m_mapFtpClient.begin();
            int nIndexTmp = 0;
            while (it != m_mapFtpClient.end())
            {
                if (nIndexTmp == nIndex)
                {
                    pstrFtpIP = it->first;
                    return it->second;
                }
                nIndexTmp++;
                it++;
            }
            return nullptr;
        }
        void RmvFtpClient(const std::string& pstrFtpIP)
        {
            lock_guard<mutex> autolock(lkFtpClient);
            m_mapFtpClient.erase(pstrFtpIP);
        }
        void ClearFtpClient()
        {
            lock_guard<mutex> autolock(lkFtpClient);
            m_mapFtpClient.clear();
        }

        Log::ILogPtr       m_pLog;
        std::map<std::string, Ftp::IFtpClientPtr> m_mapFtpClient;
        mutex lkFtpClient;
        std::map<std::string, CRecordFilePtr> m_mapRecordFile;
        mutex lkRecordFile;
        bool m_bIsDeleteRemoteFile;
    };
    typedef boost::shared_ptr<FtpDownLoadManage>  FtpDownLoadManagePtr;
};
