#pragma once

#include <string>  
#include <map>  

class HttpClient
{
public:
	HttpClient(void);
	~HttpClient(void);

public:

	/**
	* @brief HTTP POST请求
	* @param strUrl 输入参数,请求的Url地址,如:http://www.baidu.com
	* @param mapPost 输入参数,key value 格式
	* @param strResponse 输出参数,返回的内容
	* @return 返回是否Post成功
	*/
	int Post(const std::string & strUrl, const std::map<std::string, std::string> & mapPost, std::string & strResponse);

	/**
	* @brief HTTP POST请求
	* @param strUrl 输入参数,请求的Url地址,如:http://www.baidu.com
	* @param strPost 输入参数,使用如下格式para1=val1¶2=val2&…
	* @param strResponse 输出参数,返回的内容
	* @return 返回是否Post成功
	*/
	int Post(const std::string & strUrl, const std::string & strPost, std::string & strResponse);

	/**
	* @brief HTTP GET请求
	* @param strUrl 输入参数,请求的Url地址,如:http://www.baidu.com
	* @param strResponse 输出参数,返回的内容
	* @return 返回是否Post成功
	*/
	int Get(const std::string & strUrl, std::string & strResponse);

	/**
	* @brief HTTPS POST请求,无证书版本
	* @param strUrl 输入参数,请求的Url地址,如:https://www.alipay.com
	* @param strPost 输入参数,使用如下格式para1=val1¶2=val2&…
	* @param strResponse 输出参数,返回的内容
	* @param pCaPath 输入参数,为CA证书的路径.如果输入为NULL,则不验证服务器端证书的有效性.
	* @return 返回是否Post成功
	*/
	int Posts(const std::string & strUrl, const std::string & strPost, std::string & strResponse, const char * pCaPath = NULL);

	/**
	* @brief HTTPS GET请求,无证书版本
	* @param strUrl 输入参数,请求的Url地址,如:https://www.alipay.com
	* @param strResponse 输出参数,返回的内容
	* @param pCaPath 输入参数,为CA证书的路径.如果输入为NULL,则不验证服务器端证书的有效性.
	* @return 返回是否Post成功
	*/
	int Gets(const std::string & strUrl, std::string & strResponse, const char * pCaPath = NULL);

public:
	static std::string FACE_KEY;
	static std::string FACE_SECRET;

	static std::string URL_DETECT;
	static std::string URL_COMPARE;
	static std::string URL_RECOGNIZE;
	static std::string URL_SEARCH;
	static std::string URL_TRAIN;
	static std::string URL_VERIFY;

	static std::string URL_PERSON_ADDFACE;
	static std::string URL_PERSON_CREATE;
	static std::string URL_PERSON_DELETE;
	static std::string URL_PERSON_GETINFO;
	static std::string URL_PERSON_REMOVEFACE;
	static std::string URL_PERSON_SETINFO;

	static std::string URL_GROUP_ADDPERSON;
	static std::string URL_GROUP_CREATE;
	static std::string URL_GROUP_DELETE;
	static std::string URL_GROUP_GETINFO;
	static std::string URL_GROUP_REMOVEPERSON;
	static std::string URL_GROUP_SETINFO;

	static std::string URL_INFO_GETAPP;
	static std::string URL_INFO_GETFACE;
	static std::string URL_INFO_GETGROUPLIST;
	static std::string URL_INFO_GETIMAGE;
	static std::string URL_INFO_GETPERSONLIST;
	static std::string URL_INFO_GETQUOTA;
	static std::string URL_INFO_GETSESSION;
	static std::string URL_INFO_GET_FACESETLIST;

	static std::string URL_FACESET_CREATE;
	static std::string URL_FACESET_DELETE;
	static std::string URL_FACESET_ADDFACE;
	static std::string URL_FACESET_REMOVEFACE;
	static std::string URL_FACESET_SETINFO;
	static std::string URL_FACESET_GET_INFO;

	static std::string URL_TRAIN_VERIFY;
	static std::string URL_TRAIN_SEARCH;
	static std::string URL_TRAIN_IDENTIFY;
	static std::string URL_GROUPING_GROUPING;
};
