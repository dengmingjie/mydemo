#include <vector>

typedef unsigned char uint8;

static uint8 encode(int value)
{
	value = value & 0x3f;

	if (value < 26)
		return value + 'A';
	if (value < 52)
		return (value - 26) + 'a';
	if (value < 62)
		return (value - 52) + '0';
	if (value < 63)
		return '+';
	return '/';
}

static int decode(uint8 ch)
{
	if (ch >= 'A' && ch <= 'Z')
		return ch - 'A';
	if (ch >= 'a' && ch <= 'z')
		return (ch - 'a') + 26;
	if (ch >= '0' && ch <= '9')
		return (ch - '0') + 52;
	if (ch == '+')
		return 62;
	if (ch == '/')
		return 63;
	/* would never reach here! */
	return 0;
}

int base64encode(std::vector<uint8> const &input, std::string &out)
{
	out.clear();

	size_t i, j, padding, value;

	for (i = j = 0; i < input.size();) {
		padding = 0;

		value = input[i++] << 16; /* fetch first byte */

		if (i < input.size()) /* fetch second byte */
			value |= input[i++] << 8;
		else
			++padding;

		if (i < input.size()) /* fetch third byte */
			value |= input[i++];
		else
			++padding;

		out += encode(value >> 18);
		out += encode(value >> 12);

		if (padding < 2)
			out += encode(value >> 6);
		else
			out += '=';

		if (padding < 1)
			out += encode(value);
		else
			out += '=';
	}

	return 0;
}

static bool checkbase64(std::string const &str)
{
	size_t len = str.length();

	if (0 != len % 4)
		return false;

	bool find_equal = false;
	for (size_t i = 0; i != len; ++i) {
		if (find_equal && '=' != str[i])
			return false;
		if ('=' == str[i]) {
			find_equal = true;
		}
		else {
			if (str[i] >= 'A' && str[i] <= 'Z')
				continue;
			if (str[i] >= 'a' && str[i] <= 'z')
				continue;
			if (str[i] >= '0' && str[i] <= '9')
				continue;
			if (str[i] == '+' || str[i] == '/')
				continue;
			return false;
		}
	}

	return true;
}

int base64decode(std::string const &input, std::vector<uint8> &out)
{
	out.clear();

	int value, padding = 0;
	size_t len = input.length();
	uint8 const *buffer = (uint8 const*)input.c_str();
	uint8 const *ptr;

	if (!checkbase64(input))
		return -1;

	for (size_t i = 0; i != len; i += 4) {
		ptr = buffer + i;

		value = decode(ptr[0]) << 18;
		value |= decode(ptr[1]) << 12;

		if ('=' == ptr[2])
			++padding;
		else
			value |= decode(ptr[2]) << 6;

		if ('=' == ptr[3])
			++padding;
		else
			value |= decode(ptr[3]);

		out.push_back(value >> 16);

		if (padding < 2)
			out.push_back(value >> 8);

		if (padding < 1)
			out.push_back(value);
	}

	return 0;
}


/*int _tmain(int argc, _TCHAR* argv[])
{

	std::vector<uint8> byte;

	for (int i = 0; i < 1024; i++)
	{
		byte.push_back(i);
	}
	std::string str;
	base64encode(byte, str);

	std::vector<uint8> byteout;
	base64decode(str, byteout);

	for (int i = 0; i < 1024; i++)
	{
		if (byte[i] != byteout[i])
		{
			std::cout << "error" << std::endl;
		}
	}

	return 0;
}*/