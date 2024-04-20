#include <FSFtpFileDownLoad.h>
#include <boost/filesystem.hpp>

#include <boost/bind/bind.hpp>
using namespace boost::placeholders;
namespace ICC
{
    FtpDownLoadManage::FtpDownLoadManage( Log::ILogPtr p_pLog):m_pLog(p_pLog), m_bIsDeleteRemoteFile(true)
    {
        
    }
    FtpDownLoadManage::~FtpDownLoadManage()
    {
        ClearFtpClient();
        ClearRecordFileMap();
    }
    bool FtpDownLoadManage::ConnectFtpServer(Ftp::IFtpClientPtr p_pFtpClient, const std::string& pstrFSFtpIp, const std::string& pstrFSFtpPort, const std::string& pstrFSFtpUser, const std::string& pstrFSFtpPwd, const std::string& pstrDownloadDir)
    {
        if (NULL == p_pFtpClient)
        {
            ICC_LOG_DEBUG(m_pLog, "m_pFtpClient is null.");
            return false;
        }
        Ftp::IFtpClient::NotifyFileCompletedFunc func = boost::bind(&FtpDownLoadManage::NotifyFileCompleted, this, _1, _2, _3, _4);
        p_pFtpClient->SetNotifyCompleteFunc(func);
        p_pFtpClient->SetDownLoadTmpDir(pstrDownloadDir);
        bool bConnRt = p_pFtpClient->Connect(pstrFSFtpIp,pstrFSFtpPort,pstrFSFtpUser,pstrFSFtpPwd);
        if (!bConnRt)
        {
            ICC_LOG_DEBUG(m_pLog, "connect Ftp(%s:%s %s %s) failed.", pstrFSFtpIp.c_str(), pstrFSFtpPort.c_str(), pstrFSFtpUser.c_str(), pstrFSFtpPwd.c_str());
            return false;
        }
        AddFtpClient(pstrFSFtpIp, p_pFtpClient);
    }
    void FtpDownLoadManage::SetDeleteFileMode(bool bIsDelete)
    {
        m_bIsDeleteRemoteFile = bIsDelete;
    }
    void FtpDownLoadManage::NotifyFileCompleted(bool isUpLoad, const std::string& pstrFtpIp, const std::string& fileName, int nResult)
    {
        ICC_LOG_INFO(m_pLog, "isUpLoad:%d, FtpIp:%s,fileName:%s,nResult:%d", isUpLoad, pstrFtpIp.c_str(), fileName.c_str(), nResult);
        if (isUpLoad)//只处理下载消息，暂不处理上传消息
        {
            return;
        }
        std::string strFileName = pstrFtpIp + fileName;
        CRecordFilePtr pRecordFile = GetRecordFileMap(strFileName);
        if (NULL != pRecordFile)
        {
            if (nResult == 0)
            {
                if (pRecordFile->m_strFSFtpIp.empty())
                {
                    pRecordFile->m_strFSFtpIp = pstrFtpIp;
                }
                if (!boost::filesystem::exists(boost::filesystem::path(pRecordFile->m_strLocalFileName))) //文件不存在时记录一条日志，不保存文件记录
                {
                    ICC_LOG_WARNING(m_pLog, "freeswitch record file %s is not exist of callrefid:%s paraCallerId:%s paraCalledId:%s", pRecordFile->m_strLocalFileName.c_str(),
                        pRecordFile->GetCallRefID().c_str(), pRecordFile->m_strCaller.c_str(), pRecordFile->m_strCalled.c_str());
                }
                else
                {
                    CRecordFileManage::GetInstance().AddARecordFile(pRecordFile);
                }
            }
            RmvRecordFileMap(strFileName);
            ICC_LOG_DEBUG(m_pLog, "download file map size(%u).", RecordFileMapSize());
        }

    }
    void FtpDownLoadManage::DeleteFtpFile(const std::string& pstrFtpIp, const std::string& fileName)
    {
        if (!m_bIsDeleteRemoteFile)
        {
            ICC_LOG_DEBUG(m_pLog, "delete remote ftp file not set (%s:%s).", pstrFtpIp.c_str(), fileName.c_str());
            return;
        }
        Ftp::IFtpClientPtr pFtpClient = GetFtpClient(pstrFtpIp);
        if (pFtpClient != nullptr)
        {
            pFtpClient->DeleteRemoteFile(fileName);
        }
    }
    void FtpDownLoadManage::AddDownloadFile(const std::string& pstrFSFtpIp, const std::string& pstrFileName, CRecordFilePtr pRecordFilePtr)
    {
        if (pstrFSFtpIp.empty())
        {
            bool bFindFsFtp = false;
            for (int iIndex = 0;iIndex < 2;iIndex++)
            {
                std::string strFSFtpIp("");
                Ftp::IFtpClientPtr pFtpClient = GetFtpClient(iIndex, strFSFtpIp);
                if (nullptr != pFtpClient)
                {
                    AddRecordFileMap(strFSFtpIp + pstrFileName, pRecordFilePtr);
                    bool bPrior = pRecordFilePtr->m_bIsHistory ? false : true;
                    pFtpClient->DownLoadFile(pstrFileName, bPrior);
                    bFindFsFtp = true;
                }
            }
            if (!bFindFsFtp)
            {
                ICC_LOG_WARNING(m_pLog, "can not find Ftp Object of ip:%s file:%s", pstrFSFtpIp.c_str(), pstrFileName.c_str());
                return;
            }
        }
        else
        {
            Ftp::IFtpClientPtr pFtpClient = GetFtpClient(pstrFSFtpIp);
            if (nullptr == pFtpClient)
            {
                ICC_LOG_WARNING(m_pLog, "can not find Ftp Object of ip:%s file:%s", pstrFSFtpIp.c_str(), pstrFileName.c_str());
                return;
            }
            AddRecordFileMap(pstrFSFtpIp + pstrFileName, pRecordFilePtr);
            bool bPrior = pRecordFilePtr->m_bIsHistory ? false : true;
            pFtpClient->DownLoadFile(pstrFileName, bPrior);
        }

    }
}
