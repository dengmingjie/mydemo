#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <windows.h>
#include <iostream>
#include <string>
#include <thread>
#include "MD5.h"

#define REQUEST_MD5     0  // �����ļ�MD5ֵ
#define RESPONSE_MD5    1  // ��Ӧ�ļ�MD5ֵ
#define REQUEST_FILE    2  // �����ļ�����
#define RESPONSE_FILE   3  // ��Ӧ�ļ�����

class CServer
{
private:
	char sendBuf[1024];
	std::thread thr;
	SOCKET lp;
public:
	CServer(SOCKET sk) : lp(sk)
	{
		memset(sendBuf, 0, sizeof(sendBuf));
	}

	~CServer()
	{
		
	}

	void Init()
	{
		thr = std::thread([this] { this->SocketHandler(lp); });
		thr.detach();
	}

	std::string GetMD5(const char* strPath)
	{
		std::cout << strPath << std::endl;
		char strFileData[40960] = { 0 };

		FILE* pFile = fopen(strPath, "rb");  // �Զ����Ʒ�ʽ���ļ�
		if (pFile == NULL)
		{
			std::cout << "���ļ�ʧ�ܣ�" << std::endl;
			return NULL;
		}
		size_t nRet = fread(strFileData, sizeof(char), sizeof(strFileData), pFile);  // ��ȡ�ļ�����
		if (nRet > sizeof(strFileData))
		{
			std::cout << "��ȡ�ļ�ʧ�ܣ�" << std::endl;
			fclose(pFile);
			return NULL;
		}
		std::string str(strFileData);  // �õ�string
		fclose(pFile);  // �ر��ļ�

		MD5 md5(str);  // ׼��MD5����
		std::cout << "md5ֵ: " << md5.md5().c_str() << std::endl;
		return md5.md5();
	}

	void SendFile(SOCKET csock, const char* strPath)
	{
		std::cout << strPath << std::endl;
		FILE* pFile = fopen(strPath, "rb");  // �Զ����Ʒ�ʽ���ļ�
		if (pFile == NULL)
		{
			std::cout << "���ļ�ʧ�ܣ�" << std::endl;
			return;
		}

		while (!feof(pFile))
		{
			char temp[1000] = {0};
			int nRet = fread(temp, sizeof(char), sizeof(temp), pFile);
			if (nRet > sizeof(temp))
			{
				std::cout << "��ȡ�ļ�ʧ�ܣ�" << std::endl;
				break;
			}
			if (0 == nRet)
			{
				std::cout << "�ļ�������ɣ�" << std::endl;
				break;
			}

			int cmd = RESPONSE_FILE;
			int len = sizeof(len) + sizeof(cmd) + nRet;
			int netLen = htonl(len);
			memcpy(sendBuf, &netLen, 4);
			memcpy(sendBuf + 4, &cmd, 4);
			memcpy(sendBuf + 8, temp, nRet);

			nRet = send(csock, sendBuf, len, 0);
			if (nRet < 0)
			{
				std::cout << "�����ļ�ʧ�ܣ�" << std::endl;
				break;
			}
			else if (0 == nRet)
			{
				std::cout << "�ͻ��˹رգ�" << std::endl;
				break;
			}
			memset(sendBuf, 0, sizeof(sendBuf));
		}  // end of while

		fclose(pFile);
	}

	void ReadBody(SOCKET csock, int siz)
	{
		int byteCount;
		char buffer[1024];
		int requestSize = siz - 4;
		int count = 0;
		while (1)
		{
			byteCount = recv(csock, buffer + count, requestSize, 0);
			if (byteCount == SOCKET_ERROR)
			{
				std::cout << "���հ���ʧ�ܣ�" << std::endl;
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
		std::cout << "Second recv byteCount is " << byteCount << std::endl;
		std::cout << "���հ�����ɣ�" << std::endl;

		int cmd = 0;
		memcpy(&cmd, buffer, 4);
		switch (cmd)
		{
		case REQUEST_MD5:
		{
			std::cout << "������MD5ֵ��" << std::endl;
			cmd = RESPONSE_MD5;
			char md5[16] = { 0 };
			memcpy(md5, GetMD5(buffer + 4).c_str(), sizeof(md5));
			int len = sizeof(len) + sizeof(cmd) + sizeof(md5);
			int netLen = htonl(len);
			memcpy(sendBuf, &netLen, 4);
			memcpy(sendBuf + 4, &cmd, 4);
			memcpy(sendBuf + 8, md5, sizeof(md5));

			int nRet = send(csock, sendBuf, len, 0);
			if (nRet < 0)
			{
				std::cout << "����MD5ֵʧ�ܣ�" << std::endl;
				break;
			}
			else if (0 == nRet)
			{
				std::cout << "�ͻ��˹رգ�" << std::endl;
				break;
			}
			std::cout << "�ѷ���MD5ֵ��" << std::endl;
			memset(sendBuf, 0, sizeof(sendBuf));
		}
		break;
		case REQUEST_FILE:
			std::cout << "�������ļ����ݣ�" << std::endl;
			SendFile(csock, buffer + 4);
			std::cout << "�ѷ����ļ����ݣ�" << std::endl;
			break;
		default:
			break;
		}  // end of swich
	}

	void SocketHandler(SOCKET lp)
	{
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
				std::cout << "�ͻ��˹رգ�" << std::endl;
				break;
			}
			else  // byteCount == SOCKET_ERROR
			{
				std::cout << "���հ�ͷʧ�ܣ�" << std::endl;
			}

			int nError = WSAGetLastError();
			if (nError != WSAEWOULDBLOCK)
			{
				std::cout << "Winsock error code: " << nError << std::endl;
				break;
			}
		}  // end of while
		closesocket(lp);
	}

};  // end of CServer

int main(int argc, char* argv[])
{
	WSADATA wsa;
	SOCKET s, newSocket;
	struct sockaddr_in server, client;

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

	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_port = htons(5555);
	server.sin_addr.s_addr = INADDR_ANY;

	//Bind
	if (bind(s, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR)
	{
		printf("Bind failed with error code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	puts("Bind done!");

	//Listen to incoming connections
	listen(s, 3);

	//Accept and incoming connection
	puts("Waiting for incoming connections...");

	int c = sizeof(struct sockaddr_in);

	while ((newSocket = accept(s, (struct sockaddr *)&client, &c)) != INVALID_SOCKET)
	{
		puts("Connection accepted!");

		//Reply to the client
		CServer SServer(newSocket);
		SServer.Init();
		//std::thread thr = std::thread(SocketHandler, newSocket);
		//thr.detach();
	}

	if (newSocket == INVALID_SOCKET)
	{
		printf("accept failed with error code : %d", WSAGetLastError());
	}

	closesocket(s);
	WSACleanup();

	return 0;
}
