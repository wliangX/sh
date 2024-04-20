#pragma once
#include <map>
#include <PGClient/IResultSet.h>

namespace ICC
{	
	namespace DataBase
	{
		class CResultSetImpl :
			public IResultSet
		{
		public:
			typedef boost::shared_ptr<PGresult> PGResultPtr;

		public:
			CResultSetImpl(PGResultPtr p_pPGResult = nullptr);
			~CResultSetImpl();

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
			//获取第iRow行，iCol列的结果,行列索引皆从0开始
			virtual std::string GetValue(unsigned int p_uiRow, unsigned int p_uiColm, const std::string p_strDefault = "");
			//取当前行字段名为p_strColName的值
			virtual std::string GetValue(const std::string p_strColName, const std::string p_strDefault = "");
			//获取第iRow行(第一行为索引为0)，字段名为FieldName的内容
			virtual std::string GetValue(unsigned int p_uiRow, const std::string p_strColName, const std::string p_strDefault = "default");
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
			PGResultPtr m_pResult;			//Postgresql的libpq库返回的查询结果指针
			std::vector<std::string> m_vecFieldNames;			// 结果集中的列名，以查询结果中的顺序排列
			std::map<std::string, unsigned int>	m_mapFieldIndex; // field_name -- column index
			
		};
	}
}