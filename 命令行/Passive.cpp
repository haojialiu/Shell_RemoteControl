#include"Server.h"
#include"Client.h"
#include<string.h>
#include<unistd.h>
#include<sys/wait.h>
#include<stdlib.h>
#include<fcntl.h>
#define MAX_SIZE 512

void Tar_File(char *FileName[],char *Result)
{
    if(access(FileName[1],F_OK) != 0)
    {
        Client::getClient()->Send("file was not found!");
        return;
    }
    int Conduit[2];

    char *path[128] = {0};
    char Instruct[] = "tar";
    char Parameter_1[] = "-cvzf";
    

    char Excision_Name[128] = {0}; 
    strcpy(Excision_Name,FileName[1]);
    strtok(Excision_Name,".");
    strcat(Excision_Name,".tar.gz");
    path[0] = Instruct;
    path[1] = Parameter_1;
    path[2] = Excision_Name;
    path[3] = FileName[1];
    path[4] = NULL;
    pipe(Conduit);
    pid_t pid = fork();
    if(pid == 0)
    {
        dup2(Conduit[1],2);
        execvp(path[0],path);
        exit(0);
    }
    else
    {
        close(Conduit[1]);
        wait(NULL);
        if(read(Conduit[0],Result,1000) == 0)
        {
            strcpy(Result,"tar");
            strcat(Result,"  finish\n");
        }
        close(Conduit[0]);
        wait(0);
    }
}
void Rm_File(char *FileName[],char *Result)
{
    if(access(FileName[1],F_OK) != 0)
    {
        Client::getClient()->Send("file was not found!");
        return;
    }

    char *path[128] = {0};
    char Instruct[] = "rm";

    char Excision_Name[128] = {0}; 
    strcpy(Excision_Name,FileName[1]);
    path[0] = Instruct;
    path[1] = Excision_Name;
    path[2] = NULL;
    pid_t pid = fork();
    if(pid == 0)
    {
        execvp(path[0],path);
        exit(0);
    }
    else
    {
        wait(NULL);
        strcpy(Result,"rm");
        strcat(Result," finish\n");
        wait(0);
    }
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
void Send_File(char *FileName[])
{
    if(FileName[1] == NULL)
    {
        Client::getClient()->Send("get:no file name!");
        return;
    }

    int File_fd = open(FileName[1],O_RDONLY);
    if(File_fd == -1)
    {
	    Client::getClient()->Send("not found!");
        return;
    }

    int File_Size = lseek(File_fd,0,SEEK_END);
    lseek(File_fd,0,SEEK_SET);
    string Judge_from_Active;
    if(Client::getClient()->Recv(Judge_from_Active) <= 0)
    {
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
    Get_File_MD5(FileName[1],Check_MD5);
    char File_Information[128] = {0};
    sprintf(File_Information,"ok#%d#",File_Size);
    Check_MD5[32] = '\0';
    strcat(File_Information,Check_MD5);
    Client::getClient()->Send(File_Information);

    string Last_Judge_From_Active;
    if(Client::getClient()->Recv(Last_Judge_From_Active) <= 0)
    {
        return;
    }
    if(strncmp(Last_Judge_From_Active.c_str(),"ok",2) != 0)
    {
        cout << "Active refuses to recvive!" << endl;
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
            File_Size_Count = read(File_fd,File_Data,File_Size - Existing_Size);
            Send_Count = send(Client::getClient()->getFd(),File_Data,File_Size - Existing_Size,0);
	        Existing_Size += (File_Size - Existing_Size);
	        Sign = 1;
        }
        else
        {
            
	        File_Size_Count = read(File_fd,File_Data,MAX_SIZE);
            Send_Count = send(Client::getClient()->getFd(),File_Data,MAX_SIZE,0);
	        Existing_Size += MAX_SIZE;
        }
	    int Percentage = Existing_Size * 100.0 / File_Size;
        cout << "upload: " << Percentage << "%" << "\r";

        File_Data[0] = '\0';


	if(Sign == 1)
        {
            cout << endl;
            cout << "发送完成 ！" << endl;
            break;
        }
    }
    close(File_fd);
    return;
    
}
void Recv_File(char* FileName[])
{
    int File_fd = open(FileName[1],O_WRONLY|O_CREAT,0600);
    if(File_fd == -1)
    {
	Client::getClient()->Send("error:File creation failed!");
        return;
    }

    int Existing_Size = 0;
    if(access(FileName[1],F_OK) == 0)
    {
        Existing_Size = lseek(File_fd,0,SEEK_END);
        char Temporary[128] = {0}; 
        sprintf(Temporary,"%d",Existing_Size);
        string Send_Existing_Size = Temporary;
        Client::getClient()->Send(Send_Existing_Size);
    }
    else
    {
        Client::getClient()->Send("ok");
    }
    string From_Passive_SizeMd5;
    if(Client::getClient()->Recv(From_Passive_SizeMd5) <= 0)
    {
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

    Client::getClient()->Send("ok");
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
            File_Size_Count = recv(Client::getClient()->getFd(),File_Data,File_Size-Existing_Size,0);
        }
        else
        {

            File_Size_Count = recv(Client::getClient()->getFd(),File_Data,MAX_SIZE,0);
        }
        
        if(File_Size_Count <= 0)
        {
            Client::getClient()->Close();
            return;
        }
        Existing_Size = Existing_Size + File_Size_Count;
        
        write(File_fd,File_Data,File_Size_Count);
	    File_Data[0] = '\0';
        File_Data_string = "";
        int Percentage = Existing_Size * 100.0 / File_Size;
        cout << "download: " << Percentage << "%" << "\r";
	    if(Existing_Size == File_Size)
	    {
	        cout<< endl;
	        cout << "下载完成！" << endl;
	    }        
	    fflush(stdout);
    }
    cout << endl;
    char Get_MD5[256] = {0};
    Get_File_MD5(FileName[1],Get_MD5);
    Client::getClient()->Send(Get_MD5);
    close(File_fd);

}
void Get_argv(char Instruct[],char*Instruct_Parameter[])
{
    int Parameter_Count = 0;
    char *Excision = strtok(Instruct," ");
    Instruct_Parameter[Parameter_Count++] = Excision;          //第一次获取的是命令

    while((Excision = strtok(NULL," ")) != NULL)
    {
        Instruct_Parameter[Parameter_Count++] = Excision;        //每次循环获取命令中的参数
    }
}

void Get_Tar(char *FileName[])
{
    if(access(FileName[1],F_OK) != 0)
    {
        Client::getClient()->Send("file was not found!");
        return;
    }
    else
    {
        Client::getClient()->Send("ok");
    }
    
    char Result[128] = {0};
    Tar_File(FileName,Result);
    
    char Tar_FileName[128] = {0}; 
    strcpy(Tar_FileName,FileName[1]);
    strtok(Tar_FileName,".");
    strcat(Tar_FileName,".tar.gz");
    
    FileName[1] = Tar_FileName;
    Send_File(FileName);
    Rm_File(FileName,Result);
}
void Other(char* FileName[])
{
    int Conduit[2];          //开两个描述符用来给管道的读端和写端
    pipe(Conduit);           //创建管道
    pid_t pid = fork();
    if(pid == 0)                    //复制一个进程用来执行命令
    {               
	dup2(Conduit[1],1);              //把标准错误输出改为管道的写端
        dup2(Conduit[1],2);              
        execvp(FileName[0],FileName);  
        perror("exec error");
        exit(0);
    }
    else
    {
        close(Conduit[1]);
        wait(NULL);
        char Result[MAX_SIZE] = {0};
        if(read(Conduit[0],Result+strlen(Result),1000) == 0)
        {
		strncpy(Result,FileName[0],strlen(FileName[0]));
                strcat(Result,"  finish\n");
                Client::getClient()->Send(Result);
        }
        else
        {
                Client::getClient()->Send(Result);
        }
        close(Conduit[0]);
        wait(0);
    }
}
void Run(char* FileName[])
{
    Recv_File(FileName);
    char Chmod_FileName[128] = "chmod +x ";
    strcat(Chmod_FileName,FileName[1]);
    system(Chmod_FileName);
    char Run_FileName[128] = "./";
    char *Run_Instruct[128] = {0};
    strcat(Run_FileName,FileName[1]);
    Run_Instruct[0] = Run_FileName;
    Other(Run_Instruct);
}
int main()
{
    cout << "准备连接！" << endl;
    if(-1 == Client::getClient()->Send("hello"))
    {
        cerr<<"send reason fail;errno"<<errno<<endl;
    }
    else
    {
	cout << "连接成功!" << endl;
    }
    while(1)
    {
        string Instruct_Parameter;
        int Recv_Size = Client::getClient()->Recv(Instruct_Parameter);
        if(0 == Recv_Size)
        { 
            Client::getClient()->Close();
            cout << "主控端退出了控制状态" <<endl;
            return 0;
        }
        char Instruct[MAX_SIZE] = {0};
        Instruct_Parameter.copy(Instruct,Recv_Size,0);
        Instruct[Recv_Size] = '\0';

        char *Instruct_Parameter2[10] = {0};
        Get_argv(Instruct,Instruct_Parameter2);
        
        if(strcmp(Instruct_Parameter2[0],"get") == 0)
        {
	     Send_File(Instruct_Parameter2);
        }
        else if(strcmp(Instruct_Parameter2[0],"put") == 0)
        {
            Recv_File(Instruct_Parameter2);
        }
        else if(strcmp(Instruct_Parameter2[0],"tar") == 0)
        {
            char Result[MAX_SIZE] = {0};
            Tar_File(Instruct_Parameter2,Result);
            if(strlen(Result) > 0)
            {
                Client::getClient()->Send(Result);
            }
        }
        else if(strcmp(Instruct_Parameter2[0],"rm") == 0)
        {
            char Result[MAX_SIZE] = {0};
            Rm_File(Instruct_Parameter2,Result);
            if(strlen(Result) > 0)
            {
                Client::getClient()->Send(Result);
            }
        }
	else if(strcmp(Instruct_Parameter2[0],"get_tar") == 0)
        {
            
            Get_Tar(Instruct_Parameter2);
        }
	else if(strcmp(Instruct_Parameter2[0],"cd") == 0)
	{
	    if(chdir(Instruct_Parameter2[1]) == 0)
	    {
            	char Result_cd[128] = {0};
            	strncpy(Result_cd,Instruct_Parameter2[0],strlen(Instruct_Parameter2[0]));
            	strcat(Result_cd,"  finish\n");
            	Client::getClient()->Send(Result_cd);
	    	continue;
	    }
	    else
	    {
        	Client::getClient()->Send("file was not found!");
	    }
	}
	else if(strcmp(Instruct_Parameter2[0],"run") == 0 )
	{
            Run(Instruct_Parameter2);
	}
        else if(
                strcmp(Instruct_Parameter2[0],"ls") == 0 ||
                strcmp(Instruct_Parameter2[0],"ps") == 0 ||
                strcmp(Instruct_Parameter2[0],"kill") == 0 || 
                strcmp(Instruct_Parameter2[0],"mkdir") ==0 ||
                strcmp(Instruct_Parameter2[0],"pwd") == 0 ||
                strcmp(Instruct_Parameter2[0],"netstat") == 0 ||
                strncmp(Instruct_Parameter2[0],"./",2) == 0 
               )
        {
            Other(Instruct_Parameter2);
        }
        else if(strcmp(Instruct_Parameter2[0],"close") == 0)
        {
            Client::getClient()->Close();
            cout << "退出控制状态" << endl;
            return 0;
        }
        else
        {
            Client::getClient()->Send("指令错误！");
        }
    }
}
