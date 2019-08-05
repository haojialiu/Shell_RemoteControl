#ifndef __CLIENT_H
#define __CLIENT_H

#include "TcpClient.h"
#include<pthread.h>
pthread_mutex_t mutex;
class Client
{
private:
    Client()
    {
        //_tcpClient = new TcpClient("192.168.205.164",6000);
        _tcpClient = new TcpClient("127.0.0.1",6000);
  }
    static Client* _client;
    TcpClient* _tcpClient;
public:
    static Client* getClient()
    {
        if(NULL == _client)
        {
            pthread_mutex_lock(&mutex);
            if(NULL == _client)
            {
                _client = new Client();
            }
            pthread_mutex_unlock(&mutex);
        }
        return _client;
    }
    int Send(const string& str)
    {
        return _tcpClient->sendToServer(str);
    }
    int Recv(string& str)
    {
        return _tcpClient->recvFromServer(str);
    }
    void Close()
    {
        _tcpClient->closeServer();
    }
    int getFd()
    {
	    return _tcpClient->getFd();
    }
    string& getIp()
    {
	return _tcpClient->getIp();
    }
};
Client* Client::_client = NULL;

#endif
