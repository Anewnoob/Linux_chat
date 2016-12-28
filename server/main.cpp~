#include "login_server.h"
#include "chat_server.h"
#include "regis_sever.h"
#include <map>
#include <string>
#include <iostream>
using namespace std;

 void *login (void *);
 void *connect(void *);
 void *chat(void *);
 void *regis (void *);
 struct sockaddr_in login_server_sockaddr;
 struct sockaddr_in boad_connect_sockaddr;
 struct sockaddr_in chat_server_sockaddr;
 struct sockaddr_in regis_server_sockaddr;
 map<string,int> online_user_map;
 map<string,int>::iterator iter;
 
int main(){
	pthread_t connect_pthread_t[100];
	int boad_listen_socket = socket(AF_INET,SOCK_STREAM, 0);
    
    pthread_t login_pthread,chat_pthread,regis_pthread;
    
    login_server_sockaddr.sin_family = AF_INET;
    login_server_sockaddr.sin_port = htons(13000);
    login_server_sockaddr.sin_addr.s_addr =htonl(INADDR_ANY);
    boad_connect_sockaddr.sin_family = AF_INET;
    boad_connect_sockaddr.sin_port = htons(13002);
    boad_connect_sockaddr.sin_addr.s_addr =htonl(INADDR_ANY);
    chat_server_sockaddr.sin_family = AF_INET;
    chat_server_sockaddr.sin_port = htons(13003);
    chat_server_sockaddr.sin_addr.s_addr =htonl(INADDR_ANY);
    regis_server_sockaddr.sin_family = AF_INET;
    regis_server_sockaddr.sin_port = htons(13004);
    regis_server_sockaddr.sin_addr.s_addr =htonl(INADDR_ANY);
   
    pthread_create(&regis_pthread,NULL,regis,NULL);
    pthread_create(&login_pthread,NULL,login,NULL);
    pthread_create(&chat_pthread,NULL,chat,NULL);

    //建立面板连接监听
    if(bind(boad_listen_socket,(struct sockaddr *)&(boad_connect_sockaddr),sizeof(boad_connect_sockaddr))==-1)
    {
        printf("Bind socket failed!\n");
        exit(1);
    }
    if(listen(boad_listen_socket,10) == -1)
    {
        printf("boad_connect_listen failed!\n");
        exit(1);
    }
    else
       printf("boad_connect_listening...\n");
       
    //接受连接请求
    int client_socket;   
    struct sockaddr_in client_addr;
    socklen_t length = sizeof(client_addr);   
    int i =0;
    while(1){
    client_socket = accept(boad_listen_socket,(struct sockaddr*)&client_addr,&length);
    if(client_socket<0)
    {
        printf("Accept failed!");
        continue;
    }
    printf("有面板连接\n");
  	pthread_create(&connect_pthread_t[(i++)%100],NULL,connect,(void*)&client_socket);

  }
    pthread_join(regis_pthread,NULL);
    pthread_join(login_pthread,NULL);
}

void *connect(void *para){
	char buffer[100];
	char username[15];
	char temp[20];
	char tempp[20];
 	char add[20];
 	char sub[20];
 	add[0] = '+';
 	sub[0] = '-';
   	int client_socket = *(int *)(para);
   	while(1){
	memset(buffer,0,100);
	memset(username,0,15);
	int len = recv(client_socket,buffer,sizeof(buffer),0);
	if(buffer[0]=='+'){
	    strcpy(username,buffer+1);
	    strcpy(add+1,username);
	    
	   
	    //告诉你在线用户的名单
	    for(iter=online_user_map.begin(); iter != online_user_map.end(); iter++){
		   iter->first.copy(temp,iter->first.size(),0);
		   strcpy(add+1,temp);
	       send(client_socket,add,sizeof(add),0);
	       //cout<<add<<endl;
	    }
	    iter = online_user_map.find(username);
	     if(iter != online_user_map.end()){
	       online_user_map[string(username)] = client_socket;
	       printf("用户%s被顶掉了!\n",username);
	       continue;
	    }
	    //告诉所有人你上线了
	    for(iter=online_user_map.begin(); iter != online_user_map.end(); iter++){
		   strcpy(add+1,username);
	       send(iter->second,add,sizeof(add),0);
	       //cout<<add<<endl;
	    }
	    //把你放入map
		online_user_map[string(username)] = client_socket;
		printf("%s\n",username);
	}
	if(buffer[0]=='-'){
          strcpy(username,buffer+1);
		  strcpy(sub+1,username);
		  online_user_map.erase(username);
		   for(iter=online_user_map.begin(); iter != online_user_map.end(); iter++){
	       	send(iter->second,sub,sizeof(sub),0);
	        //cout<<sub<<endl;
	    }
	   printf("%s 已经断开连接!\n",username);
	   close(client_socket); 
	   break;
	}	
  }
 pthread_exit(0); 
}

void *chat(void *){
  chat_server demo(chat_server_sockaddr);
  demo.startwork();
}
void *login(void *){
  Login_server demo(login_server_sockaddr);
  demo.startwork();
}
void *regis(void *){
  regis_server demo(char_server_sockaddr);
  demo.starwork();
}

