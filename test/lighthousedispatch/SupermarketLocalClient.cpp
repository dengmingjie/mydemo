#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#define _WINSOCKAPI_
#include <windows.h>
#include <atlstr.h>
#include <io.h>
#include "SupermarketLocalClient.h"
#include "MD5.h"

//#include "../StepVR_Log.h"
//
//#include "../Common.h"
//#include "../StepVR_ParseData.h"

//using namespace google::protobuf::io;
typedef unsigned char uint8;

HANDLE g_Socket_sem;
std::string g_strOldMD5 = "";

bool g_IS_MD5Changed;

//extern NODE_VER_TABLE node_ver_table[MAX_NODE_NUM];
//extern NODE_DEINTER_INST node_deinter_inst[MAX_NODE_NUM];

//char m_recvBuf[LIGHT_MAX_NUM * LIGHT_MAX_LEN * 2];
//extern char g_data_buf0[LIGHT_MAX_NUM * LIGHT_MAX_LEN+4];
//extern int base64encode(std::vector<uint8> const &input, std::string &out);
//extern int base64decode(std::string const &input, std::vector<uint8> &out);
int g_MD5FileLen;
std::string g_strOutData;
//std::vector<uint8> g_inData;


//extern StepVR_ParseData* stepvrparsedata_;
extern std::string g_TransmatMD5;

//std::string GetMD5(const char* strfileName)
//{
//	FILE* pFile = fopen(strfileName, "rb");  
//	if (pFile == NULL)
//	{
//		std::cout << "Open MD5 File Failed！" << std::endl;
//		return NULL;
//	}
//	memset(g_data_buf0, 0, sizeof(g_data_buf0));
//	size_t nRet = fread(g_data_buf0, sizeof(char), sizeof(g_data_buf0), pFile);  
//
//	if (nRet > sizeof(g_data_buf0))
//	{
//		std::cout << "Read MD5 File Failed！" << std::endl;
//		fclose(pFile);
//		return NULL;
//	}
//	int sizexx = _filelength(_fileno(pFile));
//	g_MD5FileLen = sizexx;
//
//	std::cout << "MD5 File Length : " << g_MD5FileLen<< std::endl;
//	fclose(pFile);  // 关闭文件
//
//
//	g_inData.clear();
//	g_strOutData.clear();
//	for (int i = 0; i < sizeof(g_data_buf0); i++)
//	{
//		g_inData.push_back(g_data_buf0[i]);
//	}
//	base64encode(g_inData, g_strOutData);
//
//	std::string str(g_strOutData);  // 得到string
//	MD5 md5(str);  // 准备MD5对象
//	std::cout << "MD5 : " << md5.md5() << std::endl;
//	return md5.md5();
//}


//std::string GetTransMatMD5(const char* strfileName)
//{
//	char transmat_data_buf0[8196] = { 0 };
//
//	FILE* pFile = fopen(strfileName, "rb");
//	if (pFile == NULL)
//	{
//		std::cout << "Open Transmat File Failed！" << std::endl;
//		return NULL;
//	}
//	size_t nRet = fread(transmat_data_buf0, sizeof(char), sizeof(transmat_data_buf0), pFile);
//	if (nRet > sizeof(transmat_data_buf0))
//	{
//		std::cout << "Read Transmat File Failed！" << std::endl;
//		fclose(pFile);
//		return NULL;
//	}
//	fclose(pFile);  
//
//	g_inData.clear();
//	g_strOutData.clear();
//	for (int i = 0; i < sizeof(transmat_data_buf0); i++)
//	{
//		g_inData.push_back(transmat_data_buf0[i]);
//	}
//	base64encode(g_inData, g_strOutData);
//
//	std::string str(g_strOutData); 
//	MD5 md5(str);  
//	return md5.md5();
//}

CClient::CClient(const std::string& IP, int port) :ServerIP(IP), ServerPort(port)
{
	memset(m_sendBuf, 0x0, 1024);
}

CClient::~CClient()
{
	closesocket(s);
	WSACleanup();
}

long CClient::ReadHdr(const char *buf)
{
	int netNum = 0;
	memcpy(&netNum, buf, 4);
	int client_num = ntohl(netNum);
	return client_num;
}

void CClient::ReadBody(long siz, std::string& val)
{
	int byteCount;
	//msg_v0::msgBox payload;

	int requestSize = siz - 4;
	int count = 0;
	while (1)
	{
		byteCount = recv(s, m_recvBuf + count, requestSize, 0);
		if (byteCount == SOCKET_ERROR)
		{
			printf("Error receiving data \n");
			return;
		}
		count += byteCount;

		if (count == (siz - 4))
		{
			break;
		}
		else
		{
			requestSize = siz - count;
		}
	}

	std::cout << "Second read byte count is " << count << std::endl;
	////Assign ArrayInputStream with enough memory 
	//google::protobuf::io::ArrayInputStream ais(m_recvBuf, siz);
	//CodedInputStream coded_input(&ais);
	////Read an unsigned integer with Varint encoding, truncating to 32 bits.
	////coded_input.ReadVarint32(&siz);
	////After the message's length is read, PushLimit() is used to prevent the CodedInputStream 
	////from reading beyond that length.Limits are used when parsing length-delimited 
	////embedded messages
	//google::protobuf::io::CodedInputStream::Limit msgLimit = coded_input.PushLimit(siz);
	////De-Serialize
	////payload.ParseFromCodedStream(&coded_input);
	//payload.ParseFromArray(m_recvBuf, count);
	////Once the embedded message has been parsed, PopLimit() is called to undo the limit
	//coded_input.PopLimit(msgLimit);


	val = m_recvBuf;
	std::cout << m_recvBuf << std::endl;
	//if (payload.has_msg0())
	//{
	//	msg_v0::MsgType0 mType0 = payload.msg0();
	//	if (mType0.id() == 0)
	//	{
	//		std::cout << mType0.msg() << std::endl;
	//	}
	//	else if (mType0.id() == 1)
	//	{
	//		// Lighthouse Information
	//		val = mType0.msg();
	//	}
	//	else if (mType0.id() == 2)
	//	{
	//		// TransMat File
	//		FILE* pFile = fopen("C:\\STEPVR_MMAP\\TransMat.txt", "w");
	//		std::string tmp = mType0.msg();
	//		fwrite(tmp.c_str(), sizeof(char), tmp.length(), pFile);
	//		fclose(pFile);

	//		std::string newmd5 = GetTransMatMD5("C:\\STEPVR_MMAP\\TransMat.txt");
	//		if (0!= newmd5.compare(g_TransmatMD5))
	//		{
	//			std::cout << "Reload TransMat.txt File" << std::endl;
	//			if (stepvrparsedata_->LoadMatFile("C:\\STEPVR_MMAP\\TransMat.txt"))
	//			{
	//				std::cout << "Reload TransMat.txt File Success." << std::endl;
	//			}
	//		}
	//	}
	//	else 
	//	{
	//		// 设备状态
	//	}
	//}
}

bool CClient::SendMsg(const std::string& Msg, int id)
{
	//std::cout << "sending message..." << std::endl;
	//memset(m_sendBuf, 0, sizeof(m_sendBuf));
	//msg_v0::msgBox b;
	//msg_v0::MsgType0 a;

	//a.set_msg(Msg);
	//a.set_id(id);

	//b.set_allocated_msg0(&a);

	////int siz = b.ByteSize() + 4;
	//std::cout << "size after serilizing is " << siz << std::endl;

	//int my_net_id = htonl(siz);
	//memcpy(m_sendBuf, &my_net_id, 4);

	//b.SerializeToArray(m_sendBuf + 4, siz);

	//int SendTotal = 0;
	//while (1)
	//{
	//	int sendCount = send(s, m_sendBuf + SendTotal, siz - SendTotal, 0);
	//	if (sendCount < 0)
	//	{
	//		b.release_msg0();
	//		return false;
	//	}
	//	else if (sendCount == 0)
	//	{
	//		b.release_msg0();
	//		return false;
	//	}
	//	else
	//	{
	//		SendTotal += sendCount;
	//	}
	//	if (SendTotal == siz)
	//	{
	//		std::cout << "Send Data Finished！" << std::endl;
	//		b.release_msg0();
	//		return true;
	//	}
	//}  // end of while
	return true;
}

void CClient::SendHandler()
{
	//// 发送设备状态
	//while (1)
	//{
	//	std::string strOutData = "";
	//	char tmpBuf[16] = { 0 };
	//	for (int i = 0; i < MAX_NODE_NUM; i++)
	//	{
	//		if (node_ver_table[i].active == 1)
	//		{
	//			memset(tmpBuf, 0x0, 16);
	//			std::sprintf(tmpBuf, "%d", node_ver_table[i].active);
	//			strOutData += tmpBuf;
	//			strOutData += ":";

	//			memset(tmpBuf, 0x0, 16);
	//			std::sprintf(tmpBuf, "%d", node_ver_table[i].communation_prot);
	//			strOutData += tmpBuf;
	//			strOutData += ":";

	//			memset(tmpBuf, 0x0, 16);
	//			std::sprintf(tmpBuf, "%d", node_ver_table[i].communation_type);
	//			strOutData += tmpBuf;
	//			strOutData += ":";

	//			memset(tmpBuf, 0x0, 16);
	//			std::sprintf(tmpBuf, "%d", node_ver_table[i].hardware_type);
	//			strOutData += tmpBuf;
	//			strOutData += ":";

	//			memset(tmpBuf, 0x0, 16);
	//			std::sprintf(tmpBuf, "%d", node_ver_table[i].sdk_version);
	//			strOutData += tmpBuf;
	//			strOutData += ":";

	//			memset(tmpBuf, 0x0, 16);
	//			std::sprintf(tmpBuf, "%d", node_ver_table[i].id);
	//			strOutData += tmpBuf;
	//			strOutData += ":";

	//			memset(tmpBuf, 0x0, 16);
	//			std::sprintf(tmpBuf, "%d", node_ver_table[i].soft_version);
	//			strOutData += tmpBuf;
	//			strOutData += ":";

	//			memset(tmpBuf, 0x0, 16);
	//			std::sprintf(tmpBuf, "%d", node_ver_table[i].elec);
	//			strOutData += tmpBuf;
	//			strOutData += ":";

	//			memset(tmpBuf, 0x0, 16);
	//			std::sprintf(tmpBuf, "%d", node_ver_table[i].md5Match);
	//			strOutData += tmpBuf;
	//			strOutData += ";";
	//		}
	//	}

	//	if (!SendMsg(strOutData, 3))
	//	{
	//		break;
	//	}
	//	Sleep(2000);
	//}  // end of while

}

int CClient::init()
{
	printf("Initialising Winsock... ");
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("Failed! errno = %d\n", WSAGetLastError());
		return -1;
	}
	printf("Initialised!\n");

	if ((s = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		printf("Failed to create TCP socket! errno = %d\n", WSAGetLastError());
		//StepVR_Log::GetInstance().AddLog("Failed to create TCP socket!");
		return -1;
	}
	printf("Succeed to create TCP socket!\n");
	//StepVR_Log::GetInstance().AddLog("Succeed to create TCP socket!");

	struct sockaddr_in server;
	server.sin_addr.s_addr = inet_addr(ServerIP.c_str());
	server.sin_family = AF_INET;
	server.sin_port = htons(ServerPort);

	if (connect(s, (struct sockaddr *)&server, sizeof(server)) < 0)
	{
		printf("Failed to connect TCP server! errno = %d\n", WSAGetLastError());
		//StepVR_Log::GetInstance().AddLog("Failed to connect TCP server!");
		return -1;
	}
	printf("Succeed to connect TCP server!\n");
	//StepVR_Log::GetInstance().AddLog("Succeed to connect TCP server!");

	m_thSend = std::thread([this] { this->SendHandler(); });
	m_thSend.detach();

	return 0;
}

bool CClient::ReceMsg(std::string& msg)
{
	std::cout << "Waiting for recv data..." << std::endl;
	//StepVR_Log::GetInstance().AddLog("Waiting for recv data...");
	char buffer[4] = { 0 };
	int byteCount = 0;

	while (1)
	{
		byteCount = recv(s, buffer, 4, 0);
		std::cout << "head: " << buffer << std::endl;
		if (byteCount > 0)
		{
			ReadBody(ReadHdr(buffer), msg);
			std::cout << "Succeed to recv Lighthouse Information!" << std::endl;
			return true;
		}
		else if (byteCount == 0)
		{
			std::cout << "Server disconnected!" << std::endl;
			//StepVR_Log::GetInstance().AddLog("Server disconnected!");
			return false;
		}
		else
		{
			std::cout << "Failed to recv data!" << std::endl;
			//StepVR_Log::GetInstance().AddLog("Failed to recv data!");
			std::cout << "Winsock error code: " << WSAGetLastError() << std::endl;
			return false;
		}
	}  // end of while
}

int ReceiveBroadcast(std::string& ServerIP)
{
	WORD wVersionRequested;
	WSADATA wsaData;
	wVersionRequested = MAKEWORD(2, 2);
	WSAStartup(wVersionRequested, &wsaData);

	SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock == INVALID_SOCKET)
	{
		std::cout << "Failed to create UDP socket! errno = " << WSAGetLastError() << std::endl;
		//StepVR_Log::GetInstance().AddLog("Failed to create UDP socket!");
	}

	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	//这个端口要和广播方广播端口一致
	addr.sin_port = htons(6666);

	int ret = bind(sock, (struct sockaddr *)&addr, sizeof(addr));
	if (ret < 0)
	{
		std::cout << "Failed to bind 6666 with UDP socket! errno = " << WSAGetLastError() << std::endl;
		//StepVR_Log::GetInstance().AddLog("Failed to bind 6666 with UDP socket!");
	}

	struct sockaddr_in from;
	int len = sizeof(from);
	char buff[50];

	std::cout << "Ready to recv UDP data..."<< std::endl;
	//StepVR_Log::GetInstance().AddLog("Ready to recv UDP data...");
	while (1)
	{
		ret = recvfrom(sock, buff, 50, 0, (struct sockaddr *)&from, &len);
		if (ret > 0)
		{
			std::cout << "Succeed to recv UDP data!" << std::endl;
			//StepVR_Log::GetInstance().AddLog("Succeed to recv UDP data!");
			buff[ret] = 0;
			printf("buff-----%s\n", buff);
			printf("%s %d\n", inet_ntoa(from.sin_addr), ntohs(from.sin_port));
			ServerIP = inet_ntoa(from.sin_addr);
			break;
		}
	}
	closesocket(sock);
	WSACleanup();

	return 0;
}

int DispatchClient()
{
	//g_strOldMD5 = GetMD5("C:\\STEPVR_MMAP\\lightHouseParam.bin");

	std::string ServerIP = "";
	while (1)
	{
		//g_IS_MD5Changed = false;

		ReceiveBroadcast(ServerIP);

		CClient client(ServerIP, 5555);
		client.init();

		while (1)
		{
			std::string strRecvData="";
			if (!client.ReceMsg(strRecvData))
			{
				break;
			}
			if (strRecvData.empty())
			{
				continue;
			}
			//StepVR_Log::GetInstance().AddLog("Succeed to recv Lighthouse Information!");
			std::cout << "Succeed to recv Lighthouse Information!" << std::endl;

			//std::vector<uint8> outData;
			//base64decode(strRecvData, outData);
			//FILE* pFile = fopen("C:\\STEPVR_MMAP\\lightHouseParam.bin", "wb");
			//for (int i = 0; i < outData.size(); i++)
			//{
			//	char bufferOut = outData[i];
			//	fwrite(&bufferOut, sizeof(char), 1, pFile);
			//}
			//fclose(pFile);
			//StepVR_Log::GetInstance().AddLog("Succeed to write Lighthouse Information into file!");
			//std::cout << "DispatchClient调用getmd5,取得strNewMD5" << std::endl;

			//std::string strNewMD5 = GetMD5("C:\\STEPVR_MMAP\\lightHouseParam.bin");
			//if (g_strOldMD5.compare(strNewMD5))
			//{
			//	std::cout << "Set Restart Cmd Enable." << std::endl;
			//	g_IS_MD5Changed = true;
			//	g_strOldMD5 = strNewMD5;
			//}
		}
	}
}
