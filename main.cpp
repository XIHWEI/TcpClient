#include <iostream>
#include <string.h>

#include "TcpClient.h"

int main()
{
	TcpClient tcpClient;
	bool ininFlag = tcpClient.Init("10.1.10.7",  8888, HandleReadData);
	if(!ininFlag)
	{
		return 0;
	}

	// 接收数据
	tcpClient.RunSocket();
	getchar();
	return 0;
}

