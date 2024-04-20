#pragma once

#pragma warning(disable:4996)
#pragma warning(disable:4244)

namespace ICC
{

	//�澯����
	enum SMT_AlarmType
	{
		business_alarm,
		database_alarm,
		smt_alarmtype_count
	};

	//�澯��������
	const char* const g_smt_AlarmType[] =
	{
		"business.alarm",
		"database.alarm"
	};


	//�澯����
	enum SMT_AlarmLevel
	{
		critical,
		major,
		minor,
		warning,
		smt_alarmlevel_count
	};

	//�澯��������
	const char* const g_smt_AlarmLevel[] =
	{
		"critical",
		"major",
		"minor",
		"warning"		
	};

	//��Ԫ����
	enum SMT_NetworkType
	{
		icc_server_alarm,                   ///ҵ�����
		icc_server_cti,                     //CTI��������
		icc_server_dbagent,					//���ݿ��������
		smt_network_type_count
	};

	//�澯�����б��ɶԳ��֣��澯���룬�澯�ָ����룩
	enum SMT_AlarmCode
	{
		icc_business_disconnection_alarm,                    //ҵ�����Ͽ�
		icc_business_connection_alarm,                       //ҵ���������
		icc_cti_disconnection_alarm,                         //CTI����Ͽ�
		icc_cti_connection_alarm,                            //CTI��������
		icc_hostmachine_switch_alarm,
		smt_alarmcode_count
	};

	//�澯�����б�����
	const char* const g_smt_AlarmCode[] =
	{
		"icc.business.disconnection.alarm",
		"icc.business.connection.alarm",
		"icc.cti.disconnection.alarm",
		"icc.cti.connection.alarm",
		"icc.hostmachine.switch.alarm"
	};

	//�澯ԭ���б�
	const char* const g_smt_AlarmCause[] =
	{
		"icc:business:",
		"icc:cti:"
	};

	//����˿��б�����ûʲô���壬����Ϊ�˸�smt��ʾ��Ҫ�������������Ķ˿�
	const char* const g_smt_ServerPort[] =
	{
		"55660",//ҵ�����
		"55661"//cti����
	};

	//�������б�
	const char* const g_smt_ServerName[] =
	{
		"ICC.Server.Alarm",
		"ICC.Server.CTI"
	};
}