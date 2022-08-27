/*
    Problem Statement : Socket Programming II
    Author : Adarsh G Krishnan
    compiler : gcc
    compile command : gcc client.c -o client
    run command : ./client 8080
    operating system : Linux (UBUNTU 20.04)
    TimeStamp : 01/02/2022

*/

#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include<sys/types.h>

#define BUFFER 256
#define MAX_FILESIZE 256000

//Function to send message to server.
void sender(char buffer[],int client_sockfd){
    
    int n = send(client_sockfd, buffer, BUFFER,0);
    if (n < 0) {
	    printf("Socket write issue"); 
        exit(1); 
	}
}

//Function to check if command is valid or not.
int validation(char buffer[],int client_sockfd,char Request){

    //counting No. of White Spcaes.
    int count=0;
    for(int i=0;i<strlen(buffer);i++){
        if(buffer[i]==' '){
            count=count+1;
        }
    }

    //Reading field value.
    char temp[BUFFER],cmd[BUFFER];
    strcpy(cmd,buffer);
    strcpy(temp,buffer);
    char *token=strtok(temp," ");
    char field[2];

    //users & files Check.
    if(Request=='A' || Request=='B' || Request=='H'){

        if(count!=0){
            return 1;
        } 
    }
    //Upload  and Download Check
    else if(Request=='C' || Request=='D'){

        if(count!=1){
            return 1;
        } 
    }
    //Invite Check.
    else if(Request=='E'){

        if(count==3){
            token=strtok(NULL," ");
            token=strtok(NULL," ");
            token=strtok(NULL," ");
            //printf("\n%s\n",token);
            if(strncmp(token,"V",1)==0 || strncmp(token,"E",1)==0){
                return 0;
            }
            else{
                return 2;
            }

        } 
        else{
            return 1;
        }
    }
    //Read Check.
    else if(Request=='F' || Request=='G'){

        char filename[BUFFER],command[10];
        int start_idx,end_idx;
        int num=sscanf(buffer,"%s %s %d %d",command,buffer,&start_idx,&end_idx);
    
        switch(num){
            case 1:{
                return 1;
                break;
            }
            case 2:{
                if(count==1){
                    return 0;
                }
                else{
                    return 1;
                }
                break;
            }
            case 3:{
                if(count==2){
                    return 0;
                }
                else{
                    return 1;
                }
                break;
            }
            case 4:{
                if(count==3){
                    return 0;
                }
                else{
                    return 1;
                }
                break;
            }
            default:return 1;
        }
    }
    //Insert check.
    else if(Request=='I'){

        if(count==0 || count==1){
            return 1;
        }
        else{
            //printf("\nI'm Here 1\n");
            char *token=strtok(buffer," ");
            token=strtok(NULL," ");
            token=strtok(NULL," ");
            char message[BUFFER];
            bzero(message,BUFFER);
            int flag=0;
            
            if(token!=NULL){
                //printf("\nI'm Here 2\n");
                if(token[0]!='"'){
                    //printf("\nI'm Here 4\n");
                    for(int i=0;i<strlen(token)-1;i++){
                        if(token[i]>=48 || token[i]<=57){
                            if(token[i]=='-')
                                continue;
                            //printf("\nI'm Here 6\n");
                            return 1;
                        }
                    }
                    token=strtok(NULL," ");
                    //printf("\n*%s*2\n",token);
                    if(token[0]!='"'){
                        //printf("\nI'm Here 7\n");
                        return 2;
                    }
                    else{
                        //printf("\nI'm Here 8\n");
                        while(token!=NULL){
                            strcat(message,token);
                            //printf("\nI'm Here 20n");
                            if(message[strlen(message)-1]=='\"'){
                                //printf("\nI'm Here 19n");
                                flag=1;
                                break;
                            }
                            if(message[strlen(message)-2]=='\"' && message[strlen(message)-1]=='\n'){
                                //printf("\nI'm Here 19n");
                                flag=1;
                                break;
                            }
                            strcat(message," ");
                            //printf("\n*%s*5\n",message);
                            token=strtok(NULL," ");
                        }
                        
                        //printf("\n*%s*3\n",message);
                        strcat(message,"\n");
                        if(flag){
                            //printf("\nI'm Here 9\n");
                            sender(cmd,client_sockfd);
                            sender(message,client_sockfd);
                            return 3;
                        }
                        else{
                            //printf("\nI'm Here 10\n");
                            return 2;
                        }
                    }
                }
                else{
                    //printf("\n*%s*3\n",token);
                    //printf("\nI'm Here 5\n");
                    if(token[0]!='"'){
                        //printf("\nI'm Here 17\n");
                        return 2;
                    }
                    else{
                        while(token!=NULL){
                            strcat(message,token);
                            if(message[strlen(message)-1]=='\"'){
                                //printf("\nI'm Here 19n");
                                flag=1;
                                break;
                            }
                            if(message[strlen(message)-2]=='\"' && message[strlen(message)-1]=='\n'){
                                //printf("\nI'm Here 19n");
                                flag=1;
                                break;
                            }

                            strcat(message," ");
                            token=strtok(NULL," ");
                        }

                        //printf("\n*%s*4\n",message);
                        //strcat(message,"\n");
                        if(flag){
                            //printf("\nI'm Here 11\n");
                            sender(cmd,client_sockfd);
                            sender(message,client_sockfd);
                            return 3;
                        }
                        else{
                            //printf("\nI'm Here 12\n");
                            return 2;
                        }

                    }
                    
                }
            }
            else{
                //printf("\nI'm Here 3\n");
                return 2;
            }
        }
    }
    return 0;
}

//Function to copy contents of one file to another.
void fileCopy(char filename1[],char filename2[]){

    FILE *f1=fopen(filename1,"w");
    if(!f1){
        printf("\nFile not opened properly\n");
        exit(0);
    }
    FILE *f2=fopen(filename2,"r");
    if(!f2){
        printf("\nFile not opened properly\n");
        exit(0);
    }

    char *content=(char*)calloc(BUFFER,sizeof(char));
    while(fgets(content,BUFFER,f2)){
        fprintf(f1,"%s",content);
        bzero(content,BUFFER);
    }

    fclose(f1);
    fclose(f2);
    free(content);
}

//Function to send contents of file to the server.
int sendingFileContents(char filename[],int socket_fd){

    char *content=(char*)calloc(BUFFER,sizeof(char));
    filename[strlen(filename)-1]='\0';
    FILE *f=fopen(filename,"r");
    //printf("%s",filename);
    if(!f){
        printf("\nFile not opened properly\n");
        exit(0);
    }

    while(fgets(content,BUFFER,f)){
        //printf("\n%s\n",content);
        if(strlen(content)==0){
            continue;
        }
        int n = send(socket_fd, content, BUFFER,0);
        if (n < 0) {
	        printf("Socket write issue"); 
            return -1; 
	    }
        bzero(content,sizeof(content));
    }
    send(socket_fd,"over",6,0);

    fclose(f);
    return 1;
}

//Function to receive file content from client.
void receiveFile(char filename[],int socket_fd,int mode){

    //Read file data to buffer.
    char *content=(char*)calloc(BUFFER,sizeof(char));

    //Copying contents of client file to new file on server.
    FILE *f=fopen(filename,"w");
    if(!f){
        printf("\nFile not opened properly\n");
        exit(0);
    }

    while(read(socket_fd,content,BUFFER) > 0){
        if(strcmp(content,"over")==0)
            break;
        fprintf(f,"%s",content);
        if(mode==1){
            printf("\n%s",content);
        }
		bzero(content,BUFFER);
    }

    fclose(f);
    free(content);
}


//Function to handle /users Request.
int userFunction(char buffer[],char command[],int client_sockfd){

    //Command Validity Check.
    int flag=validation(buffer,client_sockfd,'A');
    if(flag){
        printf("\nPlease Enter a Valid Command\n");
        return 0;
    }

    sender(buffer,client_sockfd);

    //Read Server Output to Buffer
    char *content=(char*)calloc(BUFFER,sizeof(char));
    int ctr=0;
    while(read(client_sockfd,content,BUFFER) > 0){
        if(strcmp(content,"over")==0)
            break;
        if(ctr==0)
            printf("\nClients Currently Connected to Server : \n\n");
        printf("%s",content);
		bzero(content,BUFFER);
        ctr++;
    }
   

    free(content);

    return 0;
}

//Function to handle /files Request.
int filesFunction(char buffer[],char command[],int client_sockfd){

    //Command Validity Check.
    int flag=validation(buffer,client_sockfd,'B');
    if(flag){
        printf("\nPlease Enter a Valid Command\n");
        return 0;
    }
    //receiveFile("2.txt",client_sockfd);
    sender(buffer,client_sockfd);

    //Read Server Output to Buffer
    char *content=(char*)calloc(BUFFER,sizeof(char));
    printf("\nDetails of Current files in Server : \n\n");
    int ctr=0;
    while(read(client_sockfd,content,BUFFER) > 0){
        ctr++;
        if(strcmp(content,"over")==0){
            if(ctr<2)
                printf("\nNo Files Present at server\n");
            
            break;
        }
        char filename[BUFFER],nlines[BUFFER],owner[BUFFER],rcolab[BUFFER],ecolab[BUFFER];
        //sscanf(content,"%s\t%s\t%s\t%s\t%s",filename,nlines,owner,rcolab,ecolab);
        char *token=strtok(content,"\t");
        printf("%s\n",token);
        token=strtok(NULL,"\t");
        printf("%s\n",token);
        token=strtok(NULL,"\t");
        printf("%s\n",token);
        token=strtok(NULL,"\t");
        printf("%s\n",token);
        token=strtok(NULL,"\t");
        printf("%s\n",token);
		bzero(content,BUFFER);
    }

    free(content);

    return 0;
}

//Function to handle /upload Request.
int uploadFunction(char buffer[],char command[],int client_sockfd){

    //Command Validity Check.
    int flag=validation(buffer,client_sockfd,'C');
    if(flag){
        printf("\nPlease Enter a Valid Command\n");
        return 0;
    }


    char buf[BUFFER];
    strcpy(buf,buffer);
    char *token=strtok(buf," ");
    token=strtok(NULL," ");
    char filename[BUFFER],file[BUFFER];
    strcpy(filename,token);
    strcpy(file,filename);

    file[strlen(file)-1]='\0';
    FILE *f1=fopen(file,"r");
    if(!f1){
        printf("\nEnter a Proper File Name\n");
        return 0;
    }
    fclose(f1);

    sender(buffer,client_sockfd);

    bzero(buf,BUFFER);
    int n = read(client_sockfd,buf,BUFFER);
    if (n < 0) {
        printf("Socket read issue");
        return 0;
    }
    if(strncmp(buf,"OK",2)==0){
        sendingFileContents(filename,client_sockfd);
    }
    else{
        printf("\nFile Already Present in the Server!!");
    }

    return 0;
}

//Function to handle /upload Request.
int downloadFunction(char buffer[],char command[],int client_sockfd){

    //Command Validity Check.
    int flag=validation(buffer,client_sockfd,'D');
    if(flag){
        printf("\nPlease Enter a Valid Command\n");
        return 0;
    }

    sender(buffer,client_sockfd);

    char *token=strtok(buffer," ");
    token=strtok(NULL," ");
    char filename[BUFFER];
    strcpy(filename,token);
    
    filename[strlen(filename)-1]='\0';

    char buf[BUFFER];
    bzero(buf,BUFFER);
    int n = read(client_sockfd,buf,BUFFER);
    if (n < 0) {
        printf("Socket read issue");
        return 0;
    }
    if(strncmp(buf,"OK",2)==0){
        receiveFile("Current_Downloaded_File.txt",client_sockfd,1);
        fileCopy(filename,"Current_Downloaded_File.txt");
    }
    else if(strncmp(buf,"NOTOK",5)==0){
        printf("\nNo Permission to access the file!!\n");
    }
    else{
        printf("\nFile is not in the server!!\n");
    }

    return 0;
}

//Function to handle /invite Request.
int inviteFunction(char buffer[],char command[],int client_sockfd){

    //Command Validity Check.
    int flag=validation(buffer,client_sockfd,'E');
    if(flag){
        printf("\nPlease Enter a Valid Command\n");
        return 0;
    }

    sender(buffer,client_sockfd);

    char buf[BUFFER];
    bzero(buf,BUFFER);
    int n = read(client_sockfd,buf,BUFFER);
    if (n < 0) {
        printf("Socket read issue");
        return 0;
    }
    if(strncmp(buf,"OK",2)==0){
        return 0;
    }
    else if(strncmp(buf,"NOTOK",5)==0){
        printf("\nNo Permission to Invite Anyone!!\n");
    }
    else if(strncmp(buf,"NOC",3)==0){
        printf("\nGiven Client Doesn't Exist\n");
    }
    else if(strncmp(buf,"NOI",3)==0){
        printf("\nYou Can't Invite yourself\n");
    }
    else if(strncmp(buf,"ECO",3)==0){
        printf("\nCan't invite a edit collaborator again!!\n");
    }
    else if(strncmp(buf,"ROK",3)==0){
        printf("\nSending Upgrade invite for a read\n");
    }
    else if(strncmp(buf,"ROCKER",6)==0){
        printf("\nCan't invite a read collaborator again!!\n");
    }
    else{
        printf("\nFile is not in the server. Please upload first!!\n");
    }


    return 0;
}

//Function to handle /read Request.
int readFunction(char buffer[],char command[],int client_sockfd ){

    char buf[BUFFER];
    strcpy(buf,buffer);

    //Command Validity Check.
    int flag=validation(buffer,client_sockfd,'F');
    if(flag){
        printf("\nPlease Enter a Valid Command\n");
        return 0;
    }

    sender(buf,client_sockfd);
    
    bzero(buf,BUFFER);
    int n = read(client_sockfd,buf,BUFFER);
    if (n < 0) {
        printf("Socket read issue");
        return 0;
    }
    if(strncmp(buf,"OK",2)==0){
        //receiveFile("read.txt",client_sockfd);
    }
    else if(strncmp(buf,"NOTOK",5)==0){
        printf("\nNo Permission to access the file!!\n");
        return 0;
    }
    else{
        printf("\nFile is not in the server!!\n");
        return 0;
    }

    bzero(buf,BUFFER);
    n = read(client_sockfd,buf,BUFFER);
    if (n < 0) {
        printf("Socket read issue");
        return 0;
    }
    if(strncmp(buf,"NOTOK1",6)==0){
        printf("\nIndex given Invalid\n");
        return 0;
    }
    else if(strncmp(buf,"NOTOK2",6)==0){
        printf("\nGiven End Index Invalid\n");
        return 0;
    }
    else if(strncmp(buf,"NOTOK3",6)==0){
        printf("\nInvalid Command\n");
        return 0;
    }
    
    //Read Server Output to Buffer
    FILE *f=fopen("current_readFile.txt","w");
    if(!f){
        printf("\nFile not opened properly\n");
        return 0;
    }

    char *content=(char*)calloc(BUFFER,sizeof(char));
    printf("\nFile Contents for specified indices : \n\n");
    while(read(client_sockfd,content,BUFFER) > 0){
        if(strcmp(content,"over")==0)
            break;
        fprintf(f,"%s",content);
        printf("%s",content);
		bzero(content,BUFFER);
    }

    free(content);
    fclose(f);

    return 0;
}

//Function to handle /insert Request.
int insertFunction(char buffer[],char command[],int client_sockfd){
    
    
    char buf[BUFFER],buf1[BUFFER];
    strcpy(buf,buffer);
    strcpy(buf1,buffer);

    //Command Validity Check.
    int flag=validation(buffer,client_sockfd,'I');
    if(flag==1){
        printf("\nPlease Enter a Valid Command\n");
        return 0;
    }
    else if(flag==2){
        printf("\nPlease Enter a proper Message\n");
        return 0;
    }
    else if(flag==3){
        //printf("\nSpace encountered\n");
        //return 0;
    }

    //sender(buf,client_sockfd);

    char *token=strtok(buf1," ");
    token=strtok(NULL," ");
    char filename[BUFFER];
    strcpy(filename,token);

    bzero(buf,BUFFER);
    int n = read(client_sockfd,buf,BUFFER);
    if (n < 0) {
        printf("Socket read issue");
        return 0;
    }
    if(strncmp(buf,"OK",2)==0){
        //receiveFile("read.txt",client_sockfd);
    }
    else if(strncmp(buf,"NOTOK",5)==0){
        printf("\nNo Permission to access the file!!\n");
        return 0;
    }
    else{
        printf("\nFile is not in the server!!\n");
        return 0;
    }

    bzero(buf,BUFFER);
    n = read(client_sockfd,buf,BUFFER);
    if (n < 0) {
        printf("Socket read issue");
        return 0;
    }
    if(strncmp(buf,"NOTOK1",6)==0){
        printf("\nIndex given Invalid\n");
        return 0;
    }
    else if(strncmp(buf,"NOTOK",6)==0){
        printf("\nInvalid Command\n");
        return 0;
    }

    receiveFile("current_insertedFile.txt",client_sockfd,1);
    fileCopy(filename,"current_insertedFile.txt");

    return 0;
}

//Function to handle /delete Request.
int deleteFunction(char buffer[],char command[],int client_sockfd ){
    
    char buf[BUFFER],buf1[BUFFER];
    strcpy(buf,buffer);
    strcpy(buf1,buffer);

    //Command Validity Check.
    int flag=validation(buffer,client_sockfd,'G');
    if(flag){
        printf("\nPlease Enter a Valid Command\n");
        return 0;
    }

    
    sender(buf,client_sockfd);

    char *token=strtok(buf1," ");
    token=strtok(NULL," ");
    char filename[BUFFER];
    strcpy(filename,token);

    bzero(buf,BUFFER);
    int n = read(client_sockfd,buf,BUFFER);
    if (n < 0) {
        printf("Socket read issue");
        return 0;
    }
    if(strncmp(buf,"OK",2)==0){
        //receiveFile("read.txt",client_sockfd);
    }
    else if(strncmp(buf,"NOTOK",5)==0){
        printf("\nNo Permission to access the file!!\n");
        return 0;
    }
    else{
        printf("\nFile is not in the server!!\n");
        return 0;
    }

    bzero(buf,BUFFER);
    n = read(client_sockfd,buf,BUFFER);
    if (n < 0) {
        printf("Socket read issue");
        return 0;
    }
    if(strncmp(buf,"NOTOK1",6)==0){
        printf("\nIndex given Invalid\n");
        return 0;
    }
    else if(strncmp(buf,"NOTOK2",6)==0){
        printf("\nGiven End Index Invalid\n");
        return 0;
    }
    else if(strncmp(buf,"NOTOK3",6)==0){
        printf("\nInvalid Command\n");
        return 0;
    }


    receiveFile("current_deletedFile.txt",client_sockfd,1);
    fileCopy(filename,"current_deletedFile.txt");

    return 0;
}

//Function to handle user requests.
void clientRequestHandle(int client_sockfd){

    int n,m,flag=0;
    char buffer[BUFFER]={0};
    fd_set readfds;
    int pid;

    //Reading input.
    printf("\nENTER ANY REQUEST From Below\n\n");
    printf("\n1 : /users\n");
    printf("\n2 : /files\n");
    printf("\n3 : /upload <filename>\n");
    printf("\n4 : /download <filename>\n");
    printf("\n5 : /invite <filename> <client_id> <permission>\n");
    printf("\n6 : /read <filename> <start_idx> <end_idx>\n");
    printf("\n7 : /insert <filename> <idx> <message>\n");
    printf("\n8 : /delete <filename> <start_idx> <end_idx>\n");
    printf("\n9 : /exit\n\n");
    

    while (1){

        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO,&readfds);
        FD_SET(client_sockfd,&readfds);

        if(select(client_sockfd+1,&readfds,NULL,NULL,NULL)==-1){
            printf("\nSelect Error\n");
        }

        
        
        if(FD_ISSET(client_sockfd,&readfds)){

            //Reads message from server.
            bzero(buffer,255);
            n = read(client_sockfd,buffer,255);
            if (n < 0) {
                printf("Socket read issue"); 
                exit(1); 
            }
            if(n==0){
                printf("\nServer Exited!!");
                break; 
            }
                
            if(strlen(buffer)>5){
                printf("\n\nFrom Server :%s\n\n", buffer);
            }
            
        }
        else if(FD_ISSET(STDIN_FILENO,&readfds)){
            
            bzero(buffer,255);
            fgets(buffer,255,stdin);
        

            printf("\nEntered Command :%s\n",buffer);
            char command[12];
            int chk;
            sscanf(buffer,"%s",command);

            //sender(buffer,client_sockfd);

            if(strcmp("/users",command) == 0){
                chk=userFunction(buffer,command,client_sockfd);
                if(chk==0){
                    continue;
                }
            }
            else if(strcmp("/files",command) == 0){
                chk=filesFunction(buffer,command,client_sockfd);
                if(chk==0){
                    continue;
                }
            }
            else if(strcmp("/upload",command) == 0){
                chk=uploadFunction(buffer,command,client_sockfd);
                if(chk==0){
                    continue;
                }
            }
            else if(strcmp("/download",command) == 0){
                chk=downloadFunction(buffer,command,client_sockfd);
                if(chk==0){
                    continue;
                }
            }
            else if(strcmp("/invite",command) == 0){
                chk=inviteFunction(buffer,command,client_sockfd);
                if(chk==0){
                    continue;
                }
            }
            else if(strcmp("/read",command) == 0){
                chk=readFunction(buffer,command,client_sockfd);
                if(chk==0){
                    continue;
                }
            }
            else if(strcmp("/insert",command) == 0){
                chk=insertFunction(buffer,command,client_sockfd);
                if(chk==0){
                    continue;
                }
            }
            else if(strcmp("/delete",command) == 0){
                chk=deleteFunction(buffer,command,client_sockfd);
                if(chk==0){
                    continue;
                }
            }
            else if(strcmp("/exit",command) == 0){
                int flag=validation(buffer,client_sockfd,'H');
                if(flag){
                    printf("\nPlease Enter a Valid Command\n");
                    continue;
                }
                sender(command,client_sockfd);
                printf("\nDisconnected from Server\n");
                
                break;
            }
            else if(strcmp("YES",command)==0 || strcmp("NO",command)==0){
                sender(buffer,client_sockfd);
                continue;
            }
            else{
                printf("\n Please Enter a Valid Command \n");
                continue;
            }

        }

    }
}

/* Driver Function */
int main(int argc, char const *argv[])
{
    //Reading port from command line.
    int Port=atoi(argv[2]);

    //Checking for valid set of parameters.
    if(argc < 3){
		printf("Insufficient Parameters");
		return -1;
	}

    printf("\n\n**********WELCOME to ONLINE FILE MANAGER**********\n\n");

    /* Client Setup. */

    //Socket Creation.
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_fd<0){
        printf("Socket not created properly");
		return -1;
    }
    printf("\nClient Socket Created\n");

    struct hostent *server;
    server = gethostbyname(argv[1]);
    if(server==NULL){
        printf("No such host");
        exit(0);
    }
    struct sockaddr_in serv_addr;
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    bcopy((char *)server->h_addr, (char *) &serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(Port);

    //Connecting client to server.
    if (connect(socket_fd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
        herror("ERROR connecting");
        return -1;
    }

    //printf("\nConnected to Server\n");
    char buffer[BUFFER];
    int n;
    n = recv(socket_fd,buffer,BUFFER,0);
    if(strcmp(buffer,"LIMIT")==0){
        printf("\nMax. Client Limit Reached !! Try again Later\n");
        close(socket_fd);
	    return 0;
    }
    else{
        printf("\nConnected to Server\n");
    }

    bzero(buffer,BUFFER);


    /* Client Request Handling */
    clientRequestHandle(socket_fd);

    close(socket_fd);
	return 0;
}