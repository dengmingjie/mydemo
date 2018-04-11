/*******************************************************
Windows HID simplification

Alan Ott
Signal 11 Software

8/22/2009

Copyright 2009

This contents of this file may be used by anyone
for any reason without any conditions and may be
used as a starting point for your own applications
which use HIDAPI.
********************************************************/

#include <stdio.h>
#include <wchar.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <process.h>
#include <iostream>
#include "hidapi.h"
#include "MD5.h"
using namespace std;

// Headers needed for sleeping.
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

unsigned short VID = 0x1917;
unsigned short PID = 0x007F;
hid_device *handle[2];

int hidreaddata(hid_device * dev,unsigned char *buffer, int len)
{
	unsigned char buf[64] = {0};
	int res = 0;
	res = hid_read(dev, buf, 64);
	if (res > 0){
		memcpy(buffer, buf, len);
		res = len;
	}
	return res;
}

int hidwritedata(hid_device * dev,unsigned char *buffer, int len)
{
	if (dev == NULL){
		return - 1;
	}
	unsigned char buf[65] = {0};
	int res = 0;
	buf[0] = 0x0;//reprot num
	if (len > 64){
		return -2;
	}
	memcpy(buf + 1, buffer, len);
	res = hid_write(dev, buf, 65);
	return res;
}

char data_buf0[8000] = { 0 };


#define uint8_t	 unsigned char 
#define uint16_t unsigned short
#define uint32_t unsigned long


#pragma	pack(1)

typedef struct {
	uint8_t pack_head;
	uint8_t ctl_word;
	uint8_t cmd;
	uint16_t pack_no;
	uint8_t reserved[27];
} _PARAM_CMD_T;

typedef struct {
	uint8_t		head;
	uint8_t     pack_head;
	uint8_t     ctl_word;
	uint8_t     cmd;
	uint16_t    length;
	uint16_t    pack_count;
	uint16_t    light_count;
	uint8_t     reserved[23];
}_PARAM_INFO_T;

typedef struct {
	uint8_t		head;
	uint8_t     pack_head;
	uint8_t     ctl_word;
	uint8_t     cmd;
	uint16_t 	pack_no;
	uint8_t 	data_length;
	uint8_t 	data[25];
}_PARAM_DATA_T;

#pragma pack()


#define REQUEST_MD5			(0X10)
#define REQUEST_INFO		(0X11)
#define REQUEST_DATA		(0X12)


int main(int argc, char* argv[])
{
	uint16_t rp = 0;
	_PARAM_DATA_T *p_data = NULL;
	_PARAM_INFO_T *p_info = NULL;
	_PARAM_CMD_T *p_cmd = NULL;
	uint8_t md5_buf[16] = { 0 };
	int i;
	uint8_t usb_buf_rx[64] = { 0 };
	uint8_t usb_buf_tx[64] = { 0 };
	struct hid_device_info *devs, *cur_dev;

	FILE* pFile = fopen("test.bin", "rb");  // 以二进制方式打开文件
	if (pFile == NULL)
	{
		cout << "打开文件失败！" << endl;
		return -1;
	}
	size_t nRet = fread(data_buf0, sizeof(char), sizeof(data_buf0), pFile);  // 读取文件内容
	if (nRet > sizeof(data_buf0))
	{
		cout << "读取文件失败！" << endl;
		fclose(pFile);
		return -1;
	}
	string str(data_buf0);  // 得到string
	fclose(pFile);  // 关闭文件

	MD5 md5(str);  // 准备MD5对象
	memcpy(md5_buf, md5.md5().c_str(), 16);  // 得到md5值
	cout << "md5: " << md5.md5().c_str() << endl;
	
	if (hid_init())
		return -1;
	char vid_buf[8];
	char pid_buf[8];
	do{
		break;
		printf("Get VID  PID \n");
		scanf("%s %s",&vid_buf,&pid_buf);
		char ch = getchar();
		//printf("VID=%s,PID=%s\n(y/n)\n", vid_buf, pid_buf);
		printf("VID=%x,PID=%x\n(y/n)\n", strtol(vid_buf, NULL, 16), strtol(pid_buf, NULL, 16));
		ch = getchar();
		if ('y' == ch || 'Y'==ch){
			break;
		}

	}while (1);
	//VID = strtol(vid_buf, NULL, 16);
	//PID = strtol(pid_buf, NULL, 16);
	
	devs = hid_enumerate(0x0485, 0x00a8);
	cur_dev = devs;
	for (i = 0; cur_dev != NULL; i++, cur_dev = cur_dev->next) {
		hid_device *hid_devtmp;
		printf("Device Found\n  type: %04hx %04hx\n  path: %s\n  serial_number: %ls", cur_dev->vendor_id, cur_dev->product_id, cur_dev->path, cur_dev->serial_number);
		printf("\n");
		printf("  Manufacturer: %ls\n", cur_dev->manufacturer_string);
		printf("  Product:      %ls\n", cur_dev->product_string);
		printf("  Release:      %hx\n", cur_dev->release_number);
		printf("  Interface:    %d\n", cur_dev->interface_number);

		printf("\n");
		hid_devtmp = hid_open_path(cur_dev->path);
		if (NULL == (hid_devtmp)) {
			printf("unable to open device\n");
			continue;
		}
		handle[i] = hid_devtmp;
	} 

	uint8_t xxx = 0;
	//while(0)
	{
		//xxx = getchar();
		
		uint8_t loop = 1;
		while (loop)
		{
			hid_read(handle[1], usb_buf_rx, 32);
			if (usb_buf_rx[0] != 0x51)continue;
			switch (usb_buf_rx[2])
			{
			case REQUEST_MD5: {
				memset(usb_buf_tx, 0x00, 64);
				usb_buf_tx[0] = 0x00;
				usb_buf_tx[1] = 0x51;
				usb_buf_tx[2] = 0x04;
				usb_buf_tx[3] = REQUEST_MD5;
				memcpy(&usb_buf_tx[4], md5_buf, 16);
				hid_write(handle[1], usb_buf_tx, 33);
				break;
			}
			case REQUEST_INFO: {
				memset(usb_buf_tx, 0x00, 64);
				p_info = (_PARAM_INFO_T *)&usb_buf_tx[0];
				p_info->head = 0x00;
				p_info->pack_head = 0x51;
				p_info->ctl_word = 0x04;
				p_info->cmd = REQUEST_INFO;
				p_info->length = sizeof(data_buf0);
				if (p_info->length % 25)
				{
					p_info->pack_count = p_info->length / 25 + 1;
				}
				else {
					p_info->pack_count = p_info->length / 25;
				}
				p_info->light_count = p_info->length / 160;
				hid_write(handle[1], usb_buf_tx, 33);
				break;
			}
			case REQUEST_DATA: {
#if 1
				p_cmd = (_PARAM_CMD_T *)usb_buf_rx;
				memset(usb_buf_tx, 0x00, 64);
				p_data = (_PARAM_DATA_T *)usb_buf_tx;
				p_data->head = 0x00;
				p_data->pack_head = 0x51;
				p_data->ctl_word = 0x01;
				p_data->cmd = REQUEST_DATA;
				p_data->pack_no = p_cmd->pack_no;
				p_data->data_length = 25;
				rp = p_cmd->pack_no * p_data->data_length;
				if ((sizeof(data_buf0) - rp) <= p_data->data_length) {
					p_data->data_length = sizeof(data_buf0) - rp;
					p_data->ctl_word = 0x04;
					loop = 0;
				}
				memcpy(p_data->data, &data_buf0[rp], p_data->data_length);
#endif
				hid_write(handle[1], usb_buf_tx, 33);
				break;
			}
			}
		}
	}
	

end:
	hid_free_enumeration(devs);
	system("pause");
}
