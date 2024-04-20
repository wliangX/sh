#include <AESClient.h>
#include <iostream>
#include <algorithm>
#include <CommonFunc.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

using boost::property_tree::ptree;
using namespace std;

namespace ICC
{

	CAavayaAESClient::CAavayaAESClient(Log::ILogPtr pLog, const string& ip, const string& port, bool https, const string& uname, const string& pwd): m_pLog(pLog),
		m_host(ip), m_port(port), m_uname(uname), m_pwd(pwd), m_ishttps(https)
	{
		;
	}
	CAavayaAESClient::~CAavayaAESClient()
	{
		;
	}

	void CAavayaAESClient::setAvayaAddress(string ip, string port, bool https) {

		m_host = ip;
		m_port = port;
		m_ishttps = https;
	}

	void CAavayaAESClient::setAuth(string uname, string pwd) {

		m_uname = uname;
		m_pwd = pwd;
	}

	int CAavayaAESClient::getAllStationIP(map<string, string>& info) {

		string page = "/smsxml/SystemManagementService.php";

		string body = R"(<?xml version="1.0" ?>
	<S:Envelope xmlns:S="http://schemas.xmlsoap.org/soap/envelope/">
		<S:Header>
			<sessionID xmlns="http://xml.avaya.com/ws/session"></sessionID>
		</S:Header>
		<S:Body>
			<ns2:submitRequest xmlns:ns2="http://xml.avaya.com/ws/SystemManagementService/2008/07/01" xmlns:ns3="http://xml.avaya.com/ws/session">
				<modelFields xmlns="">
					<RegisteredIPStations></RegisteredIPStations>
				</modelFields>
				<operation xmlns="">list</operation>
			</ns2:submitRequest>
		</S:Body>
	</S:Envelope>)";

		//发送 HTTP 请求头部
		std::string auth = m_uname + ":" + m_pwd;
		std::string encoded_auth = base64_encode(auth);

		std::map<std::string, std::string> mapHeader;
		mapHeader.insert(std::make_pair("Accept-encoding", "gzip, deflate, br"));
		mapHeader.insert(std::make_pair("Connection", "close"));
		mapHeader.insert(std::make_pair("SOAPAction", "http://xml.avaya.com/ws/SystemManagementService/2008/07/01#soap_webservice#submitRequest"));
		mapHeader.insert(std::make_pair("Accept", "text/xml, multipart/related, text/html, image/gif, image/jpeg, *; q=.2, */*; q=.2"));
		mapHeader.insert(std::make_pair("Content-Type", "application/xml"));
		mapHeader.insert(std::make_pair("Host", m_host));
		mapHeader.insert(std::make_pair("Authorization", std::string("Basic ") + encoded_auth));


		string response_str;
		int res_code = -1;
		if (m_ishttps)
		{
			res_code = CHttpsClient::DoPostHttps(m_host, m_port, page, mapHeader, body, response_str);
		}
		else
		{
			res_code = CHttpsClient::DoPostHttp(m_host, m_port, page, mapHeader, body, response_str);
		}
		if (res_code != 200 && res_code != 0) { //出错
			ICC_LOG_ERROR(m_pLog, "sendSoapPost erro: %d erro detail: %s", res_code, response_str.c_str());
			return 1;
		}
		//cout << "getAllStationIP: response code " << res_code << " response string: " << response_str << endl;
		fstream fs;
		fs.open("response.xml", std::ios::app);
		if (fs.is_open())
		{
			fs << response_str;
			fs.close();
		}
		map<string, string> info_tmp;
		string sessionID = parseAllStationIP(response_str, info_tmp);
		if (sessionID.empty())
		{
			ICC_LOG_ERROR(m_pLog, "parse xml no sessionid error: %s", response_str.c_str());
			return 2;
		}
		info = info_tmp;
		releaseSession(sessionID);
		return 0;
	}

	string CAavayaAESClient::parseAllStationIP(const std::string& strResponse, map<string, string>& info)
	{

		const std::string from = "SOAP-ENV:";
		const std::string to = "";
		std::string xml = replace(strResponse, from, to);

		const std::string from1 = "ns1:";
		const std::string to1 = "";
		xml = replace(xml, from1, to1);

		const std::string from2 = "ns2:";
		const std::string to2 = "";
		xml = replace(xml, from2, to2);

		boost::property_tree::ptree pt;

		try {
			std::stringstream ss(xml);
			read_xml(ss, pt, boost::property_tree::xml_parser::trim_whitespace);
		}
		catch (const std::exception & e) {
			ICC_LOG_ERROR(m_pLog, "Error: %s", e.what());
			return "";
		}

		map<string, string> station_info;
		// 读取XML文件中的节点和属性
		std::string sessionID = pt.get<std::string>("Envelope.Header.sessionID");
		std::string result_code = pt.get<std::string>("Envelope.Body.submitRequestResponse.return.result_code");
		boost::property_tree::ptree result_data = pt.get_child("Envelope.Body.submitRequestResponse.return.result_data");
		boost::property_tree::ptree::iterator it;
		for (it = result_data.begin(); it != result_data.end(); ++it) {
			ICC_LOG_INFO(m_pLog, "%s:%s-->%s", it->first.c_str(), it->second.get<std::string>("Station_Extension").c_str(),
				it->second.get<std::string>("Station_IP_Address").c_str());
			string station_num = it->second.get<std::string>("Station_Extension");
			string station_ip = it->second.get<std::string>("Station_IP_Address");
			station_info.insert(pair<string, string>(station_num, station_ip));
		}
		info = station_info;
		return sessionID;
	}

	int CAavayaAESClient::parseErrorResult(const string& str, string& faultstr)
	{
		const std::string from = "SOAP-ENV:";
		const std::string to = "";
		std::string xml = replace(str, from, to);


		boost::property_tree::ptree pt;

		try {
			std::stringstream ss(xml);
			read_xml(ss, pt, boost::property_tree::xml_parser::trim_whitespace);
		}
		catch (const std::exception & e) {
			ICC_LOG_ERROR(m_pLog, "Error: %s", e.what());
			return -1;
		}

		// 读取XML文件中的节点和属性
		std::string faultcode = pt.get<std::string>("Envelope.Body.Fault.faultcode");
		std::string faultstring = pt.get<std::string>("Envelope.Body.Fault.faultstring");

		faultstr = faultstring;
		return 0;
	}

	int CAavayaAESClient::releaseSession(const string& sessionID)
	{
		string page = "/smsxml/SystemManagementService.php";

		string body_front = R"(<?xml version="1.0" ?>
					<S:Envelope xmlns:S="http://schemas.xmlsoap.org/soap/envelope/">
					<S:Header>
						<sessionID xmlns="http://xml.avaya.com/ws/session">)";
		string body_rear = R"(</sessionID>
					</S:Header>
					<S:Body>
						<ns2:release xmlns:ns2="http://xml.avaya.com/ws/SystemManagementService/2008/07/01" xmlns:ns3="http://xml.avaya.com/ws/session"></ns2:release>
					</S:Body>
					</S:Envelope>)";

		string body = body_front + sessionID + body_rear;

		//可选：发送 HTTP 请求头部
		std::string auth = m_uname + ":" + m_pwd;
		std::string encoded_auth = base64_encode(auth);

		std::map<std::string, std::string> mapHeader;
		mapHeader.insert(std::make_pair("Accept-encoding", "gzip, deflate, br"));
		mapHeader.insert(std::make_pair("Connection", "close"));
		mapHeader.insert(std::make_pair("SOAPAction", "http://xml.avaya.com/ws/SystemManagementService/2008/07/01#soap_webservice#release"));
		mapHeader.insert(std::make_pair("Accept", "text/xml, multipart/related, text/html, image/gif, image/jpeg, *; q=.2, */*; q=.2"));
		mapHeader.insert(std::make_pair("Content-Type", "application/xml"));
		mapHeader.insert(std::make_pair("Host", m_host));
		mapHeader.insert(std::make_pair("Authorization", std::string("Basic ") + encoded_auth));

		std::string action = "release";
		int htp_ret = -1;
		string response;

		if (m_ishttps)
		{
			htp_ret = CHttpsClient::DoPostHttps(m_host, m_port, page, mapHeader, body, response);
		}
		else
		{
			htp_ret = CHttpsClient::DoPostHttp(m_host, m_port, page, mapHeader, body, response);
		}
		if (htp_ret != 200 && htp_ret != 0) { //出错

		}
		return htp_ret;
	}
}
