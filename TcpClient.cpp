#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include "TcpClient.h"

TcpClient::TcpClient()
{

}

TcpClient::~TcpClient()
{

}

bool TcpClient::Init(char * pIp, int port, CallbackFunc pFun)
{
	m_strIp = pIp;
	m_port = port;
	m_socketfd = -1;
	m_runFlag = true;
	m_pFun = pFun;

	bool creatFlag = CreateSocket();
	if(!creatFlag)
	{
		printf("create socket error  \n");
		return false;
	}
	printf("create socket suc \n");
	bool connectFlag = ConnectSocket();
	if(!connectFlag)
	{
		printf("Connect socket error \n");
		return false;
	}
	printf("Connect socket suc \n");
	return true;
}

bool TcpClient::CreateSocket()
{
	m_socketfd = socket(AF_INET, SOCK_STREAM, 0);
	if(m_socketfd == -1)
	{
		printf("socket error \n");
		return false;
	}
	// 设置为非阻塞
	int flags = fcntl(m_socketfd,F_GETFL,0);//获取建立的sockfd的当前状态（非阻塞）
	fcntl(m_socketfd,F_SETFL,flags|O_NONBLOCK);//将当前sockfd设置为非阻塞
  	return true;
}

bool TcpClient::ConnectSocket()
{
	struct sockaddr_in serverAddr;
	fd_set rset, wset;
	struct timeval tval;
	tval.tv_sec=0;
	tval.tv_usec=500 * 1000;   // 连接超时设置为 0.5秒

	serverAddr.sin_family = AF_INET; 
	serverAddr.sin_port = htons(m_port);
	serverAddr.sin_addr.s_addr = inet_addr(m_strIp.c_str());
	int connectFlag = connect(m_socketfd, (struct sockaddr*)&serverAddr, sizeof(struct sockaddr));
	if(connectFlag < 0)
	{
		if(errno != EINPROGRESS)
		{
			printf("connect error \n");
			return false;
		}
	}
	if(connectFlag == 0)
	{
		return true;
	}

	FD_ZERO(&rset);
	FD_SET(m_socketfd,&rset);
	wset=rset;
	int selectFlag = select(m_socketfd+1, NULL, &wset, NULL,&tval);
	if(selectFlag <= 0)
	{
		printf("connect time out \n");
		close(m_socketfd);
		return false;
	}
	int error;
	socklen_t len = sizeof(error);
	getsockopt(m_socketfd, SOL_SOCKET, SO_ERROR, &error, &len);
	if(error)
	{
		printf("Error in connection() %d - %s/n", error, strerror(error));
		close(m_socketfd);
		return false;
	}
	return true;
}

void TcpClient::ReadData()
{

	fd_set rset, wset;
	struct timeval tval;
	tval.tv_sec=0;
	tval.tv_usec=1000 * 1000;   // 连接超时设置为 1秒

	char buffer[2048] = {0};
	
	while(m_runFlag)
	{
		FD_ZERO(&rset);
		FD_SET(m_socketfd, &rset);
		// 非阻塞   一秒返回
		// if(select(m_socketfd+1,&rset,NULL,NULL,&tval) <= 0)
		if(select(m_socketfd+1,&rset,NULL,NULL,NULL) <= 0)
        {
           perror("监视未成功");
            exit(-1);
        }
		printf("select  suc  \n");
		
		for(int fd1=0; fd1 < m_socketfd+1; fd1++)
        {
           if(FD_ISSET(fd1, &rset))
            {
              if(fd1 == m_socketfd)
              {
					int recvbytes=recv(m_socketfd, buffer, sizeof(buffer), 0);
					if(recvbytes == 0)
					{
						printf("tcp connect end \n");
						close(m_socketfd);
						return ;
					}
			  		if(recvbytes > 0)
			  		{
						m_pFun(buffer, recvbytes);
					}
					else if(errno == EINTR)
					{
						continue;
					}
					else
					{
						printf("recv error\n");
						close(m_socketfd);
						return ;
					}
					
   				}
             }
         }
	}

}


void TcpClient::RunSocket()
{
	printf("ReadData Start \n");
	ReadData();
	printf("ReadData End \n");
}

int TcpClient::SendData(char * pBuffer, int bufLen)
{
	return send(m_socketfd, pBuffer, bufLen, 0);
}
