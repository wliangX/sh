// AvayaSwitch.cpp: implementation of the Handler class.
//
//////////////////////////////////////////////////////////////////////
#include "Boost.h"
#include "SysConfig.h"
#include "AvayaSwitch.h"
#include "AvayaResult.h"
#include "AvayaEvent.h"

#include "CallManager.h"

#if defined(WIN32) || defined(WIN64)
#define STRNCPY strncpy_s
#else
#define STRNCPY strncpy
#endif

//////////////////////////////////////////////////////////////////////////
#if defined(WIN32) || defined(WIN64)
//窗口过程
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_OPEN_STREAM:
		CAvayaSwitch::Instance()->OnOpenStreamEvent(/*wParam, lParam*/);
		break;
	case WM_CLOSE_STREAM:
		CAvayaSwitch::Instance()->OnCloseStreamEvent(/*wParam, lParam*/);
		break;
	case WM_TSAPI_MSG:
		CAvayaSwitch::Instance()->OnTSAPIEvent(/*wParam, lParam*/);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_CREATE:
		break;
	case WM_PAINT:
		break;
	case WM_COMMAND:
		break;
	default:
		break;
	}

	return DefWindowProc(hwnd, message, wParam, lParam);
}

std::wstring StringToWString(LPCSTR pszSrc, int nLen)
{
	int nSize = MultiByteToWideChar(CP_ACP, 0, (LPCSTR)pszSrc, nLen, 0, 0);
	if (nSize <= 0) return L"";

	WCHAR *pwszDst = new WCHAR[nSize + 1]();
	if (NULL == pwszDst) return L"";

	MultiByteToWideChar(CP_ACP, 0, (LPCSTR)pszSrc, nLen, pwszDst, nSize);
	pwszDst[nSize] = 0;

	if (pwszDst[0] == 0xFEFF) // skip Oxfeff
		for (int i = 0; i < nSize; i++)
			pwszDst[i] = pwszDst[i + 1];

	std::wstring wcharString(pwszDst);
	delete[] pwszDst;
	pwszDst = NULL;

	return wcharString;
}
#endif // WIN32
//////////////////////////////////////////////////////////////////////////

CAvayaSwitch::CAvayaSwitch()
{
#if defined(WIN32) || defined(WIN64)
	m_hWnd = NULL;
	m_hInstance = NULL;
#endif // WIN32

	//m_bEventIsPending = false;
	m_bOpeningStream = false;
	m_bStopOpenStream = false;
	m_acsHandle = 1;
	m_usEventSize = 0;
	m_privateData.length = 0;

	m_cstaEvent.eventHeader.acsHandle = 0;
	m_cstaEvent.eventHeader.eventClass = 0;
	m_cstaEvent.eventHeader.eventType = 0;
	memset(m_cstaEvent.heap, 0, CSTA_MAX_HEAP);

	memset(m_privateData.data, 0, ATT_MAX_PRIVATE_DATA);
	memset(m_privateData.vendor, 0, 32);
	m_privateData.length = 0;

	m_bConnectSlaveAvayaFlag = false;
}
CAvayaSwitch::~CAvayaSwitch()
{
	//
}

boost::shared_ptr<CAvayaSwitch> CAvayaSwitch::m_pInstance = nullptr;
boost::shared_ptr<CAvayaSwitch> CAvayaSwitch::Instance()
{
	if (m_pInstance == nullptr)
	{
		m_pInstance = boost::make_shared<CAvayaSwitch>();
	}

	return m_pInstance;
}
void CAvayaSwitch::ExitInstance()
{
	m_bStopOpenStream = true;

	CAvayaEvent::Instance()->ExitInstance();
	CAvayaResult::Instance()->ExitInstance();

	if (m_pInstance)
	{
		m_pInstance.reset();
	}
}

void CAvayaSwitch::OnInit(IResourceManagerPtr pResourceManager)
{
	m_pLog = ICCGetResourceEx(Log::ILogFactory, ICCILogFactoryResourceName, pResourceManager)->GetLogger(MODULE_NAME);

	CAvayaEvent::Instance()->Init(pResourceManager);
	CAvayaResult::Instance()->Init(pResourceManager);

	
}
void CAvayaSwitch::OnStart()
{
	m_bStopOpenStream = false;
}
void CAvayaSwitch::OnStop()
{
	m_bStopOpenStream = true;
}
//////////////////////////////////////////////////////////////////////////
#if defined(WIN32) || defined(WIN64)

TCHAR CAvayaSwitch::m_szAppName[MAX_BUFFER] = TEXT("AvayaSwitchWindows");
bool CAvayaSwitch::CreateMyWindows()
{
	m_hInstance = ::GetModuleHandle(NULL);
	if (m_hInstance == NULL)
	{
		// TODO: change error code to suit your needs
		ICC_LOG_ERROR(m_pLog, "GetModuleHandle Error, ErrorCode: [%u]", ::GetLastError());

		return false;
	}

	WNDCLASS     wndclass;

	//定义窗口类
	wndclass.style = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc = WndProc;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = DLGWINDOWEXTRA;
	wndclass.hInstance = m_hInstance;
	wndclass.hIcon = LoadIcon(m_hInstance, m_szAppName);
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
	wndclass.lpszMenuName = NULL;
	wndclass.lpszClassName = m_szAppName;

	//注册窗口类
	if (!::RegisterClass(&wndclass))
	{
		ICC_LOG_ERROR(m_pLog, "RegisterClass Error, ErrorCode: [%u]", ::GetLastError());

		return false;
	}

	//创建对话框窗口
	m_hWnd = ::CreateWindow(m_szAppName, NULL,
		WS_DLGFRAME | WS_POPUP,
		0, 0,
		0, 0,
		NULL, NULL,
		m_hInstance, NULL);

	if (m_hWnd == NULL)
	{
		ICC_LOG_ERROR(m_pLog, "CreateWindow Error, ErrorCode: [%u]", ::GetLastError());

		return false;
	}

	ShowWindow(m_hWnd, SW_HIDE);

	return true;
}
void CAvayaSwitch::CloseMyWindows()
{
	if (m_hWnd != NULL)
	{
		PostMessage(m_hWnd, WM_DESTROY, 0, 0);
	}
}
void CAvayaSwitch::DestroyMyWindows()
{
	if (m_hWnd != NULL)
	{
		::CloseWindow(m_hWnd);
		::DestroyWindow(m_hWnd);
		::UnregisterClass(m_szAppName, m_hInstance);

		m_hWnd = NULL;
	}
}
#endif // WIN32

bool CAvayaSwitch::SetEventNotify()
{
#if defined(WIN32) || defined(WIN64)
	//设置事件通知
	RetCode_t iRet = acsEventNotify(m_acsHandle, m_hWnd, WM_TSAPI_MSG, FALSE);
	switch (iRet)
	{
	case ACSPOSITIVE_ACK://OK
		ICC_LOG_DEBUG(m_pLog, "SetEventNotify OK!");
		break;
	case ACSERR_BADHDL:
		ICC_LOG_ERROR(m_pLog, "SetEventNotify Bad Handle, Error: %s, ErrorCode: %d",
			CAvayaResult::Instance()->GetACSERRStr(iRet).c_str(), iRet);
		break;
	default:
		ICC_LOG_ERROR(m_pLog, "SetEventNotify Failed, Error: %s, ErrorCode: %d",
			CAvayaResult::Instance()->GetACSERRStr(iRet).c_str(), iRet);
		break;
	}

	return (iRet == ACSPOSITIVE_ACK);

#else
	CAvayaSwitch::m_nEventIsPending = 0;
	signal(SIGIO, HandleEvent);

	/* get its file descriptor */
	RetCode_t iRet = acsGetFile(m_acsHandle);
	if (iRet >= 0)
	{
		ICC_LOG_DEBUG(m_pLog, "SetEventNotify OK! Ret:[%d]", iRet);

		/* Indicate that this process should receive notification of pending input */
		fcntl(iRet, F_SETOWN, getpid());

		/* Enable asynchronous notification of pending I/O requests.*/
		fcntl(iRet, F_SETFL, FASYNC);

		return true;
	} 
	else
	{
		ICC_LOG_ERROR(m_pLog, "SetEventNotify Failed, Error: %s, ErrorCode: %d",
			CAvayaResult::Instance()->GetACSERRStr(iRet).c_str(), iRet);
	}

	return false;
#endif // WIN32
}

/* handleEvent() called when SIGIO is received */
int CAvayaSwitch::m_nEventIsPending = 0;
void CAvayaSwitch::HandleEvent(int p_nSig)
{
	CAvayaSwitch::m_nEventIsPending++;

	//ShowMsg("++++++++++ EventIsPending++ +++++++ " + std::to_string(CAvayaSwitch::m_nEventIsPending));
}
void CAvayaSwitch::ResetHandleEvent()
{
#if !defined(WIN32) && !defined(WIN64)
	CAvayaSwitch::m_nEventIsPending--;
	//ShowMsg("--------- EventIsPending-- ----------- " + std::to_string(CAvayaSwitch::m_nEventIsPending));
	ICC_LOG_LOWDEBUG(m_pLog, "EventIsPending--: [%d]", CAvayaSwitch::m_nEventIsPending);

	/* re-enable handler */
	signal(SIGIO, HandleEvent);
#endif // WIN32
}

bool CAvayaSwitch::GetEventPoll()
{
	RetCode_t iRet = 0;

	unsigned short wEventCount = 0;
	m_usEventSize = sizeof(CSTAEvent_t);
	m_privateData.length = ATT_MAX_PRIVATE_DATA;

	//ICC_LOG_LOWDEBUG(m_pLog, "--------------------- Begin acsGetEventPoll ------------------------");

	try{
		iRet = acsGetEventPoll(m_acsHandle,
			(void *)&m_cstaEvent,
			&m_usEventSize,
			//注意:这里是将ATTPrivateData_t*强制转换为PrivateData_t*
			//ATTPrivateData_t比PrivateData_t大，故兼容.
			(PrivateData_t*)&m_privateData,
			&wEventCount);
	}
	catch (...){
		std::string strErrorMsg = CAvayaResult::Instance()->GetErrorString(("acsGetEventPoll"), iRet);
		ICC_LOG_WARNING(m_pLog, "%s", strErrorMsg.c_str());
	}
	switch (iRet)
	{
	case ACSPOSITIVE_ACK:		// 获取成功ok
		if (wEventCount > 0)	// 还有剩余的消息．则继续取．
		{
#if defined(WIN32) || defined(WIN64)
			PostMessage(m_hWnd, WM_TSAPI_MSG, 0, 0);
#else
			//	多个事件同事到达时，信号可能会少一
			if (CAvayaSwitch::m_nEventIsPending == 1)
			{
				CAvayaSwitch::m_nEventIsPending++;
				//ShowMsg("EventCount " + std::to_string(wEventCount) + "++++++++++ EventIsPending++ +++++++ " + std::to_string(CAvayaSwitch::m_nEventIsPending));
				ICC_LOG_LOWDEBUG(m_pLog, "acsGetEventPoll EventCount: [%d] > 0, m_nEventIsPending++: [%d]", wEventCount, CAvayaSwitch::m_nEventIsPending);
			}
#endif //WIN32
		}
		break;
	case ACSERR_BADHDL:
		ICC_LOG_ERROR(m_pLog, "acsGetEventPoll: the acsHandle being used is not a valid handle for an active ACS stream !");
		break;
	case ACSERR_UNKNOWN:
		ICC_LOG_ERROR(m_pLog, "acsGetEventPoll: have an unknown Error !");
		break;
	case ACSERR_NOMESSAGE:
		ICC_LOG_ERROR(m_pLog, "acsGetEventPoll: no message !");
		break;
	case ACSERR_UBUFSMALL://分配的Buf太小．
		ICC_LOG_ERROR(m_pLog, "acsGetEventPoll: The allocated memory buf is too small !");
		break;
	default:
		ICC_LOG_ERROR(m_pLog, "acsGetEventPoll: have other error !");
		break;
	}
	//ICC_LOG_LOWDEBUG(m_pLog, "=================== End acsGetEventPoll =====================");

	return (iRet == ACSPOSITIVE_ACK);
}
int/*LRESULT*/ CAvayaSwitch::OnTSAPIEvent(/*WPARAM wParam, LPARAM lParam*/)
{
	//获得事件
	 if (!GetEventPoll())
		return 0;

	//分发处理事件
	if (m_privateData.length > 0)
	{
		ATTEvent_t attEvent;
		RetCode_t rec = attPrivateData(&m_privateData, &attEvent);
		if (rec == ACSPOSITIVE_ACK)
		{
			CAvayaEvent::Instance()->DispatchEvent(&m_cstaEvent, &attEvent);
			m_privateData.length = 0;

			return 0;
		}
		CAvayaEvent::Instance()->DispatchEvent(&m_cstaEvent, NULL);
		m_privateData.length = 0;

		return 0;
	}

	CAvayaEvent::Instance()->DispatchEvent(&m_cstaEvent, NULL);

	return 1;
}
int/*LRESULT*/ CAvayaSwitch::OnOpenStreamEvent(/*WPARAM wParam, LPARAM lParam*/)
{
	bool bTsapiInit = false;
	bool bConnectMaster = true; //先连主节点配置
	int nErrorCount = 0;

	if (CAvayaSwitch::Instance()->GetConnectSlaveAvayaFlag())
	{
		bConnectMaster = false;
	}

	int l_nFailedMaxCount = CAvayaSwitch::Instance()->GetInitFailedMaxCount();  //允许初始化失败的最大次数，超过则退出服务或者切备avaya

	while (!bTsapiInit && !m_bStopOpenStream)
	{
		
		if (bConnectMaster)
		{
			bTsapiInit = CAvayaSwitch::Instance()->Initialize();
		}
		else
		{
			bTsapiInit = CAvayaSwitch::Instance()->InitializeSlave();
		}

		if (!bTsapiInit)
		{
			ICC_LOG_ERROR(m_pLog, "Switch Initialize failed ,MasterFlag=%d,FailedMaxCount=%d ,nErrorCount=%d!!!", bConnectMaster, l_nFailedMaxCount, nErrorCount);
			ShowMsg("Switch Initialize failed !!!");
			if (l_nFailedMaxCount < 0)
			{
				SLEEP_SEC(3);
				continue;
			}

			if (++nErrorCount >= l_nFailedMaxCount)
			{
				ICC_LOG_ERROR(m_pLog, "Switch Initialize failed !!!");
				//exit(0);
				return 0;
			}
			SLEEP_SEC(3);
		}
	}

	ShowMsg("Switch Initialize Sucess");
	if (bTsapiInit)
	{
		//SLEEP_SEC(3);	//	初始化完成后需等待一会，否则打开流连接失败

		// 1, openStream
		bool l_bOpen = this->OpenAvayaStream(bConnectMaster);
		m_bOpeningStream = false;
		if (l_bOpen)
		{
			// 2, Set event
			this->SetEventNotify();
			//3, 设备监视,在 OpenStreamRef 实现
			return 1;
		}
		else
		{
			this->PostCloseStreamMessage();
			//this->PostOpenStreamMessage();
		}
	}


	return 0;
}

int/*LRESULT*/ CAvayaSwitch::OnCloseStreamEvent(/*WPARAM wParam, LPARAM lParam*/)
{
	if (this->CloseAvayaStream())
	{
		ICC_LOG_DEBUG(m_pLog, "Close switch stream success.");
	}
	else
	{
		ICC_LOG_ERROR(m_pLog, "Close switch stream fail !!!");
	}

	return 1;
}

void CAvayaSwitch::SetTSAPIConfigFile(bool p_bMasterFlag)
{
	std::string strAESServerIP;
	std::string strAESServerPort;
	if (p_bMasterFlag)
	{
		strAESServerIP = CSysConfig::Instance()->GetAEServerIP();
		strAESServerPort = CSysConfig::Instance()->GetAEServerPort();
	}
	else
	{
		strAESServerIP = CSysConfig::Instance()->GetSlaveAEServerIP();
		strAESServerPort = CSysConfig::Instance()->GetSlaveAEServerPort();
	}

	ICC_LOG_DEBUG(m_pLog, "SetTSAPIConfigFile ip:%s,port=%s!", strAESServerIP.c_str(), strAESServerPort.c_str());
	

#if defined(WIN32) || defined(WIN64)
	TCHAR szWindowsPath[MAX_PATH] = { 0 };
	UINT ui = GetWindowsDirectory(szWindowsPath, MAX_PATH - 1);
	_tcscat(szWindowsPath, _T("\\TSLIB.INI"));

	try
	{
		WIN32_FIND_DATA wfd;
		HANDLE hFind = FindFirstFile(szWindowsPath, &wfd);
		if (INVALID_HANDLE_VALUE != hFind)
		{
			DeleteFile(szWindowsPath);
		}

		FindClose(hFind);
	}
	catch (...)
	{
		//
	}

	std::wstring strServerIP = StringToWString(strAESServerIP.c_str(), strAESServerIP.size());
	std::wstring strServerPort = StringToWString(strAESServerPort.c_str(), strAESServerPort.size());
	WritePrivateProfileString(
		_T("Telephony Servers"),
		(LPCWSTR)strServerIP.c_str(),			//TsapiServerIP
		(LPCWSTR)strServerPort.c_str(),			//TsapiServerPort
		_T("TSLIB.INI"));
#else
	//Linux 
	std::string strFile = "/usr/lib/tslibrc";
	std::string strHost = strAESServerIP + std::string("\t") + strAESServerPort + std::string("\n");

	try
	{
		FILE *pFile = fopen(strFile.c_str(), "w+");
		if (!pFile)
		{
			ICC_LOG_ERROR(m_pLog, "Open file [%s] fialed!", strFile.c_str());
			return;
		}

		if (!fwrite(strHost.c_str(), sizeof(char), strHost.length(), pFile))
		{
			ICC_LOG_ERROR(m_pLog, "Writte file [%s] fialed!", strFile.c_str());
			fclose(pFile);
			return;
		}

		if (fclose(pFile))
		{
			ICC_LOG_ERROR(m_pLog, "Close file [%s] fialed!", strFile.c_str());
		}
	}
	catch (...)
	{
		//
	}
#endif // WIN32
}

bool CAvayaSwitch::Initialize()
{
	SetTSAPIConfigFile(true);

	return FindServerNameFromNet();
}

bool CAvayaSwitch::InitializeSlave()
{
	SetTSAPIConfigFile(false);

	return FindServerNameFromNet();
}

int CAvayaSwitch::GetInitFailedMaxCount()
{
	return CSysConfig::Instance()->GetInitFailedMaxCount();
}

void CAvayaSwitch::SetConnectSlaveAvayaFlag(bool l_bFlag)
{
	m_bConnectSlaveAvayaFlag = l_bFlag;
}

bool CAvayaSwitch::GetConnectSlaveAvayaFlag()
{
	return m_bConnectSlaveAvayaFlag;
}

void CAvayaSwitch::PostOpenStreamMessage()
{
	if (m_bOpeningStream)
	{
		ICC_LOG_WARNING(m_pLog, "Stream was opening, Do not Post open stream cmd");
		return;
	}

	m_bOpeningStream = true;
#if defined(WIN32) || defined(WIN64)
	SLEEP_SEC(1);	//	等待创建窗口

	if (m_hWnd)
	{
		PostMessage(m_hWnd, WM_OPEN_STREAM, 0, 0);
		ICC_LOG_DEBUG(m_pLog, "Post open stream cmd");
	}
#else
	OnOpenStreamEvent();
#endif // WIN32
}
void  CAvayaSwitch::PostCloseStreamMessage()
{
#if defined(WIN32) || defined(WIN64)
	if (m_hWnd)
	{
		PostMessage(m_hWnd, WM_CLOSE_STREAM, 0, 0);
	}
#else
	OnCloseStreamEvent();
#endif // WIN32
}

extern "C"
{
	bool AddToList(char* name, unsigned long lParam)
	{
		CAvayaSwitch* pAvayaSwitch = (CAvayaSwitch*)lParam;

		return pAvayaSwitch->AddServerName(name);
	}
}
bool CAvayaSwitch::FindServerNameFromNet()
{
	bool bReturn = false;

	//查找网络中的TSAPI服务器.
	RetCode_t iRet = ACSERR_APIVERDENIED;
	try
	{
		m_tsapiServerNameQueue.clear();
		iRet = acsEnumServerNames(ST_CSTA, (EnumServerNamesCB)AddToList, (unsigned long)this);
	}
	catch (...)
	{
		ICC_LOG_ERROR(m_pLog, "FindServerNameFromNet Get TSAPI Server Name have an exception !");

		return false;
	}

	std::string strTSAPIServer = GetServerNameFromList();
	switch (iRet)
	{
	case ACSPOSITIVE_ACK:
	{
		bReturn = true;

		ICC_LOG_DEBUG(m_pLog, "FindServerNameFromNetGet TSAPI Server Name : %s",
			strTSAPIServer.c_str());
	}
	break;
	case ACSERR_NOSERVER:
	{
		bReturn = false;
		
		ICC_LOG_ERROR(m_pLog, "FindServerNameFromNet Can not find the TSAPI Server, Please install the TSAPI Server or check the network,strTSAPIServer=%s!", strTSAPIServer.c_str());
	}
	break;
	case ACSERR_UNKNOWN:
	{
		bReturn = false;

		ICC_LOG_ERROR(m_pLog, "FindServerNameFromNet Have an unknown error when Get the TSAPI Server Name. Please check the network and Avaya switches AEServer server!");
	}
	break;
	default:
	{
		bReturn = false;
		
		ICC_LOG_ERROR(m_pLog, "FindServerNameFromNet Unknown Error!");
	}
	break;
	}

	return bReturn;
}
bool CAvayaSwitch::AddServerName(const std::string& strServerName)
{
	m_tsapiServerNameQueue.push_back(strServerName);

	return true;
}

std::string CAvayaSwitch::GetServerNameFromList()
{
	std::string strServerName = "";

	std::list<std::string>::const_iterator csit = m_tsapiServerNameQueue.begin();
	if (csit != m_tsapiServerNameQueue.end())
	{
		strServerName = *csit;
	}

	return strServerName;
}

bool CAvayaSwitch::OpenAvayaStream(bool p_bMasterFlag)
{
	//查找到TSAPI服务后,开始建立与TSAPI服务间的通讯流------------------------------------
	std::string strTSAPIServer = GetServerNameFromList();

	//---------------------------------------------------
	/*
	注意:在OpenStream时，必须设置ATTPrivateData_t，若设置为NULL不能使用att开头
	的私有函数，例如:attSingleStepConference，若调用则会出现:GENERIC_UNSPECIFIED_REJECTION
	反馈信息。
	*/
	std::string strVerSion = "VERSION";
	STRNCPY(m_privateData.vendor, strVerSion.c_str(), strVerSion.size());
	m_privateData.data[0] = PRIVATE_DATA_ENCODING;

	//"2-6"值,是从TSAPI Exerciser测试程序中的菜单说明中获得。
	attMakeVersionString("2-6", &(m_privateData.data[1]));
	m_privateData.length = strlen(&m_privateData.data[1]) + 2;

	/*
	整个privateData的数据可从TS Spy监视程序中可以获得其Hex进值的
	发送数据:

	Private Data ::=
	{
	vendor "VERSION"
	length 9
	data
	{
	00 45 43 53 23 32 2d 36 00
	}
	}
	*/

	//---------------------------------------------------
	std::string strAesUserName;
	std::string strAesPsw;
	if (p_bMasterFlag)
	{
		strAesUserName = CSysConfig::Instance()->GetAESUserName();
		strAesPsw = CSysConfig::Instance()->GetAESPassword();
	}
	else
	{
		strAesUserName = CSysConfig::Instance()->GetSlaveAESUserName();
		strAesPsw = CSysConfig::Instance()->GetSlaveAESPassword();
	}
	

	ServerID_t cServerName = {0};
	STRNCPY(cServerName, strTSAPIServer.c_str(), strTSAPIServer.size());

	LoginID_t cUserName = { 0 };
	STRNCPY(cUserName, strAesUserName.c_str(), strAesUserName.size());

	Passwd_t cPsw = { 0 };
	STRNCPY(cPsw, strAesPsw.c_str(), strAesPsw.size());

	AppName_t cAppname = { 0 };
	std::string strServerName = "TSAPI Server";
	STRNCPY(cAppname, strServerName.c_str(), strServerName.size());

	Version_t cVersion = { 0 };
	std::string strVersion = "TS1-3";
	STRNCPY(cVersion, strVersion.c_str(), strVersion.size());

	RetCode_t iRet = 0;
	try{
		iRet = acsOpenStream((ACSHandle_t*)&m_acsHandle, APP_GEN_ID,
			(InvokeID_t)this, ST_CSTA,
			(ServerID_t*)&cServerName, (LoginID_t*)&cUserName,
			(Passwd_t*)&cPsw, (AppName_t*)&cAppname,
			ACS_LEVEL1, (Version_t*)&cVersion,
			10, 5, 50, 5, (PrivateData_t*)&m_privateData);
		switch (iRet)
		{
		case ACSPOSITIVE_ACK:
			ICC_LOG_DEBUG(m_pLog, "Open Stream Sucess!");
			break;
		case ACSERR_APIVERDENIED:
			ICC_LOG_ERROR(m_pLog, "Open Stream Failed, API Version Denied!");
			break;
		case ACSERR_BADPARAMETER:
			ICC_LOG_ERROR(m_pLog, "Open Stream Failed, Bad Parameter!");
			break;
		case ACSERR_NODRIVER:
			ICC_LOG_ERROR(m_pLog, "Open Stream Failed, No Driver!");
			break;
		case ACSERR_NOSERVER:
			ICC_LOG_ERROR(m_pLog, "Open Stream Failed, No Server!");
			break;
		case ACSERR_NORESOURCE:
			ICC_LOG_ERROR(m_pLog, "Open Stream Failed, No Resource!");
			break;
		default:
			ICC_LOG_ERROR(m_pLog, "Open Stream Failed, Error Code: %d!", iRet);
			break;
		}
	}
	catch (...){
		std::string strErrorMsg = CAvayaResult::Instance()->GetErrorString(("acsOpenStream"), iRet);
		ICC_LOG_WARNING(m_pLog, "%s", strErrorMsg.c_str());
	}

	return iRet == ACSPOSITIVE_ACK;
}

bool CAvayaSwitch::CloseAvayaStream()
{
	RetCode_t iRet = 0;

	try{
		iRet = acsCloseStream(m_acsHandle, (InvokeID_t)this, NULL);
	}
	catch (...){
		std::string strErrorMsg = CAvayaResult::Instance()->GetErrorString(("acsCloseStream"), iRet);
		ICC_LOG_WARNING(m_pLog, "%s", strErrorMsg.c_str());
	}

	return (iRet == ACSPOSITIVE_ACK);
}

bool CAvayaSwitch::AbortStream(ACSHandle_t acsHandle,
	PrivateData_t	*priv)
{
	int iRet = 0;

	try{
		iRet = acsAbortStream(acsHandle, priv);
	}
	catch (...){
	//	ShowMsg(CAvayaResult::Instance()->GetErrorString(("acsAbortStream"), iRet));
	}

	return iRet == ACSPOSITIVE_ACK;
}


bool CAvayaSwitch::FlushEventQueue(ACSHandle_t acsHandle)
{
	int iRet = 0;

	try{
		iRet = acsFlushEventQueue(acsHandle);
	}
	catch (...){
	//	ShowMsg(CAvayaResult::Instance()->GetErrorString(("acsFlushEventQueue"), iRet));
	}

	return iRet == ACSPOSITIVE_ACK;
}


int CAvayaSwitch::GetEventBlock(ACSHandle_t acsHandle,
	void			*eventBuf,
	unsigned short	*eventBufSize,
	PrivateData_t	*privData,
	unsigned short	*numEvents)
{
	int iRet = 0;

	try{
		iRet = acsGetEventBlock(acsHandle, eventBuf, eventBufSize,
			privData, numEvents);
	}
	catch (...){
	//	ShowMsg(CAvayaResult::Instance()->GetErrorString(("acsGetEventBlock"), iRet));
	}

	return iRet;
}

//////////////////////////////////////////////////////////////////////////
bool CAvayaSwitch::AlternateCall(const std::string& strDeviceNum, long lActiveCallRefId, long lHoldCallRefId, InvokeID_t invokeId)
{
	int iRet = 0;
	ConnectionID_t activeConnect;
	memset(&activeConnect, 0, sizeof(ConnectionID_t));
	activeConnect.callID = lActiveCallRefId;
	activeConnect.devIDType = STATIC_ID;
	STRNCPY(activeConnect.deviceID, strDeviceNum.c_str(), strDeviceNum.size());

	ConnectionID_t heldConnect;
	memset(&heldConnect, 0, sizeof(ConnectionID_t));
	heldConnect.callID = lHoldCallRefId;
	heldConnect.devIDType = STATIC_ID;
	STRNCPY(heldConnect.deviceID, strDeviceNum.c_str(), strDeviceNum.size());

	try{
		iRet = cstaAlternateCall(m_acsHandle, 
			(InvokeID_t)invokeId,
			(ConnectionID_t *)&activeConnect, 
			(ConnectionID_t *)&heldConnect,
			NULL);
	}
	catch (...){
		std::string strErrorMsg = CAvayaResult::Instance()->GetErrorString(("cstaAlternateCall"), iRet);
		ICC_LOG_WARNING(m_pLog, "%s", strErrorMsg.c_str());
	}

	return iRet == ACSPOSITIVE_ACK;
}

bool CAvayaSwitch::AnswerCall(const std::string& strDeviceNum, long lCallRefId, InvokeID_t invokeId)
{
	int iRet = 0;

	ConnectionID_t connectId;
	memset(&connectId, 0, sizeof(ConnectionID_t));
	STRNCPY(connectId.deviceID, strDeviceNum.c_str(), strDeviceNum.size());
	connectId.callID = lCallRefId;
	connectId.devIDType = STATIC_ID;

	try{
		iRet = cstaAnswerCall(m_acsHandle,
			(InvokeID_t)invokeId,
			(ConnectionID_t *)&connectId, 
			NULL);
	}
	catch (...){
		std::string strErrorMsg = CAvayaResult::Instance()->GetErrorString(("cstaAnswerCall"), iRet);
		ICC_LOG_WARNING(m_pLog, "%s", strErrorMsg.c_str());
	}

	return iRet == ACSPOSITIVE_ACK;
}

bool CAvayaSwitch::CallCompletion(ACSHandle_t acsHandle,
	InvokeID_t invokeId,
	Feature_t feature,
	ConnectionID_t	*call)
{
	int iRet = 0;

	try{
		iRet = cstaCallCompletion(acsHandle, invokeId, feature, call, NULL);
	}
	catch (...){
	//	ShowMsg(CAvayaResult::Instance()->GetErrorString(("cstaCallCompletion"), iRet));	
	}

	return iRet == ACSPOSITIVE_ACK;
}


bool CAvayaSwitch::ClearCall(ACSHandle_t acsHandle,
	InvokeID_t invokeId,
	ConnectionID_t	*call)
{
	int iRet = 0;

	try{
		iRet = cstaClearCall(acsHandle, invokeId, call, NULL);
	}
	catch (...){
	//	ShowMsg(CAvayaResult::Instance()->GetErrorString(("cstaClearCall"), iRet));	
	}

	return iRet == ACSPOSITIVE_ACK;
}

bool CAvayaSwitch::ClearCall(const std::string& strDeviceNum, long lCallRefId, InvokeID_t invokeId)
{
	int iRet = 0;

	ConnectionID_t connectId;
	memset(&connectId, 0, sizeof(ConnectionID_t));
	connectId.callID = lCallRefId;
	connectId.devIDType = STATIC_ID;
	STRNCPY(connectId.deviceID, strDeviceNum.c_str(), strDeviceNum.size());

	try{
		iRet = cstaClearCall(m_acsHandle,
			(InvokeID_t)invokeId,
			(ConnectionID_t *)&connectId,
			NULL);
	}
	catch (...){
		std::string strErrorMsg = CAvayaResult::Instance()->GetErrorString(("cstaClearCall"), iRet);
		ICC_LOG_WARNING(m_pLog, "%s", strErrorMsg.c_str());
	}

	return iRet == ACSPOSITIVE_ACK;
}

bool CAvayaSwitch::ClearConnection(const std::string& strDeviceNum, long lCallRefId, InvokeID_t invokeId,bool isDynamicId)
{
	int iRet = 0;

	ConnectionID_t connectId;
	memset(&connectId, 0, sizeof(ConnectionID_t));
	connectId.callID = lCallRefId;
	if (isDynamicId)
	{
		connectId.devIDType = DYNAMIC_ID;
	}
	else
	{
		connectId.devIDType = STATIC_ID;
	}
	
	STRNCPY(connectId.deviceID, strDeviceNum.c_str(), strDeviceNum.size());

	try{
		iRet = cstaClearConnection(m_acsHandle, 
			(InvokeID_t)invokeId,
			(ConnectionID_t *)&connectId, 
			NULL);
	}
	catch (...){
		std::string strErrorMsg = CAvayaResult::Instance()->GetErrorString(("cstaClearConnection"), iRet);
		ICC_LOG_WARNING(m_pLog, "%s", strErrorMsg.c_str());
	}

	return iRet == ACSPOSITIVE_ACK;
}


bool CAvayaSwitch::ConferenceCall(ACSHandle_t acsHandle,
	InvokeID_t invokeId,
	ConnectionID_t* heldCall,
	ConnectionID_t* activeCall)
{
	int iRet = 0;

	try{
		iRet = cstaConferenceCall(acsHandle, invokeId, heldCall, activeCall, NULL);
	}
	catch (...){
	//	ShowMsg(CAvayaResult::Instance()->GetErrorString(("cstaConferenceCall"), iRet));
	}

	return iRet == ACSPOSITIVE_ACK;
}
bool CAvayaSwitch::ConferenceCall(const std::string& strDeviceNum, long lHeldCallRefId, long lActiveCallRefId, InvokeID_t invokeId)
{
	int iRet = 0;

	ConnectionID_t heldCall;
	memset(&heldCall, 0, sizeof(ConnectionID_t));
	STRNCPY(heldCall.deviceID, strDeviceNum.c_str(), strDeviceNum.size());
	heldCall.callID = lHeldCallRefId;
	heldCall.devIDType = STATIC_ID;

	ConnectionID_t activeCall;
	memset(&activeCall, 0, sizeof(ConnectionID_t));
	STRNCPY(activeCall.deviceID, strDeviceNum.c_str(), strDeviceNum.size());
	activeCall.callID = lActiveCallRefId;
	activeCall.devIDType = STATIC_ID;

	try{
		iRet = cstaConferenceCall(m_acsHandle, 
			(InvokeID_t)invokeId, 
			(ConnectionID_t *)&heldCall, 
			(ConnectionID_t *)&activeCall, 
			NULL);
	}
	catch (...){
		std::string strErrorMsg = CAvayaResult::Instance()->GetErrorString(("cstaConferenceCall"), iRet);
		ICC_LOG_WARNING(m_pLog, "%s", strErrorMsg.c_str());
	}

	return iRet == ACSPOSITIVE_ACK;
}

bool CAvayaSwitch::ConsultationCall(const std::string& strDeviceNum, const std::string& strCalledId,
	long lCallRefId, InvokeID_t invokeId)
{
	int iRet = 0;

	ConnectionID_t connectId;
	memset(&connectId, 0, sizeof(ConnectionID_t));
	connectId.callID = lCallRefId;
	connectId.devIDType = STATIC_ID;
	STRNCPY(connectId.deviceID, strDeviceNum.c_str(), strDeviceNum.size());

	DeviceID_t calledDevice = { 0 };
	STRNCPY(calledDevice, strCalledId.c_str(), strCalledId.size());

	try{
		iRet = cstaConsultationCall(m_acsHandle,
			(InvokeID_t)invokeId,
			(ConnectionID_t *)&connectId, 
			(DeviceID_t *)&calledDevice, 
			NULL);
	}
	catch (...){
		std::string strErrorMsg = CAvayaResult::Instance()->GetErrorString(("cstaConsultationCall"), iRet);
		ICC_LOG_WARNING(m_pLog, "%s", strErrorMsg.c_str());
	}

	return iRet == ACSPOSITIVE_ACK;
}

bool CAvayaSwitch::DeflectCall(const std::string& strDeviceNum, const std::string& strCalledId,
	long lCallRefId, InvokeID_t invokeId)
{
	int iRet = 0;

	ConnectionID_t connectId;
	memset(&connectId, 0, sizeof(ConnectionID_t));
	connectId.callID = lCallRefId;
	connectId.devIDType = STATIC_ID;
	STRNCPY(connectId.deviceID, strDeviceNum.c_str(), strDeviceNum.size());

	DeviceID_t calledDevice = { 0 };
	STRNCPY(calledDevice, strCalledId.c_str(), strCalledId.size());

	try{
		iRet = cstaDeflectCall(m_acsHandle, 
			(InvokeID_t)invokeId,
			(ConnectionID_t *)&connectId, 
			(DeviceID_t *)&calledDevice, 
			NULL);
	}
	catch (...){
		std::string strErrorMsg = CAvayaResult::Instance()->GetErrorString(("cstaDeflectCall"), iRet);
		ICC_LOG_WARNING(m_pLog, "%s", strErrorMsg.c_str());
	}

	return iRet == ACSPOSITIVE_ACK;
}


bool CAvayaSwitch::GroupPickupCall(const std::string& strDeviceNum, const std::string& strPickupId,
	long lCallRefId, InvokeID_t invokeId)
{
	int iRet = 0;

	ConnectionID_t deflectCall;
	memset(&deflectCall, 0, sizeof(ConnectionID_t));
	deflectCall.callID = lCallRefId;
	deflectCall.devIDType = STATIC_ID;
	STRNCPY(deflectCall.deviceID, strDeviceNum.c_str(), strDeviceNum.size());

	DeviceID_t pickupDevice = {0};
	STRNCPY(pickupDevice, strPickupId.c_str(), strPickupId.size());
	try{
		iRet = cstaGroupPickupCall(m_acsHandle, 
			(InvokeID_t)invokeId,
			(ConnectionID_t *)&deflectCall, 
			(DeviceID_t *)&pickupDevice, 
			NULL);
	}
	catch (...){
		std::string strErrorMsg = CAvayaResult::Instance()->GetErrorString(("cstaGroupPickupCall"), iRet);
		ICC_LOG_WARNING(m_pLog, "%s", strErrorMsg.c_str());
	}

	return iRet == ACSPOSITIVE_ACK;
}



bool CAvayaSwitch::HoldCall(const std::string& strDeviceNum, long lCallRefId, InvokeID_t invokeId)
{
	int iRet = 0;

	ConnectionID_t activeCall;
	memset(&activeCall, 0, sizeof(ConnectionID_t));
	activeCall.callID = lCallRefId;
	activeCall.devIDType = STATIC_ID;
	STRNCPY(activeCall.deviceID, strDeviceNum.c_str(), strDeviceNum.size());

	try{
		iRet = cstaHoldCall(m_acsHandle, 
			(InvokeID_t)invokeId, 
			(ConnectionID_t *)&activeCall, 
			false,
			NULL);
	}
	catch (...){
		std::string strErrorMsg = CAvayaResult::Instance()->GetErrorString(("cstaHoldCall"), iRet);
		ICC_LOG_WARNING(m_pLog, "%s", strErrorMsg.c_str());
	}

	return iRet == ACSPOSITIVE_ACK;
}


bool CAvayaSwitch::MakeCall(const std::string& strCallerId, const std::string& strCalledId, InvokeID_t invokeId)
{
	int iRet = 0;

	DeviceID_t	deviceCallerId_t = { 0 };
	DeviceID_t	deviceCalledId_t = { 0 };
	STRNCPY(deviceCallerId_t, strCallerId.c_str(), strCallerId.size());
	STRNCPY(deviceCalledId_t, strCalledId.c_str(), strCalledId.size());
	
	try{
		iRet = cstaMakeCall(m_acsHandle,
			(InvokeID_t)invokeId,
			(DeviceID_t*)&deviceCallerId_t,
			(DeviceID_t*)&deviceCalledId_t, 
			NULL);
	}
	catch (...){
		std::string strErrorMsg = CAvayaResult::Instance()->GetErrorString(("cstaMakeCall"), iRet);
		ICC_LOG_WARNING(m_pLog, "%s", strErrorMsg.c_str());
	}

	return iRet == ACSPOSITIVE_ACK;
}


bool CAvayaSwitch::MakePredictiveCall(ACSHandle_t acsHandle,
	InvokeID_t invokeId,
	const std::string& strCallerId,
	const std::string& strCalledId)
{
	int iRet = 0;

	DeviceID_t callerId = {0};
	DeviceID_t calledId = {0};
	STRNCPY(callerId, strCallerId.c_str(), strCallerId.size());
	STRNCPY(calledId, strCalledId.c_str(), strCalledId.size());
	
	try{
		iRet = cstaMakePredictiveCall(acsHandle, invokeId,
			&callerId, &calledId,
			AS_CALL_ESTABLISHED, NULL);
	}
	catch (...){
	//	ShowMsg(CAvayaResult::Instance()->GetErrorString(("cstaMakePredictiveCall"), iRet));	
	}

	return iRet == ACSPOSITIVE_ACK;
}


bool CAvayaSwitch::PickupCall(const std::string& strDeviceNum, const std::string& strPickupId,
	long lCallRefId, InvokeID_t invokeId)
{
	int iRet = 0;

	ConnectionID_t deflectCall;
	memset(&deflectCall, 0, sizeof(ConnectionID_t));
	deflectCall.callID = lCallRefId;
	deflectCall.devIDType = STATIC_ID;
	STRNCPY(deflectCall.deviceID, strPickupId.c_str(), strPickupId.size());

	DeviceID_t pickupDevice = { 0 };
	STRNCPY(pickupDevice, strDeviceNum.c_str(), strDeviceNum.size());
	
	try{
		iRet = cstaPickupCall(m_acsHandle, 
			(InvokeID_t)invokeId,
			(ConnectionID_t *)&deflectCall, 
			(DeviceID_t *)&pickupDevice,
			NULL);
	}
	catch (...){
		std::string strErrorMsg = CAvayaResult::Instance()->GetErrorString(("cstaPickupCall"), iRet);
		ICC_LOG_WARNING(m_pLog, "%s", strErrorMsg.c_str());
	}

	return iRet == ACSPOSITIVE_ACK;
}


bool CAvayaSwitch::ReconnectCall(const std::string& strDeviceNum, long lActiveCallRefId, long lHeldCallRefId, InvokeID_t invokeId)
{
	int iRet = 0;

	ConnectionID_t activeCall;
	memset(&activeCall, 0, sizeof(ConnectionID_t));
	activeCall.callID = lActiveCallRefId;
	activeCall.devIDType = STATIC_ID;
	STRNCPY(activeCall.deviceID, strDeviceNum.c_str(), strDeviceNum.size());

	ConnectionID_t heldCall;
	memset(&heldCall, 0, sizeof(ConnectionID_t));
	heldCall.callID = lHeldCallRefId;
	heldCall.devIDType = STATIC_ID;
	STRNCPY(heldCall.deviceID, strDeviceNum.c_str(), strDeviceNum.size());

	try{
		iRet = cstaReconnectCall(m_acsHandle, 
			(InvokeID_t)invokeId,
			(ConnectionID_t *)&activeCall, 
			(ConnectionID_t *)&heldCall,
			NULL);
	}
	catch (...){
		std::string strErrorMsg = CAvayaResult::Instance()->GetErrorString(("cstaReconnectCall"), iRet);
		ICC_LOG_WARNING(m_pLog, "%s", strErrorMsg.c_str());
	}

	return iRet == ACSPOSITIVE_ACK;
}

bool CAvayaSwitch::RetrieveCall(const std::string& strDeviceNum, long lCallRefId, InvokeID_t invokeId)
{
	int iRet = 0;

	ConnectionID_t heldCall;
	memset(&heldCall, 0, sizeof(ConnectionID_t));
	heldCall.callID = lCallRefId;
	heldCall.devIDType = STATIC_ID;
	STRNCPY(heldCall.deviceID, strDeviceNum.c_str(), strDeviceNum.size());

	try{
		iRet = cstaRetrieveCall(m_acsHandle, 
			(InvokeID_t)invokeId,
			(ConnectionID_t *)&heldCall, 
			NULL);
	}
	catch (...){
		std::string strErrorMsg = CAvayaResult::Instance()->GetErrorString(("cstaRetrieveCall"), iRet);
		ICC_LOG_WARNING(m_pLog, "%s", strErrorMsg.c_str());
	}

	return iRet == ACSPOSITIVE_ACK;
}

bool CAvayaSwitch::TransferCall(const std::string& strDeviceNum,long lActiveCallRefId, long lHeldCallRefId, InvokeID_t invokeId)
{
	int iRet = 0;

	ConnectionID_t activeCall;
	memset(&activeCall, 0, sizeof(ConnectionID_t));
	activeCall.callID = lActiveCallRefId;
	activeCall.devIDType = STATIC_ID;
	STRNCPY(activeCall.deviceID, strDeviceNum.c_str(), strDeviceNum.size());

	ConnectionID_t heldCall;
	memset(&heldCall, 0, sizeof(ConnectionID_t));
	heldCall.callID = lHeldCallRefId;
	heldCall.devIDType = STATIC_ID;
	STRNCPY(heldCall.deviceID, strDeviceNum.c_str(), strDeviceNum.size());

	try{
		iRet = cstaTransferCall(m_acsHandle,
			(InvokeID_t)invokeId,
			(ConnectionID_t *)&heldCall, 
			(ConnectionID_t *)&activeCall,
			NULL);
	}
	catch (...){
		std::string strErrorMsg = CAvayaResult::Instance()->GetErrorString(("cstaTransferCall"), iRet);
		ICC_LOG_WARNING(m_pLog, "%s", strErrorMsg.c_str());
	}

	return iRet == ACSPOSITIVE_ACK;
}


bool CAvayaSwitch::SetMsgWaitingInd(ACSHandle_t acsHandle,
	InvokeID_t invokeId,
	const std::string& strDeviceNum,
	bool bOn)
{
	int iRet = 0;

	DeviceID_t deviceId = {0};
	STRNCPY(deviceId, strDeviceNum.c_str(), strDeviceNum.size());
	
	try{
		iRet = cstaSetMsgWaitingInd(acsHandle, invokeId,
			&deviceId, bOn, NULL);
	}
	catch (...){
	//	ShowMsg(CAvayaResult::Instance()->GetErrorString(("cstaSetMsgWaitingInd"), iRet));	
	}

	return iRet == ACSPOSITIVE_ACK;
}


bool CAvayaSwitch::SetDoNotDisturb(ACSHandle_t acsHandle,
	InvokeID_t invokeId,
	const std::string& strDeviceNum,
	bool bOn)
{
	int iRet = 0;

	DeviceID_t deviceId = { 0 };
	STRNCPY(deviceId, strDeviceNum.c_str(), strDeviceNum.size());
	
	try{
		iRet = cstaSetDoNotDisturb(acsHandle, invokeId,
			&deviceId, bOn, NULL);
	}
	catch (...){
	//	ShowMsg(CAvayaResult::Instance()->GetErrorString(("cstaSetDoNotDisturb"), iRet));
	}

	return iRet == ACSPOSITIVE_ACK;
}


bool CAvayaSwitch::SetForwarding(ACSHandle_t acsHandle,
	InvokeID_t invokeId,
	const std::string& strDeviceNum,
	ForwardingType_t forwardingType,
	bool bOn,
	const std::string& strDest)
{
	int iRet = 0;

	DeviceID_t deviceId = { 0 };
	DeviceID_t destId = { 0 };
	STRNCPY(deviceId, strDeviceNum.c_str(), strDeviceNum.size());
	STRNCPY(destId, strDest.c_str(), strDest.size());
	
	try{
		iRet = cstaSetForwarding(acsHandle, invokeId,
			&deviceId, forwardingType,
			bOn, &destId, NULL);
	}
	catch (...){
	//	ShowMsg(CAvayaResult::Instance()->GetErrorString(("cstaSetForwarding"), iRet));
	}

	return iRet == ACSPOSITIVE_ACK;
}

bool CAvayaSwitch::SetAgentState(const std::string& strDeviceNum, const std::string& strGrp,
	const std::string& strAgent, const std::string& strPsw,
	int	iAgentMode, InvokeID_t invokeId)
{
	int iRet = 0;

	DeviceID_t	deviceId = { 0 };
	AgentGroup_t grpId = { 0 };
	AgentID_t agentId = { 0 };
	AgentPassword_t pswId = { 0 };

	STRNCPY(deviceId, strDeviceNum.c_str(), strDeviceNum.size());
	STRNCPY(grpId, strGrp.c_str(), strGrp.size());
	STRNCPY(agentId, strAgent.c_str(), strAgent.size());
	STRNCPY(pswId, strPsw.c_str(), strPsw.size());
	
	try{
		iRet = cstaSetAgentState(m_acsHandle, 
			(InvokeID_t)invokeId,
			(DeviceID_t*)&deviceId, 
			(AgentMode_t)iAgentMode, 
			(AgentID_t*)&agentId,
			(AgentGroup_t*)""/*&grpId*/, 
			(AgentPassword_t*)&pswId,
			NULL);
	}
	catch (...){
		std::string strErrorMsg = CAvayaResult::Instance()->GetErrorString(("cstaSetAgentState"), iRet);
		ICC_LOG_WARNING(m_pLog, "%s", strErrorMsg.c_str());
	}

	return iRet == ACSPOSITIVE_ACK;
}

bool CAvayaSwitch::QueryMsgWaitingInd(ACSHandle_t acsHandle,
	InvokeID_t invokeId,
	const std::string& strDeviceNum)
{
	int iRet = 0;

	DeviceID_t deviceId = { 0 };
	STRNCPY(deviceId, strDeviceNum.c_str(), strDeviceNum.size());
	
	try{
		iRet = cstaQueryMsgWaitingInd(acsHandle, invokeId,
			&deviceId, NULL);
	}
	catch (...){
	//	ShowMsg(CAvayaResult::Instance()->GetErrorString(("cstaQueryMsgWaitingInd"), iRet));	
	}

	return iRet == ACSPOSITIVE_ACK;
}


bool CAvayaSwitch::QueryDoNotDisturb(ACSHandle_t acsHandle,
	InvokeID_t invokeId,
	const std::string& strDeviceNum)
{
	int iRet = 0;

	DeviceID_t deviceId = { 0 };
	STRNCPY(deviceId, strDeviceNum.c_str(), strDeviceNum.size());
	
	try{
		iRet = cstaQueryDoNotDisturb(acsHandle, invokeId,
			&deviceId, NULL);
	}
	catch (...){
	//	ShowMsg(CAvayaResult::Instance()->GetErrorString(("cstaQueryDoNotDisturb"), iRet));
	}

	return iRet == ACSPOSITIVE_ACK;
}


bool CAvayaSwitch::QueryForwarding(ACSHandle_t acsHandle,
	InvokeID_t invokeId,
	const std::string& strDeviceNum)
{
	int iRet = 0;

	DeviceID_t deviceId = { 0 };
	STRNCPY(deviceId, strDeviceNum.c_str(), strDeviceNum.size());
	
	try{
		iRet = cstaQueryForwarding(acsHandle, invokeId,
			&deviceId, NULL);
	}
	catch (...){
	//	ShowMsg(CAvayaResult::Instance()->GetErrorString(("cstaQueryForwarding"), iRet));
	}

	return iRet == ACSPOSITIVE_ACK;
}


bool CAvayaSwitch::QueryAgentState(ACSHandle_t acsHandle,
	InvokeID_t invokeId,
	const std::string& strDeviceNum)
{
	int iRet = 0;

	DeviceID_t deviceId = { 0 };
	STRNCPY(deviceId, strDeviceNum.c_str(), strDeviceNum.size());
	
	try{
		iRet = cstaQueryAgentState(acsHandle, invokeId,
			&deviceId, NULL);
	}
	catch (...){
	//	ShowMsg(CAvayaResult::Instance()->GetErrorString(("cstaQueryAgentState"), iRet));	
	}

	return iRet == ACSPOSITIVE_ACK;
}

bool CAvayaSwitch::QueryAgentState(const std::string& strDeviceNum, InvokeID_t invokeId)
{
	int iRet = 0;

	DeviceID_t deviceId = { 0 };
	STRNCPY(deviceId, strDeviceNum.c_str(), strDeviceNum.size());

	try{
		iRet = cstaQueryAgentState(m_acsHandle,
			(InvokeID_t)invokeId,
			(DeviceID_t*)&deviceId,
			NULL);
	}
	catch (...){
		std::string strErrorMsg = CAvayaResult::Instance()->GetErrorString(("cstaQueryAgentState"), iRet);
		ICC_LOG_WARNING(m_pLog, "%s", strErrorMsg.c_str());
	}

	return iRet == ACSPOSITIVE_ACK;
}

bool CAvayaSwitch::QueryLastNumber(ACSHandle_t acsHandle,
	InvokeID_t invokeId,
	const std::string& strDeviceNum)
{
	int iRet = 0;

	DeviceID_t deviceId = { 0 };
	STRNCPY(deviceId, strDeviceNum.c_str(),strDeviceNum.size());
	
	try{
		iRet = cstaQueryLastNumber(acsHandle, invokeId,
			&deviceId, NULL);
	}
	catch (...){
	//	ShowMsg(CAvayaResult::Instance()->GetErrorString(("cstaQueryLastNumber"), iRet));	
	}

	return iRet == ACSPOSITIVE_ACK;
}

bool CAvayaSwitch::QueryDeviceInfo(const std::string& strDeviceNum, InvokeID_t invokeId)
{
	int iRet = 0;

	DeviceID_t deviceId = { 0 };
	STRNCPY(deviceId, strDeviceNum.c_str(), strDeviceNum.size());

	try{
		iRet = cstaQueryDeviceInfo(m_acsHandle,
			(InvokeID_t)invokeId,
			(DeviceID_t*)&deviceId,
			NULL);
	}
	catch (...){
		std::string strErrorMsg = CAvayaResult::Instance()->GetErrorString(("cstaQueryAgentState"), iRet);
		ICC_LOG_WARNING(m_pLog, "%s", strErrorMsg.c_str());
	}

	return iRet == ACSPOSITIVE_ACK;
}
bool CAvayaSwitch::QueryDeviceInfo(ACSHandle_t acsHandle,
	InvokeID_t invokeId,
	const std::string& strDeviceNum)
{
	int iRet = 0;

	DeviceID_t deviceId = { 0 };
	STRNCPY(deviceId, strDeviceNum.c_str(), strDeviceNum.size());
	
	try{
		iRet = cstaQueryDeviceInfo(acsHandle, invokeId,
			&deviceId, NULL);
	}
	catch (...){
	//	ShowMsg(CAvayaResult::Instance()->GetErrorString(("cstaQueryDeviceInfo"), iRet));
	}

	return iRet == ACSPOSITIVE_ACK;
}


bool CAvayaSwitch::MonitorDevice(const std::string& strDeviceNum, InvokeID_t invokeId)
{
	int iRet = 0;

	DeviceID_t deviceId = { 0 };
	STRNCPY(deviceId, strDeviceNum.c_str(), strDeviceNum.size());

	CSTAMonitorFilter_t noFilter;
	noFilter.call = 0;
	noFilter.feature = 0;
	noFilter.agent = 0;
	noFilter.maintenance = 0;
	noFilter.privateFilter = 0;

	try{
		iRet = cstaMonitorDevice(m_acsHandle, 
			(InvokeID_t)invokeId,
			(DeviceID_t*)&deviceId, 
			(CSTAMonitorFilter_t*)&noFilter,
			NULL);
	}
	catch (...){
		std::string strErrorMsg = CAvayaResult::Instance()->GetErrorString(("cstaMonitorDevice"), iRet);
		ICC_LOG_WARNING(m_pLog, "%s", strErrorMsg.c_str());
	}

	return iRet == ACSPOSITIVE_ACK;
}


bool CAvayaSwitch::MonitorCall(ACSHandle_t acsHandle,
	InvokeID_t invokeID,
	ConnectionID_t* call)
{
	int iRet = 0;

	CSTAMonitorFilter_t noFilter;
	noFilter.call = 0;
	noFilter.feature = 0;
	noFilter.agent = 0;
	noFilter.maintenance = 0;
	noFilter.privateFilter = 0;
	
	try{
		iRet = cstaMonitorCall(acsHandle, invokeID, call, &noFilter, NULL);
	}
	catch (...){
	//	ShowMsg(CAvayaResult::Instance()->GetErrorString(("cstaMonitorCall"), iRet));
	}

	return iRet == ACSPOSITIVE_ACK;
}


bool CAvayaSwitch::MonitorCallsViaDevice(const std::string& strDeviceNum, InvokeID_t invokeId)
{
	int iRet = 0;

	DeviceID_t deviceId = { 0 };
	STRNCPY(deviceId, strDeviceNum.c_str(), strDeviceNum.size());

	CSTAMonitorFilter_t noFilter;
	noFilter.call = 0;
	noFilter.feature = 0;
	noFilter.agent = 0;
	noFilter.maintenance = 0;
	noFilter.privateFilter = 0;

	try{
		iRet = cstaMonitorCallsViaDevice(m_acsHandle, 
			(InvokeID_t)invokeId,
			(DeviceID_t*)&deviceId, 
			(CSTAMonitorFilter_t*)&noFilter,
			NULL);
	}
	catch (...){
		std::string strErrorMsg = CAvayaResult::Instance()->GetErrorString(("cstaMonitorCallsViaDevice"), iRet);
		ICC_LOG_WARNING(m_pLog, "%s", strErrorMsg.c_str());
	}

	return iRet == ACSPOSITIVE_ACK;
}


bool CAvayaSwitch::ChangeMonitorFilter(ACSHandle_t acsHandle,
	InvokeID_t invokeID,
	CSTAMonitorCrossRefID_t monitorCrossRefID,
	CSTAMonitorFilter_t *filterlist)
{
	int iRet = 0;

	try{
		iRet = cstaChangeMonitorFilter(acsHandle, invokeID,
			monitorCrossRefID,
			filterlist,
			NULL);
	}
	catch (...){
	//	ShowMsg(CAvayaResult::Instance()->GetErrorString(("cstaChangeMonitorFilter"), iRet));
	}

	return iRet == ACSPOSITIVE_ACK;
}


bool CAvayaSwitch::MonitorStop(ACSHandle_t acsHandle,
	InvokeID_t invokeId,
	CSTAMonitorCrossRefID_t monitorCrossRefID)
{
	int iRet = 0;

	try{
		iRet = cstaMonitorStop(acsHandle, invokeId,
			monitorCrossRefID, NULL);
	}
	catch (...){
	//	ShowMsg(CAvayaResult::Instance()->GetErrorString(("cstaMonitorStop"), iRet));
	}

	return iRet == ACSPOSITIVE_ACK;
}


bool CAvayaSwitch::SnapshotCallReq(ACSHandle_t acsHandle,
	InvokeID_t invokeID,
	ConnectionID_t* pSnapshotObj,
	PrivateData_t* pPrivateData)
{
	int iRet = 0;

	try{
		iRet = cstaSnapshotCallReq(acsHandle, invokeID, pSnapshotObj, pPrivateData);
	}
	catch (...){
	//	ShowMsg(CAvayaResult::Instance()->GetErrorString(("cstaSnapshotCallReq"), iRet));	
	}

	return iRet == ACSPOSITIVE_ACK;
}


bool CAvayaSwitch::SnapshotDeviceReq(ACSHandle_t acsHandle,
	InvokeID_t invokeId,
	const std::string& strDeviceNum,
	PrivateData_t* pPrivateData)
{
	int iRet = 0;

	DeviceID_t deviceId = { 0 };
	STRNCPY(deviceId, strDeviceNum.c_str(), strDeviceNum.size());

	try{
		iRet = cstaSnapshotDeviceReq(acsHandle, invokeId,
			&deviceId, pPrivateData);
	}
	catch (...){
	//	ShowMsg(CAvayaResult::Instance()->GetErrorString(("cstaSnapshotDeviceReq"), iRet));
	}

	return iRet == ACSPOSITIVE_ACK;
}


bool CAvayaSwitch::RouteRegisterReq(const std::string& strDeviceNum, InvokeID_t invokeId)
{
	DeviceID_t deviceId = { 0 };
	STRNCPY(deviceId, strDeviceNum.c_str(), strDeviceNum.size());

	long iRet = ACSERR_APIVERDENIED;
	try
	{
		iRet = cstaRouteRegisterReq(m_acsHandle, 
			(InvokeID_t)invokeId, 
			(DeviceID_t*)&deviceId, 
			NULL);
	}
	catch (...)
	{
		std::string strErrorMsg = CAvayaResult::Instance()->GetErrorString(("cstaRouteRegisterReq"), iRet);
		ICC_LOG_WARNING(m_pLog, "%s", strErrorMsg.c_str());
	}

	return iRet == ACSPOSITIVE_ACK;
}

bool CAvayaSwitch::RouteRegisterReq(ACSHandle_t acsHandle,
	InvokeID_t invokeId,
	const std::string& strDeviceNum,
	PrivateData_t* pPrivateData)
{
	int iRet = 0;

	DeviceID_t deviceId = { 0 };
	STRNCPY(deviceId, strDeviceNum.c_str(), strDeviceNum.size());
	
	try
	{
		iRet = cstaRouteRegisterReq(acsHandle, invokeId,
			&deviceId, pPrivateData);
	}
	catch (...)
	{
	//	ShowMsg(CAvayaResult::Instance()->GetErrorString(("cstaRouteRegisterReq"), iRet));
	}

	return iRet == ACSPOSITIVE_ACK;
}

bool CAvayaSwitch::RouteRegisterCancel(ACSHandle_t acsHandle,
	InvokeID_t invokeId,
	RouteRegisterReqID_t routeRegisterReqID,
	PrivateData_t* pPrivateData)
{
	int iRet = 0;

	try{
		iRet = cstaRouteRegisterCancel(acsHandle, invokeId,
			routeRegisterReqID, NULL);
	}
	catch (...){
	//	ShowMsg(CAvayaResult::Instance()->GetErrorString(("cstaRouteRegisterCancel"), iRet));
	}

	return iRet == ACSPOSITIVE_ACK;
}

bool CAvayaSwitch::RouteSelectInv(ACSHandle_t acsHandle,
	InvokeID_t invokeId,
	RouteRegisterReqID_t routeRegisterReqID,
	RoutingCrossRefID_t	routingCrossRefID,
	const std::string& strRouteSelected,
	RetryValue_t remainRetry,
	SetUpValues_t* setupInformation,
	Boolean	routeUsedReq,
	PrivateData_t* pPrivateData)
{
	int iRet = 0;

	DeviceID_t routeSelected = { 0 };
	STRNCPY(routeSelected, strRouteSelected.c_str(), strRouteSelected.size());
	
	try{
		iRet = cstaRouteSelectInv(acsHandle, invokeId,
			routeRegisterReqID,
			routingCrossRefID,
			&routeSelected,
			remainRetry,
			setupInformation,
			routeUsedReq,
			pPrivateData);
	}
	catch (...){
	//	ShowMsg(CAvayaResult::Instance()->GetErrorString(("cstaRouteSelectInv"), iRet));
	}

	return iRet == ACSPOSITIVE_ACK;
}

bool CAvayaSwitch::RouteSelectInv(
	RouteRegisterReqID_t routeRegisterReqID,
	RoutingCrossRefID_t	routingCrossRefID,
	const std::string& strRouteSelected,
	InvokeID_t invokeId)
{
	int iRet = 0;

	DeviceID_t routeSelected = { 0 };
	STRNCPY(routeSelected, strRouteSelected.c_str(), strRouteSelected.size());

	try{
		iRet = cstaRouteSelectInv(m_acsHandle, 
			(InvokeID_t)invokeId,
			(RouteRegisterReqID_t)routeRegisterReqID,
			(RoutingCrossRefID_t)routingCrossRefID,
			(DeviceID_t*)&routeSelected,
			(RetryValue_t)0,
			NULL,
			false,
			NULL);
	}
	catch (...){
		std::string strErrorMsg = CAvayaResult::Instance()->GetErrorString(("cstaRouteSelectInv"), iRet);
		ICC_LOG_WARNING(m_pLog, "%s", strErrorMsg.c_str());
	}

	return iRet == ACSPOSITIVE_ACK;
}

bool CAvayaSwitch::RouteEndInv(ACSHandle_t acsHandle,
	InvokeID_t invokeId,
	RouteRegisterReqID_t routeRegisterReqID,
	RoutingCrossRefID_t routingCrossRefID,
	CSTAUniversalFailure_t errorValue,
	PrivateData_t* pPrivateData)
{
	int iRet = 0;

	try{
		iRet = cstaRouteEndInv(acsHandle, invokeId,
			routeRegisterReqID,
			routingCrossRefID,
			errorValue,
			pPrivateData);
	}
	catch (...){
	//	ShowMsg(CAvayaResult::Instance()->GetErrorString(("cstaRouteEndInv"), iRet));
	}

	return iRet == ACSPOSITIVE_ACK;
}

bool CAvayaSwitch::RouteEndInv(
	RouteRegisterReqID_t routeRegisterReqID,
	RoutingCrossRefID_t routingCrossRefID,
	InvokeID_t invokeId)
{
	int iRet = 0;

	try{
		iRet = cstaRouteEndInv(m_acsHandle,
			(InvokeID_t)invokeId,
			(RouteRegisterReqID_t)routeRegisterReqID,
			(RoutingCrossRefID_t)routingCrossRefID,
			INVALID_CSTA_DEVICE_IDENTIFIER,
			NULL);
	}
	catch (...){
		std::string strErrorMsg = CAvayaResult::Instance()->GetErrorString(("cstaRouteEndInv"), iRet);
		ICC_LOG_WARNING(m_pLog, "%s", strErrorMsg.c_str());
	}

	return iRet == ACSPOSITIVE_ACK;
}

bool CAvayaSwitch::RouteSelect(ACSHandle_t acsHandle,
	RouteRegisterReqID_t routeRegisterReqID,
	RoutingCrossRefID_t	routingCrossRefID,
	const std::string& strRouteSelected,
	RetryValue_t	remainRetry,
	SetUpValues_t*	pSetupInformation,
	bool bRouteUsedReq,
	PrivateData_t* pPrivateData)
{
	int iRet = 0;

	DeviceID_t	deviceSelected = { 0 };
	STRNCPY(deviceSelected, strRouteSelected.c_str(), strRouteSelected.size());

	try{
		iRet = cstaRouteSelect(acsHandle,
			routeRegisterReqID,
			routingCrossRefID,
			&deviceSelected,
			remainRetry,
			pSetupInformation,
			bRouteUsedReq,
			pPrivateData);
	}
	catch (...){
	//	ShowMsg(CAvayaResult::Instance()->GetErrorString(("cstaRouteSelect"), iRet));
	}

	return iRet == ACSPOSITIVE_ACK;
}

bool CAvayaSwitch::RouteEnd(ACSHandle_t acsHandle,
	RouteRegisterReqID_t routeRegisterReqID,
	RoutingCrossRefID_t routingCrossRefID,
	CSTAUniversalFailure_t errorValue,
	PrivateData_t* pPrivateData)
{
	int iRet = 0;

	try{
		iRet = cstaRouteEnd(acsHandle, routeRegisterReqID,
			routingCrossRefID, errorValue,
			pPrivateData);
	}
	catch (...){
	//	ShowMsg(CAvayaResult::Instance()->GetErrorString(("cstaRouteEnd"), iRet));
	}

	return iRet == ACSPOSITIVE_ACK;
}


int CAvayaSwitch::EscapeService(InvokeID_t invokeID)
{
	int iRet = 0;

	try{
		iRet = cstaEscapeService(m_acsHandle, 
			(InvokeID_t)invokeID, 
			(PrivateData_t*)&m_privateData);
	}
	catch (...){
		std::string strErrorMsg = CAvayaResult::Instance()->GetErrorString(("cstaEscapeService"), iRet);
		ICC_LOG_WARNING(m_pLog, "%s", strErrorMsg.c_str());
	}

	return iRet;
}


bool CAvayaSwitch::EscapeServiceConf(ACSHandle_t acsHandle,
	InvokeID_t invokeID,
	CSTAUniversalFailure_t error,
	PrivateData_t* pPrivateData)
{
	int iRet = 0;

	try{
		iRet = cstaEscapeServiceConf(acsHandle, invokeID, error, pPrivateData);
	}
	catch (...){
	//	ShowMsg(CAvayaResult::Instance()->GetErrorString(("cstaEscapeServiceConf"), iRet));	
	}

	return iRet == ACSPOSITIVE_ACK;
}

bool CAvayaSwitch::SendPrivateEvent(ACSHandle_t acsHandle,
	PrivateData_t* pPrivateData)
{
	int iRet = 0;

	try{
		iRet = cstaSendPrivateEvent(acsHandle, pPrivateData);
	}
	catch (...){
	//	ShowMsg(CAvayaResult::Instance()->GetErrorString(("cstaSendPrivateEvent"), iRet));	
	}

	return iRet == ACSPOSITIVE_ACK;
}

bool CAvayaSwitch::SysStatReq(ACSHandle_t acsHandle,
	InvokeID_t invokeID,
	PrivateData_t* pPrivateData)
{
	int iRet = 0;

	try{
		iRet = cstaSysStatReq(acsHandle, invokeID, pPrivateData);
	}
	catch (...){
	//	ShowMsg(CAvayaResult::Instance()->GetErrorString(("cstaSysStatReq"), iRet));
	}

	return iRet == ACSPOSITIVE_ACK;
}

bool CAvayaSwitch::SysStatStart(ACSHandle_t acsHandle,
	InvokeID_t invokeID,
	SystemStatusFilter_t statusFilter,
	PrivateData_t* pPrivateData)
{
	int iRet = 0;

	try{
		iRet = cstaSysStatStart(acsHandle, invokeID, statusFilter, pPrivateData);
	}
	catch (...){
	//	ShowMsg(CAvayaResult::Instance()->GetErrorString(("cstaSysStatStart"), iRet));
	}

	return iRet == ACSPOSITIVE_ACK;
}


bool CAvayaSwitch::SysStatStop(ACSHandle_t acsHandle,
	InvokeID_t invokeID,
	PrivateData_t* pPrivateData)
{
	int iRet = 0;

	try{
		iRet = cstaSysStatStop(acsHandle, invokeID, pPrivateData);
	}
	catch (...){
	//	ShowMsg(CAvayaResult::Instance()->GetErrorString(("cstaSysStatStop"), iRet));
	}

	return iRet == ACSPOSITIVE_ACK;
}

bool CAvayaSwitch::GetAPICaps(ACSHandle_t acsHandle,
	InvokeID_t invokeID)

{
	int iRet = 0;

	try{
		iRet = cstaGetAPICaps(acsHandle, invokeID);
	}
	catch (...){
	//	ShowMsg(CAvayaResult::Instance()->GetErrorString(("cstaGetAPICaps"), iRet));
	}

	return iRet == ACSPOSITIVE_ACK;
}

bool CAvayaSwitch::GetDeviceList(ACSHandle_t acsHandle,
	InvokeID_t	invokeID,
	long		index,
	CSTALevel_t	level)
{
	int iRet = 0;

	try{
		iRet = cstaGetDeviceList(acsHandle, invokeID, index, level);
	}
	catch (...){
	//	ShowMsg(CAvayaResult::Instance()->GetErrorString(("cstaGetDeviceList"), iRet));
	}

	return iRet == ACSPOSITIVE_ACK;
}

bool CAvayaSwitch::QueryCallMonitor(ACSHandle_t acsHandle,
	InvokeID_t invokeID)
{
	int iRet = 0;

	try{
		iRet = cstaQueryCallMonitor(acsHandle, invokeID);
	}
	catch (...){
	//	ShowMsg(CAvayaResult::Instance()->GetErrorString(("cstaQueryCallMonitor"), iRet));
	}

	return iRet == ACSPOSITIVE_ACK;
}

int CAvayaSwitch::SingleStepConferenceCall(const std::string& strDeviceNum, const std::string& strToBeJoinNum, 
	long lCallRefId, int iMode)
{
	int iRet = 0;

	DeviceID_t deviceJoin = { 0 };
	STRNCPY(deviceJoin, strDeviceNum.c_str(), strDeviceNum.size());
	ATTParticipationType_t joinType = PT_ACTIVE;

	ConnectionID_t activeCall;
	memset(&activeCall, 0, sizeof(ConnectionID_t));
	activeCall.callID = lCallRefId;
	activeCall.devIDType = STATIC_ID;
	STRNCPY(activeCall.deviceID, strToBeJoinNum.c_str(), strToBeJoinNum.size());

	Boolean	alertDestination = false;

	switch(iMode)
	{
	case CONF_MODE_ACTIVE:
		joinType = PT_ACTIVE;
		break;
	case CONF_MODE_SILENT:
		joinType = PT_SILENT;
		break;
	default:
		joinType = PT_ACTIVE;
		break;
	}

	try{
		iRet = attSingleStepConferenceCall((ATTPrivateData_t*)&m_privateData, 
			(ConnectionID_t*)&activeCall,
			(DeviceID_t*)&deviceJoin, 
			(ATTParticipationType_t)joinType,
			(unsigned char)alertDestination);
	}
	catch (...){
		std::string strErrorMsg = CAvayaResult::Instance()->GetErrorString(("attSingleStepConferenceCall"), iRet);
		ICC_LOG_WARNING(m_pLog, "%s", strErrorMsg.c_str());
	}

	return iRet;
}

bool CAvayaSwitch::ListenCall(const std::string& strSponsor, const std::string& strTarget,
	long lCallRefId, InvokeID_t invokeId)
{
	int iRet = SingleStepConferenceCall(strSponsor, strTarget, lCallRefId, CONF_MODE_SILENT);

	if (iRet == ACSPOSITIVE_ACK)
	{
		iRet = EscapeService(invokeId);
	}

	return iRet == ACSPOSITIVE_ACK;
}
bool CAvayaSwitch::BargeInCall(const std::string& strSponsor, const std::string& strTarget,
	long lCallRefId, InvokeID_t invokeId)
{
	int iRet = SingleStepConferenceCall(strSponsor, strTarget, lCallRefId, CONF_MODE_ACTIVE);

	if (iRet == ACSPOSITIVE_ACK)
	{
		iRet = EscapeService(invokeId);
	}

	return iRet == ACSPOSITIVE_ACK;
}
bool CAvayaSwitch::ForcePopCall(const std::string& strTarget, long lCallRefId, InvokeID_t invokeId)
{
	return ClearCall(strTarget, lCallRefId, invokeId);
}
