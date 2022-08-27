/*
    Problem Statement : Socket Programming III
    Author : Adarsh G Krishnan
    compiler : gcc
    compile command : gcc server.c -o server
    run command : ./server 8080
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
#include <arpa/inet.h>
#include <sys/time.h>
#include <errno.h>
#include <sys/shm.h>

#define BUFFER 256
#define MAXIMUM_CLIENT 5

struct FILEDATA{
    char filename[BUFFER];
    int noOfLines;
    int owner_id;
    int readCollaborators[5];
    int editCollaborators[5];
};

typedef struct FILEDATA FILEDATA;

struct userInfo{
    int client_id;
    int socket_id;
};

typedef struct userInfo userInfo;

struct Invite{
    int source;
    int dest;
};

typedef struct Invite Invite;

//Function to send message to server.
void sender(char buffer[],int client_sockfd){
    
    int n = send(client_sockfd, buffer, BUFFER,0);
    if (n < 0) {
	    printf("Socket write issue"); 
        exit(1); 
	}
}

//Function to write to a file.
void writeToFile(char filename[],char buffer[],int mode){

    FILE *f;
    if(mode==1){
        f=fopen(filename,"w");
        if(!f){
            printf("\nFile not opened properly\n");
            exit(0);
        }
        fprintf(f,"%s",buffer);
    }
    else{
        f=fopen(filename,"a");
        if(!f){
            printf("\nFile not opened properly\n");
            exit(0);
        }
        fprintf(f,"%s",buffer);
    }
    fclose(f);
}

//Function to remove data of a client when exited.
void removeData(int clientSockfd,FILEDATA *filedata,userInfo *user){

    for(int i=0;i<100;i++){
        if(filedata[i].owner_id==clientSockfd){
            remove(filedata[i].filename);
            strcpy(filedata[i].filename,"");
            filedata[i].noOfLines=0;
            filedata[i].owner_id=0;
            for(int j=0;j<MAXIMUM_CLIENT;j++){
                filedata[i].editCollaborators[j]=0;
                filedata[i].readCollaborators[j]=0;
            }
            break;
        }
        else{
            for(int j=0;j<MAXIMUM_CLIENT;j++){
                if(filedata[i].editCollaborators[j]==clientSockfd){
                    filedata[i].editCollaborators[j]=0;
                }
                if(filedata[i].readCollaborators[j]==clientSockfd){
                    filedata[i].readCollaborators[j]=0;
                }
            }
        }
    }
    for(int i=0;i<MAXIMUM_CLIENT;i++){
        if(user[i].socket_id==clientSockfd){
            user[i].client_id=0;
            user[i].socket_id=0;
            break;
        }
    }

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

//Function to find total number of lines in the file.
int noOfLines(char filename[]){

    FILE *f=fopen(filename,"r");
    if(!f){
        printf("File not opened properly");
        exit(0);
    }

    //Finding no. of lines in the file.
    int nlines=0;
    char line[BUFFER];
    while (fgets(line,BUFFER,f)){
        nlines++;
    }
    fclose(f);
    return nlines;
    
}

//Function to send contents of file to the server.
int sendingFileContents(char filename[],int socket_fd){

    char *content=(char*)calloc(BUFFER,sizeof(char));
    FILE *f=fopen(filename,"r");
    //printf("%s",filename);
    if(!f){
        printf("\nFile not opened properly\n");
        exit(0);
    }

    while(fgets(content,BUFFER,f)){
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

//Function to store file details when it is uploaded.
void addFileDetailsU(FILEDATA *filedata,char filename[],int owner_id){

    for(int i=0;i<100;i++){
        if(filedata[i].owner_id==0){
            strcpy(filedata[i].filename,filename);
            filedata[i].noOfLines=noOfLines(filename);
            filedata[i].owner_id=owner_id;
            for(int j=0;j<MAXIMUM_CLIENT;j++){
                filedata[i].editCollaborators[j]=0;
                filedata[i].readCollaborators[j]=0;
            }
            break;
        }
    }
    
}

//Function to update collaborator details.
void addFileDetailsI(FILEDATA *filedata,char filename[],int owner_id,int colab_id,char permission[]){

    for(int i=0;i<100;i++){
        if(filedata[i].owner_id==owner_id && strcmp(filedata[i].filename,filename)==0){
            for(int j=0;j<MAXIMUM_CLIENT;j++){
                if(strncmp(permission,"V",1)==0){
                    if(filedata[i].readCollaborators[j]==0){
                        filedata[i].readCollaborators[j]=colab_id;
                        return;
                    }
                }
                else if(strncmp(permission,"E",1)==0){
                    if(filedata[i].editCollaborators[j]==0){
                        filedata[i].editCollaborators[j]=colab_id;
                        return;
                    }
                }
            }
        }
    }
    
}

//Function to receive file content from client.
void receiveFile(char filename[],int socket_fd){

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
		bzero(content,BUFFER);
    }

    fclose(f);
    free(content);
}

//Function to handle /users Request.
int userFunction(char buffer[],char command[],int client_sockfd,userInfo *user){

    char *content=(char*)calloc(BUFFER,sizeof(char));

    FILE *f=fopen("client_records.txt","rb+");
    if(!f){
        printf("\nFile not opened properly\n");
        exit(0);
    }


    char client_id[10];
    while(fgets(content,BUFFER,f)){
        int ctr = sscanf(content,"%s",client_id);
        if(ctr!=1 || strlen(content)==0){
            continue;
        }
        int n = send(client_sockfd, content, BUFFER,0);
        if (n < 0) {
	        printf("Socket write issue"); 
            return -1; 
	    }
        bzero(content,sizeof(content));
    }
    send(client_sockfd,"over",6,0);

    fclose(f);
    free(content);

    return 0;
}

///Function to handle /files Request.
int filesFunction(char buffer[],char command[],int client_sockfd,FILEDATA *filedata){

    FILE *f=fopen("fileDetails.txt","w");
    if(!f){
        printf("\nFile not opened properly\n");
        exit(0);
    }

    for(int i=0;i<100;i++){
        if(filedata[i].owner_id!=0){
            fprintf(f,"FILENAME:%s\tNO.Of.LINES:%d\tOWNER_ID:%d\tREADCOLLABORATORS:",filedata[i].filename,filedata[i].noOfLines,filedata[i].owner_id);
            for(int j=0;j<MAXIMUM_CLIENT;j++){
                if(filedata[i].readCollaborators[j]!=0){
                    //printf("\nI'm Here #\n");
                    fprintf(f,"%d ",filedata[i].readCollaborators[j]);
                }
            }
            fprintf(f,"\tEDITCOLLABORATORS:");
            for(int j=0;j<MAXIMUM_CLIENT;j++){
                if(filedata[i].editCollaborators[j]!=0){
                    fprintf(f,"%d ",filedata[i].editCollaborators[j]);
                }
            }
            //fprintf(f,"\tEdit Collaborators : ");
        }
    }
    fclose(f);
    sendingFileContents("fileDetails.txt",client_sockfd);
    

    return 0;
}

//Function to Check Owner or Collaborator.
int checker(int client_sockfd,FILEDATA *filedata,char filename[],char permission){

    for(int i=0;i<100;i++){
        if(strcmp(filedata[i].filename,filename)==0){
            if(filedata[i].owner_id==client_sockfd){
                return 1;
            }
            else if(permission=='V'){
                for(int j=0;j<MAXIMUM_CLIENT;j++){
                    if(filedata[i].readCollaborators[j]==client_sockfd){
                        return 2;
                    }
                }
            }
            else if(permission=='E'){
                for(int j=0;j<MAXIMUM_CLIENT;j++){
                    if(filedata[i].editCollaborators[j]==client_sockfd){
                        return 3;
                    }
                }
            }
        }
    }

    return 0;
}

//Function to check if a file exists or not.
int exist(FILEDATA *filedata,char filename[]){

    for(int i=0;i<100;i++){
        if(strcmp(filedata[i].filename,filename)==0){
            return 1;
        }
    }

    return 0;
}

//Function to handle /upload Request.
int uploadFunction(char buffer[],char command[],int client_sockfd,FILEDATA *fileDetails){

    char *token=strtok(buffer," ");
    token=strtok(NULL," ");
    char filename[BUFFER];
    strcpy(filename,token);
    filename[strlen(filename)-1]='\0';

    int check=exist(fileDetails,filename);
    if(check){
        sender("NOTOK",client_sockfd);
        return 1;
    }
    else{
        sender("OK",client_sockfd);
        receiveFile("tempUpload.txt",client_sockfd);
        fileCopy(filename,"tempUpload.txt");
        addFileDetailsU(fileDetails,filename,client_sockfd);
    }

    return 0;
}

//Function to handle /download Request.
int downloadFunction(char buffer[],char command[],int client_sockfd,FILEDATA *fileDetails){

    char *token=strtok(buffer," ");
    token=strtok(NULL," ");
    char filename[BUFFER];
    strcpy(filename,token);
    
    filename[strlen(filename)-1]='\0';

    
    int check1=exist(fileDetails,filename);
    if(check1){
        int check2=checker(client_sockfd,fileDetails,filename,'A');
        int check3=checker(client_sockfd,fileDetails,filename,'V');
        int check4=checker(client_sockfd,fileDetails,filename,'E');
        if(check2 || check3 || check4){
            sender("OK",client_sockfd);
            sendingFileContents(filename,client_sockfd);
        }
        else{
            sender("NOTOK",client_sockfd);
            return 1;
        }
    }
    else{
        sender("NOEXIST",client_sockfd);
        return 2;
    }
    
    
    return 0;
}

//Function to handle YES or NO.
int yesNoFunction(int client_id,int client_sockfd,char buffer[],char permission[]){

    if(client_id==0 || client_sockfd==0){
        sender("No Pending Invites",client_id);
        return 0;
    }
    if(strncmp("YES",buffer,3)==0){
        if(strncmp(permission,"V",1)==0){
            sender("Successfully accepted the invite.Now u are the Read Collaborator.",client_id);
        }
        else if(strncmp(permission,"E",1)==0){
            sender("Successfully accepted the invite.Now u are the Edit Collaborator.",client_id);
        }
        sender("Client Accepted your Invite",client_sockfd);
        return 1;
    }
    else if(strncmp("NO",buffer,2)==0){
        sender("Invite Rejected.",client_id);
        sender("Client Rejected your Invite",client_sockfd);
        return 2;
    }
    
}

//Function to handle /invite Request.
int inviteFunction(char buffer[],char command[],int client_sockfd,int clientSoc[],FILEDATA *filedata,Invite *invite){

    //printf("\n%s*",buffer);

    char *token=strtok(buffer," ");
    token=strtok(NULL," ");
    char filename[BUFFER],file[BUFFER];
    strcpy(filename,token);
    strcpy(file,token);
    

    //Reading index.
    token=strtok(NULL," ");
    int client_id=atoi(token);
    client_id=client_id%10000;

    //Reading Permission.
    token=strtok(NULL," ");
    char permission[2];
    strcpy(permission,token);
    
    //printf("\n%s*",permission);

    int ctr=0;
    for(int i=0;i<MAXIMUM_CLIENT;i++){
        if(clientSoc[i]==client_id){
            ctr++;
        }
    }
    if(ctr!=1){
        bzero(buffer,BUFFER);
        sender("NOC",client_sockfd);
        return 0;
    }
    if(client_id==client_sockfd){
        bzero(buffer,BUFFER);
        sender("NOI",client_sockfd);
        return 0;
    }
    

    int check1=exist(filedata,filename);
    if(check1){
        int check2=checker(client_sockfd,filedata,file,'A');
        if(check2){
            int check3=checker(client_id,filedata,file,'V');
            int check4=checker(client_id,filedata,file,'E');

            if(check4){
                sender("ECO",client_sockfd);
                return 0;
            }
            else{
                if(check3 && strncmp(permission,"E",1)==0){
                    //printf("\nTTer\n");
                   sender("ROK",client_sockfd); 
                }
                if(check3 && strncmp(permission,"V",1)==0){
                    //printf("\nTTer2\n");
                   sender("ROCKER",client_sockfd); 
                   return 0;
                }
                else{
                    sender("OK",client_sockfd);
                }

            }
        }
        else{
            sender("NOTOK",client_sockfd);
            return 0;
        }
    }
    else{
        sender("NOEXIST",client_sockfd);
        return 0;
    }

    char buf[BUFFER];
    if(strncmp(permission,"V",1)==0){
        bzero(buf,BUFFER);
        sprintf(buf,"%s%d%s","You have got an invite to read file owned by ",client_sockfd+10000," ACCEPT? YES or NO");
        sender(buf,client_id);
        //return 0;
    }
    if(strncmp(permission,"E",1)==0){
        bzero(buffer,BUFFER);
        sprintf(buf,"%s%d%s","You have got an invite to edit file owned by ",client_sockfd+10000," ACCEPT? YES or NO");
        sender(buf,client_id);
        //return 0;
    }
    sender("Invite sent Successfully.",client_sockfd);

    invite->source=client_sockfd;
    invite->dest=client_id;

    printf("\nInvite Pocess [source %d: Destination %d]\n",invite->source,invite->dest);


    //Read command from client to buffer.
    bzero(buf,BUFFER);
    int n = read(client_id,buf,sizeof(buf));
    if(n==0 || strncmp(buf,"/exit",5)==0){
        return client_id;
    }
    if (n < 0) {
        printf("Socket read issue");
        exit(0);
    }
    //printf("\n\nFrom client :%s*\n", buf);

    int flag=yesNoFunction(invite->dest,invite->source,buf,permission);
    if(flag==1){
        addFileDetailsI(filedata,file,client_sockfd,client_id,permission);
    }

    return 0;
}

//Utility function for /read request.
void utilityRead(char filename[],int start,int end,int client_sockfd){

    int nlines=noOfLines(filename);
    FILE *f=fopen(filename,"r");
    if(!f){
        printf("File not opened properly");
        exit(0);
    }

    if(start<0){
        //printf("\nHere 23\n");
        start+=nlines;
    }
    if(end<0){
        //printf("\nHere 24\n");
        end+=nlines;
    }

    char *content=(char*)calloc(BUFFER,sizeof(char));
    for(int i=0;i<nlines;i++){
        fgets(content,BUFFER,f);
        if(i>=start && i<=end){
            //printf("\nHere 21\n");
            if(i==end){
                content[strlen(content)-1]='\0';
            }
            sender(content,client_sockfd);
        }
        if(i==end){
            send(client_sockfd,"over",6,0);
            break;
        }
    }

}

//Function to handle /read Request.
int readFunction(char buffer[],char command[],int client_sockfd,FILEDATA *fileDetails){

    char filename[BUFFER],cmd[10];
    int start_idx,end_idx;
    char buf[BUFFER];
    strcpy(buf,buffer);
    int num=sscanf(buffer,"%s %s %d %d",cmd,filename,&start_idx,&end_idx);
    
    //printf("\n%s*\n",buf);

    int check1=exist(fileDetails,filename);
    if(check1){
        //printf("\nHere 1\n");
        int check2=checker(client_sockfd,fileDetails,filename,'A');
        int check3=checker(client_sockfd,fileDetails,filename,'V');
        int check4=checker(client_sockfd,fileDetails,filename,'E');
        if(check2 || check3 || check4){
            //printf("\nHere 2\n");
            sender("OK",client_sockfd);
        }
        else{
            //printf("\nHere 3\n");
            sender("NOTOK",client_sockfd);
            return 0;
        }
    }
    else{
        //printf("\nHere 4\n");
        sender("NOEXIST",client_sockfd);
        return 0;
    }

    int nlines=noOfLines(filename);
    //printf("\n%d\n",nlines);

    switch(num){
        case 2:{
            //printf("\nHere 5\n");
            sender("OK1",client_sockfd);
            utilityRead(filename,0,nlines-1,client_sockfd);
            sprintf(buffer,"%s","File Contents for specified indices Sent!");
            sender(buffer,client_sockfd);
            break;
        }
        case 3:{
            //printf("\nHere 6\n");
            char *token=strtok(buf," ");
            token=strtok(NULL," ");
            token=strtok(NULL," ");
            if(token==NULL){
                //printf("\nHere 7\n");
                sender("NOTOK1",client_sockfd);
                break;
            }
            else{
                //printf("\nHere 8\n");
                token[strlen(token)-1]='\0';
                for(int i=1;token[i]!='\0';i++){
                    if(token[i]<48 || token[i]>57){
                        //printf("\nHere 9\n");
                        sender("NOTOK1",client_sockfd);
                        break;
                    }
                }
                if((start_idx>=0 && start_idx>=nlines) || (start_idx<0 && start_idx<(-1*nlines))){
                    //printf("\nHere 10\n");
                    sender("NOTOK1",client_sockfd);
                    break;
                }
                else{
                    //printf("\nHere 11\n");
                    sender("OK2",client_sockfd);
                    utilityRead(filename,start_idx,start_idx,client_sockfd);
                    sprintf(buffer,"%s","File Contents for specified indices Sent!");
                    sender(buffer,client_sockfd);
                    break;
                }
            }
            break;
        }
        case 4:{
            //printf("\nHere 12\n");
            char *token=strtok(buf," ");
            token=strtok(NULL," ");
            token=strtok(NULL," ");
            token=strtok(NULL," ");
            //printf("\n%s\n",token);
            if(token==NULL){
                //printf("\nHere 13\n");
                sender("NOTOK2",client_sockfd);
                break;
            }
            else{
                //printf("\nHere 14\n");
                token[strlen(token)-1]='\0';
                for(int i=1;token[i]!='\0';i++){
                    if(token[i]<48 || token[i]>57){
                        sender("NOTOK2",client_sockfd);
                        break;
                    }
                }
                if((start_idx>=0 && start_idx>=nlines) || (start_idx<0 && start_idx<(-1*nlines))){
                    //printf("\nHere 15\n");
                    sender("NOTOK2",client_sockfd);
                    break;
                }

                if((end_idx>=0 && end_idx>=nlines) || (end_idx<0 && end_idx<(-1*nlines))){
                    //printf("\nHere 16\n");
                    sender("NOTOK2",client_sockfd);
                    break;
                }
                if(start_idx>end_idx){
                    if(start_idx>=0 && end_idx<0){
                        //printf("\nHere 17\n");
                        sender("OK2",client_sockfd);
                        utilityRead(filename,start_idx,end_idx,client_sockfd);
                        sprintf(buffer,"%s","File Contents for specified indices Sent!");
                        sender(buffer,client_sockfd);
                    }
                    else{
                        //printf("\nHere 17\n");
                        sender("NOTOK2",client_sockfd);
                    }
                    
                    break;
                }
                else{
                    //printf("\nHere 18\n");
                    sender("OK3",client_sockfd);
                    utilityRead(filename,start_idx,end_idx,client_sockfd);
                    sprintf(buffer,"%s","File Contents for specified indices Sent!");
                    sender(buffer,client_sockfd);
                    break;
                }
            }
            break;
        }
        default:{
            //printf("\nHere 19\n");
            sender("NOTOK3",client_sockfd);
            break;
        }

    }

    return 0;
}

//Update no. of lines after insert and delete.
void lineUpdate(FILEDATA *filedata,int client_sockfd){

    for(int i=0;i<100;i++){
        if(filedata[i].owner_id==client_sockfd){
            filedata[i].noOfLines=noOfLines(filedata[i].filename);
        }
        else{
            for(int j=0;j<MAXIMUM_CLIENT;j++){
                if(filedata[i].editCollaborators[j]==client_sockfd){
                    filedata[i].noOfLines=noOfLines(filedata[i].filename);
                }
            }
        }
    }
}

//Utility function for /insert Request.
int insertxfn(int queryline,char lineToInsert[],int count,char filename[]){

    int status=0; //status bit for denoting success or failure.
    
    FILE *f1=fopen(filename,"r");
    if(!f1){
        printf("File not opened properly");
        exit(0);
    }
    FILE *f2=fopen("insertFileServer.txt","w");
    if(!f2){
        printf("File not opened properly");
        exit(0);
    }

    int temp=0;
    char *line;
    line=(char*)malloc(BUFFER);

    //Reverse Index shifting.
    if(queryline<0){
        queryline+=count;
    }
    
    //Reading and storing (k-1) lines from the file to a temporary file.
    while(temp<queryline){
        int num;
        fgets(line,BUFFER,f1);
        fprintf(f2,"%s",line);
        temp++;
        status=1;
    }
    
    //Inserting the line taken from the user.
    if(queryline==count){
        temp=count;
        lineToInsert[strlen(lineToInsert)-1]='\0';
        fprintf(f2,"%s","\n");
        for(int k=1;k<strlen(lineToInsert)-1;k++){
            if(lineToInsert[k]==92 && lineToInsert[k+1]=='n' || lineToInsert[k]=='"'){
                k++;
                fprintf(f2,"%s","\n");
                continue;
            }
            else{
                fputc(lineToInsert[k],f2);
            }
            
        }
    }
    else{
        for(int k=1;k<strlen(lineToInsert)-1;k++){
            if(lineToInsert[k]==92 && lineToInsert[k+1]=='n' || lineToInsert[k]=='"'){
                k++;
                fprintf(f2,"%s","\n");
                continue;
            }
            else{
                fputc(lineToInsert[k],f2);
            }
            
        }
    }
        


    //Reading and storing remaining lines from the file to a temporary file.
    while(temp<count){
        
        int num;
        fgets(line,BUFFER,f1);
        fprintf(f2,"%s",line);
        temp++;
        status=1;
    }
    fclose(f1);
    fclose(f2);

    return status;
}

//Function to handle /insert Request.
int insertFunction(char buffer[],char command[],int client_sockfd,FILEDATA *fileDetails){
    

    //counting No. of White Spcaes.
    int count=0;
    for(int i=0;i<strlen(buffer);i++){
        if(buffer[i]==' '){
            count=count+1;
        }
        if(buffer[i]=='\"')
            break;
    }

    //Reading File Name;
    char *token=strtok(buffer," ");
    token=strtok(NULL," ");
    char filename[BUFFER];
    strcpy(filename,token);

    
    int control_bit=0,set_bit=0,message_bit=0,num_bit=0,newline_bit=0;
    int ctr=0;

    int idx=0;
    char message[BUFFER];
    if(count==2){
        //Reading Message.
        token=strtok(NULL," ");
        strcpy(message,token);
        ctr=1;
    }
    else if(count==3){
        //Reading index.
        token=strtok(NULL," ");

        if(token==NULL){
            control_bit=1;
        }
        else{
            idx=atoi(token);
            int num=idx;
            if(num<0){  
                for(int i=1;i<strlen(token)-1;i++){
                    if(token[i]<48 || token[i]>57){
                        num_bit=0;
                    }
                    else{
                        num_bit=1;
                    }
                }
            }
            else{
                for(int i=0;i<strlen(token)-1;i++){
                    if(token[i]<48 || token[i]>57){
                        num_bit=0;
                    }
                    else{
                        num_bit=1;
                    }
                }
            }
            idx=num;
        }

        //Reading Message.
        token=strtok(NULL," ");
        strcpy(message,token);
    }

    //Read command from client to buffer.
    bzero(message,BUFFER);
    int n = read(client_sockfd,message,BUFFER);
    if (n < 0) {
        printf("Socket read issue");
        return 0;
    }


    int check1=exist(fileDetails,filename);

    if(check1){
        //printf("\nHere 1\n");
        int check2=checker(client_sockfd,fileDetails,filename,'A');
        int check4=checker(client_sockfd,fileDetails,filename,'E');
        if(check2 || check4){
            //printf("\nHere 2\n");
            sender("OK",client_sockfd);
        }
        else{
            //printf("\nHere 3\n");
            sender("NOTOK",client_sockfd);
            return 0;
        }
    }
    else{
        //printf("\nHere 4\n");
        sender("NOEXIST",client_sockfd);
        return 0;
    }

    int nlines=noOfLines(filename);

    
    if(set_bit){
        sender("NOTOK",client_sockfd);
        return 0;
    }
    else if(control_bit){
        sender("NOTOK",client_sockfd);
        return 0;
    }
    else if((idx>=0 && idx>=nlines) || (idx<0 && idx<(-1*nlines))){
        //printf("\nHere 4\n");
        sender("NOTOK1",client_sockfd);
        return 0;
    }
    else{
        if(ctr)
            idx=nlines;
        sender("OK",client_sockfd);
        int result=insertxfn(idx,message,nlines,filename);

        fileCopy(filename,"insertFileServer.txt");
        sendingFileContents(filename,client_sockfd);
    }
    
    //Update Lines.
    lineUpdate(fileDetails,client_sockfd);

    return 0;
}

//Utility Function for /delete Request.
void utilityDelete(char filename[],int start,int end,int client_sockfd){
    
    int nlines=noOfLines(filename);
    FILE *f=fopen(filename,"r");
    if(!f){
        printf("File not opened properly");
        exit(0);
    }

    if(start<0){
        printf("\nHere 23\n");
        start+=nlines;
    }
    if(end<0){
        printf("\nHere 24\n");
        end+=nlines;
    }


    char *content=(char*)calloc(BUFFER,sizeof(char));
    writeToFile("deleteFileServer.txt","",1);
    for(int i=0;i<nlines;i++){
        fgets(content,BUFFER,f);
        if(i>=start && i<=end){
            continue;
        }
        else{
            writeToFile("deleteFileServer.txt",content,2);
        }
        
    }

    fclose(f);
    free(content);
}

//Function to handle /delete Request.
int deleteFunction(char buffer[],char command[],int client_sockfd,FILEDATA *fileDetails ){

   char filename[BUFFER],cmd[10];
    int start_idx,end_idx;
    char buf[BUFFER];
    strcpy(buf,buffer);
    int num=sscanf(buffer,"%s %s %d %d",cmd,filename,&start_idx,&end_idx);
    
    //printf("\n%s*\n",buf);

    int check1=exist(fileDetails,filename);

    if(check1){
        //printf("\nHere 1\n");
        int check2=checker(client_sockfd,fileDetails,filename,'A');
        int check4=checker(client_sockfd,fileDetails,filename,'E');
        if(check2 || check4){
            //printf("\nHere 2\n");
            sender("OK",client_sockfd);
        }
        else{
            //printf("\nHere 3\n");
            sender("NOTOK",client_sockfd);
            return 0;
        }
    }
    else{
        //printf("\nHere 4\n");
        sender("NOEXIST",client_sockfd);
        return 0;
    }

    int nlines=noOfLines(filename);
    printf("\n%d\n",nlines);

    switch(num){
        case 2:{
            //printf("\nHere 5\n");
            sender("OK1",client_sockfd);
            utilityDelete(filename,0,nlines-1,client_sockfd);
            fileCopy(filename,"deleteFileServer.txt");
            sendingFileContents("deleteFileServer.txt",client_sockfd);
            sprintf(buffer,"%s","File Contents for specified indices deleted!");
            sender(buffer,client_sockfd);
            break;
        }
        case 3:{
            //printf("\nHere 6\n");
            char *token=strtok(buf," ");
            token=strtok(NULL," ");
            token=strtok(NULL," ");
            if(token==NULL){
                //printf("\nHere 7\n");
                sender("NOTOK1",client_sockfd);
                break;
            }
            else{
                //printf("\nHere 8\n");
                token[strlen(token)-1]='\0';
                for(int i=1;token[i]!='\0';i++){
                    if(token[i]<48 || token[i]>57){
                        //printf("\nHere 9\n");
                        sender("NOTOK1",client_sockfd);
                        break;
                    }
                }
                if((start_idx>=0 && start_idx>=nlines) || (start_idx<0 && start_idx<(-1*nlines))){
                    //printf("\nHere 10\n");
                    sender("NOTOK1",client_sockfd);
                    break;
                }
                else{
                    //printf("\nHere 11\n");
                    sender("OK2",client_sockfd);
                    utilityDelete(filename,start_idx,start_idx,client_sockfd);
                    fileCopy(filename,"deleteFileServer.txt");
                    sendingFileContents(filename,client_sockfd);
                    sprintf(buffer,"%s","File Contents for specified indices deleted!");
                    sender(buffer,client_sockfd);
                    break;
                }
            }
            break;
        }
        case 4:{
            //printf("\nHere 12\n");
            char *token=strtok(buf," ");
            token=strtok(NULL," ");
            token=strtok(NULL," ");
            token=strtok(NULL," ");
            //printf("\n%s\n",token);
            if(token==NULL){
                //printf("\nHere 13\n");
                sender("NOTOK2",client_sockfd);
                break;
            }
            else{
                //printf("\nHere 14\n");
                token[strlen(token)-1]='\0';
                for(int i=1;token[i]!='\0';i++){
                    if(token[i]<48 || token[i]>57){
                        sender("NOTOK2",client_sockfd);
                        break;
                    }
                }
                if((start_idx>=0 && start_idx>=nlines) || (start_idx<0 && start_idx<(-1*nlines))){
                    //printf("\nHere 15\n");
                    sender("NOTOK2",client_sockfd);
                    break;
                }

                if((end_idx>=0 && end_idx>=nlines) || (end_idx<0 && end_idx<(-1*nlines))){
                    //printf("\nHere 16\n");
                    sender("NOTOK2",client_sockfd);
                    break;
                }
                if(start_idx>end_idx){
                    if(start_idx>=0 && end_idx<0){
                        //printf("\nHere 17\n");
                        sender("OK2",client_sockfd);
                        utilityDelete(filename,start_idx,end_idx,client_sockfd);
                        fileCopy(filename,"deleteFileServer.txt");
                        sendingFileContents(filename,client_sockfd);
                        sprintf(buffer,"%s","File Contents for specified indices deleted!");
                        sender(buffer,client_sockfd);
                    }
                    else{
                        //printf("\nHere 17\n");
                        sender("NOTOK2",client_sockfd);
                    }
                    
                    break;
                }
                else{
                    //printf("\nHere 18\n");
                    sender("OK3",client_sockfd);
                    //printf("\nHere 18\n");
                    utilityDelete(filename,start_idx,end_idx,client_sockfd);
                    fileCopy(filename,"deleteFileServer.txt");
                    sendingFileContents(filename,client_sockfd);
                    sprintf(buffer,"%s","File Contents for specified indices deleted!");
                    sender(buffer,client_sockfd);
                    break;
                }
            }
            break;
        }
        default:{
            //printf("\nHere 19\n");
            sender("NOTOK3",client_sockfd);
            break;
        }

    }

    //Update Lines.
    lineUpdate(fileDetails,client_sockfd);

    

    return 0;
}

//Function to handle request from client.
void clientRequestHandleByServer(int socket_fd,fd_set readfds){

    //Socket descriptor for client.
    struct sockaddr_in cli_addr;
    socklen_t clilen;
    clilen = sizeof(cli_addr);

	int client_socfd,clientSoc[MAXIMUM_CLIENT],sd;
    int maxSoc_fd;

    //Initialize all client socket to zero
    for(int i=0;i<MAXIMUM_CLIENT;i++){
        clientSoc[i]=0;
    }

    char buffer[BUFFER]={0};
    int n;
    FILEDATA *fileDetails;
    fileDetails=(FILEDATA*)malloc(100*sizeof(FILEDATA));

    userInfo *user;
    user=(userInfo*)malloc(MAXIMUM_CLIENT*sizeof(userInfo));
    for(int i=0;i<MAXIMUM_CLIENT;i++){
        user[i].client_id=0;
        user[i].socket_id=0;
    }

    while(1){

        //clear socket array.
        FD_ZERO(&readfds);

        //Adding Server Socket to set.
        FD_SET(socket_fd,&readfds);
        maxSoc_fd=socket_fd;

        //Adding client socket to set.
        for(int i=0;i<MAXIMUM_CLIENT;i++){
            sd=clientSoc[i];
            if(sd>0){
                FD_SET(sd,&readfds);
            }
            if(sd>maxSoc_fd){
                maxSoc_fd=sd;
            }
        }

        int m=select(maxSoc_fd+1,&readfds,NULL,NULL,NULL);
        if(m<0 && errno!=EINTR){
            perror("\nERROR IN SELECTING\n");
        }

        if(FD_ISSET(socket_fd,&readfds)){
            //Accept client.
            client_socfd=accept(socket_fd, (struct sockaddr *) &cli_addr,&clilen);
    	    if(client_socfd<0){
        	    perror("Error in accepting");
			    exit(EXIT_FAILURE);
    	    }
    
            //Checking if connection is success or not.
            bzero(buffer,BUFFER);
            sprintf(buffer,"%s","LIMIT");
            for(int i=0;i<MAXIMUM_CLIENT;i++){
                if(clientSoc[i]==0){
                    bzero(buffer,BUFFER);
                    sprintf(buffer,"%s","Success");
                    printf("\nClient with ID : %d Established connection with server\n",client_socfd);
                    break;
                }
            }
            n=send(client_socfd,buffer,sizeof(buffer),0);
        	if (n < 0) {
	 	    	printf("Socket write issue");
            	continue;
	    	}
            bzero(buffer,BUFFER);
    	    //Adding client socket to socket array.
            for(int i=0;i<MAXIMUM_CLIENT;i++){
                if(clientSoc[i]==0){
                    clientSoc[i]=client_socfd;
                    printf("\nClient socket added to %d position",i+1);
                    for(int k=0;k<MAXIMUM_CLIENT;k++){
                        if(user[k].client_id==0){
                            user[k].client_id=client_socfd+10000;
                            user[k].socket_id=client_socfd;
                            break;
                        }
                    }
                    break;
                }
            }

        }
        //Performing Operations.
        for (int i = 0; i < MAXIMUM_CLIENT; i++){

            sd=clientSoc[i];
            if(FD_ISSET(sd,&readfds)){

                //Read command from client to buffer.
                n = read(sd,buffer,sizeof(buffer));
        		if (n < 0) {
	 	    	printf("Socket read issue");
            	continue;
	    		}

                printf("\nUser Command : %s",buffer);

                char command[12];
                int chk;
                sscanf(buffer,"%s",command);
                Invite *invite;
                invite=(Invite*)malloc(sizeof(Invite));


                /*if(strcmp("YES",command)!=0 || strcmp("NO",command)!=0){
                    sender("Normal",sd);
                }*/

                if(strcmp("/users",command) == 0){
                    writeToFile("client_records.txt","CLIENT_ID\tSocket_ID",1);
                    for(int k=0;k<MAXIMUM_CLIENT;k++){
                        if(user[k].client_id!=0){
                            sprintf(buffer,"\n%d\t\t%d",user[k].client_id,user[k].socket_id);
                            writeToFile("client_records.txt",buffer,2);
                        }
                    }
                    chk=userFunction(buffer,command,sd,user);
                    if(chk==0){
                        bzero(buffer,BUFFER);
                        sprintf(buffer,"%s","List of Active users sent!!");
                        sender(buffer,sd);
                    }
                }
                else if(strcmp("/files",command) == 0){
                    chk=filesFunction(buffer,command,sd,fileDetails);
                    if(chk==0){
                        bzero(buffer,BUFFER);
                        sprintf(buffer,"%s","Details of Files present sent!!");
                        sender(buffer,sd);
                    }
                }
                else if(strcmp("/upload",command) == 0){
                    chk=uploadFunction(buffer,command,sd,fileDetails);
                    if(chk==0){
                        bzero(buffer,BUFFER);
                        sprintf(buffer,"%s","File Uploaded Successfully.");
                        sender(buffer,sd);
                    }
                    else{
                        bzero(buffer,BUFFER);
                        sprintf(buffer,"%s","File Already Exists.");
                        sender(buffer,sd);
                    }
                }
                else if(strcmp("/download",command) == 0){
                    chk=downloadFunction(buffer,command,sd,fileDetails);
                    if(chk==0){
                        bzero(buffer,BUFFER);
                        sprintf(buffer,"%s","Requested File sent.");
                        sender(buffer,sd);
                    }
                    else if(chk==1){
                        //
                    }
                    else{
                        //
                    }
                }
                else if(strcmp("/invite",command) == 0){
                    chk=inviteFunction(buffer,command,sd,clientSoc,fileDetails,invite);
                    if(chk==0){
                        bzero(buffer,BUFFER);
                        //sprintf(buffer,"%s","Invite sent Successfully.");
                        //continue;
                    }
                    else{
                        printf("\nClient %d closed connection with server unexpectedly\n",chk+10000);
                        removeData(chk,fileDetails,user);
                        for(int j=0;j<MAXIMUM_CLIENT;j++){
                            if(clientSoc[j]==chk){
                                close(chk);
					            clientSoc[j] = 0;
                            }
                        }
                    }
                }
                else if(strcmp("/read",command) == 0){
                    chk=readFunction(buffer,command,sd,fileDetails);
                    if(chk==0){
                        continue;
                    }
                }
                else if(strcmp("/insert",command) == 0){
                    chk=insertFunction(buffer,command,sd,fileDetails);
                    if(chk==0){
                        continue;
                    }
                }
                else if(strcmp("/delete",command) == 0){
                    chk=deleteFunction(buffer,command,sd,fileDetails);
                    if(chk==0){
                        continue;
                    }
                }
                else if(strcmp("/exit",command) == 0){
                    bzero(buffer,BUFFER);
                    sprintf(buffer,"%s","Client disconnected");
                    sender(buffer,sd);
                    printf("\nClient %d closed connection with server\n",sd+10000);
                    removeData(sd,fileDetails,user);
                    close(sd);
					clientSoc[i] = 0;
                    continue;
                }
                else{
                    bzero(buffer,BUFFER);
                    sprintf(buffer,"%s","Client disconnected");
                    sender(buffer,sd);
                    printf("\nClient %d closed connection with server\n",sd+10000);
                    removeData(sd,fileDetails,user);
                    close(sd);
					clientSoc[i] = 0;
                    continue;
                }

            }
        }
    }
}

/* Driver Function */
int main(int argc, char const *argv[])
{
    //Reading port from command line.
    int Port=atoi(argv[1]);

    //Checking for valid no. of parameters.
    if(argc < 2){
		printf("Insufficient Parameters");
		return -1;
	}

    printf("\n\n**********WELCOME to ONLINE FILE MANAGER**********\n\n");

    /* Server Setup. */

    //Create a socket.
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_fd<0){
        printf("Socket not created properly");
		return -1;
    }
    printf("\nServer Socket Created\n");

    struct sockaddr_in serv_addr;
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(Port);

    //Socket Descriptor Array
    fd_set readfds;

    //Bind the socket.
    if (bind(socket_fd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0 ) {
        perror("ERROR on binding");
        exit(EXIT_FAILURE);
    }
    printf("\nBind to port : %d\n",Port);

    //Listen to socket.
    if (listen(socket_fd, 5) != 0)
	{
		perror("Error in listening");
		exit(EXIT_FAILURE);
	}
    printf("\nListening\n");


    /* Client Request Handling */
    clientRequestHandleByServer(socket_fd,readfds);

    return 0;
}