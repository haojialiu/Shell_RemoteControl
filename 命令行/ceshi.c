#include<iostream>
//#include<string.h> 　
//#include<fcntl.h>
#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>
using namespace std;
/*int main()
{
        string sentence = "hello world";
        char p[40];
        sentence.copy(p,sentence.size()+1,0);
        p[sentence.size()] = '\0';
        int i=0;
        while(p[i] != 0)
        {
            cout<<p[i++];
        }
        cout<<endl;
        strtok(p," ");
        char *tokenPtr = strtok(NULL, " ");
            cout << tokenPtr << '\n';
            cout <<"/"<<sentence<<endl;
        if(strcmp(p,"hello") == 0)
        {
            cout <<"xiangtong"<<endl;
        }
        int ii = 0;
        while(p[ii] != 0)
        {
            cout<<p[ii++];
        }
        cout << endl;
        return 0;
}*/

/*string Get_argv(string Instruct)
{
    string parameter;
    char excision[128];
    Instruct.copy(excision,Instruct.size()+1,0);
    excision[Instruct.size()] = '\0';

    strtok(excision," ");
    parameter = strtok(NULL," ");
    cout << parameter <<endl;
    return parameter;
}
int main()
{
    string parameter;
    string Instruct = "hello world";
    parameter = Get_argv(Instruct);
    cout << parameter <<endl;
    cout << Instruct <<endl;
}*/

/*int main()
{
    string str;
    getline(cin,str);
    cout<<"str:" << str <<endl;
    char excision[256] = {0};
    str.copy(excision,);

   // char str[256] = "abc aaa bbb ccc";
   // string n1 = strtok(str," ");
   // string n2 = strtok(NULL," ");
    //string str = "abc aaa bbb ccc ddd";
    //string n1 = str.find(" ");
    //cout << str <<endl;
    //cout << n1 <<endl;
    
    cout << str << endl;
    cout << n1 << endl;
    cout << n2 << endl;
}*/
/*
int main()
{
        string Instruct;
        //fflush(stdout);
        getline(cin,Instruct);
        cout << Instruct <<endl;
        
        char excision[256] = {0};
        Instruct.copy(excision,15,0);
        //cout << "---------------" <<endl;
        excision[15] = '\0';
        //cout << excision <<endl;
        char* parameter = strtok(excision," ");
        //cout << "===============" <<endl;
        //string parameter = strtok(NULL," ");
        //cout << "---------------" <<endl;
        char str[50] = {0};
         
        while(1)
        {
            parameter = strtok(NULL," ");
            if(parameter == NULL)
            {
                break;
            }
            cout << "parameter:" << parameter<<endl;
            
            strcat(str,parameter);
            strcat(str," ");
            cout << "str:" <<str<<endl;
        }
        //while(parameter.c_str() != NULL)
        //{
        //    cout << "parameter:" << parameter<<endl;
            
        //    strcat(str,parameter.c_str());
        //    strcat(str," ");
        //    cout << "str:" <<str<<endl;
        //    parameter = strtok(NULL," ");
        //}
        cout << "over"<<endl;
}*/


/*int main()
{
    string  str = NULL;
    if(str.c_str() == NULL)
    {
        cout << "ok" <<endl;
    }
}*/


/*int main()
{
    sleep(100);
}*/

/*int main()
{
    char* str[2];
    char arr[20] = "12345";
    char brr[20] = "67890";
    str[0] = arr;
    str[1] = brr; 
    cout << str[0] <<endl;
    cout << str[1] <<endl;
}*/



/*int main()
{
    string str = "abcdef";
    cout << str.c_str()+3<<endl;
}*/


/*int main()
{
    string str = "abcd";
    char arr[100] = {0};
    strcpy(arr,str.c_str());
    cout << arr <<endl;
}*/
/*
int main()
{
    char str1[10] = "abcd";
    str1[0] = '\0';
    cout << str1 <<endl;
}*/


/*int main()
{
    //char Excision_Name[3] = {0};
    //char *FileName[3] = {0};
    //char str[] = "01";
    //char str2[] = "abc.pdf";
    //FileName[0] = str;
    //FileName[1] = str2;
    //strcpy(Excision_Name,FileName[1]);
    //strtok(Excision_Name,".");
    //strcat(Excision_Name,".tar.gz");
    //cout << Excision_Name << endl;
    //cout << FileName[1] << endl;
    char *FileName[3] = {0};
    char str[3] = "01";
    char str2[] = "abc.pdf";
    FileName[0] = str;
    FileName[1] = str2;
    char *path[128] = {0};
    char Instruct[] = "tar";
    cout << "INstruct:" <<Instruct<<endl;
    path[0] = Instruct;
    cout << "0:" << path[0] << endl;
    char Parameter_1[] = "-cvzf";
    path[1] = Parameter_1;
    cout << "1:" << path[1] << endl;
cout << 0 <<endl;
    fflush(stdout);
    char Excision_Name[128] = {0}; 
    strcpy(Excision_Name,FileName[1]);
    strtok(Excision_Name,".");
    //char Tar_FileName[128] = {0};
    //strcpy(Tar_FileName,Excision_Name);
    strcat(Excision_Name,".tar.gz");
    path[2] = Excision_Name;
    path[3] = FileName[1];
    cout << "0:" << path[0] << endl;
    cout << "1:" << path[1] << endl;
    cout << "2:" << path[2] << endl;
    cout << "3:" << path[3] << endl;
}*/
/*int main()
{
    sleep(300);
}*/

/*int main()
{
	//char buf[1024] = {"/home/liujiahao/桌面/ssss/Shell_RemoteControl/Act"};
	//chdir("/home/liujiahao/桌面/ssss/Shell_RemoteControl/Act");
	//char *arr[10] = {0};
	char arr[10] = "Act";
	chdir(arr);
	//cout << getcwd(NULL,255) <<endl;
	//system("cd /");
}*/
#include<string.h>
int main()
{
	char str[128] = "ls";
	strcat(str," -a");
	system(str);
	cout << "12345678790" <<endl; 
}
