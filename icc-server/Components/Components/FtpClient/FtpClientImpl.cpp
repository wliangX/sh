#include "Boost.h"
#include "FtpClientImpl.h"
#include <fstream>


//解析服务回复数据的正则表达式
static boost::regex expression("^([0-9]+)(\\-| |$)(.*)$");
//全局变量，递归中做一个中间变量,后期改进
std::list<std::string>g_strListDir;

std::string g_strRootDir = "/";

CFtpClientImpl::CFtpClientImpl(IResourceManagerPtr pResourceManager) :m_cmd_socket(m_io_service),
m_work(m_io_service), m_data_socket(m_io_service), m_is_logined(false), m_is_connected(false), m_currentdir(""), m_downloadcnt(0), m_bIsDownloadError(false), m_thrd_data_socket_monitor(nullptr)
{
    m_pResourceManager = pResourceManager;
    m_pLog = ICCGetILogFactory()->GetLogger(MODULE_NAME);
    m_lock = ICCGetILockFactory()->CreateLock(Lock::TypeRecursiveMutex);
    m_HelpTool = ICCGetHelpToolFactory()->CreateHelpTool();
    //m_Xml = ICCGetIXmlFactory(IXmlFactory)->CreateXml();
    m_bloginTodir = false;
    m_IsCwd = false;
    m_thread = boost::thread(boost::bind(&boost::asio::io_service::run, boost::ref(m_io_service)));

}

CFtpClientImpl::~CFtpClientImpl()
{
    CloseDataSocket();
    CloseCmdSocket();
    m_work.~work();
    m_io_service.stop();
    m_thread.join();
}

void CFtpClientImpl::CloseCmdSocket()
{
    if (m_cmd_socket.is_open())
    {
        try
        {
            m_cmd_socket.shutdown(boost::asio::socket_base::shutdown_both);
        }
        catch (std::exception & ex)
        {
            ICC_LOG_WARNING(m_pLog, "exception :%s", ex.what());
        }
        m_cmd_socket.close();
    }
}


/////////////////////////////////////////////////////////////////////////
//功能: 连接ftp服务
//作者: w26326 wangyumeng
//传入参数: host(ip),port,user,password;
//传出参数: 

bool CFtpClientImpl::Connect(const std::string& host, const std::string& port, const std::string& user, const std::string& passwd)
{
    if (m_is_connected)
    {
        return true;
    }
    if (host.empty() || port.empty() || user.empty() || passwd.empty())
    {
        return false;
    }

    m_user = user;
    m_passwd = passwd;
    m_host = host;
    m_port = port;
    boost::asio::ip::tcp::resolver resolver(m_io_service);
    boost::asio::ip::tcp::resolver::query query(m_host, m_port);
    boost::asio::ip::tcp::resolver::iterator it = resolver.resolve(query);

    m_cmd_socket.async_connect(*it,
        boost::bind(&CFtpClientImpl::HandleConnect,
            this, boost::asio::placeholders::error));

    //等待3秒，连接成功返回连接成功
    try
    {
        if (!m_evtCondition.wait(3000))
        {
            return true;
        }
    }
    catch (...)
    {
        return false;
    }

    return true;
}


/////////////////////////////////////////////////////////////////////////
//功能: 上传文件
//作者: w26326 wangyumeng
//传入参数: 文件名
//传出参数: 

void CFtpClientImpl::UpLoadFile(const std::string& filename, bool bPrio)
{
    try
    {
        PushUpLoadFile(filename, bPrio);
        m_evtCondition.set();
    }
    catch (...)
    {
        ICC_LOG_ERROR(m_pLog, "Remote is leave, session is close");
    }
}


/////////////////////////////////////////////////////////////////////////
//功能: 下载文件
//作者: w26326 wangyumeng
//传入参数: 文件名
//传出参数: 

void CFtpClientImpl::DownLoadFile(const std::string& filename, bool bPrio)
{
    try
    {
        PushDownLoadFile(filename, bPrio);
        ICC_LOG_INFO(m_pLog, "PushDownLoadFile(%s) filelist size(%u).", filename.c_str(), FileListSize());
        m_evtCondition.set();
    }
    catch (...)
    {
        ICC_LOG_ERROR(m_pLog, "Remote is leave, session is close");
    }
}

void CFtpClientImpl::PushUpLoadFile(const std::string& filename, bool bPrio)
{
    boost::mutex::scoped_lock lock(m_filelstmutex);
    if (bPrio)
    {
        m_filelist.push_front(boost::make_shared<FileInfo>(true, filename));
    }
    else
    {
        m_filelist.push_back(boost::make_shared<FileInfo>(true, filename));
    }
}

void CFtpClientImpl::PushDownLoadFile(const std::string& filename, bool bPrio)
{
    boost::mutex::scoped_lock lock(m_filelstmutex);
    if (bPrio)
    {
        m_filelist.push_front(boost::make_shared<FileInfo>(false, filename));
    }
    else
    {
        m_filelist.push_back(boost::make_shared<FileInfo>(false, filename));
    }
}
FileInfoPtr CFtpClientImpl::GetFirstFile()
{
    boost::mutex::scoped_lock lock(m_filelstmutex);
    if (m_curFile == nullptr)
    {
        if(!m_filelist.empty())
        {
            m_curFile = m_filelist.front();
            m_filelist.pop_front();
        }
    }
    return m_curFile;
}
void CFtpClientImpl::PopCurFile()
{
    boost::mutex::scoped_lock lock(m_filelstmutex);
    m_curFile = nullptr;
}
bool CFtpClientImpl::RePushCurFile()
{
    bool nRet = true;
    boost::mutex::scoped_lock lock(m_filelstmutex);
    if (m_curFile != nullptr)
    {
        if (--m_curFile->nTryCount != 0)
        {
            m_filelist.push_back(m_curFile);
        }
        else
        {
            nRet = false;
        }
        m_curFile = nullptr;
    }
    return nRet;
}
bool CFtpClientImpl::FileListEmpty()
{
    boost::mutex::scoped_lock lock(m_filelstmutex);
    return m_filelist.empty();
}

size_t CFtpClientImpl::FileListSize()
{
    boost::mutex::scoped_lock lock(m_filelstmutex);
    return m_filelist.size();
}

/////////////////////////////////////////////////////////////////////////
//功能: 服务允许连接后进行命令连接
//作者: w26326 wangyumeng
//传入参数: 
//传出参数: 

void CFtpClientImpl::Start()
{
    boost::asio::async_read_until(m_cmd_socket,
        m_receive_msg, '\n',
        boost::bind(&CFtpClientImpl::ReadComplete, this,
            boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
}

/////////////////////////////////////////////////////////////////////////
//功能: 处理连接的回调
//作者: w26326 wangyumeng
//传入参数: 回调参数
//传出参数: 

void CFtpClientImpl::HandleConnect(const boost::system::error_code& error)
{
    if (!error)
    {
        m_is_connected = true;
        Start();
    }
    else
    {
        ICC_LOG_ERROR(m_pLog, "%d %s", error.value(), error.message().c_str());
        m_evtCondition.wait(30000);
        ReconnectCmdSock();
    }
}

/////////////////////////////////////////////////////////////////////////
//功能: 读取服务端发出的命令和命令代码
//作者: w26326 wangyumeng
//传入参数: resp_code:命令代码
//			resp_arg：命令详情
//传出参数: 
////////////////////////////////////////////////////////////////////////
void CFtpClientImpl::PaserResponse(size_t resp_code, std::string resp_arg)
{
    Lock::AutoLock		lock(m_lock);
    std::string		cmd;
    std::string		filename;
    ICC_LOG_INFO(m_pLog, "Receive cmd from Server: %u : %s", resp_code, resp_arg.c_str());


    size_t szRet = 0;
    switch (resp_code)
    {
    case 125://数据链路埠开启，准备传送。
    case 150://文件状态良好，将要打开数据连接
        ReadyForData(resp_code, resp_arg);
        break;
    case 200://切换模式
    case 213://获取文件大小
        break;

    case 220: //welcome  服务就绪
        cmd = "User " + m_user + "\r\n";
        szRet = SendFtpMessage(cmd);
        ICC_LOG_INFO(m_pLog, "return %u cmd : %s ", szRet, cmd.c_str());
        break;

    case 226: // Transfer complete. 关闭数据连接，请求的文件操作成功
        TranComplete(resp_code, resp_arg);
        break;

    case 227://Entering passive Mode  进入被动模式
        EnterPassive(resp_code, resp_arg);
        break;

    case 230://loging success    用户已登录
        //////////////////             
        LoginSuccess(resp_code, resp_arg);
        m_evtCondition.set();
        break;

    case 250://请求的文件操作完成
        ChangedDir(resp_code, resp_arg);
        break;

    case 257://创建路径名
        CreatedDir(resp_code, resp_arg);
        break;

    case 331:// enter password    用户名正确，需要口令
        cmd = "Pass " + m_passwd + "\r\n";
        szRet = SendFtpMessage(cmd);
        ICC_LOG_INFO(m_pLog, "return %u cmd : %s ", szRet, cmd.c_str());
        break;

    case 530://Login or password incorrect!  未登录
    {
        size_t szRet = SendFtpMessage("QUIT\r\n");
        ICC_LOG_INFO(m_pLog, "Send cmd To Server: QUIT return %u", szRet);
        break;
    }

    case 421:// connect timeout  服务关闭
        TimeOut(resp_code, resp_arg);
        break;

    case 425:// data connect 连接失败,重新进入一次PASV模式
    {
        size_t szRet = SendFtpMessage("PASV \r\n");
        ICC_LOG_INFO(m_pLog, "Send cmd To Server: PASV return %u", szRet);
    }
    break;

    case 500:// oops   无效命令
        break;

    case 550://File not found   未执行请求操作
        //////////////////////
        FileNotFound(resp_code, resp_arg);
        break;

    default:
    {
        ICC_LOG_INFO(m_pLog, "Can not process Resp code:%d Resp args:%s ", resp_code, resp_arg.c_str());
        ReconnectCmdSock();
        break;
    }
    }
}

void CFtpClientImpl::ReconnectCmdSock()
{
    m_is_logined = false;
    m_is_connected = false;
    CloseDataSocket();
    CloseCmdSocket();
    RePushCurFile();
    ICC_LOG_INFO(m_pLog, "try to reconnect ftp %s %s ", m_host.c_str(), m_port.c_str());
    Connect(m_host, m_port, m_user, m_passwd);
}

void CFtpClientImpl::CreateMutiDirectory(boost::filesystem::path path)
{
    boost::filesystem::path::iterator itor = path.begin();
    boost::filesystem::path pathTmp;
    while (itor != path.end())
    {
        pathTmp /= (*itor).string();
        if (!pathTmp.empty() && !boost::filesystem::exists(pathTmp))
        {
            boost::filesystem::create_directory(pathTmp);
        }
        ++itor;
    }
}

/////////////////////////////////////////////////////////////////////////
//功能: 登录成功的处理函数
//作者: w26326 wangyumeng
//传入参数: 
//传出参数: 

void  CFtpClientImpl::LoginSuccess(const size_t& resp_code, const std::string& resp_arg)
{
    m_is_logined = true;
    std::string m_HttpStrPath;
    std::string m_FileName;
    std::list<FileNode> vecFile;
    std::streampos m_Pos;
    FileNode m_struct;

    if (!FileListEmpty())
    {
        SendFtpMessage("PASV \r\n");
        return;
    }
    {//刚刚登录,文件为空,循环，等待文件传输
        ChangeToDir(g_strRootDir);
    }
    return;
}

void CFtpClientImpl::ChangeToDir(const std::string& strDir)
{
    std::string strCmd = "CWD " + strDir + "\r\n";
    size_t szRet = SendFtpMessage(strCmd);
    ICC_LOG_INFO(m_pLog, "return %u cmd: %s ", szRet, strCmd.c_str());
}

/////////////////////////////////////////////////////////////////////////
//功能: 成功创建目录处理函数
//作者: w26326 wangyumeng
//传入参数: 
//传出参数: 

void CFtpClientImpl::CreatedDir(const size_t& resp_code, const std::string& resp_arg)
{
    //created directory send change directory
    std::string strCmd;
    std::string strDir;
    if (!FileListEmpty())
    {
        ChangeToDir(g_strRootDir);
        return;
    }
    return;
}


/////////////////////////////////////////////////////////////////////////
//功能: 改变服务工作目录的处理函数
//作者: w26326 wangyumeng
//传入参数: 
//传出参数: 

void CFtpClientImpl::ChangedDir(const size_t& resp_code, const std::string& resp_arg)
{//changed directory

    std::string strCmd;
    std::string strDir;

    if (!FileListEmpty())
    {//上传目录列表为空，判断当前目录是否为根目录，进行文件传输
        SendFtpMessage("PASV \r\n");
        return;
    }
    //检查删除列表是不是为空
    if (DeleteFileListSize() != 0)
    {
        DoDeleteFtpFile();
        ChangeToDir(g_strRootDir);
        return;
    }

    //没有文件传入等待30秒
    m_evtCondition.wait(30000);
  
    /*static  int nCount = 0;
    ICC_LOG_INFO(m_pLog, "test code executed(%d) filelist size(%u).", nCount, FileListSize());
    if (++nCount > 10)
    {
        PushUpLoadFile("zjq.txt", false);
        nCount = 0;
    }*/
    ChangeToDir(g_strRootDir);
}


/////////////////////////////////////////////////////////////////////////
//功能: 服务处于数据传输状态
//作者: w26326 wangyumeng
//传入参数: 
//传出参数: 

void CFtpClientImpl::ReadyForData(const size_t& resp_code, const std::string& resp_arg)
{
    std::string strInfo;
    std::string strCmd;
    std::string strDir;
    FileNode m_node;
    FileInfoPtr finfo = GetFirstFile();
    if (finfo != nullptr)
    {
        if (true == finfo->bIsUpload)
        {//uploadfile  
            if ((TransData(finfo->strFileName)))
            {
                strInfo = "file " + finfo->strFileName + " is upload to server";
                ICC_LOG_INFO(m_pLog, "%s", strInfo.c_str());

                //RemoveFrontFromFileList(0); 收到226 Transfer complete消息后再删除列表文件
            }
            else
            {
                strInfo = "file " + finfo->strFileName + " is failed upload to server";
                ICC_LOG_INFO(m_pLog, "%s", strInfo.c_str());
                RemoveFrontFromFileList(-1);
                ChangeToDir(g_strRootDir);
            }
        }
        else
        {// download file;  
            if ((ReceiveData(finfo->strFileName)))
            {
                strInfo = "file " + finfo->strFileName + " is download from server";
                ICC_LOG_INFO(m_pLog, "%s", strInfo.c_str());
                //RemoveFrontFromFileList(0);收到226 Transfer complete消息后再删除列表文件
            }
            else
            {
                strInfo = "file " + finfo->strFileName + " is failed download from server";
                ICC_LOG_INFO(m_pLog, "%s", strInfo.c_str());
                RemoveFrontFromFileList(-1);
                ChangeToDir(g_strRootDir);
            }

        }
    }
    else
    {
        ChangeToDir(g_strRootDir);
    }
}

void CFtpClientImpl::RemoveFrontFromFileList(int nRet)
{
    FileInfoPtr fInfo = GetFirstFile();
    if (fInfo != nullptr)
    {
        bool bRePushRet = false;
        if (nRet == -1)
        {
            bRePushRet = RePushCurFile();
        }
        if(!bRePushRet)
        {
            m_pFtpFileCompletedFun(fInfo->bIsUpload, m_host, fInfo->strFileName, nRet);
            PopCurFile();
        }
        ICC_LOG_INFO(m_pLog, "isUpLoad:%d FtpIp:%s fileName:%s nResult:%d bRePush:%d filelistsize:%u", fInfo->bIsUpload, m_host.c_str(), fInfo->strFileName.c_str(), nRet, bRePushRet, FileListSize());
    }
}

/////////////////////////////////////////////////////////////////////////
//功能: 文件传输完毕
//作者: w26326 wangyumeng
//传入参数: 
//传出参数: 

void CFtpClientImpl::TranComplete(const size_t& resp_code, const std::string& resp_arg)
{
    std::string strCmd;
    std::string strDir;

    RemoveFrontFromFileList(0);
    if (!FileListEmpty())
    {//文件列表不为空，进入被动数据传输模式
        SendFtpMessage("PASV \r\n");
        return;
    }
    {//传输完毕，循环，等待文件传输
        ChangeCharMode("A");
        ChangeToDir(g_strRootDir);
    }
    return;
}


/////////////////////////////////////////////////////////////////////////
//功能: 服务进入被动传输模式
//作者: w26326 wangyumeng
//传入参数: 
//传出参数: 

void CFtpClientImpl::EnterPassive(const size_t& resp_code, const std::string& resp_arg)
{
    std::string strCmd;
    std::string strDir;
    std::string strFilename;

    //解析服务信息
    ParserPasv(resp_arg);
    if (!m_dirclist.empty())
    {//如果目录不为空，优先创建目录
        ChangeToDir(g_strRootDir);
        return;
    }

    FileInfoPtr fInfo = GetFirstFile();
    if (fInfo != nullptr)
    {
        if (fInfo->bIsUpload)
        {
            strFilename = fInfo->strFileName.substr(strFilename.find_last_of('\\') + 1);
            strCmd = "STOR " + strFilename + "\r\n";
        }
        else
        {
            size_t fileSize = GetRemoteFileSize(fInfo->strFileName);
            if (fileSize == 0)
            {
                RemoveFrontFromFileList(-2);
                ChangeToDir(g_strRootDir);
                return;
            }
            ChangeCharMode("I");
            strCmd = "RETR " + fInfo->strFileName + "\r\n";
        }
        size_t szRet = SendFtpMessage(strCmd);
        ICC_LOG_INFO(m_pLog, "return %u cmd: %s ", szRet, strCmd.c_str());
        return;
    }
    else
    {
        //传输完毕，循环，等待文件传输
        ChangeToDir(g_strRootDir);
    }
    return;
}


/////////////////////////////////////////////////////////////////////////
//功能: 连接超时
//作者: w26326 wangyumeng
//传入参数: 
//传出参数: 

void CFtpClientImpl::TimeOut(const size_t& resp_code, const std::string& resp_arg)
{
    ReconnectCmdSock();
}

size_t CFtpClientImpl::GetRemoteFileSize(const std::string& strFileName)
{
    size_t szFileSize = 0;
    std::string strCommand = "SIZE " + strFileName + "\r\n";
    std::string strResponse;
    SendFtpMessage(strCommand, strResponse);
    boost::cmatch what;
    if (boost::regex_match(strResponse.c_str(), what, expression))
    {
        int code = ::atoi(what[1].first);
        std::string strFileSize = std::string(what[3].first, what[3].second);
        szFileSize = ::atoi(strFileSize.c_str());
    }
    ICC_LOG_INFO(m_pLog, "Get file size of %s is %u", strFileName.c_str(), szFileSize);
    return szFileSize;
}

int CFtpClientImpl::ChangeCharMode(const std::string& strMode)
{
    int nRet = 0;
    std::string strCommand = "TYPE " + strMode + "\r\n";
    std::string strResponse;
    SendFtpMessage(strCommand, strResponse);
    boost::cmatch what;
    if (boost::regex_match(strResponse.c_str(), what, expression))
    {
        int code = ::atoi(what[1].first);
        if (code == 200)
        {
            nRet = 0;
        }
        std::string strResult = std::string(what[3].first, what[3].second);
    }
    ICC_LOG_INFO(m_pLog, "return %s ChangeCharMode %s ", strResponse.c_str(), strCommand.c_str());
    return nRet;
}

int CFtpClientImpl::DeleteRemoteFile(const std::string& strFileName)
{
    return PushDeleteFile(strFileName);
}

int CFtpClientImpl::DoDeleteFtpFile()
{
    size_t ulDeleteFileLstSize = DeleteFileListSize();
    if (0 == ulDeleteFileLstSize)
    {
        return 0;
    }
    std::string strFileName = PopDeleteFile();
    int nRet = 0;
    std::string strResponse("");
    size_t retval = SendFtpMessage("DELE " + strFileName + "\r\n", strResponse);
    boost::cmatch what;
    if (boost::regex_match(strResponse.c_str(), what, expression))
    {
        int code = ::atoi(what[1].first);
        if (code == 250)
        {
            nRet = 0;
        }
        std::string strResult = std::string(what[3].first, what[3].second);
    }
    ICC_LOG_INFO(m_pLog, "DeleteRemoteFile %s listsize(%u) return %s", strFileName.c_str(), ulDeleteFileLstSize, strResponse.c_str());
    return nRet;
}


size_t CFtpClientImpl::SendFtpMessage(const std::string& strCommand, std::string& strResponse)
{
    boost::system::error_code ec;
    if (!m_cmd_socket.is_open())
    {
        ICC_LOG_INFO(m_pLog, "Server is not connect: %s", m_host.c_str());
        return 0;
    }
    boost::asio::streambuf response;
    std::istream response_stream(&response);
    size_t ret = boost::asio::write(m_cmd_socket, boost::asio::buffer(strCommand), ec);
    boost::asio::read_until(m_cmd_socket, response, '\n');
    boost::asio::streambuf::const_buffers_type bufs = response.data();
    std::getline(response_stream, strResponse);
    return ret;
}


void CFtpClientImpl::SetNotifyCompleteFunc(IFtpClient::NotifyFileCompletedFunc p_pFunc)
{
    m_pFtpFileCompletedFun = p_pFunc;
}

/////////////////////////////////////////////////////////////////////////
//功能: 服务器处理文件或目录目录失败
//作者: w26326 wangyumeng
//传入参数: 
//传出参数: 

void CFtpClientImpl::FileNotFound(const size_t& resp_code, const std::string& resp_arg)
{
    RemoveFrontFromFileList(-3);
    ChangeToDir(g_strRootDir);
    return;
}

bool CFtpClientImpl::GetDownLoadError()
{
    boost::mutex::scoped_lock lock(m_mtDownloadError);
    return m_bIsDownloadError;
}
void CFtpClientImpl::SetDownLoadError(bool bDownLoadError)
{
    boost::mutex::scoped_lock lock(m_mtDownloadError);
    m_bIsDownloadError = bDownLoadError;
}

inline void CFtpClientImpl::ResetDownloadCnt()
{
    boost::mutex::scoped_lock lock(m_mtDownLoadCnt);
    m_downloadcnt = 0;
}

inline void CFtpClientImpl::IncreamDownloadCnt()
{
    boost::mutex::scoped_lock lock(m_mtDownLoadCnt);
    m_downloadcnt++;
}

inline unsigned int CFtpClientImpl::GetDownloadCnt()
{
    boost::mutex::scoped_lock lock(m_mtDownLoadCnt);
    return m_downloadcnt;
}
//监控文件下载socket的计数器，文件下载socket每下载一次Cnt加1，每隔10秒检查一次。
//如果两次前后不一致证明在下载文件，如果前后一致证明没有下载了，则关闭数据连接
void CFtpClientImpl::MonitorDataSocket()
{
    unsigned int uiLastDownLoadCnt = GetDownloadCnt();
    if (uiLastDownLoadCnt == 0)//线程启动时获取下载计数为0代表没有下载任务
    {
        ICC_LOG_DEBUG(m_pLog, "Check Data DownloadCnt lastTime is zero, quit thread. (%u)", uiLastDownLoadCnt);
        return;
    }
    do
    {
        boost::this_thread::sleep(boost::posix_time::seconds(10));
        unsigned int uiDownLoadCnt = GetDownloadCnt();
        ICC_LOG_DEBUG(m_pLog, "Check Data DownloadCnt last(%u) now(%u)", uiLastDownLoadCnt, uiDownLoadCnt);
        if (uiDownLoadCnt == 0)//等待若干秒后获取下载计数为0代表下载任务已经结束
        {
            return;
        }
        if (uiDownLoadCnt != uiLastDownLoadCnt)
        {
            uiLastDownLoadCnt = uiDownLoadCnt;
        }
        else//这里不仅要关闭数据连接，还要保证继续下载文件
        {
            SetDownLoadError(true);
            CloseDataSocket();
            break;
        }
    } while (1);
    boost::this_thread::interruption_point();
}

//增加下载计数，开始监控线程
void CFtpClientImpl::StartMonitorThread()
{
    IncreamDownloadCnt();
    m_thrd_data_socket_monitor.reset(new boost::thread(boost::bind(&CFtpClientImpl::MonitorDataSocket, this)));
}

//停止监控线程，清零下载计数
void CFtpClientImpl::StopMonitorThread()
{
    if (m_thrd_data_socket_monitor != nullptr)
    {
        m_thrd_data_socket_monitor->interrupt();
        if (m_thrd_data_socket_monitor->joinable())
        {
            m_thrd_data_socket_monitor->join();
        }
    }
    ResetDownloadCnt();
}


/////////////////////////////////////////////////////////////////////////
//功能: 解析主机IP和端口
//作者: w26326 wangyumeng
//传入参数: pasv:服务回复的命令详情
//传出参数: 

void CFtpClientImpl::ParserPasv(std::string pasv)
{
    if ((pasv.find('(') == std::string::npos) && (pasv.find(')') == std::string::npos))
    {
        return;
    }
    std::pair<std::string, std::string>porthost = ParseHostPort(pasv);
    ICC_LOG_INFO(m_pLog, "connect data socket %s : %s", porthost.first.c_str(), porthost.second.c_str());
    boost::asio::ip::tcp::resolver resolver(m_io_service);
    boost::asio::ip::tcp::resolver::query query(porthost.first, porthost.second);
    boost::asio::ip::tcp::resolver::iterator it = resolver.resolve(query);
    boost::asio::ip::tcp::endpoint endpoint = *it;
    CloseDataSocket();
    try
    {
        m_data_socket.connect(endpoint);
    }
    catch (boost::system::system_error & ex)
    {
        ICC_LOG_ERROR(m_pLog, "catch a exception while connect %s", ex.what());
    }
    catch (...)
    {
        ICC_LOG_ERROR(m_pLog, "catch a exception while connect");
    }
}

void CFtpClientImpl::CloseDataSocket()
{
    if (m_data_socket.is_open())
    {
        try
        {
            m_data_socket.shutdown(boost::asio::socket_base::shutdown_both);
        }
        catch (boost::system::system_error & ex)
        {
            ICC_LOG_WARNING(m_pLog, "catch a exception when shutdown data socket %s", ex.what());
        }
        catch (...)
        {
            ICC_LOG_ERROR(m_pLog, "catch a exception when shutdown data socket");
        }
        m_data_socket.close();
    }
}


/////////////////////////////////////////////////////////////////////////
//功能: 解析主机IP和ID端口
//作者: w26326 wangyumeng
//传入参数: s:服务回复的命令详情
//传出参数:

std::pair<std::string, std::string> CFtpClientImpl::ParseHostPort(std::string s)
{
    size_t paramspos = s.find('(');
    std::string params = s.substr(paramspos + 1);

    size_t ip1pos = params.find(',');
    std::string ip1 = params.substr(0, ip1pos);

    size_t ip2pos = params.find(',', ip1pos + 1);
    std::string ip2 = params.substr(ip1pos + 1, ip2pos - ip1pos - 1);

    size_t ip3pos = params.find(',', ip2pos + 1);
    std::string ip3 = params.substr(ip2pos + 1, ip3pos - ip2pos - 1);

    size_t ip4pos = params.find(',', ip3pos + 1);
    std::string ip4 = params.substr(ip3pos + 1, ip4pos - ip3pos - 1);

    size_t port1pos = params.find(',', ip4pos + 1);
    std::string port1 = params.substr(ip4pos + 1, port1pos - ip4pos - 1);

    size_t port2pos = params.find(')', port1pos + 1);
    std::string port2 = params.substr(port1pos + 1, port2pos - port1pos - 1);

    std::pair<std::string, std::string> hostport;
    hostport.first = ip1 + "." + ip2 + "." + ip3 + "." + ip4;
    int portval = atoi(port1.c_str()) * 256 + atoi(port2.c_str());
    hostport.second = boost::lexical_cast<std::string>(portval);
    return hostport;
}



/////////////////////////////////////////////////////////////////////////
//功能: 打开本地文件，进行数据传输
//作者: w26326 wangyumeng
//传入参数: 文件名
//传出参数:

bool CFtpClientImpl::TransData(const std::string& strFileName)
{

    Lock::AutoLock lock(m_lock);
    std::ifstream ifs(strFileName, std::ifstream::binary);
    //ifs.seekg(strFileNode.filePos, std::ios::cur);

    char	buffer[1448];
    bool	bRet = true;
    //循环控制需要改进
    if (ifs)
    {
        while (1)
        {
            try
            {
                ifs.read(buffer, 1448);
                if (m_data_socket.send(boost::asio::buffer(buffer, ifs.gcount())) != ifs.gcount())
                {
                    break;
                }
                if (ifs.gcount() < 1448)
                {
                    break;
                }
            }
            catch (std::exception & e)
            {
                ICC_LOG_ERROR(m_pLog, "%s", e.what());
                break;
            }
            catch (...)
            {
                ICC_LOG_ERROR(m_pLog, "Program Exception");
                break;
            }
        }
        ifs.close();
        CloseDataSocket();
        return bRet;
    }
    else
    {
        ICC_LOG_ERROR(m_pLog, "Open %s Failed", strFileName.c_str());
        CloseDataSocket();
        return false;
    }

}



/////////////////////////////////////////////////////////////////////////
//功能: 建立文件，进行数据下载
//作者: w26326 wangyumeng
//传入参数: 文件名
//传出参数:

bool CFtpClientImpl::ReceiveData(const std::string& filename)
{
    Lock::AutoLock lock(m_lock);
    std::string strDownLoadTmpPath = m_currentdir;
    if (m_currentdir.empty())
    {
        strDownLoadTmpPath = boost::filesystem::current_path().string();
        strDownLoadTmpPath += "/download/";

    }
    boost::filesystem::path path(strDownLoadTmpPath);
    CreateMutiDirectory(path);

    path += boost::filesystem::path(filename).filename();

    std::ofstream ofs(path.string().c_str(), std::ios::out | std::ios::binary);
    char	buffer[1448];
    size_t  receive_size = 0;
    bool    bRet = true;
    if (ofs)
    {
        //循环控制需要改进
        StartMonitorThread();
        ICC_LOG_DEBUG(m_pLog, "start down file %s", filename.c_str());
        while (1)
        {// if server send  all data, server is end of file  
            try
            {
                
                receive_size = m_data_socket.receive(boost::asio::buffer(buffer, 1448));
                if (receive_size == 0)
                {
                    ICC_LOG_WARNING(m_pLog, "%s break, receive data length is zero.", filename.c_str());
                    break;
                }
                ofs.write(buffer, receive_size);
                ofs.flush();
                IncreamDownloadCnt();
            }
            catch (std::exception & e)
            {
                ICC_LOG_WARNING(m_pLog, "%s", e.what());
                break;
            }
            catch (...)
            {
                ICC_LOG_ERROR(m_pLog, "Ftp Receive data failed");
                SetDownLoadError(true);
                CloseDataSocket();
                break;
            }
        }
        ICC_LOG_DEBUG(m_pLog, "stop down file %s", filename.c_str());
        ofs.close();
        //停止监控线程
        StopMonitorThread();
        //检查是否下载出错
        if (GetDownLoadError())
        {
            ICC_LOG_ERROR(m_pLog, "down file %s error, restart ftp connection.", filename.c_str());
            SetDownLoadError(false);
            ReconnectCmdSock();
            return false;
        }
        CloseDataSocket();
        return bRet;
    }
    else
    {
        ICC_LOG_INFO(m_pLog, "Open %s Failed", filename.c_str());
        CloseDataSocket();
        return false;
    }
}



/////////////////////////////////////////////////////////////////////////
//功能:读取服务端发出的命令和命令代码
//作者: w26326 wangyumeng
//传入参数: 
//传出参数:

void CFtpClientImpl::ReadComplete(const boost::system::error_code& error, size_t bytes_transferred)
{
    if (!error)
    {
        boost::asio::streambuf::const_buffers_type bufs = m_receive_msg.data();
        std::string line(boost::asio::buffers_begin(bufs), boost::asio::buffers_begin(bufs) + bytes_transferred);
        m_receive_msg.consume(bytes_transferred);
        boost::cmatch what;
        if (boost::regex_match(line.c_str(), what, expression))
        {
            PaserResponse(::atoi(what[1].first), std::string(what[3].first, what[3].second));
        }
        Start();
    }
    else
    {
        ICC_LOG_INFO(m_pLog, "error occur %d %s", error.value(), error.message().c_str());
        if (error.value() != 125) //125 Operation canceled FtpClient是自动化运行，不会出现这个错误，唯一的场景是重连后，服务器发回来上一个命令的结果
        {
            ReconnectCmdSock();
        }

    }
}

/////////////////////////////////////////////////////////////////////////
//功能: 发送命令到服务端
//作者: w26326 wangyumeng
//传入参数: 命令
//传出参数:

size_t CFtpClientImpl::SendFtpMessage(std::string msg)
{
    boost::system::error_code ec;
    if (!m_cmd_socket.is_open())
    {
        ICC_LOG_INFO(m_pLog, "Server is not connect: %s", msg.c_str());
        return 0;
    }
    return boost::asio::write(m_cmd_socket, boost::asio::buffer(msg), ec);
}

/////////////////////////////////////////////////////////////////////////
//功能: 保存相关日志或错误到Log文件中
//作者: w26326 wangyumeng
//传入参数: info
//传出参数:

void CFtpClientImpl::SaveInfo(std::string info)
{

    Lock::AutoLock lock(m_lock);
#ifdef  _DEBUG  
    std::cout << info << std::endl;
#endif   
    m_error_msg = info;
    info = m_error_msg + "\n";
    std::ofstream of("LogInfo.txt", std::ios::app | std::ios::out | std::ios::in);
    if (of.is_open())
    {
        of.write(info.c_str(), info.size());
    }
    of.close();
}

/////////////////////////////////////////////////////////////////////////
//功能: 上传目录
//作者: w26326 wangyumeng
//传入参数: 目录名
//传出参数: 

void CFtpClientImpl::UpLoadDirectory(const std::string& directname)
{
    Lock::AutoLock lock(m_lock);
    std::string strPath = directname;

    //保存所有的文件名
    std::list<FileNode>vecFile;
    vecFile = ScanFiles(strPath, vecFile, m_dirclist);

    for (std::list<std::pair<std::string, std::list<FileNode>>>::iterator iter = m_dirclist.begin();
        iter != m_dirclist.end(); iter++)
    {
        iter->first = iter->first.substr(iter->first.find_first_of('\\') + 1);
        boost::algorithm::replace_all(iter->first, "\\", "/");

        for (std::list<FileNode>::iterator it = iter->second.begin(); it != iter->second.end(); it++)
        {
            //boost::algorithm::replace_all(*it, "\\", "\\\\");
        }
    }
    for (std::list<std::pair<std::string, std::list<FileNode>>>::iterator iter = m_dirclist.begin();
        iter != m_dirclist.end(); iter++)
    {
        m_updirclist.push_back(*iter);
        m_dirclistCache.push_back(*iter);  //备份目录与文件的关系列表,并将列表写入文件中
    }
}

/////////////////////////////////////////////////////////////////////////
//功能: 获取目录以及目录下的文件列表
//作者: w26326 wangyumeng
//传入参数: rootPath：目录名
//			vector:   保存文件的list
//          m_dirlist:保存目录与文件关系的列表
//传出参数: 

const std::list<FileNode>& CFtpClientImpl::ScanFiles(const std::string& rootPath,
    std::list<FileNode>& container = *(new std::list<FileNode>()),
    std::list<std::pair<std::string, std::list<FileNode>>>& container2 = *(new std::list<std::pair<std::string, std::list<FileNode>>>()))
{
    namespace fs = boost::filesystem;
    fs::path  fullpath(rootPath);

    std::list<FileNode>& ret = container;
    std::list<std::pair<std::string, std::list<FileNode>>>& ret2 = container2;
    FileNode m_Node;
    if (!fs::exists(fullpath))
    {//递归结束条件
        return ret;
    }
    fs::directory_iterator end_iter;
    for (fs::directory_iterator iter(fullpath); iter != end_iter; ++iter)
    {
        try
        {
            if (fs::is_directory(*iter))
            {//如果是目录，就存入目录队列中
                g_strListDir.push_back(iter->path().string());
            }
            else
            {//如果是文件，就存入文件队列中
                std::string fileName = iter->path().string();
                m_Node.fileName = fileName;
                ret.push_back(m_Node);
            }
        }
        catch (const std::exception & ex)
        {
            continue;
        }
    }
    //将当前目录名，和当前目录下的文件存入表中
    ret2.push_back(std::make_pair(fullpath.string(), ret));
    ret.clear();

    if (!g_strListDir.empty())
    {//如果目录队列不为空，进行递归
        std::string path = g_strListDir.front();
        g_strListDir.pop_front();
        CFtpClientImpl::ScanFiles(path, ret, ret2);
    }
    return ret;
}

//下载目录
void CFtpClientImpl::DownLoadDirectory(const std::string& directname)
{
    //Lock::AutoLock lock(m_lock);
    try
    {
        //创建多级目录
        std::string strDownLoadDir = directname;
        boost::algorithm::replace_all(strDownLoadDir, "\\", "/");
        std::string strDownLoadTmpPath = m_currentdir;
        if (m_currentdir.empty())
        {
            strDownLoadTmpPath = boost::filesystem::current_path().string();
            strDownLoadTmpPath += "/download/";

        }
        boost::filesystem::path path(strDownLoadTmpPath);
        path += "/" + strDownLoadDir;
        CreateMutiDirectory(path);
        m_loaddirlist.push_back(strDownLoadDir);
    }
    catch (std::exception & e)
    {
        ICC_LOG_ERROR(m_pLog, "download direct failed: %s", e.what());
    }
}


void CFtpClientImpl::SetDownLoadTmpDir(const std::string& p_strDirName)
{
    m_currentdir = p_strDirName;
    if (m_currentdir[m_currentdir.length() - 1] != '/' && m_currentdir[m_currentdir.length() - 1] != '\\')
    {
        m_currentdir += "/";
    }
}
