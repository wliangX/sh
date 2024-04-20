#include <RecordFileManage.h>


CRecordFileManage CRecordFileManage::m_instance;

CRecordFileManage& CRecordFileManage::GetInstance()
{
	return m_instance;
}

void CRecordFileManage::AddARecordFile(CRecordFilePtr pRecordFile)
{
	std::lock_guard<mutex> lk(m_lstLock);
	//历史话务放在队列尾，实时话务放在队列头
	if (pRecordFile->m_bIsHistory)
	{
		m_lstNotUploadFile.push_back(pRecordFile);
	}
	else
	{
		m_lstNotUploadFile.push_front(pRecordFile);
	}
}

void CRecordFileManage::SetLocalFilePath(const std::string& strLocalFilePath)
{
	m_strLocalFilePath = strLocalFilePath;
}
std::string CRecordFileManage::GetLocalFilePath()
{
	return m_strLocalFilePath;
}

CRecordFilePtr CRecordFileManage::GetANotUploadFile()
{
	std::lock_guard<mutex> lk(m_lstLock);
	if (m_lstNotUploadFile.empty())
	{
		return NULL;
	}
	CRecordFilePtr pRecordFile = m_lstNotUploadFile.front();
	m_lstNotUploadFile.pop_front();
	return pRecordFile;
}
size_t CRecordFileManage::NotUploadFileListSize()
{
	std::lock_guard<mutex> lk(m_lstLock);
	return m_lstNotUploadFile.size();
}
