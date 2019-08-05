#ifndef __TCPCLIENT_H
#define __TCPCLIENT_H

#include<iostream>
#include<string>
#include<errno.h>
#include<unistd.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#define MAX_SIZE 512
using namespace std;

class TcpClient
{
private:
    string _ip;
    unsigned short _port;
    int _fd;
public:
    TcpClient(string ip,int port)
    {
	    _fd = socket(AF_INET,SOCK_STREAM,0);
	    if( -1 == _fd)
	    {
		    cerr<<"_fd creat fail;errno:"<<errno<<endl;
		    return;
	    }
	    _ip = ip;
	    _port = port; 
	    struct sockaddr_in saddr;
    	saddr.sin_family = AF_INET;
    	saddr.sin_port = htons(_port);
    	saddr.sin_addr.s_addr = inet_addr(_ip.c_str());
    
    	if(-1 == connect(_fd,(struct sockaddr*)&saddr,sizeof(saddr)))
    	{
    		cerr<<"connect fail;errno:"<<errno<<endl;
    		return;
    	}

     }
    int sendToServer(const string& str)
    {
    	int len = str.size();
        int n = send(_fd,str.c_str(),len,0);
    	if(-1 == n)
    	{
    		cerr<<"send fail;errno:"<<errno<<endl;
    	}
        return n;
    }
    int recvFromServer(string& str)
    {
        char arr[MAX_SIZE] = {0};
        int n = recv(_fd,arr,MAX_SIZE,0);
        str = arr;
    	if(-1 == n)
    	{
    		cerr<<"send fail;errno:"<<errno<<endl;
    		return -1;
    	}
        return n;
    }
    void closeServer()
    {
        close(_fd);
    }
    int getFd()
    {
        return _fd;
    }
    string& getIp()
    {
        return _ip;
    }
    unsigned short getPort()
    {
        return _port;
    }
};


#endif
