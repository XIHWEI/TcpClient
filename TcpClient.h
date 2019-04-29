#ifndef TCP_CLIENT_H
#define TCP_CLIENT_H

#include <iostream>
#include <string>

typedef void(*CallbackFunc)(char *,  int);

class TcpClient
{
	public :
		TcpClient();
		~TcpClient();
		bool Init(char * pIp, int port, CallbackFunc pFun);		
		void RunSocket();
		int SendData(char * pBuffer, int bufLen);
		
	protected:
		bool CreateSocket();
		bool ConnectSocket();
		void ReadData();
		
	protected:
		bool m_runFlag;
		std::string m_strIp;
		int m_port;
		int m_socketfd;
		CallbackFunc  m_pFun;
};


#endif //TCP_CLIENT_H