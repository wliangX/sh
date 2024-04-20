#pragma once
#include <map>
#include <memory>
#include "occi.h"

namespace ICC
{	
	class CResultSet 			
	{
	public:
		CResultSet();
		~CResultSet();
		
		void SetConnection(oracle::occi::StatelessConnectionPool* pPool, oracle::occi::Connection* pConn);
		void SetStatement(oracle::occi::Statement* pStmt);
		void SetResultSet(oracle::occi::ResultSet* pSet);	
		void SetValid(bool bFlag);

		//结果集是否有效，有效true,无效false
		virtual bool IsValid();
		//无可用结果false,有可用结果true
		virtual bool Next();
		//返回结果集中的列数
		virtual unsigned int FieldSize();
		//返回结果集中的记录数（行数）
		virtual unsigned int RecordSize();
		//获取当前行的索引值
		virtual unsigned int GetCurRow();
		//设置当前处理的行数
		virtual bool SetCurRow(unsigned int p_uiCurRow);
		//获取所有的列名
		virtual std::vector<std::string> GetFieldNames();	
		//取当前行字段名为p_strColName的值
		virtual std::string GetValue(const std::string p_strColName, const std::string p_strDefault = "");		
		//设置错误描述信息
		virtual void SetErrorMsg(std::string p_strErrorMsg);
		//返回错误描述信息
		virtual std::string GetErrorMsg();			
		//返回内部执行的SQL
		virtual std::string GetSQL();

	public:			
		void SetSQL(std::string p_strSQL);	//设置内部执行的SQL
		void SetResultStatus(bool p_bIsValid);
		void SetRecordSize(unsigned int p_uiSize);			
		void AddFieldIndex(std::string p_strFieldName, unsigned int p_uiFieldIndex);
		

	private:
		bool m_bValid;					//查询成功或失败，成功true,失败false
		int	 m_uiCurRow;				//结果集的当前行
		unsigned int m_uiRecordSize;	//结果集中的记录数（行数）
		std::string m_strErrorMsg;		//错误消息
		std::string m_strSQL;			//执行的SQL
		
		std::vector<std::string> m_vecFieldNames;			// 结果集中的列名，以查询结果中的顺序排列
		std::map<std::string, unsigned int>	m_mapFieldIndex; // field_name -- column index
		std::map<std::string, int> m_mapDataType;

		oracle::occi::StatelessConnectionPool* m_pPool;
		oracle::occi::Connection*              m_pConnection;
		oracle::occi::Statement*               m_pStmt;
		oracle::occi::ResultSet*               m_pResultSet;			
	};
	
	typedef std::shared_ptr<CResultSet> ResultSetPtr;
}