#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <string>
#include <map>
#include <iostream>

using namespace std;

class chat_server {
	private:
	 static void *transmit(void *para);
	 map<string,int> chat_map;
	 struct sockaddr_in chat_sockaddr;
    public:
     chat_server(struct sockaddr_in addr):chat_sockaddr(addr){} //构造函数
     void startwork();
     
};

struct parameter{
	int sock;
	chat_server *ptr;
	parameter(int sock, chat_server *ptr):sock(sock), ptr(ptr){}
};

void chat_server::startwork(){
  int server_sockfd = socket(AF_INET,SOCK_STREAM, 0);
  pthread_t thread_t[100];
  int i=0; 
        //bind，成功返回0，出错返回-1
   
    if(bind(server_sockfd,(struct sockaddr *)&(this->chat_sockaddr),sizeof(this->chat_sockaddr))==-1)
    {
        printf("Bind socket failed!");
        exit(1);
    }

    //listen，成功返回0，出错返回-1
    if(listen(server_sockfd,10) == -1)
    {
        perror("listen");
        exit(1);
    }
    else
    	printf("chat_server_listening...\n");
    //客户端套接字和地址
    int client_socket;
    struct sockaddr_in client_addr;
    socklen_t length = sizeof(client_addr);
    while(1){
    client_socket = accept(server_sockfd, (struct sockaddr*)&client_addr,&length);
     //send(client_socket,"hello",5,0);
    if(client_socket<0)
    {
        printf("Accept failed!");
        continue;
    }
    parameter client_para(client_socket, this);
  	pthread_create(&thread_t[(i++)%100],NULL,transmit,(void*)&client_para);

  }
}
void * chat_server::transmit(void *para){
  char buffer[100];
  char username[20];
  char destuser[20];
  char message[1000];
  string srcuser;
  char delims[] =">";
  char *result = NULL;
  int client_socket = ((parameter *)para)->sock;
  map<string,int>::iterator iter;
  chat_server *ptr = ((parameter *)para)->ptr;
  
  //将客户client_socket存入chat_map
  memset(buffer,0,100);
  int len = recv(client_socket,buffer,sizeof(buffer),0);
  
  strcpy(username,buffer);
  srcuser = username;
  ptr->chat_map[srcuser]=client_socket;
  printf("放入map:%s %d\n",username,client_socket);
  while(1){
  memset(buffer,0,100);
  memset(message,0,100);
  len = recv(client_socket,buffer,sizeof(buffer),0);
  //send(client_socket,buffer,sizeof(buffer),0);
  printf("%s\n",buffer);
  if(len == 0){
     ptr->chat_map.erase(srcuser);
     printf("移出map:%s %d\n",username,client_socket);
  	 close(client_socket);
     pthread_exit(0); 
     }
   result = strtok(buffer,delims);
   strcpy(destuser,result);
   result = strtok(NULL, delims);
   strcpy(message,result);
   
   string mes = message;
   string finalstring; 
   if(strcmp(destuser,"ALL")==0){
     finalstring ="#" + srcuser + ">" + mes;
     finalstring.copy(message,sizeof(message),0);
     for(iter = ptr->chat_map.begin();iter != ptr->chat_map.end(); iter++){
        if(iter->first != srcuser){
      send(iter->second,message,strlen(message),0);
        printf("往套接字%d 发送消息:%s\n",iter->second,message);
        }
      }
   }
   else{
    finalstring = srcuser + ">" + mes;
    finalstring.copy(message,sizeof(message),0);
    send(ptr->chat_map.find(destuser)->second,message,strlen(message),0);
    printf("给%s往套接字%d 发送消息\n",destuser,ptr->chat_map.find(destuser)->second);
   }
   

 }
}









