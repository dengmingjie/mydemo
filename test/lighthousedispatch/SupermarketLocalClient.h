#ifndef SupermarketLocalClient_h__
#define SupermarketLocalClient_h__

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <thread>
#include "lm.Position.pb.h"
//#include <google/protobuf/message.h>
//#include <google/protobuf/descriptor.h>
//#include <google/protobuf/io/zero_copy_stream_impl.h>
//#include <google/protobuf/io/coded_stream.h>
//#include <google/protobuf/io/zero_copy_stream_impl_lite.h>

//#include "../StepVR_Thread.h"
#pragma comment(lib, "Ws2_32.lib")
//using namespace google::protobuf::io;
#include <WinSock2.h>

std::string GetMD5(const char* strfileName);

class CClient
{
private:
	std::string ServerIP;
	int ServerPort;
	WSADATA wsa;
	SOCKET s;
	char m_sendBuf[102400];
	char m_recvBuf[102400];
	int siz = 0;
	std::thread m_thSend; 
public:
	CClient(const std::string& IP, int port);
	~CClient();
	long ReadHdr(const char *buf);
	void ReadBody(long siz, std::string &val);
	bool SendMsg(const std::string& Msg, int id);
	void SendHandler();
	int init();
	bool ReceMsg(std::string& msg);
};

int ReceiveBroadcast(std::string &ServerIP);

int DispatchClient();

#endif // SupermarketLocalClient_h__
