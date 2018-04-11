#include "httpclient.h"  
#include "curl/curl.h"  
#include <string>  

std::string HttpClient::FACE_KEY = "自己key";
std::string HttpClient::FACE_SECRET = "自己secret";
std::string HttpClient::URL_DETECT = "http://apicn.faceplusplus.com/v2/detection/detect";
std::string HttpClient::URL_COMPARE = "http://apicn.faceplusplus.com/v2/recognition/compare";
std::string HttpClient::URL_RECOGNIZE = "http://apicn.faceplusplus.com/v2/recognition/recognize";
std::string HttpClient::URL_SEARCH = "http://apicn.faceplusplus.com/v2/recognition/search";
std::string HttpClient::URL_TRAIN = "http://apicn.faceplusplus.com/v2/recognition/train";
std::string HttpClient::URL_VERIFY = "http://apicn.faceplusplus.com/v2/recognition/verify";

std::string HttpClient::URL_PERSON_ADDFACE = "http://apicn.faceplusplus.com/v2/person/add_face";
std::string HttpClient::URL_PERSON_CREATE = "http://apicn.faceplusplus.com/v2/person/create";
std::string HttpClient::URL_PERSON_DELETE = "http://apicn.faceplusplus.com/v2/person/delete";
std::string HttpClient::URL_PERSON_GETINFO = "http://apicn.faceplusplus.com/v2/person/get_info";
std::string HttpClient::URL_PERSON_REMOVEFACE = "http://apicn.faceplusplus.com/v2/person/remove_face";
std::string HttpClient::URL_PERSON_SETINFO = "http://apicn.faceplusplus.com/v2/person/set_info";

std::string HttpClient::URL_GROUP_ADDPERSON = "http://apicn.faceplusplus.com/v2/group/add_person";
std::string HttpClient::URL_GROUP_CREATE = "http://apicn.faceplusplus.com/v2/group/create";
std::string HttpClient::URL_GROUP_DELETE = "http://apicn.faceplusplus.com/v2/group/delete";
std::string HttpClient::URL_GROUP_GETINFO = "http://apicn.faceplusplus.com/v2/group/get_info";
std::string HttpClient::URL_GROUP_REMOVEPERSON = "http://apicn.faceplusplus.com/v2/group/remove_person";
std::string HttpClient::URL_GROUP_SETINFO = "http://apicn.faceplusplus.com/v2/group/set_info";

std::string HttpClient::URL_INFO_GETAPP = "http://apicn.faceplusplus.com/v2/info/get_app";
std::string HttpClient::URL_INFO_GETFACE = "http://apicn.faceplusplus.com/v2/info/get_face";
std::string HttpClient::URL_INFO_GETGROUPLIST = "http://apicn.faceplusplus.com/v2/info/get_group_list";
std::string HttpClient::URL_INFO_GETIMAGE = "http://apicn.faceplusplus.com/v2/info/get_image";
std::string HttpClient::URL_INFO_GETPERSONLIST = "http://apicn.faceplusplus.com/v2/info/get_person_list";
std::string HttpClient::URL_INFO_GETQUOTA = "http://apicn.faceplusplus.com/v2/info/get_quota";
std::string HttpClient::URL_INFO_GETSESSION = "http://apicn.faceplusplus.com/v2/info/get_session";
std::string HttpClient::URL_INFO_GET_FACESETLIST = "http://apicn.faceplusplus.com/v2/info/get_faceset_list";

std::string HttpClient::URL_FACESET_CREATE = "http://apicn.faceplusplus.com/v2/faceset/create";
std::string HttpClient::URL_FACESET_DELETE = "http://apicn.faceplusplus.com/v2/faceset/delete";
std::string HttpClient::URL_FACESET_ADDFACE = "http://apicn.faceplusplus.com/v2/faceset/add_face";
std::string HttpClient::URL_FACESET_REMOVEFACE = "http://apicn.faceplusplus.com/v2/faceset/remove_face";
std::string HttpClient::URL_FACESET_SETINFO = "http://apicn.faceplusplus.com/v2/faceset/set_info";
std::string HttpClient::URL_FACESET_GET_INFO = "http://apicn.faceplusplus.com/v2/faceset/get_info";

std::string HttpClient::URL_TRAIN_VERIFY = "http://apicn.faceplusplus.com/v2/train/verify";
std::string HttpClient::URL_TRAIN_SEARCH = "http://apicn.faceplusplus.com/v2/train/search";
std::string HttpClient::URL_TRAIN_IDENTIFY = "http://apicn.faceplusplus.com/v2/train/identify";
std::string HttpClient::URL_GROUPING_GROUPING = "http://apicn.faceplusplus.com/v2/grouping/grouping";

HttpClient::HttpClient(void)
{

}

HttpClient::~HttpClient(void)
{

}

static size_t OnWriteData(void* buffer, size_t size, size_t nmemb, void* lpVoid)
{
	std::string* str = dynamic_cast<std::string*>((std::string *)lpVoid);
	if (NULL == str || NULL == buffer)
	{
		return -1;
	}

	char* pData = (char*)buffer;
	str->append(pData, size * nmemb);
	return nmemb;
}

int HttpClient::Post(const std::string & strUrl, const std::map<std::string, std::string> & mapPost, std::string & strResponse)
{
	std::string strPost = "api_secret=" + FACE_SECRET + "&api_key=" + FACE_KEY;
	std::map<std::string, std::string>::const_iterator it = mapPost.begin();
	while (it != mapPost.end())
	{
		strPost += "&";
		strPost += it->first;
		strPost += "=";
		strPost += it->second;
		it++;
	}
	return Post(strUrl, strPost, strResponse);
}

int HttpClient::Post(const std::string & strUrl, const std::string & strPost, std::string & strResponse)
{
	CURLcode res;
	CURL* curl = curl_easy_init();
	if (NULL == curl)
	{
		return CURLE_FAILED_INIT;
	}
	curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str());
	curl_easy_setopt(curl, CURLOPT_POST, 1);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, strPost.c_str());
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteData);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&strResponse);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 3);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3);
	res = curl_easy_perform(curl);
	curl_easy_cleanup(curl);
	return res;
}

int HttpClient::Get(const std::string & strUrl, std::string & strResponse)
{
	CURLcode res;
	CURL* curl = curl_easy_init();
	if (NULL == curl)
	{
		return CURLE_FAILED_INIT;
	}
	curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str());
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteData);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&strResponse);
	/**
	* 当多个线程都使用超时处理的时候，同时主线程中有sleep或是wait等操作。
	* 如果不设置这个选项，libcurl将会发信号打断这个wait从而导致程序退出。
	*/
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 3);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3);
	res = curl_easy_perform(curl);
	curl_easy_cleanup(curl);
	return res;
}

int HttpClient::Posts(const std::string & strUrl, const std::string & strPost, std::string & strResponse, const char * pCaPath)
{
	CURLcode res;
	CURL* curl = curl_easy_init();
	if (NULL == curl)
	{
		return CURLE_FAILED_INIT;
	}
	curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str());
	curl_easy_setopt(curl, CURLOPT_POST, 1);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, strPost.c_str());
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteData);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&strResponse);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	if (NULL == pCaPath)
	{
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);
	}
	else
	{
		//缺省情况就是PEM，所以无需设置，另外支持DER  
		//curl_easy_setopt(curl,CURLOPT_SSLCERTTYPE,"PEM");  
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, true);
		curl_easy_setopt(curl, CURLOPT_CAINFO, pCaPath);
	}
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 3);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3);
	res = curl_easy_perform(curl);
	curl_easy_cleanup(curl);
	return res;
}

int HttpClient::Gets(const std::string & strUrl, std::string & strResponse, const char * pCaPath)
{
	CURLcode res;
	CURL* curl = curl_easy_init();
	if (NULL == curl)
	{
		return CURLE_FAILED_INIT;
	}
	curl_easy_setopt(curl, CURLOPT_URL, strUrl.c_str());
	curl_easy_setopt(curl, CURLOPT_READFUNCTION, NULL);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, OnWriteData);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&strResponse);
	curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);
	if (NULL == pCaPath)
	{
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, false);
	}
	else
	{
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, true);
		curl_easy_setopt(curl, CURLOPT_CAINFO, pCaPath);
	}
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 3);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, 3);
	res = curl_easy_perform(curl);
	curl_easy_cleanup(curl);
	return res;
}
