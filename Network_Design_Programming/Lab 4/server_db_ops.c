#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <ctype.h>
#include <sys/wait.h>
#include <sys/select.h>

#define TCP_PORT 6000
#define UDP_PORT 6001
#define MAX 1024

typedef struct {
    char regno[10];
    char name[50];
    char address[100];
    char dept[20];
    int semester;
    char section[5];
    char courses[5][10];
    int marks[5];
} Student;

// Sample student database
Student db[] = {
    {"R101","Alice","123 Street","CSE",3,"A",{"CS101","CS102","CS103","CS104","CS105"},{90,85,92,88,75}},
    {"R102","Bob","456 Avenue","ECE",2,"B",{"EC101","EC102","EC103","EC104","EC105"},{80,75,78,82,70}},
    {"R103","Charlie","789 Road","ME",4,"C",{"ME101","ME102","ME103","ME104","ME105"},{85,88,80,90,77}}
};

int db_size = 3;

// Function to handle TCP requests (forked child)
void handle_tcp(int newsock) {
    char buffer[MAX];
    recv(newsock, buffer, MAX, 0);

    char option[2];
    char data[50];
    sscanf(buffer,"%1s %s", option, data);

    Student *s = NULL;

    // Find student if needed
    for(int i=0;i<db_size;i++){
        if(option[0]=='1' && strcmp(db[i].regno,data)==0) { s=&db[i]; break; }
        if(option[0]=='2' && strcmp(db[i].name,data)==0) { s=&db[i]; break; }
        if(option[0]=='3') { s=&db[i]; break; } // Subject code lookup handled later
    }

    char msg[MAX];
    pid_t pid = getpid();

    if(option[0]=='1'){ // Registration number -> send name & address
        if(s){
            snprintf(msg,MAX,"Child PID=%d\nName: %s\nAddress: %s",pid,s->name,s->address);
        } else strcpy(msg,"Student not found");
    }
    else if(option[0]=='2'){ // Name -> send enrollment
        if(s){
            snprintf(msg,MAX,"Child PID=%d\nDept: %s\nSemester: %d\nSection: %s\nCourses: %s,%s,%s,%s,%s",
                pid,s->dept,s->semester,s->section,
                s->courses[0],s->courses[1],s->courses[2],s->courses[3],s->courses[4]);
        } else strcpy(msg,"Student not found");
    }
    else if(option[0]=='3'){ // Subject code -> send marks
        int found=0;
        if(s){
            for(int i=0;i<5;i++){
                if(strcmp(s->courses[i],data)==0){
                    snprintf(msg,MAX,"Child PID=%d\nMarks in %s: %d",pid,data,s->marks[i]);
                    found=1; break;
                }
            }
        }
        if(!found) strcpy(msg,"Subject not found");
    }
    else strcpy(msg,"Invalid option");

    send(newsock, msg, strlen(msg), 0);
    close(newsock);
}

// Function to handle UDP requests
void handle_udp(int sockfd) {
    char buffer[MAX];
    struct sockaddr_in cliaddr;
    socklen_t len = sizeof(cliaddr);

    recvfrom(sockfd, buffer, MAX, 0, (struct sockaddr*)&cliaddr, &len);

    char option[2];
    char data[50];
    sscanf(buffer,"%1s %s", option, data);

    Student *s = NULL;
    for(int i=0;i<db_size;i++){
        if(option[0]=='1' && strcmp(db[i].regno,data)==0) { s=&db[i]; break; }
        if(option[0]=='2' && strcmp(db[i].name,data)==0) { s=&db[i]; break; }
        if(option[0]=='3') { s=&db[i]; break; }
    }

    char msg[MAX];
    pid_t pid = getpid();

    if(option[0]=='1'){ // Registration number -> send name & address
        if(s){
            snprintf(msg,MAX,"Child PID=%d\nName: %s\nAddress: %s",pid,s->name,s->address);
        } else strcpy(msg,"Student not found");
    }
    else if(option[0]=='2'){ // Name -> send enrollment
        if(s){
            snprintf(msg,MAX,"Child PID=%d\nDept: %s\nSemester: %d\nSection: %s\nCourses: %s,%s,%s,%s,%s",
                pid,s->dept,s->semester,s->section,
                s->courses[0],s->courses[1],s->courses[2],s->courses[3],s->courses[4]);
        } else strcpy(msg,"Student not found");
    }
    else if(option[0]=='3'){ // Subject code -> send marks
        int found=0;
        if(s){
            for(int i=0;i<5;i++){
                if(strcmp(s->courses[i],data)==0){
                    snprintf(msg,MAX,"Child PID=%d\nMarks in %s: %d",pid,data,s->marks[i]);
                    found=1; break;
                }
            }
        }
        if(!found) strcpy(msg,"Subject not found");
    }
    else strcpy(msg,"Invalid option");

    sendto(sockfd, msg, strlen(msg), 0, (struct sockaddr*)&cliaddr, len);
}

int main() {
    int tcp_sock, udp_sock;
    struct sockaddr_in tcp_addr, udp_addr;

    // --- TCP setup ---
    tcp_sock = socket(AF_INET, SOCK_STREAM, 0);
    tcp_addr.sin_family = AF_INET;
    tcp_addr.sin_addr.s_addr = INADDR_ANY;
    tcp_addr.sin_port = htons(TCP_PORT);
    bind(tcp_sock, (struct sockaddr*)&tcp_addr, sizeof(tcp_addr));
    listen(tcp_sock, 5);

    // --- UDP setup ---
    udp_sock = socket(AF_INET, SOCK_DGRAM, 0);
    udp_addr.sin_family = AF_INET;
    udp_addr.sin_addr.s_addr = INADDR_ANY;
    udp_addr.sin_port = htons(UDP_PORT);
    bind(udp_sock, (struct sockaddr*)&udp_addr, sizeof(udp_addr));

    printf("Server running TCP:%d UDP:%d\n", TCP_PORT, UDP_PORT);

    fd_set readfds;
    int maxfd = (tcp_sock>udp_sock?tcp_sock:udp_sock)+1;

    while(1){
        FD_ZERO(&readfds);
        FD_SET(tcp_sock, &readfds);
        FD_SET(udp_sock, &readfds);

        select(maxfd, &readfds, NULL, NULL, NULL);

        if(FD_ISSET(tcp_sock, &readfds)){
            int newsock;
            struct sockaddr_in cli;
            socklen_t len = sizeof(cli);
            newsock = accept(tcp_sock, (struct sockaddr*)&cli, &len);
            if(fork()==0){
                handle_tcp(newsock);
                exit(0);
            } else close(newsock);
            waitpid(-1,NULL,WNOHANG); // clean zombie
        }

        if(FD_ISSET(udp_sock, &readfds)){
            if(fork()==0){
                handle_udp(udp_sock);
                exit(0);
            } 
            waitpid(-1,NULL,WNOHANG);
        }
    }

    close(tcp_sock);
    close(udp_sock);
    return 0;
}
