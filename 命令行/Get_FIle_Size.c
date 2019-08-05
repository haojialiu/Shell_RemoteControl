
#include"Server.h"
#include"Client.h"
#include<string.h>
#include<memory.h>
#include<unistd.h>
#include<sys/wait.h>
#include<stdlib.h>
#include<fcntl.h>
using namespace std;

int main()
{
int File_fd = open("ceshi.c",O_RDONLY);
int File_Size = lseek(File_fd,0,SEEK_END);
lseek(File_fd,0,SEEK_SET);
cout << " File_Size:" << File_Size <<endl;
}
