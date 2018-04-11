#include <iostream>
#include <string>
#include "MD5.h"
using namespace std;

int main(int argc, char* argv[]) 
{
	char buf[2048] = {0};

	FILE* pFile = fopen("test.bin", "rb");  // �Զ����Ʒ�ʽ���ļ�
	if (pFile == NULL)
	{
		cout << "���ļ�ʧ�ܣ�" << endl;
		return -1;
	}
	size_t nRet = fread(buf, sizeof(char), sizeof(buf), pFile);  // ��ȡ�ļ�����
	if (nRet > sizeof(buf))
	{
		cout << "��ȡ�ļ�ʧ�ܣ�" << endl;
		fclose(pFile);
		return -1;
	}
	string str(buf);  // �õ�string
	fclose(pFile);  // �ر��ļ�

	MD5 md5(str);  // ׼��MD5����
	cout << "md5: " << md5.md5().c_str() << endl;  // �õ�md5ֵ

	getchar();
	return 0;
}
