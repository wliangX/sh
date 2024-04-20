#pragma once

namespace ICC
{
//-------------- Components -------------------
#define	PRIORITY_LEVEL_XMLPARSER		100
#define	PRIORITY_LEVEL_LOCK				110
#define	PRIORITY_LEVEL_STRINGUTIL		120
#define	PRIORITY_LEVEL_DATETIME			130
#define	PRIORITY_LEVEL_THREAD			140

#define	PRIORITY_LEVEL_THREADPOOL		150
#define	PRIORITY_LEVEL_HELPTOOL			160
#define	PRIORITY_LEVEL_HTTPCLIENT		170
#define	PRIORITY_LEVEL_CONFIG			180
#define	PRIORITY_LEVEL_FTPCLIENT		190

#define	PRIORITY_LEVEL_JSONPARESER		200
#define	PRIORITY_LEVEL_LOGGER			210
#define	PRIORITY_LEVEL_AMQCLIENT		220
#define	PRIORITY_LEVEL_SQLBUILDER		230
#define	PRIORITY_LEVEL_LICENSE			240
#define	PRIORITY_LEVEL_HTTPSSERVER		245

#define	PRIORITY_LEVEL_POSTGRES			250
#define	PRIORITY_LEVEL_UDP				260
#define	PRIORITY_LEVEL_TCPSERVER		270
#define	PRIORITY_LEVEL_TCPCLIENT		280
#define	PRIORITY_LEVEL_REDISCLIENT		290

#define	PRIORITY_LEVEL_DBCONN			300
#define	PRIORITY_LEVEL_OBSERVER			310
#define	PRIORITY_LEVEL_TIMER			320
#define	PRIORITY_LEVEL_TSAPICLIENT		330

#define PRIORITY_LEVEL_MESSAGECENTER    340

//-------------- Plugins GateWay --------------
#define PRIORITY_LEVEL_FAST110			90
#define PRIORITY_LEVEL_GPS				100
#define PRIORITY_LEVEL_IFDS				100
#define PRIORITY_LEVEL_LSP				100
#define PRIORITY_LEVEL_MPA				100

#define PRIORITY_LEVEL_MRPS				100
#define PRIORITY_LEVEL_SMP				100
#define PRIORITY_LEVEL_SMSLJ			100
#define PRIORITY_LEVEL_SMT				100
#define PRIORITY_LEVEL_THIRDALARM		100

#define PRIORITY_LEVEL_VCS				100
#define PRIORITY_LEVEL_WEB				100
#define PRIORITY_LEVEL_WEBCHATLJ		100
#define PRIORITY_LEVEL_WEBCHATST		100

#define PRIORITY_LEVEL_IDENTITYINFO     100

//-------------- Plugins Server --------------
	//Alarm
#define PRIORITY_LEVEL_ALARMSTATISTICS	100
#define PRIORITY_LEVEL_EDITLOCK			100
#define PRIORITY_LEVEL_FEEDBACK			100
#define PRIORITY_LEVEL_PROCESS			100
#define PRIORITY_LEVEL_PROCESSTIMEOUT	110
#define PRIORITY_LEVEL_RECIEPT			100
#define PRIORITY_LEVEL_SHIFT			100
#define PRIORITY_LEVEL_SYNTHETICAL		100
#define PRIORITY_LEVEL_TODAYALARM		100
#define PRIORITY_LEVEL_TRANSPORTALARM	100
#define PRIORITY_LEVEL_NOTICE			100

#define BASEDATA_LEVEL_BASE             100

	//BaseData
#define PRIORITY_LEVEL_ADMIN			BASEDATA_LEVEL_BASE + 1
#define PRIORITY_LEVEL_BINDINFO			BASEDATA_LEVEL_BASE + 2
#define PRIORITY_LEVEL_DEPT				BASEDATA_LEVEL_BASE + 3
#define PRIORITY_LEVEL_DICT				BASEDATA_LEVEL_BASE + 4
#define PRIORITY_LEVEL_DISPATCH			BASEDATA_LEVEL_BASE + 5
#define PRIORITY_LEVEL_FUNC				BASEDATA_LEVEL_BASE + 6
#define PRIORITY_LEVEL_KEYDEPT			BASEDATA_LEVEL_BASE + 7
#define PRIORITY_LEVEL_LANGUAGE			BASEDATA_LEVEL_BASE + 8	
#define PRIORITY_LEVEL_LOGIN			BASEDATA_LEVEL_BASE + 20
#define PRIORITY_LEVEL_PHONEINFO		BASEDATA_LEVEL_BASE + 9
#define PRIORITY_LEVEL_PHONEMARK		BASEDATA_LEVEL_BASE + 10
#define PRIORITY_LEVEL_ROLE				BASEDATA_LEVEL_BASE + 11
#define PRIORITY_LEVEL_SEAT				BASEDATA_LEVEL_BASE + 12
#define PRIORITY_LEVEL_STAFF			BASEDATA_LEVEL_BASE + 13
#define PRIORITY_LEVEL_TEXTTEMPLATE		BASEDATA_LEVEL_BASE + 14

	//CTI
#define PRIORITY_LEVEL_CTI				100
#define PRIORITY_LEVEL_DBPROCESS		99
#define PRIORITY_LEVEL_TAPI				100

	//DBAgent
#define PRIORITY_LEVEL_DBAGENT			100
	//Message
#define PRIORITY_LEVEL_MESSAGE			999

	// Record Server
#define PRIORITY_LEVEL_RECORD			120

}