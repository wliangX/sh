#pragma once

/*!
 * \file AvayaResult.h
 *	@Copyright Copyright (C), 2012-2013
 * \author PSY
 * \date 一月 2018
 *@brief 交换机事件格式化类
 *	负责将对应的事件，格式化成相应的字符串,供程序log显示和调试使用
 *	该类与Avaya交换机相匹配,与AvayaEvent类配合使用
 *
 */

namespace ICC
{
	class CAvayaResult
	{
	public:
		CAvayaResult(void);
		virtual ~CAvayaResult(void);

		static boost::shared_ptr<CAvayaResult> Instance();
		void ExitInstance();

		void OnInit();

		void SetStringUtilPtr(StringUtil::IStringUtilPtr p_pStringUtil){ m_pStrUtil = p_pStringUtil; }

	private:
		static boost::shared_ptr<CAvayaResult> m_pResultInstance;

		StringUtil::IStringUtilPtr	m_pStrUtil;

	public:
		std::string DescribeError(DWORD p_dwErrorID);
		std::string DescribeCallStateDetail(DWORD p_dwCallState, DWORD p_dwCallStateDetail);
		std::string DescribeCallState(std::string& p_strResult, DWORD p_dwCallState);
		std::string DescribePrivilege(std::string& p_strResult, DWORD p_dwCallPrivilege);
		std::string DescribeDeviceStatus(std::string& p_strResult, DWORD p_dwDeviceState);
		std::string DescribeAddressStatus(std::string& p_strResult, DWORD p_dwAddressState);
		std::string DescribeCallInfo(std::string& p_strResult, DWORD p_dwCallInfoState);
		std::string DescribeCallReason(std::string& p_strResult, DWORD p_dwCallReason);
		std::string DescribeCallOrigin(std::string& p_strResult, DWORD p_dwCallOrigin);
		std::string DescribeCallCompletion(std::string& p_strResult, DWORD p_dwCallCompletion);
		std::string DescribeAddressCapabilities(std::string& p_strResult, DWORD p_dwAddressCaps);
		std::string DescribeCallFeatures(std::string& p_strResult, DWORD p_dwCallFeatures);
		std::string DescribeCallFeatures2(std::string& p_strResult, DWORD p_dwCallFeatures2);
	};

}	// end namespace


