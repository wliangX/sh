#include "Boost.h"
#include "AvayaResult.h"

boost::shared_ptr<CAvayaResult> CAvayaResult::m_pResultInstance = nullptr;
boost::shared_ptr<CAvayaResult> CAvayaResult::Instance()
{
	if (m_pResultInstance == nullptr)
	{
		m_pResultInstance = boost::make_shared<CAvayaResult>();
	}

	return m_pResultInstance;
}
void CAvayaResult::ExitInstance()
{
	//
}

CAvayaResult::CAvayaResult(void)
{
	m_pStrUtil = nullptr;
}
CAvayaResult::~CAvayaResult(void)
{
	//
}

void CAvayaResult::OnInit()
{
	//
}
//////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// Describe a call privilege level (in English)
std::string CAvayaResult::DescribeError(DWORD p_dwError)
{
	switch (p_dwError) {
	case LINEERR_ALLOCATED: return "Allocated";
	case LINEERR_BADDEVICEID: return "BadDeviceId";
	case LINEERR_BEARERMODEUNAVAIL: return "BearerModeUnavail";
	case LINEERR_CALLUNAVAIL: return "CallUnavail";
	case LINEERR_COMPLETIONOVERRUN: return "CompletionOverrun";
	case LINEERR_CONFERENCEFULL: return "ConferenceFull";
	case LINEERR_DIALBILLING: return "DialBilling";
	case LINEERR_DIALDIALTONE: return "DialDialtone";
	case LINEERR_DIALPROMPT: return "DialPrompt";
	case LINEERR_DIALQUIET: return "DialQuiet";
	case LINEERR_INCOMPATIBLEAPIVERSION: return "IncompatibleAPIVersion";
	case LINEERR_INCOMPATIBLEEXTVERSION: return "IncompatibleExtVersion";
	case LINEERR_INIFILECORRUPT: return "IniFileCorrupt";
	case LINEERR_INUSE: return "InUse";
	case LINEERR_INVALADDRESS: return "InvalAddress";
	case LINEERR_INVALADDRESSID: return "InvalAddressID";
	case LINEERR_INVALADDRESSMODE: return "InvalAddressMode";
	case LINEERR_INVALADDRESSSTATE: return "InvalAddressState";
	case LINEERR_INVALAPPHANDLE: return "InvalAppHandle";
	case LINEERR_INVALAPPNAME: return "InvalAppName";
	case LINEERR_INVALBEARERMODE: return "InvalBearerMode";
	case LINEERR_INVALCALLCOMPLMODE: return "InvalCallCompMode";
	case LINEERR_INVALCALLHANDLE: return "InvalCallHandle";
	case LINEERR_INVALCALLPARAMS: return "InvalCallParams";
	case LINEERR_INVALCALLPRIVILEGE: return "InvalCallPrivilege";
	case LINEERR_INVALCALLSELECT: return "InvalCallSelect";
	case LINEERR_INVALCALLSTATE: return "InvalCallState";
	case LINEERR_INVALCALLSTATELIST: return "InvalCallStateList";
	case LINEERR_INVALCARD: return "InvalCard";
	case LINEERR_INVALCOMPLETIONID: return "InvalCompletionID";
	case LINEERR_INVALCONFCALLHANDLE: return "InvalConfCallHandle";
	case LINEERR_INVALCONSULTCALLHANDLE: return "InvalConsultCallHandle";
	case LINEERR_INVALCOUNTRYCODE: return "InvalCountryCode";
	case LINEERR_INVALDEVICECLASS: return "InvalDeviceClass";
	case LINEERR_INVALDEVICEHANDLE: return "InvalDeviceHandle";
	case LINEERR_INVALDIALPARAMS: return "InvalDialParams";
	case LINEERR_INVALDIGITLIST: return "InvalDigitList";
	case LINEERR_INVALDIGITMODE: return "InvalDigitMode";
	case LINEERR_INVALDIGITS: return "InvalDigits";
	case LINEERR_INVALEXTVERSION: return "InvalExtVersion";
	case LINEERR_INVALGROUPID: return "InvalGroupID";
	case LINEERR_INVALLINEHANDLE: return "InvalLineHandle";
	case LINEERR_INVALLINESTATE: return "InvalLineState";
	case LINEERR_INVALLOCATION: return "InvalLocation";
	case LINEERR_INVALMEDIALIST: return "InvalMediaList";
	case LINEERR_INVALMEDIAMODE: return "InvalMediaMode";
	case LINEERR_INVALMESSAGEID: return "InvalMessageID";
	case LINEERR_INVALPARAM: return "InvalParam";
	case LINEERR_INVALPARKID: return "InvalParkID";
	case LINEERR_INVALPARKMODE: return "InvalParkMode";
	case LINEERR_INVALPOINTER: return "InvalPointer";
	case LINEERR_INVALPRIVSELECT: return "InvalPrivSelect";
	case LINEERR_INVALRATE: return "InvalRate";
	case LINEERR_INVALREQUESTMODE: return "InvalRequestMode";
	case LINEERR_INVALTERMINALID: return "InvalTerminalID";
	case LINEERR_INVALTERMINALMODE: return "InvalTerminalMode";
	case LINEERR_INVALTIMEOUT: return "InvalTimeout";
	case LINEERR_INVALTONE: return "InvalTone";
	case LINEERR_INVALTONELIST: return "InvalToneList";
	case LINEERR_INVALTONEMODE: return "InvalToneMode";
	case LINEERR_INVALTRANSFERMODE: return "InvalTransferMode";
	case LINEERR_LINEMAPPERFAILED: return "LineMapperFailed";
	case LINEERR_NOCONFERENCE: return "NoConference";
	case LINEERR_NODEVICE: return "NoDevice";
	case LINEERR_NODRIVER: return "NoDriver";
	case LINEERR_NOMEM: return "NoMem";
	case LINEERR_NOREQUEST: return "NoRequest";
	case LINEERR_NOTOWNER: return "NotOwner";
	case LINEERR_NOTREGISTERED: return "NotRegistered";
	case LINEERR_OPERATIONFAILED: return "OperationFailed";
	case LINEERR_OPERATIONUNAVAIL: return "OperationUnavail";
	case LINEERR_RATEUNAVAIL: return "RateUnavail";
	case LINEERR_RESOURCEUNAVAIL: return "ResourceUnavail";
	case LINEERR_REQUESTOVERRUN: return "RequestOverrun";
	case LINEERR_STRUCTURETOOSMALL: return "StructureTooSmall";
	case LINEERR_TARGETNOTFOUND: return "TargetNotFound";
	case LINEERR_TARGETSELF: return "TargetSelf";
	case LINEERR_UNINITIALIZED: return "Uninitialized";
	case LINEERR_USERUSERINFOTOOBIG: return "UserUserInfoTooBig";
	case LINEERR_REINIT: return "ReInit";
	case LINEERR_ADDRESSBLOCKED: return "AddressBlocked";
	case LINEERR_BILLINGREJECTED: return "BillingRejected";
	case LINEERR_INVALFEATURE: return "InvalFeature";
	case LINEERR_NOMULTIPLEINSTANCE: return "NoMultipleInstance";
	case LINEERR_INVALAGENTID: return "InvalAgentID";
	case LINEERR_INVALAGENTGROUP: return "InvalAgentGroup";
	case LINEERR_INVALPASSWORD: return "InvalPassword";
	case LINEERR_INVALAGENTSTATE: return "InvalAgentState";
	case LINEERR_INVALAGENTACTIVITY: return "InvalAgentActivity";
	case LINEERR_DIALVOICEDETECT: return "DialVoiceDetect";
	default: return "Unknown";
	};
}


// ----------------------------------------------------------------------------
// Describe details about a call state (in English)
std::string CAvayaResult::DescribeCallStateDetail(DWORD p_dwCallState, DWORD p_dwCallStateDetail)
{
	std::string l_strRet = "Unknown";

	switch (p_dwCallState) {
	case LINECALLSTATE_IDLE:
		l_strRet = "Idle";
		break;
	case LINECALLSTATE_OFFERING:
		switch (p_dwCallStateDetail) {
		case LINEOFFERINGMODE_ACTIVE: l_strRet = "Offering:Active"; break;
		case LINEOFFERINGMODE_INACTIVE: l_strRet = "Offering:Inactive"; break;
		default: l_strRet = "Offering:Unknown"; break;
		}
		break;
	case LINECALLSTATE_ACCEPTED:
		l_strRet = "Accepted";
		break;
	case LINECALLSTATE_DIALTONE:
		switch (p_dwCallStateDetail) {
		case LINEDIALTONEMODE_NORMAL: l_strRet = "DialTone:Normal";break;
		case LINEDIALTONEMODE_SPECIAL: l_strRet = "DialTone:Special";break;
		case LINEDIALTONEMODE_INTERNAL: l_strRet = "DialTone:Internal";break;
		case LINEDIALTONEMODE_EXTERNAL: l_strRet = "DialTone:External";break;
		case LINEDIALTONEMODE_UNKNOWN: l_strRet = "DialTone:Unknown";break;
		case LINEDIALTONEMODE_UNAVAIL: l_strRet = "DialTone:Unavailable";break;
		default: l_strRet = "DialTone:Unknown";break;
		};
		break;
	case LINECALLSTATE_DIALING:
		l_strRet = "Dialing";
		break;
	case LINECALLSTATE_RINGBACK:
		l_strRet = "RingBack";
		break;
	case LINECALLSTATE_BUSY:
		switch (p_dwCallStateDetail) {
		case LINEBUSYMODE_STATION: l_strRet = "Busy:Station"; break;
		case LINEBUSYMODE_TRUNK:   l_strRet = "Busy:Trunk"; break;
		case LINEBUSYMODE_UNKNOWN: l_strRet = "Busy:Unknown"; break;
		case LINEBUSYMODE_UNAVAIL: l_strRet = "Busy:Unavailable"; break;
		default: l_strRet = "Busy:Unknown"; break;
		};
		break;
	case LINECALLSTATE_SPECIALINFO:
		switch (p_dwCallStateDetail) {
		case LINESPECIALINFO_NOCIRCUIT: l_strRet = "SpecialInfo:NoCircuit"; break;
		case LINESPECIALINFO_CUSTIRREG: l_strRet = "SpecialInfo:CustIrreg"; break;
		case LINESPECIALINFO_REORDER: l_strRet = "SpecialInfo:ReOrder"; break;
		case LINESPECIALINFO_UNKNOWN: l_strRet = "SpecialInfo:Unknown"; break;
		case LINESPECIALINFO_UNAVAIL: l_strRet = "SpecialInfo:Unavailable"; break;
		default: l_strRet = "SpecialInfo:Unknown"; break;
		};
		break;
	case LINECALLSTATE_CONNECTED:
		switch (p_dwCallStateDetail) {
		case LINECONNECTEDMODE_ACTIVE: l_strRet = "Connected:Active"; break;
		case LINECONNECTEDMODE_INACTIVE: l_strRet = "Connected:Inactive"; break;
		case LINECONNECTEDMODE_ACTIVEHELD: l_strRet = "Connected:ActiveHeld"; break;
		case LINECONNECTEDMODE_INACTIVEHELD: l_strRet = "Connected:InactiveHeld"; break;
		case LINECONNECTEDMODE_CONFIRMED: l_strRet = "Connected:Confirmed"; break;
		default: l_strRet = "Connected:Unknown"; break;
		};
		break;
	case LINECALLSTATE_PROCEEDING:
		l_strRet = "Proceeding";
		break;
	case LINECALLSTATE_ONHOLD:
		l_strRet = "OnHold";
		break;
	case LINECALLSTATE_CONFERENCED:
		l_strRet = "Conference";
		break;
	case LINECALLSTATE_ONHOLDPENDCONF:
		l_strRet = "OnHoldPendConf";
		break;
	case LINECALLSTATE_ONHOLDPENDTRANSFER:
		l_strRet = "OnHoldPendTransfer";
		break;
	case LINECALLSTATE_DISCONNECTED:
		switch (p_dwCallStateDetail) {
		case LINEDISCONNECTMODE_NORMAL: l_strRet = "Disconnected:Normal"; break;
		case LINEDISCONNECTMODE_UNKNOWN: l_strRet = "Disconnected:Unknown"; break;
		case LINEDISCONNECTMODE_REJECT: l_strRet = "Disconnected:Reject"; break;
		case LINEDISCONNECTMODE_PICKUP: l_strRet = "Disconnected:Pickup"; break;
		case LINEDISCONNECTMODE_FORWARDED: l_strRet = "Disconnected:Forwarded"; break;
		case LINEDISCONNECTMODE_BUSY: l_strRet = "Disconnected:Busy"; break;
		case LINEDISCONNECTMODE_NOANSWER: l_strRet = "Disconnected:NoAnswer"; break;
		case LINEDISCONNECTMODE_BADADDRESS: l_strRet = "Disconnected:BadAddress"; break;
		case LINEDISCONNECTMODE_UNREACHABLE: l_strRet = "Disconnected:Unreachable"; break;
		case LINEDISCONNECTMODE_CONGESTION: l_strRet = "Disconnected:Congestion"; break;
		case LINEDISCONNECTMODE_INCOMPATIBLE: l_strRet = "Disconnected:Incompatible"; break;
		case LINEDISCONNECTMODE_UNAVAIL: l_strRet = "Disconnected:Unavailable"; break;
		case LINEDISCONNECTMODE_NODIALTONE: l_strRet = "Disconnected:NoDialTone"; break;
		case LINEDISCONNECTMODE_NUMBERCHANGED: l_strRet = "Disconnected:NumberChanged"; break;
		case LINEDISCONNECTMODE_OUTOFORDER: l_strRet = "Disconnected:OutOfOrder"; break;
		case LINEDISCONNECTMODE_TEMPFAILURE: l_strRet = "Disconnected:TempFailure"; break;
		case LINEDISCONNECTMODE_QOSUNAVAIL: l_strRet = "Disconnected:QOSUnavail"; break;
		case LINEDISCONNECTMODE_BLOCKED: l_strRet = "Disconnected:Blocked"; break;
		case LINEDISCONNECTMODE_DONOTDISTURB: l_strRet = "Disconnected:DoNotDisturb"; break;
		case LINEDISCONNECTMODE_CANCELLED: l_strRet = "Disconnected:Cancelled"; break;
		default: l_strRet = "Disconnected:Unknown"; break;
		};
		break;
	default:
		l_strRet = "Unknown";
		break;
	};

	return l_strRet;
}

// ----------------------------------------------------------------------------
// Describe a call state (in English)
std::string CAvayaResult::DescribeCallState(std::string& p_strResult, DWORD p_dwCallState)
{
	p_strResult.clear();

	if (p_dwCallState == 0) p_strResult += "None";
	if (p_dwCallState & LINECALLSTATE_IDLE) p_strResult += "Idle ";
	if (p_dwCallState & LINECALLSTATE_OFFERING) p_strResult += "Offering ";
	if (p_dwCallState & LINECALLSTATE_ACCEPTED) p_strResult += "Accepted ";
	if (p_dwCallState & LINECALLSTATE_DIALTONE) p_strResult += "DialTone ";
	if (p_dwCallState & LINECALLSTATE_DIALING) p_strResult += "Dialing ";
	if (p_dwCallState & LINECALLSTATE_RINGBACK) p_strResult += "RingBack ";
	if (p_dwCallState & LINECALLSTATE_BUSY) p_strResult += "Busy ";
	if (p_dwCallState & LINECALLSTATE_SPECIALINFO) p_strResult += "SpecialInfo ";
	if (p_dwCallState & LINECALLSTATE_CONNECTED) p_strResult += "Connected ";
	if (p_dwCallState & LINECALLSTATE_PROCEEDING) p_strResult += "Proceeding ";
	if (p_dwCallState & LINECALLSTATE_ONHOLD) p_strResult += "Held ";
	if (p_dwCallState & LINECALLSTATE_CONFERENCED) p_strResult += "Conferenced ";
	if (p_dwCallState & LINECALLSTATE_ONHOLDPENDCONF) p_strResult += "HoldPendingConference ";
	if (p_dwCallState & LINECALLSTATE_ONHOLDPENDTRANSFER) p_strResult += "HoldPendingTransfer ";
	if (p_dwCallState & LINECALLSTATE_DISCONNECTED) p_strResult += "Disconnected ";
	if (p_dwCallState & LINECALLSTATE_UNKNOWN) p_strResult += "Unknown ";
	if (p_strResult.empty()) p_strResult += "Unknown ";

	return p_strResult;
}

// ----------------------------------------------------------------------------
// Describe a call privilege level (in English)
std::string CAvayaResult::DescribePrivilege(std::string& p_strResult, DWORD p_dwPrivilege)
{
	p_strResult.clear();

	if (p_dwPrivilege == 0) p_strResult += "SamePrivilege";
	if (p_dwPrivilege & LINECALLPRIVILEGE_NONE) p_strResult += "NoPrivilege ";
	if (p_dwPrivilege & LINECALLPRIVILEGE_MONITOR) p_strResult += "Monitor ";
	if (p_dwPrivilege & LINECALLPRIVILEGE_OWNER) p_strResult += "Owner ";
	if (p_strResult.empty()) p_strResult += "Unknown ";

	return p_strResult;
}


// ----------------------------------------------------------------------------
// Describe details about a device state (in English)
std::string CAvayaResult::DescribeDeviceStatus(std::string& p_strResult, DWORD p_dwDeviceState)
{
	p_strResult.clear();

	if (p_dwDeviceState == 0) p_strResult += "None";
	if (p_dwDeviceState & LINEDEVSTATE_OTHER) p_strResult += "Other ";
	if (p_dwDeviceState & LINEDEVSTATE_RINGING) p_strResult += "Ringing ";
	if (p_dwDeviceState & LINEDEVSTATE_CONNECTED) p_strResult += "Connected ";
	if (p_dwDeviceState & LINEDEVSTATE_DISCONNECTED) p_strResult += "Disconnected ";
	if (p_dwDeviceState & LINEDEVSTATE_MSGWAITON) p_strResult += "MsgWaitOn ";
	if (p_dwDeviceState & LINEDEVSTATE_MSGWAITOFF) p_strResult += "MsgWaitOff ";
	if (p_dwDeviceState & LINEDEVSTATE_NUMCOMPLETIONS) p_strResult += "NumCompletions ";
	if (p_dwDeviceState & LINEDEVSTATE_INSERVICE) p_strResult += "InService ";
	if (p_dwDeviceState & LINEDEVSTATE_OUTOFSERVICE) p_strResult += "OutOfService ";
	if (p_dwDeviceState & LINEDEVSTATE_MAINTENANCE) p_strResult += "Maintenance ";
	if (p_dwDeviceState & LINEDEVSTATE_OPEN) p_strResult += "Open ";
	if (p_dwDeviceState & LINEDEVSTATE_CLOSE) p_strResult += "Close ";
	if (p_dwDeviceState & LINEDEVSTATE_NUMCALLS) p_strResult += "NumCalls ";
	if (p_dwDeviceState & LINEDEVSTATE_TERMINALS) p_strResult += "Terminals ";
	if (p_dwDeviceState & LINEDEVSTATE_ROAMMODE) p_strResult += "RoamMode ";
	if (p_dwDeviceState & LINEDEVSTATE_BATTERY) p_strResult += "Battery ";
	if (p_dwDeviceState & LINEDEVSTATE_SIGNAL) p_strResult += "Signal ";
	if (p_dwDeviceState & LINEDEVSTATE_DEVSPECIFIC) p_strResult += "DevSpecific ";
	if (p_dwDeviceState & LINEDEVSTATE_REINIT) p_strResult += "ReInit ";
	if (p_dwDeviceState & LINEDEVSTATE_LOCK) p_strResult += "Lock ";
	if (p_dwDeviceState & LINEDEVSTATE_CAPSCHANGE) p_strResult += "CapsChange ";
	if (p_dwDeviceState & LINEDEVSTATE_CONFIGCHANGE) p_strResult += "ConfigChange ";
	if (p_dwDeviceState & LINEDEVSTATE_TRANSLATECHANGE) p_strResult += "TranslateChange ";
	if (p_dwDeviceState & LINEDEVSTATE_COMPLCANCEL) p_strResult += "ComplCancel ";
	if (p_dwDeviceState & LINEDEVSTATE_REMOVED) p_strResult += "Removed ";
	if (p_strResult.empty()) p_strResult += "Unknown ";

	return p_strResult;
}


// ----------------------------------------------------------------------------
// Describe details about an address state (in English)
std::string CAvayaResult::DescribeAddressStatus(std::string& p_strResult, DWORD p_dwAddressState)
{
	p_strResult.clear();

	if (p_dwAddressState == 0) p_strResult += "None";
	if (p_dwAddressState & LINEADDRESSSTATE_OTHER) p_strResult += "Other ";
	if (p_dwAddressState & LINEADDRESSSTATE_DEVSPECIFIC) p_strResult += "DeviceSpecific ";
	if (p_dwAddressState & LINEADDRESSSTATE_INUSEZERO) p_strResult += "InUseZero "; // Address is idle
	if (p_dwAddressState & LINEADDRESSSTATE_INUSEONE) p_strResult += "InUseOne ";
	if (p_dwAddressState & LINEADDRESSSTATE_INUSEMANY) p_strResult += "InUseMany ";
	if (p_dwAddressState & LINEADDRESSSTATE_NUMCALLS) p_strResult += "NumCalls ";
	if (p_dwAddressState & LINEADDRESSSTATE_FORWARD) p_strResult += "Forward ";
	if (p_dwAddressState & LINEADDRESSSTATE_TERMINALS) p_strResult += "Terminals ";
	if (p_dwAddressState & LINEADDRESSSTATE_CAPSCHANGE) p_strResult += "CapsChange ";
	if (p_strResult.empty()) p_strResult += "Unknown ";

	return p_strResult;
}

// ----------------------------------------------------------------------------
// Describe details about call information (in English)
std::string CAvayaResult::DescribeCallInfo(std::string& p_strResult, DWORD p_dwCallInfoState)
{
	p_strResult.clear();

	if (p_dwCallInfoState == 0) p_strResult += "None";
	if (p_dwCallInfoState & LINECALLINFOSTATE_OTHER) p_strResult += "Other ";
	if (p_dwCallInfoState & LINECALLINFOSTATE_DEVSPECIFIC) p_strResult += "DevSpecific ";
	if (p_dwCallInfoState & LINECALLINFOSTATE_BEARERMODE) p_strResult += "BearerMode ";
	if (p_dwCallInfoState & LINECALLINFOSTATE_RATE) p_strResult += "Rate ";
	if (p_dwCallInfoState & LINECALLINFOSTATE_MEDIAMODE) p_strResult += "MediaMode ";
	if (p_dwCallInfoState & LINECALLINFOSTATE_APPSPECIFIC) p_strResult += "AppSpecific ";
	if (p_dwCallInfoState & LINECALLINFOSTATE_CALLID) p_strResult += "CallID ";
	if (p_dwCallInfoState & LINECALLINFOSTATE_RELATEDCALLID) p_strResult += "RelatedCallID ";
	if (p_dwCallInfoState & LINECALLINFOSTATE_ORIGIN) p_strResult += "Origin ";
	if (p_dwCallInfoState & LINECALLINFOSTATE_REASON) p_strResult += "Reason ";
	if (p_dwCallInfoState & LINECALLINFOSTATE_COMPLETIONID) p_strResult += "CompletionID ";
	if (p_dwCallInfoState & LINECALLINFOSTATE_NUMOWNERINCR) p_strResult += "NumOwnerIncr ";
	if (p_dwCallInfoState & LINECALLINFOSTATE_NUMOWNERDECR) p_strResult += "NumOwnerDecr ";
	if (p_dwCallInfoState & LINECALLINFOSTATE_NUMMONITORS) p_strResult += "NumMonitors ";
	if (p_dwCallInfoState & LINECALLINFOSTATE_TRUNK) p_strResult += "Trunk ";
	if (p_dwCallInfoState & LINECALLINFOSTATE_CALLERID) p_strResult += "CallerID ";
	if (p_dwCallInfoState & LINECALLINFOSTATE_CALLEDID) p_strResult += "CalledID ";
	if (p_dwCallInfoState & LINECALLINFOSTATE_CONNECTEDID) p_strResult += "ConnectedID ";
	if (p_dwCallInfoState & LINECALLINFOSTATE_REDIRECTIONID) p_strResult += "RedirectionID ";
	if (p_dwCallInfoState & LINECALLINFOSTATE_REDIRECTINGID) p_strResult += "RedirectingID ";
	if (p_dwCallInfoState & LINECALLINFOSTATE_DISPLAY) p_strResult += "Display ";
	if (p_dwCallInfoState & LINECALLINFOSTATE_USERUSERINFO) p_strResult += "UserUserInfo ";
	if (p_dwCallInfoState & LINECALLINFOSTATE_HIGHLEVELCOMP) p_strResult += "HighLevelComp ";
	if (p_dwCallInfoState & LINECALLINFOSTATE_LOWLEVELCOMP) p_strResult += "LowLevelComp ";
	if (p_dwCallInfoState & LINECALLINFOSTATE_CHARGINGINFO) p_strResult += "ChargingInfo ";
	if (p_dwCallInfoState & LINECALLINFOSTATE_TERMINAL) p_strResult += "Terminal ";
	if (p_dwCallInfoState & LINECALLINFOSTATE_DIALPARAMS) p_strResult += "DialParams ";
	if (p_dwCallInfoState & LINECALLINFOSTATE_MONITORMODES) p_strResult += "MonitorModes ";
	if (p_dwCallInfoState & LINECALLINFOSTATE_TREATMENT) p_strResult += "Treatment ";
	if (p_dwCallInfoState & LINECALLINFOSTATE_QOS) p_strResult += "QOS ";
	if (p_dwCallInfoState & LINECALLINFOSTATE_CALLDATA) p_strResult += "CallData ";
	if (p_strResult.empty()) p_strResult += "Unknown ";

	return p_strResult;
}


// ----------------------------------------------------------------------------
// Why was the call made?
std::string CAvayaResult::DescribeCallReason(std::string& p_strResult, DWORD p_dwCallReason)
{
	p_strResult.clear();

	if (p_dwCallReason == 0) p_strResult += "None";
	if (p_dwCallReason & LINECALLREASON_DIRECT) p_strResult += "Direct ";
	if (p_dwCallReason & LINECALLREASON_FWDBUSY) p_strResult += "FwdBusy ";
	if (p_dwCallReason & LINECALLREASON_FWDNOANSWER) p_strResult += "FwdNoAnswer ";
	if (p_dwCallReason & LINECALLREASON_FWDUNCOND) p_strResult += "FwdUncond ";
	if (p_dwCallReason & LINECALLREASON_PICKUP) p_strResult += "PickUp ";
	if (p_dwCallReason & LINECALLREASON_UNPARK) p_strResult += "UnPark ";
	if (p_dwCallReason & LINECALLREASON_REDIRECT) p_strResult += "Redirect ";
	if (p_dwCallReason & LINECALLREASON_CALLCOMPLETION) p_strResult += "p_dwCallCompletion ";
	if (p_dwCallReason & LINECALLREASON_TRANSFER) p_strResult += "Transfer ";
	if (p_dwCallReason & LINECALLREASON_REMINDER) p_strResult += "Reminder ";
	if (p_dwCallReason & LINECALLREASON_UNKNOWN) p_strResult += "Unknown ";
	if (p_dwCallReason & LINECALLREASON_UNAVAIL) p_strResult += "Unavail ";
	if (p_dwCallReason & LINECALLREASON_INTRUDE) p_strResult += "Intrude ";
	if (p_dwCallReason & LINECALLREASON_PARKED) p_strResult += "Parked ";
	if (p_dwCallReason & LINECALLREASON_CAMPEDON) p_strResult += "CampedOn ";
	if (p_dwCallReason & LINECALLREASON_ROUTEREQUEST) p_strResult += "RouteRequest ";
	if (p_strResult.empty()) p_strResult += "Unknown ";

	return p_strResult;
}

// ----------------------------------------------------------------------------
// Where was the call from?
std::string CAvayaResult::DescribeCallOrigin(std::string& p_strResult, DWORD p_dwCallOrigin)
{
	p_strResult.clear();

	if (p_dwCallOrigin == 0) p_strResult += "None";
	if (p_dwCallOrigin & LINECALLORIGIN_INBOUND) p_strResult += "Inbound ";
	if (p_dwCallOrigin & LINECALLORIGIN_OUTBOUND) p_strResult += "Outbound ";
	if (p_dwCallOrigin & LINECALLORIGIN_INTERNAL) p_strResult += "Internal ";
	if (p_dwCallOrigin & LINECALLORIGIN_EXTERNAL) p_strResult += "External ";
	if (p_dwCallOrigin & LINECALLORIGIN_UNKNOWN) p_strResult += "Unknown ";
	if (p_dwCallOrigin & LINECALLORIGIN_UNAVAIL) p_strResult += "Unavail ";
	if (p_dwCallOrigin & LINECALLORIGIN_CONFERENCE) p_strResult += "Conference ";
	if (p_strResult.empty()) p_strResult += "Unknown";

	return p_strResult;
}

// ----------------------------------------------------------------------------
// Describe details about a device state (in English)
std::string CAvayaResult::DescribeCallCompletion(std::string& p_strResult, DWORD p_dwCallCompletion)
{
	p_strResult.clear();

	if (p_dwCallCompletion == 0) p_strResult += "None";
	if (p_dwCallCompletion & LINECALLCOMPLMODE_CAMPON) p_strResult += "CampOn ";
	if (p_dwCallCompletion & LINECALLCOMPLMODE_CALLBACK) p_strResult += "CallBack ";
	if (p_dwCallCompletion & LINECALLCOMPLMODE_INTRUDE) p_strResult += "Intrude ";
	if (p_dwCallCompletion & LINECALLCOMPLMODE_MESSAGE) p_strResult += "Message ";
	if (p_strResult.empty()) p_strResult += "Unknown";

	return p_strResult;
}

// ----------------------------------------------------------------------------
// Describe capabilities of an address on a line
std::string CAvayaResult::DescribeAddressCapabilities(std::string& p_strResult, DWORD p_dwAddressCaps)
{
	p_strResult.clear();

	if (p_dwAddressCaps == 0) p_strResult += "None";
	if (p_dwAddressCaps & LINEADDRCAPFLAGS_FWDNUMRINGS) p_strResult += "FwdNumRings ";
	if (p_dwAddressCaps & LINEADDRCAPFLAGS_PICKUPGROUPID) p_strResult += "PickupGroupID ";
	if (p_dwAddressCaps & LINEADDRCAPFLAGS_SECURE) p_strResult += "Secure ";
	if (p_dwAddressCaps & LINEADDRCAPFLAGS_BLOCKIDDEFAULT) p_strResult += "BlockIDDefault ";
	if (p_dwAddressCaps & LINEADDRCAPFLAGS_BLOCKIDOVERRIDE) p_strResult += "BlockIDOverride ";
	if (p_dwAddressCaps & LINEADDRCAPFLAGS_DIALED) p_strResult += "Dialed ";
	if (p_dwAddressCaps & LINEADDRCAPFLAGS_ORIGOFFHOOK) p_strResult += "OrigOffHook ";
	if (p_dwAddressCaps & LINEADDRCAPFLAGS_DESTOFFHOOK) p_strResult += "DeskOffHook ";
	if (p_dwAddressCaps & LINEADDRCAPFLAGS_FWDCONSULT) p_strResult += "FwdConsult ";
	if (p_dwAddressCaps & LINEADDRCAPFLAGS_SETUPCONFNULL) p_strResult += "SetupConfNull ";
	if (p_dwAddressCaps & LINEADDRCAPFLAGS_AUTORECONNECT) p_strResult += "AutoReconnect ";
	if (p_dwAddressCaps & LINEADDRCAPFLAGS_COMPLETIONID) p_strResult += "CompletionID ";
	if (p_dwAddressCaps & LINEADDRCAPFLAGS_TRANSFERHELD) p_strResult += "TransferHeld ";
	if (p_dwAddressCaps & LINEADDRCAPFLAGS_TRANSFERMAKE) p_strResult += "TransferMake ";
	if (p_dwAddressCaps & LINEADDRCAPFLAGS_CONFERENCEHELD) p_strResult += "ConferenceHeld ";
	if (p_dwAddressCaps & LINEADDRCAPFLAGS_CONFERENCEMAKE) p_strResult += "ConferenceMake ";
	if (p_dwAddressCaps & LINEADDRCAPFLAGS_PARTIALDIAL) p_strResult += "PartialDial ";
	if (p_dwAddressCaps & LINEADDRCAPFLAGS_FWDSTATUSVALID) p_strResult += "FwdStatusValid ";
	if (p_dwAddressCaps & LINEADDRCAPFLAGS_FWDINTEXTADDR) p_strResult += "FwdIntExtAddr ";
	if (p_dwAddressCaps & LINEADDRCAPFLAGS_FWDBUSYNAADDR) p_strResult += "FwdBusyNAAddr ";
	if (p_dwAddressCaps & LINEADDRCAPFLAGS_ACCEPTTOALERT) p_strResult += "AcceptToAlert ";
	if (p_dwAddressCaps & LINEADDRCAPFLAGS_CONFDROP) p_strResult += "ConfDrop ";
	if (p_dwAddressCaps & LINEADDRCAPFLAGS_PICKUPCALLWAIT) p_strResult += "PickupCallWait ";
	if (p_dwAddressCaps & LINEADDRCAPFLAGS_PREDICTIVEDIALER) p_strResult += "PredictiveDialer ";
	if (p_dwAddressCaps & LINEADDRCAPFLAGS_QUEUE) p_strResult += "Queue ";
	if (p_dwAddressCaps & LINEADDRCAPFLAGS_ROUTEPOINT) p_strResult += "RoutePoint ";
	if (p_dwAddressCaps & LINEADDRCAPFLAGS_HOLDMAKESNEW) p_strResult += "HoldMakesNew ";
	if (p_dwAddressCaps & LINEADDRCAPFLAGS_NOINTERNALCALLS) p_strResult += "NoInternalCalls ";
	if (p_dwAddressCaps & LINEADDRCAPFLAGS_NOEXTERNALCALLS) p_strResult += "NoExternalCalls ";
	if (p_dwAddressCaps & LINEADDRCAPFLAGS_SETCALLINGID) p_strResult += "SetCallingID ";
	if (p_strResult.empty()) p_strResult += "Unknown";

	return std::string(p_strResult);
}

// ----------------------------------------------------------------------------
// Describe capabilities of an address on a line
std::string CAvayaResult::DescribeCallFeatures(std::string& p_strResult, DWORD p_dwCallFeatures)
{
	p_strResult.clear();

	if (p_dwCallFeatures == 0) p_strResult += "None";
	if (p_dwCallFeatures & LINECALLFEATURE_ACCEPT) p_strResult += "Accept ";
	if (p_dwCallFeatures & LINECALLFEATURE_ADDTOCONF) p_strResult += "AddToConf ";
	if (p_dwCallFeatures & LINECALLFEATURE_ANSWER) p_strResult += "Answer ";
	if (p_dwCallFeatures & LINECALLFEATURE_BLINDTRANSFER) p_strResult += "BlindTransfer ";
	if (p_dwCallFeatures & LINECALLFEATURE_COMPLETECALL) p_strResult += "CompleteCall ";
	if (p_dwCallFeatures & LINECALLFEATURE_COMPLETETRANSF) p_strResult += "CompleteTransf ";
	if (p_dwCallFeatures & LINECALLFEATURE_DIAL) p_strResult += "Dial ";
	if (p_dwCallFeatures & LINECALLFEATURE_DROP) p_strResult += "Drop ";
	if (p_dwCallFeatures & LINECALLFEATURE_GATHERDIGITS) p_strResult += "GatherDigits ";
	if (p_dwCallFeatures & LINECALLFEATURE_GENERATEDIGITS) p_strResult += "GenerateDigits ";
	if (p_dwCallFeatures & LINECALLFEATURE_GENERATETONE) p_strResult += "GenerateTone ";
	if (p_dwCallFeatures & LINECALLFEATURE_HOLD) p_strResult += "Hold ";
	if (p_dwCallFeatures & LINECALLFEATURE_MONITORDIGITS) p_strResult += "MonitorDigits ";
	if (p_dwCallFeatures & LINECALLFEATURE_MONITORMEDIA) p_strResult += "MonitorMedia ";
	if (p_dwCallFeatures & LINECALLFEATURE_MONITORTONES) p_strResult += "MonitorTones ";
	if (p_dwCallFeatures & LINECALLFEATURE_PARK) p_strResult += "Park ";
	if (p_dwCallFeatures & LINECALLFEATURE_PREPAREADDCONF) p_strResult += "PrepareAddConf ";
	if (p_dwCallFeatures & LINECALLFEATURE_REDIRECT) p_strResult += "Redirect ";
	if (p_dwCallFeatures & LINECALLFEATURE_REMOVEFROMCONF) p_strResult += "RemoveFromConf ";
	if (p_dwCallFeatures & LINECALLFEATURE_SECURECALL) p_strResult += "SecureCall ";
	if (p_dwCallFeatures & LINECALLFEATURE_SENDUSERUSER) p_strResult += "SendUserUser ";
	if (p_dwCallFeatures & LINECALLFEATURE_SETCALLPARAMS) p_strResult += "SetCallParams ";
	if (p_dwCallFeatures & LINECALLFEATURE_SETMEDIACONTROL) p_strResult += "SetMediaControl ";
	if (p_dwCallFeatures & LINECALLFEATURE_SETTERMINAL) p_strResult += "SetTerminal ";
	if (p_dwCallFeatures & LINECALLFEATURE_SETUPCONF) p_strResult += "SetupConf ";
	if (p_dwCallFeatures & LINECALLFEATURE_SETUPTRANSFER) p_strResult += "SetupTransfer ";
	if (p_dwCallFeatures & LINECALLFEATURE_SWAPHOLD) p_strResult += "SwapHold ";
	if (p_dwCallFeatures & LINECALLFEATURE_UNHOLD) p_strResult += "Unhold ";
	if (p_dwCallFeatures & LINECALLFEATURE_RELEASEUSERUSERINFO) p_strResult += "ReleaseUserUserInfo ";
	if (p_dwCallFeatures & LINECALLFEATURE_SETTREATMENT) p_strResult += "SetTreatment ";
	if (p_dwCallFeatures & LINECALLFEATURE_SETQOS) p_strResult += "SetQOS ";
	if (p_dwCallFeatures & LINECALLFEATURE_SETCALLDATA) p_strResult += "SetCallData ";
	if (p_strResult.empty()) p_strResult += "Unknown";

	return p_strResult;
}

// ----------------------------------------------------------------------------
// Describe capabilities of an address on a line
std::string CAvayaResult::DescribeCallFeatures2(std::string& p_strResult, DWORD p_dwCallFeatures2)
{
	p_strResult.clear();

	if (p_dwCallFeatures2 == 0) p_strResult += " "; // Don't mess up the string
	if (p_dwCallFeatures2 & LINECALLFEATURE2_NOHOLDCONFERENCE) p_strResult += "NoHoldConference ";
	if (p_dwCallFeatures2 & LINECALLFEATURE2_ONESTEPTRANSFER) p_strResult += "OneStepTransfer ";
	if (p_dwCallFeatures2 & LINECALLFEATURE2_COMPLCAMPON) p_strResult += "ComplCampOn ";
	if (p_dwCallFeatures2 & LINECALLFEATURE2_COMPLCALLBACK) p_strResult += "ComplCallBack ";
	if (p_dwCallFeatures2 & LINECALLFEATURE2_COMPLINTRUDE) p_strResult += "ComplIntrude ";
	if (p_dwCallFeatures2 & LINECALLFEATURE2_COMPLMESSAGE) p_strResult += "ComplMessage ";
	if (p_dwCallFeatures2 & LINECALLFEATURE2_TRANSFERNORM) p_strResult += "TransferNorm ";
	if (p_dwCallFeatures2 & LINECALLFEATURE2_TRANSFERCONF) p_strResult += "TransferConf ";
	if (p_dwCallFeatures2 & LINECALLFEATURE2_PARKDIRECT) p_strResult += "ParkDirect ";
	if (p_dwCallFeatures2 & LINECALLFEATURE2_PARKNONDIRECT) p_strResult += "ParkNonDirect ";
	if (p_strResult.empty()) p_strResult += "Unknown";

	return p_strResult;
}