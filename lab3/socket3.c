//prin SOCKET: tatl trimite un sir fiului.
//fiul concateneaza la sirul prmit un alt sir si va intoarce procesului tata raspunsul obtinut.

#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MSG1 "Comunicam prin socketi!" 
#define MSG2 "Atentie!Socketpair() o generalizarea a pipe-urilor"

int main() 
{ 
    int sockp[2], child; 
    char msg[1024];

    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sockp) < 0) 
      { 
        perror("Err... socketpair"); 
        exit(1); 
      }

    if ((child = fork()) == -1) perror("Err...fork"); 
    else 
      if (child)   //parinte 
        {  
            close(sockp[0]); 
            if (read(sockp[1], msg, 1024) < 0) perror("[parinte]Err...read"); 
            printf("[parinte] %s\n", msg); 
            if (write(sockp[1], MSG2, sizeof(MSG2)) < 0) perror("[parinte]Err...write"); 
            close(sockp[1]); 
        } 
        else     //copil
          { 
            close(sockp[1]);
            if (read(sockp[0], msg, 1024) < 0) perror("[copil]Err..read");
            
            if (write(sockp[0], msg, sizeof(MSG1)) < 0) perror("[copil]Err...write"); 
             
            printf("[copil]  %s\n", msg); 
            close(sockp[0]);
           } 
    return 0; 
}  
           
