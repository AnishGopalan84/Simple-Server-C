#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sqlite3.h>
#include <string.h>
const int PORTNUM = 1234;
/*
GREETING MESSAGE
SEND 
COLLECT YOLO
LIST ALL YOSR
EXIT NOYO
*/
void write_fun(int sock);
void complaint(int sock);
int main(int argc,char *argv[])
{
    ///create a socket///////////////
    int sockfd;
    struct sockaddr_in serv_add , cli_add;
    sockfd = socket(AF_INET,SOCK_STREAM,0);
    if (sockfd < 0)
    {
        printf("can'f open socket ;;;");
        exit(1);
    }
    /////////////////////////////////
    serv_add.sin_family = AF_INET;
    serv_add.sin_port = htons(PORTNUM);
    serv_add.sin_addr.s_addr = INADDR_ANY;
    //////////BIND TO SOCKET//////////
    int res = bind(sockfd,(struct sockaddr*) &serv_add,sizeof(serv_add));
    if (res < 0)
    {
        perror("can't bind to Socket;;; ");
        exit(1);
    }
    ///LISTENING FOR CONNECTION
    listen(sockfd, 5);
    ///ACCEPT CONNECTION
    int cli_add_Size = sizeof(cli_add);
    int newsockfd = accept(sockfd, (struct sockaddr *) &cli_add,&cli_add_Size);
     ///HANDS OF TO FUNCTION
        write_fun(newsockfd);
    //close the socket
    close(newsockfd);
    close(sockfd);
   /// for checking open port netstat -an | grep 1234
    return 0;
}
///////////////////////////////////
void write_fun(int sock)
{
    const int NOT_AUTH = 0;
    const int LOGGED_IN = 1;
    char buf[1000];
    char username[995];
//strcpy(buf,"go to End\n");
//send(sock,buf,strlen(buf),0);
//DATABASE CONNECTION as db
sqlite3 *db;
if(sqlite3_open("user.db",&db))
{
    printf("Cant open DataBase::\n");
    exit(1);
}
// SENDING GREETINGS
strcpy(buf,"hi there Greeting ......:\n");
send(sock,buf,strlen(buf),0);
int state = NOT_AUTH;
//Receive Message Wait for Reply
while (1)
{
    /* code */
    recv(sock,buf,1000,0);
    ///////////////////////////////
    if (!strncmp("exit",buf,4))
    {
        printf("Exiting ........\n");
        state = NOT_AUTH;
        close(sock);
        return;
    }
    ///////////////////////////////
    else if(!strncmp("login",buf,5))
    {
        //char * username = buf + 5;
    if(!state){
        
        sscanf(buf + 5,"%s",username);
        printf("user name is::: %s \n",username);
        ///Adding user in the Database
            char query[1000];
            sqlite3_stmt *stmt;            
            ///check user Avaiable in List Before Adding
            sqlite3_stmt *stmt1;
            char *query1 ="select uname from userlist order by id";
            sqlite3_prepare(db,query1,strlen(query1),&stmt1,NULL);
            int usradd = 1;
            while (sqlite3_step(stmt1) != SQLITE_DONE)
        {            
            strcpy(buf,(char *)sqlite3_column_text(stmt1,0));
            if (buf == username)
            {
                printf("Username Avaiable/n");
                usradd = 0;
                break;
            }
            else
            {                
                usradd = 1;
                printf("New User/n");                
            }    
        }       
            if (usradd){
            //Adding User In Db
            sprintf(query ,"INSERT INTO userlist (uname) values ('%s')",username);            
            sqlite3_prepare(db,query,strlen(query),&stmt,NULL);
            sqlite3_step(stmt);
            //Adding Finished
            }
            /*if (sqlite3_step(stmt) == SQLITE_ERROR)
            {
                strcpy(buf,"User Already Exists\n");
                continue;
            }*/
        // Send Back Conformation
        strcpy(buf,"Welcome you are Logged in>>\n");
        state = LOGGED_IN;
        send(sock,buf,strlen(buf),0);
    }
    else
    {
        strcpy(buf,"Already you are Logged in>>\n");
        send(sock,buf,strlen(buf),0); 
    }    
    }////////////////////////////////////////
     else if(!strncmp("send",buf,4))
    {
        //char * username = buf + 5;
        if (!state){complaint(sock); continue;}
        char msg[995];
        sscanf(buf + 5,"%s",msg);
        printf("Message from User::::%s \n",msg);

        // Send Back Conformation
        strcpy(buf,"Message Send.......>>\n");
        send(sock,buf,strlen(buf),0);
    }///////////////////////////////////
     else if(!strncmp("logout",buf,6))
    {
        if (!state){complaint(sock); continue;}
        //char * username = buf + 5;
        printf("logging out.... \n");
        state = NOT_AUTH;
        // Send Back Conformation
        strcpy(buf, "you are Logged out..>>\n");
        send(sock,buf,strlen(buf),0);
    }//////////////////////////////////
     else if(!strncmp("listuser",buf,8))
    {
        if (!state){complaint(sock); continue;}
        //char * username = buf + 5;
        printf("listing user..server \n");

        // Send Back Conformation
        strcpy(buf,"300  listing user....client>>\n");
        send(sock,buf,strlen(buf),0);
        ///list all user(Query DB For All User)
            sqlite3_stmt *stmt;
            sqlite3_stmt *stmt1;
            char *query ="select uname from userlist order by id";
            char *query1 ="select id from userlist order by id";
            sqlite3_prepare(db,query,strlen(query),&stmt,NULL);
            sqlite3_prepare(db,query1,strlen(query1),&stmt1,NULL);
        ///Display List of Clients
        char *uname;
        while (sqlite3_step(stmt) != SQLITE_DONE)
        {
            sqlite3_step(stmt1);
            char * div = " | ";
            strcpy(buf,(char *)sqlite3_column_text(stmt1,0));
            strncat(buf,div,strlen(div));
            send(sock,buf,strlen(buf),0);
            strcpy(buf,(char *)sqlite3_column_text(stmt,0));                      
            char *nline = "\n";
            send(sock,buf,strlen(buf),0);
            send(sock,nline,strlen(nline),0);
        }   ///send".""
         strcpy(buf,".\n");
        send(sock,buf,strlen(buf),0);
    }    ////////////////////////////////////////
      else if(!strncmp("queue",buf,5))
    {
        if (!state){complaint(sock); continue;}
        //Queuing the user List:::::
        sqlite3_stmt *stmt;
        char query[1000];
        char userTo[1000];
        sscanf(buf + 5,"%s",userTo);
        sprintf(query ,"INSERT INTO queue (qFrom,qTo) VALUES ('%s','%s')",username,userTo);
        sqlite3_prepare(db,query,strlen(query),&stmt,NULL);
        sqlite3_step(stmt);
        sprintf(buf,"'%s' is Queued\n",userTo);
        send(sock,buf,strlen(buf),0);
        //////////////////////////////////////////
    }
    else if(!strncmp("himsg",buf,5))
    {
        if (!state){complaint(sock); continue;}
        ///List the Person HIIIIs to you
        sqlite3_stmt *stmt;
        char query[1000];
        char userTo[1000];        
        sprintf(query ,"select qFrom from queue Where qTo = ('%s')",username);
        sqlite3_prepare(db,query,strlen(query),&stmt,NULL);
        strcpy(buf, "Msg HI from users............>>\n");
        send(sock,buf,strlen(buf),0);
        while (sqlite3_step(stmt) != SQLITE_DONE)
        {            
            char * div = " | ";
            strcpy(buf,(char *)sqlite3_column_text(stmt,0));                      
            char *nline = "\n";
            send(sock,buf,strlen(buf),0);
            send(sock,nline,strlen(nline),0);
        }       
         strcpy(buf, "DEleting the Messages...........>>\n");
        send(sock,buf,strlen(buf),0);
        //DELETING THE HI MSG LOG//
        sprintf(query ,"DELETE FROM queue Where qTo = ('%s')",username);
        sqlite3_prepare(db,query,strlen(query),&stmt,NULL);
        sqlite3_step(stmt);
    }
    ///////////////////////////////////////////////
else
    {
         strcpy(buf,"Wrong Code >>>>\n");
        send(sock,buf,strlen(buf),0);
    }   
  }
}
void complaint(int sock)
{
char buf[1000];
strcpy(buf,"Not Logged in....\n");
send(sock,buf,strlen(buf),0);
}