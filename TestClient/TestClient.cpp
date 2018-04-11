#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <WinSock2.h>

#pragma comment(lib,"ws2_32.lib")

char localMD5[16] = "aa";  // 本地缓存MD5值

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
		memset(sendBuf, 0x0, 1024);
	}
	~CClient() 
	{
		closesocket(s);
		WSACleanup();
	}

	void SocketHandler(SOCKET lp)
	{
		int my_net_id = htonl(4);
		memcpy((char*)sendBuf, &my_net_id, 4);

		//Send some data
		while (1)
		{
			int ret = 0;
			ret += send(s, sendBuf, 4, 0);
			if (ret < 0)
			{
				puts("Send failed");
				printf("ret = %d\n ",ret);
				return;
			}
			if (ret == 4)
			{
				break;
			}
		}
		puts("Data Send\n");

		//system("pause");
/*		int cmd = REQUEST_MD5;
		char strPath[] = {"test.bin"};
		len = htonl(sizeof(len) + sizeof(cmd) + strlen(strPath));
		send(lp, (char*)&len, sizeof(len), 0);
		send(lp, (char*)&cmd, sizeof(cmd), 0);
		send(lp, strPath, strlen(strPath), 0);
		std::cout << "已请求MD5值！" << std::endl;
		system("pause");

		char buf[1024];

		bool bLoop = true;
		while (bLoop)
		{
			std::cout << "接收数据中。。。" << std::endl;
			memset(buf, 0, sizeof(buf));

			int len = 0;
			int nRet = recv(lp, (char*)&len, 4, 0);
			if (0 == nRet)  // 服务器关闭
				bLoop = false;
			std::cout << "len: " << len << std::endl;

			int sum = len - 4;
			int count = 0;
			while (1)
			{
				nRet = recv(lp, buf + count, sum, 0);
				count += nRet;
				sum -= nRet;

				if (0 == sum)
					break;
			}

			int cmd = 0;
			memcpy(&cmd, buf, 4);
			std::cout << "cmd: " << cmd << std::endl;
			std::cout << "data: " << buf + len << std::endl;
			std::cout << "接收数据完成！" << std::endl;
			system("pause");
			switch (cmd)
			{
			case RESPONSE_MD5:
			{
				std::cout << "接收MD5值！" << std::endl;
				if (memcmp(localMD5, buf + 4, 16) == 0)
				{
					std::cout << "localMD5: " << localMD5 << std::endl;
					std::cout << "newMD5: " << buf + 4 << std::endl;
					std::cout << "MD5匹配成功！" << std::endl;
					bLoop = false;
				}
				else
				{
					cmd = REQUEST_FILE;
					len = sizeof(len) + sizeof(cmd) + strlen(strPath);
					send(lp, (char*)&len, sizeof(len), 0);
					send(lp, (char*)&cmd, sizeof(cmd), 0);
					send(lp, strPath, strlen(strPath), 0);
				}
			}
			break;
			case RESPONSE_FILE:
				std::cout << "接收文件内容！" << std::endl;
			{
				FILE* pFile = fopen(strPath, "ab");
				fwrite(buf + 4, sizeof(char), sizeof(buf) - 4, pFile);
				fclose(pFile);
			}
				break;
			default:
				break;
			}  // end of swich
		}  // end of while*/
	}

	int init()
	{
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

		struct sockaddr_in server;
		server.sin_family = AF_INET;
		server.sin_port = htons(ServerPort);
		server.sin_addr.s_addr = inet_addr(ServerIP.c_str());

		//Connect to remote server
		if (connect(s, (struct sockaddr *)&server, sizeof(server)) < 0)
		{
			puts("connect error...retrying...");
			return 1;
		}
		puts("Connected!\n");

		std::thread thr = std::thread([this] { this->SocketHandler(s); });
		thr.detach();

		return 0;
	}

};

int main(int argc, char* argv[])
{
	CClient client("192.168.3.143", 5555);
	client.init();
	while (1)
	{
		Sleep(5);
	}
	return 0;
}