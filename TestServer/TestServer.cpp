#include <string>
#include <iostream>
#include <winsock2.h>
#include <windows.h>
#include <thread>
#include "MD5.h"

#pragma comment(lib, "ws2_32.lib")

char data_buf0[8000] = { 0 };

#define REQUEST_MD5     0  // 请求文件MD5值
#define RESPONSE_MD5    1  // 回应文件MD5值
#define REQUEST_FILE    2  // 请求文件内容
#define RESPONSE_FILE   3  // 回应文件内容

class CServer
{
private:
	std::thread thr;
	SOCKET lp;
public:
	CServer(SOCKET sk) : lp(sk) {}
	~CServer() 
	{
		closesocket(lp);
	}

	void init()
	{
		thr = std::thread([this] { this->SocketHandler(lp); });
		thr.detach();
	}

	std::string getMD5(const char* strPath)
	{
		std::cout << strPath << std::endl;
		FILE* pFile = fopen(strPath, "rb");  // 以二进制方式打开文件
		if (pFile == NULL)
		{
			std::cout << "打开文件失败！" << std::endl;
			return NULL;
		}
		size_t nRet = fread(data_buf0, sizeof(char), sizeof(data_buf0), pFile);  // 读取文件内容
		if (nRet > sizeof(data_buf0))
		{
			std::cout << "读取文件失败！" << std::endl;
			fclose(pFile);
			return NULL;
		}
		std::string str(data_buf0);  // 得到string
		fclose(pFile);  // 关闭文件

		MD5 md5(str);  // 准备MD5对象
		return md5.md5();
	}

	void sendFile(const char* strPath)
	{
		int len = 0;
		int cmd = RESPONSE_FILE;
		char data[1000];

		std::cout << strPath << std::endl;
		FILE* pFile = fopen(strPath, "rb");  // 以二进制方式打开文件
		if (pFile == NULL)
		{
			std::cout << "打开文件失败！" << std::endl;
			return;
		}

		bool bLoop = true;
		while (bLoop)
		{
			memset(data, 0, sizeof(data));

			size_t nRet = fread(data, sizeof(char), sizeof(data), pFile);  // 读取文件内容
			if (nRet > sizeof(data))
			{
				std::cout << "读取文件失败！" << std::endl;
				bLoop = false;
			}
			if (0 == nRet)
				bLoop = false;

			len = sizeof(len) + sizeof(cmd) + nRet;
			send(lp, (char*)&len, sizeof(len), 0);
			send(lp, (char*)&cmd, sizeof(cmd), 0);
			send(lp, data, nRet, 0);
		}

		fclose(pFile);
	}

	void SocketHandler(SOCKET lp)
	{
		printf("Start Recv\n");
		char buffer[4] = {0};
		int bytecount = 0;
		while (1){
			bytecount = recv(lp, buffer, 4, 0);
			if (bytecount > 0){
				printf("bytecount: %d\n", bytecount);
				int netNum = 0;
				memcpy(&netNum, buffer, 4);
				int client_num = ntohl(netNum);
				std::cout << "client_num: " << client_num << std::endl;
			}
			else{
				int nError = WSAGetLastError();
				if (nError != WSAEWOULDBLOCK)
				{
					std::cout << "Winsock error code: " << nError << std::endl;
					break;
				}
			}
				
		}
		while (1) 
		{
			//Peek into the socket and get the packet size
			bytecount = recv(lp, buffer, 4, 0);
			if (bytecount > 0)
			{
				// inDataLength number of bytes were received, use buffer as needed...
				std::cout << "First read byte count is " << bytecount << std::endl;

				int netNum = 0;
				memcpy(&netNum, buffer, 4);
				int client_num = ntohl(netNum);
				std::cout << "client_num: " << client_num << std::endl;

				int bytecount;
				char buffer[1024];
				int requestSize = client_num - 4;
				int count = 0;
				while (1)
				{
					if ((bytecount = recv(lp, (char *)buffer + count, requestSize, 0)) == SOCKET_ERROR){
						printf("Error receiving data \n");
						return;
					}
					count += bytecount;

					////for test//
					//if (send(csock, (char *)buffer + count, bytecount, 0) < 0)
					//{
					//	puts("Response failed");
					//}
					////for test//

					if (count == (client_num - 4))
					{
						break;
					}
					else
					{
						requestSize = client_num - count;
					}
				}

				std::cout << "Second read byte count is " << bytecount << std::endl;

				////for test//
				//if (send(lp, buffer, 4, 0) < 0)
				//{
				//	puts("Response failed");
				//}
				////for test//
				continue;
			}
			else if (bytecount == 0)
			{
				std::cout << "Client disconnected!" << std::endl;
				break;
			}
			if (bytecount == SOCKET_ERROR)
			{
				printf("Error receiving data %d\n", errno);
			}

			int nError = WSAGetLastError();
			if (nError != WSAEWOULDBLOCK)
			{
				std::cout << "Winsock error code: " << nError << std::endl;
				break;
			}

		}
/*		char buf[1024];

		bool bLoop = true;
		while (bLoop) 
		{
			std::cout << "接收数据中。。。" << std::endl;
			memset(buf, 0, sizeof(buf));

			int len = 0;
			int nRet = recv(lp, (char*)&len, 4, 0);
			if (0 == nRet)  // 客户端关闭
				bLoop = false;
			len = ntohl(len);
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
			case REQUEST_MD5:
			{
				std::cout << "被请求MD5值！" << std::endl;
				cmd = RESPONSE_MD5;
				char md5[16] = { 0 };
				memcpy(md5, getMD5(buf + 4).c_str(), sizeof(md5));
				std::cout << getMD5(buf + 4) << std::endl;
				len = sizeof(len) + sizeof(cmd) + strlen(md5);
				send(lp, (char*)&len, sizeof(len), 0);
				send(lp, (char*)&cmd, sizeof(cmd), 0);
				send(lp, md5, strlen(md5), 0);
				std::cout << "已发送MD5值！" << std::endl;
			}
				break;
			case REQUEST_FILE:
				std::cout << "被请求文件内容！" << std::endl;
				sendFile(buf + 4);
				std::cout << "已发送文件内容！" << std::endl;
				break;
			default:
				break;
			}  // end of swich
		}  // end of while*/
	}

};

int main(int argc, char* argv[])
{
	WSADATA wsa;
	SOCKET s, new_socket;
	struct sockaddr_in server, client;
	int c;

	printf("\nInitialising Winsock...");
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("Failed. Error Code : %d", WSAGetLastError());
		return 1;
	}

	printf("Initialised.\n");

	//Create a socket
	if ((s = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		printf("Could not create socket : %d", WSAGetLastError());
	}

	printf("Socket created.\n");

	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(5555);

	//Bind
	if (bind(s, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR)
	{
		printf("Bind failed with error code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}

	puts("Bind done");

	//Listen to incoming connections
	listen(s, 3);

	//Accept and incoming connection
	puts("Waiting for incoming connections...");

	c = sizeof(struct sockaddr_in);

	while ((new_socket = accept(s, (struct sockaddr *)&client, &c)) != INVALID_SOCKET)
	{
		puts("Connection accepted");
		printf("new_socket: %d", new_socket);

		//Reply to the client
		CServer server(new_socket);
		server.init();
		//std::thread thr = std::thread(SocketHandler, new_socket);
		//thr.detach();
		//message = "Hello Client , I have received your connection. But I have to go now, bye\n";
		//send(new_socket, message, strlen(message), 0);
	}

	if (new_socket == INVALID_SOCKET)
	{
		printf("accept failed with error code : %d", WSAGetLastError());
		return 1;
	}

	closesocket(s);
	WSACleanup();

	return 0;
}
