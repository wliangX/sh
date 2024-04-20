#include <CommonFunc.h>
#include <boost/filesystem/operations.hpp>
#include <boost/asio.hpp>
#include <boost/asio/ssl.hpp>
#include <ctime>

using namespace boost::asio;
using boost::asio::ip::tcp;

const std::string base64_chars =
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz"
"0123456789+/";

std::string base64_encode(const std::string& input) {
    std::string encoded_string;
    int i = 0, j = 0;
    unsigned char char_array_3[3], char_array_4[4];
    for (const auto& c : input) {
        char_array_3[i++] = c;
        if (i == 3) {
            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;

            for (i = 0; i < 4; i++)
                encoded_string += base64_chars[char_array_4[i]];

            i = 0;
        }
    }

    if (i) {
        for (j = i; j < 3; j++)
            char_array_3[j] = '\0';

        char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
        char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
        char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
        char_array_4[3] = char_array_3[2] & 0x3f;

        for (j = 0; j < i + 1; j++)
            encoded_string += base64_chars[char_array_4[j]];

        while ((i++ < 3))
            encoded_string += '=';
    }

    return encoded_string;
}

std::string replace(std::string str, const std::string& from, const std::string& to) {
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
    return str;
}

bool IsFileExits(const std::string& strFile)
{
    boost::filesystem::path p(strFile);
    return boost::filesystem::exists(p);
}

void CreateFolder(std::string& msgPath)
{
    if (msgPath.empty())
    {
        return;
    }
    int iLen = msgPath.length();
    std::string pszDir = msgPath;
    //在末尾加/
    if (pszDir[iLen - 1] != '\\' && pszDir[iLen - 1] != '/')
    {
        pszDir += "/";
    }
    for (int i = 0; i <= iLen; i++)  //如果没有目录循环创建多级目录
    {
        if (pszDir[i] == '\\' || pszDir[i] == '/')
        {
            std::string strCreateFolder = pszDir.substr(0, i);
            //调过空目录和根目录
            if (strCreateFolder.empty() || strCreateFolder == "/" || strCreateFolder == "\\")
            {
                continue;
            }

            //如果不存在,创建
            boost::filesystem::path p(strCreateFolder);
            if (!boost::filesystem::exists(p))
            {
                bool bRet = boost::filesystem::create_directory(strCreateFolder.c_str());
                if (!bRet)
                {
                    break;
                }
            }
        }
    }
}

//删除nDay天前的文件
int RemoveFile(const std::string& filename, int nDay)
{
    boost::filesystem::path p(filename);
    if (!boost::filesystem::exists(p))
    {
        return -1;
    }
    time_t t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    if ((t - boost::filesystem::last_write_time(p)) > nDay * 86400)//nDay天的秒数
    {
        boost::filesystem::remove(p);
    }
    return 0;
}

//遍历子目录和目录中的文件, 删除大于nDay的文件
int DelFileRecurse(const std::string& dirname, int nDay)
{

    boost::filesystem::path p(dirname);
    if (!boost::filesystem::exists(p))
    {
        return -1;
    }
    boost::filesystem::directory_iterator end_itr;
    for (boost::filesystem::directory_iterator itr(p); itr != end_itr;++itr)
    {
        if (boost::filesystem::is_directory(itr->status()))
        {
            if (!itr->path().filename_is_dot() || !itr->path().filename_is_dot_dot())
            {
                DelFileRecurse(itr->path().string(), nDay);
            }
        }
        else if(boost::filesystem::is_regular(itr->status()) ||boost::filesystem::is_symlink(itr->status()))
        {
            RemoveFile(itr->path().string(), nDay);
        }
    }
    return 0;
  
}

int String2Int(const std::string& strNumber)
{
	try
	{
		return std::stoi(strNumber);
	}
	catch (...)
	{

	}
	return 0;
}

bool str2list(std::string str, std::string sep, std::vector<std::string>& strList)
{
	int pos;
	std::string subStr("");
	int iSepLen = sep.length();
	if (str.length() == 0)
	{
		return false;
	}
	if (iSepLen == 0)
	{
		strList.push_back(str);
		return true;
	}

	pos = str.find(sep);
	while (pos >= 0)
	{
		if (pos == 0)
		{
			subStr = "";
			strList.push_back(subStr);
		}
		else
		{
			subStr = str.substr(0, pos);
			strList.push_back(subStr);
		}
		str = str.substr(pos + iSepLen, str.length() - pos + iSepLen - 1);
		pos = str.find(sep);
	}
	if (str.length() > 0)
		strList.push_back(str);
	return true;
}

std::string GetFileName(const std::string& strFilePath)
{
	std::string::size_type nFileNamePos=strFilePath.find_last_of("/");
	if (nFileNamePos != std::string::npos)
	{
		return strFilePath.substr(nFileNamePos + 1);
	}
	else
	{
		nFileNamePos = strFilePath.find_last_of("\\");
		if (nFileNamePos != std::string::npos)
		{
			return strFilePath.substr(nFileNamePos + 1);
		}
	}
	return strFilePath;
}


int CHttpsClient::DoPostHttps(const string& host, const string& port, const string& page, std::map<std::string, std::string>& mapHeaders, const std::string& strContent, string& reponse_data)
{
	try
	{
		typedef ssl::stream<ip::tcp::socket> ssl_socket;
		ssl::context ctx(ssl::context::tlsv12_client);
		ctx.set_options(ssl::context::default_workarounds
			| ssl::context::sslv2
			| ssl::context::sslv3
			| ssl::context::tlsv12_client);
		// ctx.set_default_verify_paths();

		io_service service;
		ssl_socket sock(service, ctx);
		ip::tcp::resolver resolver(service);
		ip::tcp::resolver::query query(host, port);
		connect(sock.lowest_layer(), resolver.resolve(query));
		// SSL 证书校验设置
		sock.set_verify_mode(ssl::verify_none);
		sock.set_verify_callback(ssl::rfc2818_verification(host));
		sock.handshake(ssl_socket::client);

		// Form the request. We specify the "Connection: close" header so that the
		// server will close the socket after transmitting the response. This will
		// allow us to treat all data up until the EOF as the content.
		boost::asio::streambuf request;
		std::ostream request_stream(&request);
		request_stream << "POST " << page << " HTTP/1.1\r\n";
		for (std::map<std::string, std::string>::iterator iter = mapHeaders.begin(); iter != mapHeaders.end(); iter++)
		{
			request_stream << iter->first << ": " << iter->second << "\r\n";
		}
		request_stream << "Content-Length: " << strContent.length() << "\r\n\r\n";
		request_stream << strContent;

		// Send the request.
		write(sock, request);

		// Read the response headers end line.
		boost::asio::streambuf response;
		read_until(sock, response, "\r\n\r\n");

		// Check that response is OK.
		std::istream response_stream(&response);
		std::string http_version;
		response_stream >> http_version;
		unsigned int status_code;
		response_stream >> status_code;
		std::string status_message;
		std::getline(response_stream, status_message);
		// Check that response is OK.
		if (!response_stream || http_version.substr(0, 5) != "HTTP/")
		{
			reponse_data = "Invalid response";
			return -2;//非正常的HTTP报文
		}
		std::string header;
		while (std::getline(response_stream, header) && header != "\r")
		{
			//std::cout << header << "\n";
		}

		// 读取所有剩下的数据作为包体
		boost::system::error_code error;
		while (read(sock, response,
			transfer_at_least(1), error))
		{
		}

		//响应有数据
		if (response.size())
		{
			std::istream response_stream(&response);
			std::istreambuf_iterator<char> eos;
			reponse_data = string(std::istreambuf_iterator<char>(response_stream), eos);
		}

		//if (error != boost::asio::error::eof)
		//{
		//	reponse_data = error.message();
		//	return -3;//错误的文件结尾
		//}

		return status_code; //返回HTTP状态码

	}
	catch (std::exception & e)
	{
		reponse_data = e.what();
		return -4;//网络异常
	}
}

int CHttpsClient::DoPostHttp(const string& host, const string& port, const string& page, std::map<std::string, std::string>& mapHeaders, const std::string& strContent, string& reponse_data)
{
	try
	{
		boost::asio::io_service io_service;
		//如果io_service存在复用的情况
		if (io_service.stopped())
			io_service.reset();

		// 从dns取得域名下的所有ip
		tcp::resolver resolver(io_service);
		tcp::resolver::query query(host, port);
		tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);

		// 尝试连接到其中的某个ip直到成功 
		tcp::socket socket(io_service);
		boost::asio::connect(socket, endpoint_iterator);

		// Form the request. We specify the "Connection: close" header so that the
		// server will close the socket after transmitting the response. This will
		// allow us to treat all data up until the EOF as the content.
		boost::asio::streambuf request;
		std::ostream request_stream(&request);
		for (std::map<std::string, std::string>::iterator iter = mapHeaders.begin(); iter != mapHeaders.end(); iter++)
		{
			request_stream << iter->first << ": " << iter->second << "\r\n";
		}
		request_stream << "Content-Length: " << strContent.length() << "\r\n\r\n";
		request_stream << strContent;

		// Send the request.
		boost::asio::write(socket, request);

		// Read the response status line.
		boost::asio::streambuf response;
		boost::asio::read_until(socket, response, "\r\n\r\n");

		// Check that response is OK.
		std::istream response_stream(&response);
		std::string http_version;
		response_stream >> http_version;
		unsigned int status_code;
		response_stream >> status_code;
		std::string status_message;
		std::getline(response_stream, status_message);
		if (!response_stream || http_version.substr(0, 5) != "HTTP/")
		{
			reponse_data = "Invalid response";
			return -2;
		}
		std::string header;
		while (std::getline(response_stream, header) && header != "\r")
		{
			//std::cout << header << "\n";
		}

		// 读取所有剩下的数据作为包体
		boost::system::error_code error;
		while (boost::asio::read(socket, response,
			boost::asio::transfer_at_least(1), error))
		{
		}

		//响应有数据
		if (response.size())
		{
			std::istream response_stream(&response);
			std::istreambuf_iterator<char> eos;
			reponse_data = string(std::istreambuf_iterator<char>(response_stream), eos);
		}

		if (error != boost::asio::error::eof)
		{
			reponse_data = error.message();
			return -3;
		}

		return status_code; //>0返回HTTP状态码
	}
	catch (std::exception & e)
	{
		reponse_data = e.what();
		return -4;
	}
}
