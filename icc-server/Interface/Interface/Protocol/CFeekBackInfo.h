/**/
#pragma once
#include <Protocol/CHeader.h>
#include <PGClient/IResultSet.h>

namespace ICC
{
	namespace PROTOCOL
	{
		class CFeekBackInfo
		{
			public:
			bool ParseString(const std::string &p_strPrefix, JsonParser::IJsonPtr &p_pJson)
			{
				m_strMsgSource = p_pJson->GetNodeValue(p_strPrefix+"/msg_source", "");	
				m_strID = p_pJson->GetNodeValue(p_strPrefix+"/id", "");
				m_strReceiptDeptDistrictCode = p_pJson->GetNodeValue(p_strPrefix+"/district", "");
				m_strAlarmID = p_pJson->GetNodeValue(p_strPrefix+"/caseId", "");
				m_strProcessID = p_pJson->GetNodeValue(p_strPrefix+"/dispatchId", "");
				m_strRecordID = p_pJson->GetNodeValue(p_strPrefix+"/record", "");
				m_strFeedbackOrgCode = p_pJson->GetNodeValue(p_strPrefix+"/feedbackOrgCode", "");
				m_strFeedbackCode = p_pJson->GetNodeValue(p_strPrefix+"/feedbackCode", "");
				m_strFeedbackName = p_pJson->GetNodeValue(p_strPrefix+"/feedbackName", "");
				m_strFeedbackTime = p_pJson->GetNodeValue(p_strPrefix+"/feedbackTime", "");
				m_strForceAcceptTime = p_pJson->GetNodeValue(p_strPrefix+"/forceAcceptTime", "");
				m_strArriveTime = p_pJson->GetNodeValue(p_strPrefix+"/arriveTime", "");
				m_strSiteDoneTime = p_pJson->GetNodeValue(p_strPrefix+"/siteDoneTime", "");
				m_strCaseType = p_pJson->GetNodeValue(p_strPrefix+"/caseType", "");
				m_strAlarmSecondType = p_pJson->GetNodeValue(p_strPrefix+"/case2ndType", "");
				m_strAlarmThirdType = p_pJson->GetNodeValue(p_strPrefix+"/case3rdType", "");
				m_strHappenTime = p_pJson->GetNodeValue(p_strPrefix+"/happenTime", "");
				m_strAlarmLongitude = p_pJson->GetNodeValue(p_strPrefix+"/longitude", "");
				m_strAlarmLatitude = p_pJson->GetNodeValue(p_strPrefix+"/latitude", "");
				m_strDisposalDec = p_pJson->GetNodeValue(p_strPrefix+"/disposalDec", "");
				m_strDispatcheVehicles = p_pJson->GetNodeValue(p_strPrefix+"/dispatcheVehicles", "");
				m_strDispatchePeople = p_pJson->GetNodeValue(p_strPrefix+"/dispatchePeople", "");
				m_strRescuers = p_pJson->GetNodeValue(p_strPrefix+"/rescuers", "");
				m_strRescuersDes = p_pJson->GetNodeValue(p_strPrefix+"/rescuersDes", "");
				m_strIinjured = p_pJson->GetNodeValue(p_strPrefix+"/injured", "");
				m_strInjuredDes = p_pJson->GetNodeValue(p_strPrefix+"/injuredDes", "");
				m_strDead = p_pJson->GetNodeValue(p_strPrefix+"/dead", "");
				m_strDeadDes = p_pJson->GetNodeValue(p_strPrefix+"/deadDes", "");
				m_strResultCode = p_pJson->GetNodeValue(p_strPrefix+"/resultCode", "");
				m_strResultDes = p_pJson->GetNodeValue(p_strPrefix+"/resultDes", "");
				m_strWeatherCode = p_pJson->GetNodeValue(p_strPrefix+"/weatherCode", "");
				m_strLossDes = p_pJson->GetNodeValue(p_strPrefix+"/lossDes", "");
				m_strCaught = p_pJson->GetNodeValue(p_strPrefix+"/caught", "");
				m_strInvolved = p_pJson->GetNodeValue(p_strPrefix+"/involved", "");
				m_strEscapees = p_pJson->GetNodeValue(p_strPrefix+"/escapees", "");
				m_strTrafficAccidentCode = p_pJson->GetNodeValue(p_strPrefix+"/trafficAccidentCode", "");
				m_strIsCarryDangerousGoods = p_pJson->GetNodeValue(p_strPrefix+"/isCarryDangerousGoods", "");
				m_strTrafficAccidentCauseCode = p_pJson->GetNodeValue(p_strPrefix+"/trafficAccidentCauseCode", "");
				m_strVisibilityCode = p_pJson->GetNodeValue(p_strPrefix+"/visibilityCode", "");
				m_strRoadConditionCode = p_pJson->GetNodeValue(p_strPrefix+"/roadConditionCode", "");
				m_strDamagedVehicles = p_pJson->GetNodeValue(p_strPrefix+"/damagedVehicles", "");
				m_strDamagedNonMotorVehicles = p_pJson->GetNodeValue(p_strPrefix+"/damagedNonMotorVehicles", "");
				m_strRoadTypeCode = p_pJson->GetNodeValue(p_strPrefix+"/roadTypeCode", "");
				m_strStatus = p_pJson->GetNodeValue(p_strPrefix+"/status", "");
				m_strCreateTime = p_pJson->GetNodeValue(p_strPrefix+"/createTime", "");
				m_strUpdateTime = p_pJson->GetNodeValue(p_strPrefix+"/updateTime", "");
				m_strTownShip = p_pJson->GetNodeValue(p_strPrefix+"/townShip", "");
				m_strTerritorialVillage = p_pJson->GetNodeValue(p_strPrefix+"/territorialVillage", "");
				m_strIsEvidence = p_pJson->GetNodeValue(p_strPrefix+"/isEvidence", "");
				m_strTransferOrgCode = p_pJson->GetNodeValue(p_strPrefix+"/transferOrgCode", "");
				m_strTransferOrgContacts = p_pJson->GetNodeValue(p_strPrefix+"/transferOrgContacts", "");
				m_strTransferOrgFeedbackDesCode = p_pJson->GetNodeValue(p_strPrefix+"/transferOrgFeedbackDesCode", "");
				m_strTransferOrgFeedbackDes = p_pJson->GetNodeValue(p_strPrefix+"/transferOrgFeedbackDes", "");
				m_strCaseNumber = p_pJson->GetNodeValue(p_strPrefix+"/caseNumber", "");
				m_strLable = p_pJson->GetNodeValue(p_strPrefix+"/lable", "");
				m_strOccurredArea = p_pJson->GetNodeValue(p_strPrefix+"/occurredArea", "");
				m_strLocationCode = p_pJson->GetNodeValue(p_strPrefix+"/locationCode", "");
				m_strForceId = p_pJson->GetNodeValue(p_strPrefix+"/forceId", "");
				m_strPoliceType = p_pJson->GetNodeValue(p_strPrefix+"/policeType", "");
				m_strVersion = p_pJson->GetNodeValue(p_strPrefix+"/version", "");
				m_strBllStatus = p_pJson->GetNodeValue(p_strPrefix+"/bllStatus", "");
				m_strFeedbackOrgIdentifier = p_pJson->GetNodeValue(p_strPrefix+"/feedbackOrgIdentifier", "");
				m_strFeedbackOrgId = p_pJson->GetNodeValue(p_strPrefix+"/feedbackOrgId", "");
				m_strFeedbackOrgName = p_pJson->GetNodeValue(p_strPrefix+"/feedbackOrgName", "");
				m_strLiveStatus = p_pJson->GetNodeValue(p_strPrefix+"/liveStatus", "");
				m_strCase4thType = p_pJson->GetNodeValue(p_strPrefix+"/case4thType", "");
				m_strUpdateOrgIdentifier = p_pJson->GetNodeValue(p_strPrefix+"/updateOrgIdentifier", "");
				m_strTimeOutStatus = p_pJson->GetNodeValue(p_strPrefix+"/timeOutStatus", "");
				m_strLaunchBoat = p_pJson->GetNodeValue(p_strPrefix+"/launchBoat", "");
				m_strAlarmAddress = p_pJson->GetNodeValue(p_strPrefix+"/alarmAddress", "");
				m_strAddress = p_pJson->GetNodeValue(p_strPrefix+"/address", "");
				m_strIsInvaild = p_pJson->GetNodeValue(p_strPrefix+"/isInvaild", "");
				m_strTransferOrgName = p_pJson->GetNodeValue(p_strPrefix+"/transferOrgName", "");
				m_strTransferOrgIdentifier = p_pJson->GetNodeValue(p_strPrefix+"/transferOrgIdentifier", "");
				m_strTransferOrgContactsId = p_pJson->GetNodeValue(p_strPrefix+"/transferOrgContactsId", "");
				m_strModifyAreaOrgId = p_pJson->GetNodeValue(p_strPrefix+"/modifyAreaOrgId", "");
				m_strModifyAreaOrgName = p_pJson->GetNodeValue(p_strPrefix+"/modifyAreaOrgName", "");
				m_strModifyAreaOrgIdentifier = p_pJson->GetNodeValue(p_strPrefix+"/modifyAreaOrgIdentifier", "");
				m_strTransferOrgId = p_pJson->GetNodeValue(p_strPrefix+"/transferOrgId", "");
				m_strReceivedTime = p_pJson->GetNodeValue(p_strPrefix + "/received_time", "");
				m_strDistrictName = p_pJson->GetNodeValue(p_strPrefix + "/district_name", "");
				return true;
			}
			
			void ComJson(const std::string& p_strPrefix, JsonParser::IJsonPtr& p_pJson)
			{
				if (nullptr == p_pJson)
				{
					return;
				}
				p_pJson->SetNodeValue(p_strPrefix + "/msg_source", m_strMsgSource);
				p_pJson->SetNodeValue(p_strPrefix + "/id", m_strID);
				p_pJson->SetNodeValue(p_strPrefix + "/destrict", m_strReceiptDeptDistrictCode);
				p_pJson->SetNodeValue(p_strPrefix + "/caseId", m_strAlarmID);
				p_pJson->SetNodeValue(p_strPrefix + "/dispatchId", m_strProcessID);
				p_pJson->SetNodeValue(p_strPrefix + "/record", m_strRecordID);
				p_pJson->SetNodeValue(p_strPrefix + "/feedbackOrgCode", m_strFeedbackOrgCode);
				p_pJson->SetNodeValue(p_strPrefix + "/feedbackCode", m_strFeedbackCode);
				p_pJson->SetNodeValue(p_strPrefix + "/feedbackName", m_strFeedbackName);
				p_pJson->SetNodeValue(p_strPrefix + "/feedbackTime", m_strFeedbackTime);
				p_pJson->SetNodeValue(p_strPrefix + "/forceAcceptTime", m_strForceAcceptTime);
				p_pJson->SetNodeValue(p_strPrefix + "/arriveTime", m_strArriveTime);
				p_pJson->SetNodeValue(p_strPrefix + "/siteDoneTime", m_strSiteDoneTime);
				p_pJson->SetNodeValue(p_strPrefix + "/caseType", m_strCaseType);
				p_pJson->SetNodeValue(p_strPrefix + "/case2ndType", m_strAlarmSecondType);
				p_pJson->SetNodeValue(p_strPrefix + "/case3rdType", m_strAlarmThirdType);
				p_pJson->SetNodeValue(p_strPrefix + "/happenTime", m_strHappenTime);
				p_pJson->SetNodeValue(p_strPrefix + "/longitude", m_strAlarmLongitude);
				p_pJson->SetNodeValue(p_strPrefix + "/latitude", m_strAlarmLatitude);
				p_pJson->SetNodeValue(p_strPrefix + "/disposalDec", m_strDisposalDec);
				p_pJson->SetNodeValue(p_strPrefix + "/dispatcheVehicles", m_strDispatcheVehicles);
				p_pJson->SetNodeValue(p_strPrefix + "/dispatchePeople", m_strDispatchePeople);
				p_pJson->SetNodeValue(p_strPrefix + "/rescuers", m_strRescuers);
				p_pJson->SetNodeValue(p_strPrefix + "/rescuersDes", m_strRescuersDes);
				p_pJson->SetNodeValue(p_strPrefix + "/injured", m_strIinjured);
				p_pJson->SetNodeValue(p_strPrefix + "/injuredDes", m_strInjuredDes);
				p_pJson->SetNodeValue(p_strPrefix + "/dead", m_strDead);
				p_pJson->SetNodeValue(p_strPrefix + "/deadDes", m_strDeadDes);
				p_pJson->SetNodeValue(p_strPrefix + "/resultCode", m_strResultCode);
				p_pJson->SetNodeValue(p_strPrefix + "/resultDes", m_strResultDes);
				p_pJson->SetNodeValue(p_strPrefix + "/weatherCode", m_strWeatherCode);
				p_pJson->SetNodeValue(p_strPrefix + "/lossDes", m_strLossDes);
				p_pJson->SetNodeValue(p_strPrefix + "/caught", m_strCaught);
				p_pJson->SetNodeValue(p_strPrefix + "/involved", m_strInvolved);
				p_pJson->SetNodeValue(p_strPrefix + "/escapees", m_strEscapees);
				p_pJson->SetNodeValue(p_strPrefix + "/trafficAccidentCode", m_strTrafficAccidentCode);
				p_pJson->SetNodeValue(p_strPrefix + "/isCarryDangerousGoods", m_strIsCarryDangerousGoods);
				p_pJson->SetNodeValue(p_strPrefix + "/trafficAccidentCauseCode", m_strTrafficAccidentCauseCode);
				p_pJson->SetNodeValue(p_strPrefix + "/visibilityCode", m_strVisibilityCode);
				p_pJson->SetNodeValue(p_strPrefix + "/roadConditionCode", m_strRoadConditionCode);
				p_pJson->SetNodeValue(p_strPrefix + "/damagedVehicles", m_strDamagedVehicles);
				p_pJson->SetNodeValue(p_strPrefix + "/damagedNonMotorVehicles", m_strDamagedNonMotorVehicles);
				p_pJson->SetNodeValue(p_strPrefix + "/roadTypeCode", m_strRoadTypeCode);
				p_pJson->SetNodeValue(p_strPrefix + "/status", m_strStatus);
				p_pJson->SetNodeValue(p_strPrefix + "/createTime", m_strCreateTime);
				p_pJson->SetNodeValue(p_strPrefix + "/updateTime", m_strUpdateTime);
				p_pJson->SetNodeValue(p_strPrefix + "/townShip", m_strTownShip);
				p_pJson->SetNodeValue(p_strPrefix + "/territorialVillage", m_strTerritorialVillage);
				p_pJson->SetNodeValue(p_strPrefix + "/isEvidence", m_strIsEvidence);
				p_pJson->SetNodeValue(p_strPrefix + "/transferOrgCode", m_strTransferOrgCode);
				p_pJson->SetNodeValue(p_strPrefix + "/transferOrgContacts", m_strTransferOrgContacts);
				p_pJson->SetNodeValue(p_strPrefix + "/transferOrgFeedbackDesCode", m_strTransferOrgFeedbackDesCode);
				p_pJson->SetNodeValue(p_strPrefix + "/transferOrgFeedbackDes", m_strTransferOrgFeedbackDes);
				p_pJson->SetNodeValue(p_strPrefix + "/caseNumber", m_strCaseNumber);
				p_pJson->SetNodeValue(p_strPrefix + "/lable", m_strLable);
				p_pJson->SetNodeValue(p_strPrefix + "/occurredArea", m_strOccurredArea);
				p_pJson->SetNodeValue(p_strPrefix + "/locationCode", m_strLocationCode);
				p_pJson->SetNodeValue(p_strPrefix + "/forceId", m_strForceId);
				p_pJson->SetNodeValue(p_strPrefix + "/policeType", m_strPoliceType);
				p_pJson->SetNodeValue(p_strPrefix + "/version", m_strVersion);
				p_pJson->SetNodeValue(p_strPrefix + "/bllStatus", m_strBllStatus);
				p_pJson->SetNodeValue(p_strPrefix + "/feedbackOrgIdentifier", m_strFeedbackOrgIdentifier);
				p_pJson->SetNodeValue(p_strPrefix + "/feedbackOrgId", m_strFeedbackOrgId);
				p_pJson->SetNodeValue(p_strPrefix + "/feedbackOrgName", m_strFeedbackOrgName);
				p_pJson->SetNodeValue(p_strPrefix + "/liveStatus", m_strLiveStatus);
				p_pJson->SetNodeValue(p_strPrefix + "/case4thType", m_strCase4thType);
				p_pJson->SetNodeValue(p_strPrefix + "/updateOrgIdentifier", m_strUpdateOrgIdentifier);
				p_pJson->SetNodeValue(p_strPrefix + "/timeOutStatus", m_strTimeOutStatus);
				p_pJson->SetNodeValue(p_strPrefix + "/launchBoat", m_strLaunchBoat);
				p_pJson->SetNodeValue(p_strPrefix + "/alarmAddress", m_strAlarmAddress);
				p_pJson->SetNodeValue(p_strPrefix + "/address", m_strAddress);
				p_pJson->SetNodeValue(p_strPrefix + "/isInvaild", m_strIsInvaild);
				p_pJson->SetNodeValue(p_strPrefix + "/transferOrgName", m_strTransferOrgName);
				p_pJson->SetNodeValue(p_strPrefix + "/transferOrgIdentifier", m_strTransferOrgIdentifier);
				p_pJson->SetNodeValue(p_strPrefix + "/transferOrgContactsId", m_strTransferOrgContactsId);
				p_pJson->SetNodeValue(p_strPrefix + "/modifyAreaOrgId", m_strModifyAreaOrgId);
				p_pJson->SetNodeValue(p_strPrefix + "/modifyAreaOrgName", m_strModifyAreaOrgName);
				p_pJson->SetNodeValue(p_strPrefix + "/modifyAreaOrgIdentifier", m_strModifyAreaOrgIdentifier);
				p_pJson->SetNodeValue(p_strPrefix + "/transferOrgId", m_strTransferOrgId);
				p_pJson->SetNodeValue(p_strPrefix + "/district_name", m_strDistrictName);

			}

			bool ParseFeekBackRecord(DataBase::IResultSetPtr &l_pRSet)
			{
				if (NULL == l_pRSet.get())
				{
					return false;
				}
				m_strID = l_pRSet->GetValue("id");
				//m_strMergeID = l_pRSet->GetValue("merge_id");
				return true;
			}

			public:
				std::string m_strMsgSource;
				
				std::string m_strID;	//反馈单编号
				std::string m_strReceiptDeptDistrictCode;	//行政区划代码
				std::string m_strAlarmID;	//接警单编号
				std::string m_strProcessID;	//派警单编号
				std::string m_strRecordID;	//反馈录音号
				std::string m_strFeedbackOrgCode;	//反馈单位代码
				std::string m_strFeedbackCode;	//反馈员编号
				std::string m_strFeedbackName;	//反馈员姓名
				std::string m_strFeedbackTime;	//反馈时间
				std::string m_strForceAcceptTime;	//出警时间
				std::string m_strArriveTime;	//到达现场时间
				std::string m_strSiteDoneTime;	//现场处理完毕时间
				std::string m_strCaseType;	//警情类别代码
				std::string m_strAlarmSecondType;	//警情类型代码
				std::string m_strAlarmThirdType;	//警情细类代码
				std::string m_strHappenTime;	//警情发生时间
				std::string m_strAlarmLongitude;	//反馈定位X坐标（处置现场位置）
				std::string m_strAlarmLatitude;	//反馈定位Y坐标（处置现场位置）
				std::string m_strDisposalDec;	//出警处置情况
				std::string m_strDispatcheVehicles;	//出动车次
				std::string m_strDispatchePeople;	//出动人次
				std::string m_strRescuers;	//救助人数
				std::string m_strRescuersDes;	//救助人数说明
				std::string m_strIinjured;	//受伤人数
				std::string m_strInjuredDes;	//受伤人数说明
				std::string m_strDead;	//死亡人数
				std::string m_strDeadDes;	//死亡人数说明
				std::string m_strResultCode;	//警情处理结果代码
				std::string m_strResultDes;	//警情处理结果说明
				std::string m_strWeatherCode;	//天气情况代码
				std::string m_strLossDes;	//损失情况描述
				std::string m_strCaught;	//抓获人数
				std::string m_strInvolved;	//涉案人数
				std::string m_strEscapees;	//逃跑人数
				std::string m_strTrafficAccidentCode;	//交通事故形态代码
				std::string m_strIsCarryDangerousGoods;	//是否装载危险品
				std::string m_strTrafficAccidentCauseCode;	//交通事故初查原因代码
				std::string m_strVisibilityCode;	//能见度代码
				std::string m_strRoadConditionCode;	//路面状况代码
				std::string m_strDamagedVehicles;	//损坏机动车数
				std::string m_strDamagedNonMotorVehicles;	//损坏非机动车数
				std::string m_strRoadTypeCode;	//道路类型代码
				std::string m_strStatus;	//警情处理状态代码
				std::string m_strCreateTime;	//创建时间
				std::string m_strUpdateTime;	//更新时间
				std::string m_strTownShip;	//乡镇（街道）
				std::string m_strTerritorialVillage;	//属地村社
				std::string m_strIsEvidence;	//是否取证
				std::string m_strTransferOrgCode;	//移交单位代码
				std::string m_strTransferOrgContacts;	//移交单位联系人
				std::string m_strTransferOrgFeedbackDesCode;	//移交单位反馈情况
				std::string m_strTransferOrgFeedbackDes;	//移交单位反馈情况说明
				std::string m_strCaseNumber;	//案件编号
				std::string m_strLable;	//警情标签
				std::string m_strOccurredArea;	//警情发生地域 
				std::string m_strLocationCode;	//警情部位代码
				std::string m_strForceId;	//警力id
				std::string m_strPoliceType;	//警力类型
				std::string m_strVersion;	//最后同步版本号
				std::string m_strBllStatus;	//vcs当前业务备注
				std::string m_strFeedbackOrgIdentifier;	//反馈单位短码
				std::string m_strFeedbackOrgId;	//反馈单位id
				std::string m_strFeedbackOrgName;	//反馈单位名称
				std::string m_strLiveStatus;	//现场状态
				std::string m_strCase4thType; 	//警情子类代码
				std::string m_strUpdateOrgIdentifier;	//提交单位短码
				std::string m_strTimeOutStatus;	//超时状态
				std::string m_strLaunchBoat;	//出动船艇
				std::string m_strAlarmAddress;	//报警地址
				std::string m_strAddress;	//警情地址
				std::string m_strIsInvaild;	//是否无效警情标识
				std::string m_strTransferOrgName;	//移交单位名称
				std::string m_strTransferOrgIdentifier;	//移交单位短码
				std::string m_strTransferOrgContactsId;	//移交单位联系人id  
				std::string m_strModifyAreaOrgId;	//调整后管辖单位id
				std::string m_strModifyAreaOrgName;	//调整后管辖单位名称
				std::string m_strModifyAreaOrgIdentifier;	//调整后管辖单位短码
				std::string m_strTransferOrgId;	//移交单位id
				std::string m_strReceivedTime;		// 接警时间
				std::string m_strDistrictName;		// 行政区划名称
		};	
	}
}
