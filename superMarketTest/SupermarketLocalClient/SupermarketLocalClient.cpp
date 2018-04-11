#include <winsock2.h>
#include <windows.h>
#include <iostream>
#include <string>
#include <thread>

char oldMD5[] = "e807f1fcf82d132f";  // 本地缓存MD5值

#define REQUEST_MD5     0  // 请求文件MD5值
#define RESPONSE_MD5    1  // 回应文件MD5值
#define REQUEST_FILE    2  // 请求文件内容
#define RESPONSE_FILE   3  // 回应文件内容

class CClient
{
private:
	std::string ServerIP;
	int ServerPort;
	WSADATA wsa;
	SOCKET s;
	char sendBuf[1024];
public:
	CClient(std::string ip, int port) :ServerIP(ip), ServerPort(port)
	{
		memset(sendBuf, 0, sizeof(sendBuf));
	}

	~CClient()
	{
		closesocket(s);
		WSACleanup();
		std::cout << "客户端已关闭。。。" << std::endl;
	}

	void ReadBody(SOCKET csock, int siz)
	{
		if (siz > 1024) 
			return;

		int byteCount;
		char buffer[1024];
		int requestSize = siz - 4;
		int count = 0;
		while (1)
		{
			byteCount = recv(csock, buffer + count, requestSize, 0);
			if (byteCount == SOCKET_ERROR)
			{
				std::cout << "接收包体失败！" << std::endl;
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
		std::cout << "Second read byteCount is " << byteCount << std::endl;
		std::cout << "接收包体完成！" << std::endl;

		int cmd = 0;
		memcpy(&cmd, buffer, 4);
		switch (cmd)
		{
		case RESPONSE_MD5:
		{
			std::cout << "已接收MD5值！" << std::endl;
			if (memicmp(oldMD5, buffer + 4, 16) == 0)
			{
				std::cout << "旧MD5值: " << oldMD5 << std::endl;
				char newMD5[17] = { 0 };
				memcpy(newMD5, buffer + 4, 16);
				std::cout << "新MD5值: " << newMD5 << std::endl;
				std::cout << "MD5匹配成功！" << std::endl;
				// 其他操作
			}
			else
			{
				std::cout << "MD5匹配失败！请求文件内容。。。" << std::endl;
				cmd = REQUEST_FILE;
				char strPath[] = { "test.bin" };
				int len = sizeof(len) + sizeof(cmd) + sizeof(strPath);
				int netLen = htonl(len);
				memcpy(sendBuf, &netLen, 4);
				memcpy(sendBuf + 4, &cmd, 4);
				memcpy(sendBuf + 8, strPath, sizeof(strPath));

				int nRet = send(csock, sendBuf, len, 0);
				if (nRet < 0)
				{
					std::cout << "请求文件内容失败！" << std::endl;
					break;
				}
				else if (0 == nRet)
				{
					std::cout << "服务器关闭！" << std::endl;
					break;
				}
				std::cout << "已请求文件内容！" << std::endl;
				memset(sendBuf, 0, sizeof(sendBuf));
			}
		}
		break;
		case RESPONSE_FILE:
			{
				std::cout << "写入文件开始。。。" << std::endl;
				FILE* pFile = fopen("recv.bin", "ab");
				fwrite(buffer + 4, sizeof(char), siz - 8, pFile);
				fclose(pFile);
				std::cout << "写入文件完成！" << std::endl;
			}
			break;
		default:
			break;
		}  // end of swich
	}

	void SocketHandler(SOCKET lp)
	{
		int cmd = REQUEST_MD5;
		char strPath[] = { "test.bin" };
		int len = sizeof(len) + sizeof(cmd) + sizeof(strPath);
		int netLen = htonl(len);
		memcpy(sendBuf, &netLen, 4);
		memcpy(sendBuf + 4, &cmd, 4);
		memcpy(sendBuf + 8, strPath, sizeof(strPath));

		int nRet = send(lp, sendBuf, len, 0);
		if (nRet < 0)
		{
			std::cout << "请求MD5值失败！" << std::endl;
			return;
		}
		else if (0 == nRet)
		{
			std::cout << "服务器关闭！" << std::endl;
			return;
		}
		std::cout << "已请求MD5值！" << std::endl;
		memset(sendBuf, 0, sizeof(sendBuf));

		char buffer[4] = {0};
		int byteCount = 0;

		while (1) 
		{
			byteCount = recv(lp, buffer, 4, 0);
			if (byteCount > 0)
			{
				std::cout << "First recv byteCount is " << byteCount << std::endl;
				int netLen = 0;
				memcpy(&netLen, buffer, 4);
				int len = ntohl(netLen);
				ReadBody(lp, len);
				continue;
			}
			else if (byteCount == 0)
			{
				std::cout << "服务器关闭！" << std::endl;
				break;
			}
			else  // byteCount == SOCKET_ERROR
			{
				std::cout << "接收包头失败！" << std::endl;
			}

			int nError = WSAGetLastError();
			if (nError != WSAEWOULDBLOCK)
			{
				std::cout << "Winsock error code: " << nError << std::endl;
				break;
			}
		}  // end of while
	}

	int Init()
	{
		struct sockaddr_in server;

		printf("Initialising Winsock...\n");
		if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		{
			printf("Failed. Error Code : %d", WSAGetLastError());
			return 1;
		}
		printf("Initialised!\n");

		//Create a socket
		if ((s = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
		{
			printf("Could not create socket : %d", WSAGetLastError());
		}
		printf("Socket created!\n");

		server.sin_family = AF_INET;
		server.sin_port = htons(ServerPort);
		server.sin_addr.s_addr = inet_addr(ServerIP.c_str());

CONNECT:
		//Connect to remote server
		if (connect(s, (struct sockaddr *)&server, sizeof(server)) < 0)
		{
			puts("connect error...retrying...");
			Sleep(1000);
			goto CONNECT;
		}
		puts("Connected");

		std::thread thr = std::thread([this] { this->SocketHandler(s); });
		//thr.detach();
		thr.join();

		return 0;
	}

};  // end of CClient

int main(int argc, char* argv[])
{
	CClient client("192.168.3.143", 5555);
	client.Init();
	system("pause");
	return 0;
}
