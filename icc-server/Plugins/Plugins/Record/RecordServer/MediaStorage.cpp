#include <MediaStorage.h>
#include <fstream>
#include <string.h>
#include <RTPPackProc.h>
#include <RecordFileManage.h>
#include <AgentIPMap.h>

#define PACKETTIME_DELTA 3 //允许语音包间隔的时间差值(毫秒)

static unsigned char silence_data_g711a[512] = 
{ 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55,
  0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55,
  0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55,
  0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55,
  0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55,
  0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55,
  0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55,
  0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55,
  0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55,
  0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55,
  0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55,
  0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55,
  0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55,
  0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55,
  0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55,
  0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55, 0xd5, 0x55
  };


CWaveFile::CWaveFile(uint16_t nb_channel, uint32_t sample_rate, uint16_t sample_bits, uint16_t voice_fmt):m_strFileName(""), m_header(nb_channel, sample_rate, sample_bits, voice_fmt)
{
}
void CWaveFile::SetFileName(const std::string& strFileName)
{
    m_strFileName= strFileName;
}
int CWaveFile::WriteHeader()
{
    std::ofstream rtpFile;
    rtpFile.open(m_strFileName.c_str(), std::ios::in | std::ios::out | std::ios::binary);
    if (rtpFile.is_open())
    {
        const uint32_t sizeHeader = sizeof(FOURCC) + sizeof(Base_chunk) + sizeof(Wave_format) + sizeof(Base_chunk);
        m_header.riff->cb_size = m_header.data->cb_size + sizeHeader;
        rtpFile.seekp(0, std::ios_base::beg);
        rtpFile.write((const char*)m_header.riff.get(), sizeof(Base_chunk));
        rtpFile.seekp(sizeHeader, std::ios_base::beg);
        rtpFile.write((const char*)m_header.data.get(), sizeof(Base_chunk));
        rtpFile.close();
        return 0;
    }
    return -1;
}
int CWaveFile::Write(const char* pPCMData, uint32_t usPCMLen)
{
    std::ofstream rtpFile;
    rtpFile.open(m_strFileName.c_str(), std::ios::app | std::ios::out | std::ios::binary);
    if (rtpFile.is_open())
    {
        rtpFile.seekp(0, std::ios_base::end);
        streampos filelength = rtpFile.tellp();
        const uint32_t sizeAllHeader = sizeof(Base_chunk) + sizeof(FOURCC) + sizeof(Base_chunk) + sizeof(Wave_format) + sizeof(Base_chunk);
        if (filelength < sizeAllHeader)//空文件写文件头
        {
            rtpFile.seekp(0, std::ios_base::beg);
            rtpFile.write((const char*)m_header.riff.get(), sizeof(Base_chunk));
            rtpFile.write((const char*)&m_header.wave_fcc, sizeof(FOURCC));
            rtpFile.write((const char*)m_header.fmt.get(), sizeof(Base_chunk));
            rtpFile.write((const char*)m_header.fmt_data.get(), sizeof(Wave_format));
            rtpFile.write((const char*)m_header.data.get(), sizeof(Base_chunk));
        }
        rtpFile.write(pPCMData, usPCMLen);//写入数据

        m_header.data->cb_size += usPCMLen;
        rtpFile.close();
        return 0;
    }
    return -1;
}
CWaveFile::~CWaveFile()
{
    ;
}

namespace ICC
{

    CMediaStorage::CMediaStorage(Log::ILogPtr pLog, const std::string& strAgentIP, uint16_t usAgentPort, uint16_t voice_interval/* = DefaultVoiceInterval*/,
        uint32_t sample_rate/* = DefaultSampleRate*/, uint16_t code_type /*= FORMAT_G711A*/, uint16_t sample_bits/* = bitsPerSampleG711A*/,
         uint16_t nb_channel/* = DefaultFileChannels*/) : m_pLog(pLog), m_isRunning(false), m_usAgentCallPort(usAgentPort), m_strAgentIP(strAgentIP),
        m_strAgentID(""), m_strFileID(""), m_strCaller(""), m_strCalled(""), m_bIsBindCTICall(false), m_startTime(0, 0), m_stopTime(0, 0), m_rtpinterval(voice_interval),
        m_packet_len(voice_interval* sample_rate* (sample_bits / bitsofbyte) / 1000),
        m_waveFile(nb_channel, sample_rate, sample_bits, code_type)
    {
        m_setCallID.clear();
    }
    CMediaStorage::~CMediaStorage()
    {
        ;
    }
    void CMediaStorage::PushInRTPRawPacket(RTPRawPacket* data)
    {
        if (NULL == data)
        {
            return;
        }
        {
            lock_guard<mutex> unique(m_mtInRaw);
            if (m_inraw.empty() && m_startTime.IsInValidTime())
            {
                m_startTime = data->GetReceiveTime();
            }
            m_inraw.push_back(data);
        }
        
    }
    void CMediaStorage::PushOutRTPRawPacket(RTPRawPacket* data)
    {
        if (NULL == data)
        {
            return;
        }
        {
            lock_guard<mutex> unique(m_mtOutRaw);
            if (m_outraw.empty() && m_startTime.IsInValidTime())
            {
                m_startTime = data->GetReceiveTime();
            }
            m_outraw.push_back(data);
        }
    }

    int CMediaStorage::StartProcStorage()
    {
        if (TrySetRunState(true))
        {
            auto func = [this]() {this->ProcRTPPacket(); };
            ICC_LOG_INFO(m_pLog, "CMediaStorage StartProcStorage %s %d.", m_strAgentIP.c_str(), m_usAgentCallPort);
            try
            {
                m_thread = std::thread(func);
                if (m_thread.joinable())
                {
                    m_thread.join();
                }
            }
            catch (std::exception& ex)
            {
                TrySetRunState(false);
                ICC_LOG_ERROR(m_pLog, "exception : %s", ex.what());
                return -1;
            };
            ICC_LOG_INFO(m_pLog, "CMediaStorage StartProcStorage end  %s %d.", m_strAgentIP.c_str(), m_usAgentCallPort);
            TrySetRunState(false);
        }
        return 0;
    }

    int CMediaStorage::BindCTICallInfo(const CallInfo& callInfo)
    {
        ICC_LOG_INFO(m_pLog, "CMediaStorage BindCTICallInfo agentip %s agentport %d agentid %s  callid %s IsBindCTICall %d .",
            m_strAgentIP.c_str(), m_usAgentCallPort, callInfo.m_strAgentID.c_str(), callInfo.m_strCallID.c_str(), m_bIsBindCTICall);
        if (m_bIsBindCTICall)
        {
            if (m_strAgentID.empty())
            {
                m_strAgentID = callInfo.m_strAgentID;
            }
            if (m_strAgentID == callInfo.m_strAgentID)
            {
                m_setCallID.insert(callInfo.m_strCallID);
            }
            if (m_strCaller.empty())
            {
                m_strCaller = callInfo.m_strCaller;
            }
            if (m_strCalled.empty())
            {
                m_strCalled = callInfo.m_strCalled;
            }
        }
        else
        {
            m_strAgentID = callInfo.m_strAgentID;
            m_setCallID.insert(callInfo.m_strCallID);
            m_strCaller = callInfo.m_strCaller;
            m_strCalled = callInfo.m_strCalled;
            m_bIsBindCTICall = true;
        }
        ICC_LOG_INFO(m_pLog, "after BindCTICallInfo agentip %s agentport %d agentid %s  callid %s .",
            m_strAgentIP.c_str(), m_usAgentCallPort, m_strAgentID.c_str(), GetCallIDSetString().c_str(), m_bIsBindCTICall);
        return 0;
    }

    int CMediaStorage::ClearCallInfo()
    {
        m_strAgentID.clear();
        m_setCallID.clear();
        m_strCaller.clear();
        m_strCalled.clear();
        m_bIsBindCTICall = false;
        return 0;
    }

    bool CMediaStorage::IsMatchStartTime(uint64_t callStartTime)
    {
        int64_t tSpan = callStartTime - m_startTime.GetSeconds();
        //匹配开始时间在1S之内的呼叫
        if ((tSpan == 0) || (tSpan == 1) || (tSpan == -1))
        {
            return true;
        }
        return false;
    }

    RTPRawPacket* CMediaStorage::PopInRtpRawPacket(RTPTime& pkgTime)
    {
        lock_guard<mutex> unique(m_mtInRaw);
        if (m_inraw.empty())
        {
            return NULL;
        }
        RTPRawPacket* rtpdata = m_inraw.front();
        if (rtpdata->GetReceiveTime() > (pkgTime + (unsigned short)PACKETTIME_DELTA))
        {
            return NULL;
        }
        m_inraw.pop_front();
        return rtpdata;
    }
    RTPRawPacket* CMediaStorage::PopOutRtpRawPacket(RTPTime& pkgTime)
    {
        lock_guard<mutex> unique(m_mtOutRaw);
        if (m_outraw.empty())
        {
            return NULL;
        }
        RTPRawPacket* rtpdata = m_outraw.front();
        if (rtpdata->GetReceiveTime() > (pkgTime + (unsigned short)PACKETTIME_DELTA))
        {
            return NULL;
        }
        m_outraw.pop_front();
        return rtpdata;
    }

    RTPTime CMediaStorage::GetLastRtpTime()
    {
        RTPTime rtpInStopTime(0, 0);
        GetRtpInLastTime(rtpInStopTime);
        RTPTime rtpOutStopTime(0, 0);
        GetRtpOutLastTime(rtpOutStopTime);
        if (rtpInStopTime > rtpOutStopTime)
        {
            return rtpInStopTime;
        }
        return rtpOutStopTime;
    }

    void CMediaStorage::GetRtpInLastTime(RTPTime& rtpStopTime)
    {
        lock_guard<mutex> unique(m_mtInRaw);
        if (!m_inraw.empty())
        {
            RTPRawPacket* rtpdata = m_inraw.back();
            if (NULL != rtpdata)
            {
                rtpStopTime = rtpdata->GetReceiveTime();
            }
        }
    }

    void CMediaStorage::GetRtpOutLastTime(RTPTime& rtpStopTime)
    {
        lock_guard<mutex> unique(m_mtOutRaw);
        if (!m_outraw.empty())
        {
            RTPRawPacket* rtpdata = m_outraw.back();
            if (NULL != rtpdata)
            {
                rtpStopTime = rtpdata->GetReceiveTime();
            }
        }
    }

    void CMediaStorage::ProcRTPPacket()
    {
        m_stopTime = GetLastRtpTime();
        std::string strLocalFilePath = CRecordFileManage::GetInstance().GetLocalFilePath() + "/" + m_strAgentIP;//获取默认存放文件路径

        if (!IsFileExits(strLocalFilePath))
        {
            CreateFolder(strLocalFilePath);
        }

        std::string strWaveFileName = strLocalFilePath + "/" + std::to_string(m_usAgentCallPort) + "_" + m_startTime.ToString() + "_" + m_stopTime.ToString() + "_cap.wav";
        m_waveFile.SetFileName(strWaveFileName);
        RTPTime packetTime = m_startTime;
        RTPRawPacket* pInRawPacket = PopInRtpRawPacket(packetTime);
        RTPRawPacket* pOutRawPacket = PopOutRtpRawPacket(packetTime);
        while (NULL != pInRawPacket || NULL != pOutRawPacket)
        {
            RTPPacket* pRtpInPacket = NULL;
            RTPPacket* pRtpOutPacket = NULL;

            //转换进来的包
            if (NULL != pInRawPacket)
            {
                pRtpInPacket = new RTPPacket(*pInRawPacket);
                if (NULL != pRtpInPacket)
                {
                    if (pRtpInPacket->GetCreationError() < 0)
                    {
                        delete pRtpInPacket;
                        pRtpInPacket = NULL;
                    }
                    //else//写收到包文件
                    //{
                    //    std::string strFileName = m_strAgentIP + "_" + std::to_string(m_usAgentCallPort) + "_" + m_startTime.ToString()+"_"+m_stopTime.ToString() + "_cap_in.pcm";
                    //    WriteRtpPacket(strFileName, pRtpInPacket, NULL);
                    //}
                }
            }

            //转换发出的包
            if (NULL != pOutRawPacket)
            {
                pRtpOutPacket = new RTPPacket(*pOutRawPacket);
                if (NULL != pRtpOutPacket)
                {
                    if (pRtpOutPacket->GetCreationError() < 0)
                    {
                        delete pRtpOutPacket;
                        pRtpOutPacket = NULL;
                    }
                    //else//写发出包文件
                    //{
                    //    std::string strFileName = m_strAgentIP + "_" + std::to_string(m_usAgentCallPort) + "_" + m_startTime.ToString() + "_" + m_stopTime.ToString() + "_cap_out.pcm";
                    //    WriteRtpPacket(strFileName, pRtpOutPacket, NULL);
                    //}
                }
            }

            //写双声道文件
            //std::string strFileName = strLocalFilePath + "_" + std::to_string(m_usAgentCallPort) + "_" + m_startTime.ToString() + "_" + m_stopTime.ToString() + "_cap_both.pcm";
            //WriteRtpPacket(strFileName, pRtpInPacket, pRtpOutPacket);


            if (0 != WriteWaveFile(pRtpInPacket, pRtpOutPacket))
            {
                ICC_LOG_ERROR(m_pLog, "write packet of wavfile failed(%s)", strWaveFileName.c_str());
            }

            //清理资源
            if (NULL != pRtpInPacket)
            {
                delete pRtpInPacket;
                pRtpInPacket = NULL;
            }
            if (NULL != pRtpOutPacket)
            {
                delete pRtpOutPacket;
                pRtpOutPacket = NULL;
            }
            if (NULL != pInRawPacket)
            {
                delete pInRawPacket;
                pInRawPacket = NULL;
            }
            if (NULL != pOutRawPacket)
            {
                delete pOutRawPacket;
                pOutRawPacket = NULL;
            }

            //重新取包：先按当前时间取包，取不出来后再加上发包间隔时间进行取包
            pInRawPacket = PopInRtpRawPacket(packetTime);
            pOutRawPacket = PopOutRtpRawPacket(packetTime);
            while (NULL == pInRawPacket && NULL == pOutRawPacket)
            {
                if (packetTime > m_stopTime)
                {
                    break;
                }
                packetTime += m_rtpinterval;
                pInRawPacket = PopInRtpRawPacket(packetTime);
                pOutRawPacket = PopOutRtpRawPacket(packetTime);
            }
        }

        if (0 != m_waveFile.WriteHeader())
        {
            ICC_LOG_ERROR(m_pLog, "write header of wavfile failed(%s)", strWaveFileName.c_str());
        }

        //添加文件记录
        CRecordFilePtr pARecFile = CRecordFilePtr(new CRecordFile(strWaveFileName, m_startTime.ToDateTimeString(), m_stopTime.ToDateTimeString(), m_strAgentIP, m_usAgentCallPort));
        pARecFile->m_setCallID = m_setCallID;
        pARecFile->m_strCaller = m_strCaller;
        pARecFile->m_strCalled = m_strCalled;
        pARecFile->m_strAgentPhoneID = m_strAgentID; //CAgentIPTable::GetInstance().QueryPhoneByIP(m_strAgentIP);
        CRecordFileManage::GetInstance().AddARecordFile(pARecFile);
        //清理呼叫信息记录
        ClearCallInfo();
    }

    int CMediaStorage::WriteRtpPacket(const std::string strFileName, RTPPacket* pRtpLeftPacket, RTPPacket* pRtpRightPacket)
    {
        try
        {
            char OutBuffLeft[1024] = { 0 };
            char OutBuffRigth[1024] = { 0 };
            char OutBuff[2048] = { 0 };
            unsigned int OutLen = 0;

            if (NULL != pRtpLeftPacket)
            {
                memcpy(OutBuffLeft, pRtpLeftPacket->GetPayloadData(), pRtpLeftPacket->GetPayloadLength());
            }
            else
            {
                memcpy(OutBuffLeft, silence_data_g711a, m_packet_len);
            }
            if (NULL != pRtpRightPacket)
            {
                memcpy(OutBuffRigth, pRtpRightPacket->GetPayloadData(), pRtpRightPacket->GetPayloadLength());
            }
            else
            {
                memcpy(OutBuffRigth, silence_data_g711a, m_packet_len);
            }

            char* pLeft = (char*)OutBuffLeft;
            char* pRight = (char*)OutBuffRigth;
            char* pData = (char*)OutBuff;
            for (unsigned int i = 0; i < m_packet_len; i++)
            {
                pData[2 * i] = pLeft[i];
                pData[2 * i + 1] = pRight[i];
            }
            OutLen = m_packet_len * 2;

            std::ofstream rtpFile;
            rtpFile.open(strFileName.c_str(), std::ios::app | std::ios::binary);
            if (rtpFile.is_open())
            {
                rtpFile.write(OutBuff, OutLen);
                rtpFile.close();
            }
            return 0;
        }
        catch (std::exception& ex)
        {
            ICC_LOG_ERROR(m_pLog, "exception : %s", ex.what());
        };
        return -1;
    }

    int CMediaStorage::WriteWaveFile(RTPPacket* pRtpLeftPacket, RTPPacket* pRtpRightPacket)
    {
        try
        {
            char OutBuffLeft[1024] = { 0 };
            char OutBuffRigth[1024] = { 0 };
            char OutBuff[2048] = { 0 };

            unsigned int OutLen = 0;

            if (NULL != pRtpLeftPacket)
            {
                memcpy(OutBuffLeft, pRtpLeftPacket->GetPayloadData(), pRtpLeftPacket->GetPayloadLength());
            }
            else
            {
                memcpy(OutBuffLeft, silence_data_g711a, m_packet_len);
            }

            if (NULL != pRtpRightPacket)
            {
                memcpy(OutBuffRigth, pRtpRightPacket->GetPayloadData(), pRtpRightPacket->GetPayloadLength());
            }
            else
            {
                memcpy(OutBuffRigth, silence_data_g711a, m_packet_len);
            }
            char* pLeft = (char*)OutBuffLeft;
            char* pRight = (char*)OutBuffRigth;
            char* pData = (char*)OutBuff;
            for (unsigned int i = 0; i < m_packet_len; i++)
            {
                pData[2 * i] = pLeft[i];
                pData[2 * i + 1] = pRight[i];
            }
            OutLen = m_packet_len * 2;

            return m_waveFile.Write(OutBuff, OutLen);
        }
        catch (std::exception& ex)
        {
            ICC_LOG_ERROR(m_pLog, "exception : %s", ex.what());
        };
        return -1;
    }
}

