#pragma once

#pragma warning(disable:4996)
#pragma warning(disable:4244)

namespace ICC
{

	//告警类型
	enum SMT_AlarmType
	{
		business_alarm,
		database_alarm,
		smt_alarmtype_count
	};

	//告警类型描述
	const char* const g_smt_AlarmType[] =
	{
		"business.alarm",
		"database.alarm"
	};


	//告警级别
	enum SMT_AlarmLevel
	{
		critical,
		major,
		minor,
		warning,
		smt_alarmlevel_count
	};

	//告警级别描述
	const char* const g_smt_AlarmLevel[] =
	{
		"critical",
		"major",
		"minor",
		"warning"		
	};

	//网元类型
	enum SMT_NetworkType
	{
		icc_server_alarm,                   ///业务服务
		icc_server_cti,                     //CTI服务正常
		icc_server_dbagent,					//数据库服务正常
		smt_network_type_count
	};

	//告警编码列表，成对出现（告警代码，告警恢复代码）
	enum SMT_AlarmCode
	{
		icc_business_disconnection_alarm,                    //业务服务断开
		icc_business_connection_alarm,                       //业务服务正常
		icc_cti_disconnection_alarm,                         //CTI服务断开
		icc_cti_connection_alarm,                            //CTI服务正常
		icc_hostmachine_switch_alarm,
		smt_alarmcode_count
	};

	//告警编码列表描述
	const char* const g_smt_AlarmCode[] =
	{
		"icc.business.disconnection.alarm",
		"icc.business.connection.alarm",
		"icc.cti.disconnection.alarm",
		"icc.cti.connection.alarm",
		"icc.hostmachine.switch.alarm"
	};

	//告警原因列表
	const char* const g_smt_AlarmCause[] =
	{
		"icc:business:",
		"icc:cti:"
	};

	//服务端口列表，这里没什么意义，就是为了给smt显示需要，并不是真正的端口
	const char* const g_smt_ServerPort[] =
	{
		"55660",//业务服务
		"55661"//cti服务
	};

	//服务名列表
	const char* const g_smt_ServerName[] =
	{
		"ICC.Server.Alarm",
		"ICC.Server.CTI"
	};
}