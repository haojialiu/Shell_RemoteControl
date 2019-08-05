#ifndef __TCPSERVER_H
#define __TCPSERVER_H

#include<iostream>
#include<unistd.h>
#include<string>
//#include<event.h>
#include<errno.h>
#include<netinet/in.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#define MAX_SIZE 512
using namespace std;


class TcpServer
{
private:
    string _ip;
    unsigned short _port;
    int _listenFd;
    
public:
    TcpServer(string ip,int port)
    {
        
	    _listenFd = socket(AF_INET,SOCK_STREAM,0);
	    if(-1 == _listenFd)
	    {
		    cerr<<"fd creat fail;errno:"<<errno<<endl;
		    return;
	    }
        _ip = ip;
        _port = port;	

	    struct sockaddr_in saddr;
	    saddr.sin_family = AF_INET;
	    saddr.sin_port = htons(_port);
	    saddr.sin_addr.s_addr = inet_addr(_ip.c_str());

	    if(-1 == bind(_listenFd,(struct sockaddr*)&saddr,sizeof(saddr)))
	    {
		    cerr<<"bind fail;errno:"<<errno<<endl;
		    return;
	    }	

	    if(-1 == listen(_listenFd,20))
	    {   
		    cerr<<"listen fail;errno:"<<errno<<endl;
		    return;
	    }
    }
    int acceptClient()
    {
        struct sockaddr_in caddr;
    	int len = sizeof(caddr);
	
	    int cli_fd = accept(_listenFd,(struct sockaddr*)&caddr,(socklen_t*)&len);
    	if(-1 == cli_fd)
    	{
    		cerr<<"accept fail;errno:"<<errno<<endl;
    	}
	    return cli_fd;
    
    }
    
    int sendToClient(int cliFd,const string& str)
    {
	    int len = str.size();
	    int n = send(cliFd,str.c_str(),len,0);
	    if(-1 == n)
	    {
		    cerr<<"send fail;errno:"<<errno<<endl;
		    return -1;
	    }
	    return n;

    }
    
    

    int recvFromClient(int cliFd,string& str)
    {
        char arr[MAX_SIZE] = {0};
	    int n = recv(cliFd,arr,MAX_SIZE,0);
        str = arr;
	    if(-1 == n)
	    {
	    	cerr<<"recv fail;errno:"<<errno<<endl;
	    	return -1;
	    }
	    return n;
    }
    void closeClient(int cliFd)
    {
        close(_listenFd);
    }
    int getListenFd()
    {
        return _listenFd;
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
