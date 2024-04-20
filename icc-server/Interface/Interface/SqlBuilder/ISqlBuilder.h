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
			SQLBD_EMPTY_RESULT,								//结果为空字符串
			SQLBD_REQ_NOT_JSON,								//json格式错误
			SQLBD_REQ_ID_EMPTY,								//请求id为空
			SQLBD_REQ_ID_ERROR,								//sql 配置文件没有所请求的某个sqlid对应的配置
			SQLBD_CFG_ID_EMPTY,								//sql 配置文件某sql配置项的id属性没有值，即配置错误
			SQLBD_CFG_FILE_EMPTY,							//sql 配置文件是空的
			SQLBD_CFG_ID_REPEATED,							//sql 配置文件中有相同的 id 配置项
			SQLBD_CFG_TEXT_EMPTY,							//sql 配置文件某sql配置项没有value,即sql text
			SQLBD_LOAD_CFG_FAILED,							//sql 配置文件不是正确的xml格式
			SQLBD_CFG_TEXT_SYNTAX_ERROR,					//sql 配置文件某sql配置项text的格式错误，即其text中'{'或'}'不匹配

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
			//根据SQL请求构造sql语句
			virtual unsigned int BuildSql(std::string p_strSqlRequest, std::string& p_strSqlRes) = 0;
			virtual unsigned int BuildSql(ISqlRequestPtr p_pSqlRequest, std::string& p_strSqlRes) = 0;
		};

		typedef boost::shared_ptr<ISqlBuilder> ISqlBuilderPtr;
	}
}