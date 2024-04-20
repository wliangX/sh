#pragma once
#include <string>
#include <IObject.h>
#include <SqlBuilder/ISqlRequest.h>
namespace ICC
{
	namespace DataBase
	{
		enum SQLBD
		{
			SQLBD_SUCCESS = 10,
			SQLBD_EMPTY_RESULT,								//���Ϊ���ַ���
			SQLBD_REQ_NOT_JSON,								//json��ʽ����
			SQLBD_REQ_ID_EMPTY,								//����idΪ��
			SQLBD_REQ_ID_ERROR,								//sql �����ļ�û���������ĳ��sqlid��Ӧ������
			SQLBD_CFG_ID_EMPTY,								//sql �����ļ�ĳsql�������id����û��ֵ�������ô���
			SQLBD_CFG_FILE_EMPTY,							//sql �����ļ��ǿյ�
			SQLBD_CFG_ID_REPEATED,							//sql �����ļ�������ͬ�� id ������
			SQLBD_CFG_TEXT_EMPTY,							//sql �����ļ�ĳsql������û��value,��sql text
			SQLBD_LOAD_CFG_FAILED,							//sql �����ļ�������ȷ��xml��ʽ
			SQLBD_CFG_TEXT_SYNTAX_ERROR,					//sql �����ļ�ĳsql������text�ĸ�ʽ���󣬼���text��'{'��'}'��ƥ��

		};

		/*
		* class   SqlBuilder
		* author  t26150
		* purpose
		* note
		*/
		class ISqlBuilder : public IObject
		{
		public:
			virtual std::string GetErrorMsg(unsigned int p_uiErrorCode) = 0;
			//����SQL������sql���
			virtual unsigned int BuildSql(std::string p_strSqlRequest, std::string& p_strSqlRes) = 0;
			virtual unsigned int BuildSql(ISqlRequestPtr p_pSqlRequest, std::string& p_strSqlRes) = 0;
		};

		typedef boost::shared_ptr<ISqlBuilder> ISqlBuilderPtr;
	}
}