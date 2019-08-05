#ifndef __SERVER_H
#define __SERVER_H

#include "TcpServer.h"
#include<pthread.h>

pthread_mutex_t mutex1;
class Server
{
private:
    Server()
    {
        _tcpServer = new TcpServer("172.18.93.81",6000);
    }
    static Server *_server;
    TcpServer *_tcpServer;
public:
    static Server* getServer()
    {
        if(NULL == _server)
        {
            pthread_mutex_lock(&mutex1);
            if(NULL == _server)
            {
                _server = new Server();
            }
            pthread_mutex_unlock(&mutex1);
        }
        return _server;

    }
    int accept()
    {
        return _tcpServer->acceptClient();
    }
    int send(int cliFd,const string& str)
    {
        return _tcpServer->sendToClient(cliFd,str);
    }
    int recv(int cliFd,string& str)
    {
        return _tcpServer->recvFromClient(cliFd,str);
    }
    void close(int cliFd)
    {
        _tcpServer->closeClient(cliFd);
    }
    int getListenFd()
    {
        return _tcpServer->getListenFd();
    }

};
Server* Server::_server = NULL;


#endif

