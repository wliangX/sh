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

		//������Ƿ���Ч����Чtrue,��Чfalse
		virtual bool IsValid();
		//�޿��ý��false,�п��ý��true
		virtual bool Next();
		//���ؽ�����е�����
		virtual unsigned int FieldSize();
		//���ؽ�����еļ�¼����������
		virtual unsigned int RecordSize();
		//��ȡ��ǰ�е�����ֵ
		virtual unsigned int GetCurRow();
		//���õ�ǰ���������
		virtual bool SetCurRow(unsigned int p_uiCurRow);
		//��ȡ���е�����
		virtual std::vector<std::string> GetFieldNames();	
		//ȡ��ǰ���ֶ���Ϊp_strColName��ֵ
		virtual std::string GetValue(const std::string p_strColName, const std::string p_strDefault = "");		
		//���ô���������Ϣ
		virtual void SetErrorMsg(std::string p_strErrorMsg);
		//���ش���������Ϣ
		virtual std::string GetErrorMsg();			
		//�����ڲ�ִ�е�SQL
		virtual std::string GetSQL();

	public:			
		void SetSQL(std::string p_strSQL);	//�����ڲ�ִ�е�SQL
		void SetResultStatus(bool p_bIsValid);
		void SetRecordSize(unsigned int p_uiSize);			
		void AddFieldIndex(std::string p_strFieldName, unsigned int p_uiFieldIndex);
		

	private:
		bool m_bValid;					//��ѯ�ɹ���ʧ�ܣ��ɹ�true,ʧ��false
		int	 m_uiCurRow;				//������ĵ�ǰ��
		unsigned int m_uiRecordSize;	//������еļ�¼����������
		std::string m_strErrorMsg;		//������Ϣ
		std::string m_strSQL;			//ִ�е�SQL
		
		std::vector<std::string> m_vecFieldNames;			// ������е��������Բ�ѯ����е�˳������
		std::map<std::string, unsigned int>	m_mapFieldIndex; // field_name -- column index
		std::map<std::string, int> m_mapDataType;

		oracle::occi::StatelessConnectionPool* m_pPool;
		oracle::occi::Connection*              m_pConnection;
		oracle::occi::Statement*               m_pStmt;
		oracle::occi::ResultSet*               m_pResultSet;			
	};
	
	typedef std::shared_ptr<CResultSet> ResultSetPtr;
}