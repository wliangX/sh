#pragma once

// boost
#include <boost/algorithm/algorithm.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/thread.hpp>

#include <algorithm>
// icc
#include <ICC/ICCLibExport.h>
#include <ICC/ICCGetResource.h>
#include <Base/FactoryBase.h>
#include <Base/PluginBase.h>
#include <Base/BusinessBase.h>

#include <Thread/IThreadFactory.h>
#include <AmqClient/IAmqClient.h>
#include <Observer/IObserverFactory.h>
#include <Lock/ILockFactory.h>
#include <Json/IJsonFactory.h>
#include <DateTime/IDateTimeFactory.h>
#include <StringUtil/IStringFactory.h>
#include <Log/ILogFactory.h>
#include <DBConn/IDBConnFactory.h>
#include <Config/IConfigFactory.h>
#include <Redisclient/IRedisClientFactory.h>
#include <Timer/ITimerFactory.h>
#include <HttpClient/IHttpClientFactory.h>
#include <MessageCenter/IMessageCenterFactory.h>

#include "Protocol/CAlarmSync.h"
#include "Protocol/CAlarmProcessSync.h"
#include "Protocol/CFeedBackSync.h"
#include "Protocol/CAlarmLogSync.h"
#include "Protocol/CAlarmInfoChange.h"
#include "Protocol/CProcessInfoChange.h"
#include "Protocol/CFeedBackInfoChange.h"
#include "Protocol/CTelinComingChange.h"
#include "Protocol/CTIServer/CallStateEvent.h"
#include "Protocol/CTIServer/DeviceStateEvent.h"
#include "Protocol/CVCSRequestICCInfo.h"
#include "Protocol/CCaseInfoVCSRespond.h"
#include "Protocol/CCaseFeedBackVCSRespond.h"
#include "Protocol/CCaseDisposalVCSRespond.h"
#include "Protocol/CVcsSetAlarmInfoRequest.h"
#include "Protocol/CVcsSetProcessInfoRequest.h"
#include "Protocol/CVcsSetFeedbackInfoRequest.h"
#include "Protocol/CVcsSetDeviceInfoRequest.h"
#include "Protocol/CVcsSetRecordInfoRequest.h"
#include "Protocol/CAddOrUpdateAlarmWithProcessRequest.h"
#include "Protocol/CSetFeedbackRequest.h"
#include "Protocol/CAddAlarmRemarkRequest.h"
#include "Protocol/CGetGpsRequest.h"
#include "Protocol/CGetGpsRespond.h"
#include "Protocol/CGpsSync.h"
#include "Protocol/CVcsTextMsg.h"
#include "Protocol/CVcsSynDataSar.h"
#include "Protocol/AlarmStructDefine.h"
#include "Protocol/CAlarmLinkedSync.h"
#include "Protocol/CSetFeedbackRequest.h"
#include "Protocol/CUpdateDisposalRequest.h"
#include "Protocol/CUpdateFeedbackRequest.h"
#include "Protocol/CTelHangupRespond.h"

#include "Protocol/CAdminDeptRequset.h"
#include "Protocol/CVcsUpdateLogRequset.h"
#include "Protocol/CVcsDeleteDisposalRequset.h"

#include "Protocol/CAlarmRelatedCarsProtocol.h"
#include "Protocol/CAlarmRelatedPersonsProtocol.h"
#include "Protocol/CVcsSynDataProtocol.h"
#include "Protocol/CVcsBillStatusSync.h"
#include "Protocol/CSetTemporaryHoldProtocol.h"

#include "Protocol/SyncServerLock.h"
#include <Protocol/CSearchCallerRequest.h>

#include "Protocol/CFeedBackSyncEx.h"
#include <Protocol/ParamInfo.h>

#include <Protocol/AlarmAttachRequest.h>
#include <Protocol/AlarmAttachResponse.h>

#define MODULE_NAME "vcs"

using namespace ICC;