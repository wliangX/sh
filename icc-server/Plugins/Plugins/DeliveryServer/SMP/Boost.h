#pragma once

#include <queue>
#include <vector>
// boost
#include <boost/algorithm/algorithm.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/thread.hpp>

// icc
#include <algorithm>

#include <ICC/ICCLibExport.h>
#include <ICC/ICCGetResource.h>
#include <Base/FactoryBase.h>
#include <Base/PluginBase.h>
#include <Base/BusinessBase.h>

#include <AmqClient/IAmqClient.h>
#include <Config/IConfigFactory.h>
#include <Log/ILogFactory.h>
#include <Lock/ILockFactory.h>
#include <Json/IJsonFactory.h>
#include <StringUtil/IStringFactory.h>
#include <DateTime/IDateTimeFactory.h>
#include <Lock/ILockFactory.h>
#include <Observer/IObserverFactory.h>
#include <Timer/ITimerFactory.h>
#include <Xml/IXmlFactory.h>
#include <DBConn/IDBConnFactory.h>
#include <Redisclient/IRedisClientFactory.h>
#include <License/LicenselFactory.h>
#include <HttpClient/IHttpClientFactory.h>

#include <Protocol/CSetDeptRequest.h>
#include <Protocol/CSetDeptRespond.h>
#include <Protocol/CSetStaffRequest.h>
#include <Protocol/CSetStaffRespond.h>
#include <Protocol/CPushUserRequest.h>
#include <Protocol/CPushUserRespond.h>
#include <Protocol/CSetDictRequest.h>
#include <Protocol/CSetDictRespond.h>
#include <Protocol/CSetLanguageRequest.h>
#include <Protocol/CSetLanguageRespond.h>
#include <Protocol/CGetDeptInfoRequest.h>
#include <Protocol/CGetStaffInfoRequest.h>
#include <Protocol/CGetAuthInfoRequest.h>
#include <Protocol/CGetDictInfoRequest.h>
#include <Protocol/CGetUserInfoRequest.h>
#include <Protocol/CGetUserStaffInfoRequest.h>
#include <Protocol/CGetUserRoleInfoRequest.h>
#include <Protocol/CGetLanguageInfoRequest.h>
#include <Protocol/CGetRoleFuncBindRequest.h>
#include <Protocol/CSetBindRequest.h>
#include <Protocol/CSetBindRespond.h>
#include <Protocol/CSetRoleRequest.h>
#include <Protocol/CSetRoleRespond.h>
#include <Protocol/CDeleteRoleRequest.h>
#include <Protocol/CDeleteRoleRespond.h>
#include <Protocol/CSetPrivilegeRequest.h>
#include <Protocol/CSetPrivilegeRespond.h>
#include <Protocol/CSmpSyncRequest.h>
#include <Protocol/CDeptInfoChangeRequest.h>
#include <Protocol/CDeleteDeptRequest.h>
#include <Protocol/CDeleteDeptRespond.h>
#include <Protocol/CStaffInfoChangeRequest.h>
#include <Protocol/CDeleteStaffRequest.h>
#include <Protocol/CDeleteStaffRespond.h>
#include <Protocol/CDictInfoChangeRequest.h>
#include <Protocol/CDeleteDictRequest.h>
#include <Protocol/CDeleteDictRespond.h>
#include <Protocol/CUserInfoChangeRequest.h>
#include <Protocol/CDeleteUserRequest.h>
#include <Protocol/CDeleteUserRespond.h>
#include <Protocol/CDeleteBindRequest.h>
#include <Protocol/CDeleteBindRespond.h>

#include <Protocol/CSmpRoleInfoChangeNotify.h>
#include <Protocol/CSmpFuncInfoChangeNotify.h>
#include <Protocol/CSmpLangInfoChangeNotify.h>
#include <Protocol/CDeletePrivilegeRequest.h>
#include <Protocol/CDeleteLanguageRequest.h>
#include <Protocol/CSynBaseDataResultNotify.h>
#include <Protocol/CSynBaseDataResultRequest.h>
#include <Protocol/CSmpSynCmds.h>
#include <Protocol/CSmpSynPackages.h>
#include <Protocol/CSmpDataChangedNotify.h>
#include <Protocol/CSmpLogin.h>
#include <Protocol/CSmpLoginResult.h>
#include <Protocol/CSmpSynDatas.h>
#include <Protocol/CSmpSynDataDict.h>
#include <Protocol/CSmpSynDataUser.h>
#include <Protocol/CSmpSynDataStaff.h>
#include <Protocol/CSmpSynDataFunc.h>
#include <Protocol/CSmpSynDataRole.h>
#include <Protocol/CSmpSynDataOrg.h>
#include <Protocol/CSmpSynNotifyDatas.h>
#include <Protocol/CSmpSynDataDictNotify.h>
#include <Protocol/CSmpSynDataUserNotify.h>
#include <Protocol/CSmpSynDataStaffNotify.h>
#include <Protocol/CSmpSynDataFuncNotify.h>
#include <Protocol/CSmpSynDataRoleNotify.h>
#include <Protocol/CSmpSynDataOrgNotify.h>
#include <Protocol/CDictSync.h>
#include <Protocol/SyncNacosParams.h>
#include <Protocol/SmpSynListResult.h>

#include "Protocol/SyncLicense.h"
#include "Protocol/GetLicense.h"

#include "Protocol/SyncServerLock.h"

#include "ProcessSeatManager.h"

#define MODULE_NAME "smp"

using namespace ICC;