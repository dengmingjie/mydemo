#include <iostream>
#include <string>
#include "MD5.h"
using namespace std;

int main(int argc, char* argv[]) 
{
	char buf[2048] = {0};

	FILE* pFile = fopen("test.bin", "rb");  // 以二进制方式打开文件
	if (pFile == NULL)
	{
		cout << "打开文件失败！" << endl;
		return -1;
	}
	size_t nRet = fread(buf, sizeof(char), sizeof(buf), pFile);  // 读取文件内容
	if (nRet > sizeof(buf))
	{
		cout << "读取文件失败！" << endl;
		fclose(pFile);
		return -1;
	}
	string str(buf);  // 得到string
	fclose(pFile);  // 关闭文件

	MD5 md5(str);  // 准备MD5对象
	cout << "md5: " << md5.md5().c_str() << endl;  // 得到md5值

	getchar();
	return 0;
}
