#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <mysql/mysql.h>
#include <pthread.h>

void *regis(void *para){
	char buffer[100];
	char username[20];
	char password[20];
	int client_socket = *(int *)(para);
	while(1){
	memset(buffer,0,100);
	memset(username,0,20);
	memset(password,0,20);
	int len recv(client_socket,buffer,sizeof(buffer),0);
	if(len == 0){
		close(client_socket);
		pthread_exit(0);
	}
	char delims[] = "&";
	char *result = NULL;
	printf("someone input %s\n",buffer);
	result = strtok(buffer,delims);
	strcpy(username,result);
	result = strtok(NULL,delims);
	strcpy(password,result);
	//check
	MYSQL *conn_ptr;
	MYSQL_RES *g_res;
	conn_ptr = mysql_init(NULL);
	conn_ptr = mysql_real_connect(conn_ptr,"localhost","root","4075","chat",0,NULL,0);
	if(!conn_ptr)
		printf("Connection database failed\n");
	char sql[100];
	sprintf(sql,"select * from userinfo where username = '%s'",username);
	mysql_real_query(conn_ptr,sql,strlen(sql));  //check
	g_res = mysql_store_result(conn_ptr);
	int num_rows = mysql_num_rows(g_res);
	if(num_rows == 0){
	    send(client_socket,"Y",1,0);
	    printf("no same user\n");
		
	    //insert
	    sprintf(sql,"insert into userinfo values('%s','%s')",username,password);	
	}
	else{
	    send(client_socket,"N",1,0);
	    printf("user already existed!\n");
	}
	mysql_close(conn_ptr);
  }
	return NULL;
}
class regis_server{
	private:
		struct sockaddr_in regis_sockaddr;
	public:
		regis_server(struct sockaddr_in addr):regis_sockaddr(addr){}
		void startwork();
};

void regis_server::starwork(){
    int server_sockfd = socket(AF_INET,SOCK_STREAM,0);
    pthread_t thread_t[100];
    int i =0;
	
    //bind
    if(bind(server_sockfd,(struct sockaddr *)&(this->regis_sockaddr),sizeof(this->regis_sockaddr))==-1)
    {
        printf("Bind socket failed!");
        exit(1);
    }

    ///listen
    if(listen(server_sockfd,10) == -1)
    {
        perror("listen");
        exit(1);
    }
    else
    	printf("regis_listening...\n");

    //客户端套接字和地址
    int client_socket;
    struct sockaddr_in client_addr;
    socklen_t length = sizeof(client_addr);
    while(1){
    client_socket = accept(server_sockfd, (struct sockaddr*)&client_addr,&length);
    if(client_socket<0)
    {
        printf("Accept failed!");
        continue;
    }
  	pthread_create(&thread_t[(i++)%100],NULL,regis,(void*)&client_socket);

  }
}
