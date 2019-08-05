#include"Server.h"
#include<string>
#include<stdio.h>
#include<string.h>
#include<fcntl.h>
#include<stdlib.h>
#include<sys/wait.h>
#define MAX_SIZE 512
string Get_argv(string Instruct)
{
    char excision[MAX_SIZE];
    string parameter;
    Instruct.copy(excision,Instruct.size()+1,0);
    excision[Instruct.size()] = '\0';

    strtok(excision," ");
    parameter = strtok(NULL," ");
    return parameter;
}

void Get_File_MD5(char *FileName,char *Check_MD5)
{
    int Conduit[2];

    char *path[128] = {0};
    char Instruct[] = "md5sum";
    path[0] = Instruct;
    path[1] = FileName;

    pipe(Conduit);
    pid_t pid = fork();
    if(pid == 0)
    {
        dup2(Conduit[1],1);
        execvp(path[0],path);
        exit(0);
    }
    close(Conduit[1]);
    wait(NULL);
    read(Conduit[0],Check_MD5,127);
}
void File_Check(char *File_MD5,char *FileName)
{
    char Truth_File_MD5[128] = {0};
    Get_File_MD5(FileName,Truth_File_MD5);
    if(strncmp(File_MD5,Truth_File_MD5,32) == 0)
    {
        cout << "文件传输完成！"<< endl;
        cout << "文件完整，没有缺失！" <<endl;
    }
    else
    {
        cout << "文件传输完成！"<< endl;
        cout << "文件已损坏，有缺失！" <<endl;
    }
}


void Recv_File(int Socket_fd,char* FileName)
{
    int File_fd = open(FileName,O_WRONLY|O_CREAT,0600);
    if(File_fd == -1)
    {
	    Server::getServer()->send(Socket_fd,"error:File creation failed!");
        return;
    }

    int Existing_Size = 0;
    if(access(FileName,F_OK) == 0)
    {
        Existing_Size = lseek(File_fd,0,SEEK_END);
        char Temporary[128] = {0}; 
        sprintf(Temporary,"%d",Existing_Size);
        string Send_Existing_Size = Temporary;
        Server::getServer()->send(Socket_fd,Send_Existing_Size);
    }
    else
    {
        Server::getServer()->send(Socket_fd,"ok");
    }
    string From_Passive_SizeMd5;
    if(Server::getServer()->recv(Socket_fd,From_Passive_SizeMd5) <= 0)
    {
        cout << "被控端退出了控制状态！" << endl;
        return;
    }
    if(strncmp(From_Passive_SizeMd5.c_str(),"ok",2) != 0)
    {
        cout << From_Passive_SizeMd5 << endl;
        return;
    }
    fflush(stdout);
    char From_Passive_SizeMd5_char[256] = {0};
    strcpy(From_Passive_SizeMd5_char,From_Passive_SizeMd5.c_str());
    char* File_MD5 = strtok(From_Passive_SizeMd5_char,"#");
    File_MD5 = strtok(NULL,"#");
    int Send_File_Size = 0;
    sscanf(File_MD5,"%d",&Send_File_Size);
    File_MD5 = strtok(NULL,"#");

    Server::getServer()->send(Socket_fd,"ok");
    int File_Size = Send_File_Size + Existing_Size;
    int File_Size_Count = 0;
    char File_Data[MAX_SIZE] = {0};

    while(1)
    {
        if(Existing_Size >= File_Size)
        {
            break;
        }
        string File_Data_string;
        if(File_Size - Existing_Size < MAX_SIZE)
        {
            File_Size_Count = recv(Socket_fd,File_Data,File_Size-Existing_Size,0);
            if(File_Size_Count <= 0)
            {
                cout << "被控端退出了控制状态！" << endl;
                return;
            }
        }
        else
        {

            File_Size_Count = recv(Socket_fd,File_Data,MAX_SIZE,0);
            if(File_Size_Count <= 0)
            {
                cout << "被控端退出了控制状态！" << endl;
                return;
            }
        }
        
        
        if(File_Size_Count <= 0)
        {
            close(Socket_fd);
            return;
        }
        Existing_Size = Existing_Size + File_Size_Count;
        
        write(File_fd,File_Data,File_Size_Count);
	    File_Data[0] = '\0';
        File_Data_string = "";
        int Percentage = Existing_Size * 100.0 / File_Size;
        cout << "download: " << Percentage << "%" << "\r";
        //cout<< endl;
	fflush(stdout);
    }
    cout << endl;
    
    close(File_fd);

    File_Check(File_MD5,FileName);
}


void Send_File(int Socket_fd,char* FileName)
{  
    if(FileName == NULL)
    {
        Server::getServer()->send(Socket_fd,"get:no file name!");
        return;
    }

    int File_fd = open(FileName,O_RDONLY);
    if(File_fd == -1)
    {
	Server::getServer()->send(Socket_fd,"not found!");
        return;
    }

    int File_Size = lseek(File_fd,0,SEEK_END);
    lseek(File_fd,0,SEEK_SET);
    string Judge_from_Active;
    if(Server::getServer()->recv(Socket_fd,Judge_from_Active) <= 0)
    {
        cout << "被控端退出了控制状态！" << endl;
        return;
    }
    int Existing_Size = 0;
    if(strncmp(Judge_from_Active.c_str(),"ok",2) != 0)
    {
        sscanf(Judge_from_Active.c_str(),"%d",&Existing_Size);
        lseek(File_fd,Existing_Size,SEEK_SET);
        File_Size = File_Size - Existing_Size;
    }
    
    char Check_MD5[256] = {0};
    Get_File_MD5(FileName,Check_MD5);
    char File_Information[128] = {0};
    sprintf(File_Information,"ok#%d#",File_Size);
    Check_MD5[32] = '\0';
    strcat(File_Information,Check_MD5);
    Server::getServer()->send(Socket_fd,File_Information);

    string Last_Judge_From_Active;
    if(Server::getServer()->recv(Socket_fd,Last_Judge_From_Active) <= 0)
    {
        cout << "被控端退出了控制状态！" << endl;
        return;
    }
    if(strncmp(Last_Judge_From_Active.c_str(),"ok",2) != 0)
    {
        cout << "Passive refuses to recvive!" << endl;
    }
    char File_Data[MAX_SIZE] = {0};
    File_Size += Existing_Size;
    int File_Size_Count = 0;
    while(1)
    {
	int Send_Count = 0;
	int Sign = 0;
        if(File_Size - Existing_Size < MAX_SIZE)
        {            
            File_Size_Count = read(File_fd,File_Data,File_Size- Existing_Size);
            Send_Count = send(Socket_fd,File_Data,File_Size- Existing_Size,0);
	    Existing_Size += (File_Size - Existing_Size);
	    Sign = 1;
        }
        else
        {
            
	    File_Size_Count = read(File_fd,File_Data,MAX_SIZE);
            Send_Count = send(Socket_fd,File_Data,MAX_SIZE,0);
	    Existing_Size += MAX_SIZE;
        }

	int Percentage = Existing_Size * 100.0 / File_Size;
        cout << "upload: " << Percentage << "%" << "\r";

        File_Data[0] = '\0';
	if(Sign == 1)
        {
            break;
        }
    }
    cout << endl;
    string File_MD5;
    if(Server::getServer()->recv(Socket_fd,File_MD5) <= 0)
    {
        cout << "被控端退出了控制状态！" << endl;
        return;
    }
    if(strncmp(File_MD5.c_str(),Check_MD5,32) == 0)
    {        
        cout << "文件上传完成！"<< endl;
        cout << "文件完整，没有缺失！" <<endl;
    }
    else
    {
        cout << "文件传输完成！"<< endl;
        cout << "文件已损坏，有缺失！" <<endl;
    }
    cout << endl;
    close(File_fd);
    return;
}
void Get_Tar(char FileName[],int Socket_fd)
{
    string Result;
    Server::getServer()->recv(Socket_fd,Result);
    if(strcmp(Result.c_str(),"ok") != 0)
    {
	cout << Result << endl;
	return;	
    }
    
    char Tar_FileName[128] = {0}; 
    strcpy(Tar_FileName,FileName);
    strtok(Tar_FileName,".");
    strcat(Tar_FileName,".tar.gz");
    FileName = Tar_FileName;

    Recv_File(Socket_fd,Tar_FileName);
}

void Run_File(int Socket_fd,char* FileName)
{
    Send_File(Socket_fd,FileName);


}
int main()
{
    cout << "等待链接" << endl;
    int Socket_fd = Server::getServer()->accept();
    if(Socket_fd != -1)
    {
        cout<< "accept success!"<<endl;
    }
    else
    {
        cout << "链接失败！" << endl;
        return 0;
    }
    string First_Recv;
    if(Server::getServer()->recv(Socket_fd,First_Recv) <= 0)
    {
        cout << "被控端退出了控制状态！" << endl;
        return 0;
    }
    cout<<First_Recv<<endl;
    cout << "请输入指令" <<endl;
    cout << "<ps>       	             " << "查看进程信息"<<endl;
    cout << "<kill -pid>                " << "杀死此pid进程"<<endl;
    cout << "<ls>       	             " << "获取目录文件列表"<<endl;
    cout << "<get 文件名>                " << "下载此文件"<<endl;
    cout << "<put 文件名>                " << "上传此文件"<<endl;
    cout << "<close>                     " << "退出控制"<<endl;       
    cout << "<get_tar 文件名/目录名>     " << "压缩下载文件或目录" <<endl;
    cout << "<tar 文件名>                " << "压缩文件或目录"<<endl;
    cout << "<netstat -t>                " << "查看网络链接状态" << endl;
    cout << "<rm 文件名>                 " << "删除文件" <<endl;
    cout << "<mkdir 目录名>              " << "创建目录" <<endl;
    cout << "<cd 目录名>		    " << "切换目录" <<endl;
    cout << "<run 可执行文件>		" << "在被控端执行此文件" <<endl;
    cout << "<./可执行文件>		    " << "被控端执行可执行的文件" <<endl;
    cout << "<pwd>			    " << "显示当前工作目录" <<endl;
    while(1)
    {
        string Instruct_Parameter;
        getline(cin,Instruct_Parameter);
        int Send_Size = Server::getServer()->send(Socket_fd,Instruct_Parameter);
        
        char Instruct[MAX_SIZE] = {0};
        Instruct_Parameter.copy(Instruct,Send_Size,0);
        char* excision = strtok(Instruct," ");

        char Parameter[MAX_SIZE] = {0};
        while(1)
        {
            excision = strtok(NULL," ");
            if(excision == NULL)
            {
                break;
            }
            
            strcat(Parameter,excision);
        }
       
        if(strcmp(Instruct,"get") == 0)
        {
            Recv_File(Socket_fd,Parameter);
        }
        else if(strcmp(Instruct,"put") == 0)
        {
            Send_File(Socket_fd,Parameter);
        }
        else if(strcmp(Instruct,"close") == 0)
        {
            Server::getServer()->close(Socket_fd);
            cout << "退出控制状态" << endl;
            return 0;;
        }
	else if(strcmp(Instruct,"get_tar") == 0)
	{
	    Get_Tar(Parameter,Socket_fd);
	}
        else if(strcmp(Instruct,"run") == 0)
        {
            Run_File(Socket_fd,Parameter);
            string Recv_From_Passive;
            int Recv_SIZE = Server::getServer()->recv(Socket_fd,Recv_From_Passive);
            if(Recv_SIZE <= 0 )
            {
                cout << "被控端退出了控制状态！" << endl;
                return 0;
            }
	    cout << "被控端运行结果为:" << endl;
            cout << Recv_From_Passive << endl;
        }
        else
        {
            string Recv_From_Passive;
            int Recv_SIZE = Server::getServer()->recv(Socket_fd,Recv_From_Passive);
            if(Recv_SIZE <= 0 )
            {
                cout << "被控端退出了控制状态！" << endl;
                return 0;
            }
            cout << Recv_From_Passive << endl;
        }
    }
}
