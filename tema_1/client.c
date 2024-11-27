// CLIENT

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

/*
    const char *comanda = "Hello, this is a message!\n";
    write(fd_client, comanda, strlen(comanda));
    close(fd_client);


    fgets(comanda, sizeof(comanda), stdin);
    write(fd_client, comanda, strlen(comanda));
    close(fd_client);
*/

char comanda[256], raspuns[256];
const char fifo_client[] = "fifo_client";
const char fifo_server[] = "fifo_server";
int fd_client, fd_server1;

int main(int argc, char *argv[])
{
    // comunicarea cu clientul (sa stie clientul din ce instructiuni poate alege)
    printf("Salutare, client! Poti alege una dintre urmatoarele comenzi:\n"
           /*1.*/ "login : <username>\n"
           /*2.*/ "get-logged-users\n"
           /*3.*/ "get-proc-info : <pid>\n"
           /*4.*/ "logout\n"
           /*5.*/ "quit\n");

    // creez fifo ca sa comunice comenzile clientul la server
    if (mkfifo(fifo_client, 0600) == -1)
    {
        if (errno == EEXIST)
        {
            perror("fifo_client exista deja!\n");
            // exit(1);
        }
        else
        {
            perror("eroare la crearea fifo_client");
            // exit(2);
        }
    }

    if ((fd_client = open(fifo_client, O_WRONLY)) == -1)
    {
        perror("deschidere fifo_client");
        exit(3);
    }

    int test = 1;
    // citesc de la tastatura comenzile si apoi le transmit prin fifo la server
    // do
    while (test)
    {
        printf("introdu comanda: ");
        fgets(comanda, sizeof(comanda), stdin);
        comanda[strcspn(comanda, "\n")] = '\0'; // elimin newline adaugat de fgets
        // int l = read(0, comanda, 256);
        // raspuns[l] = '\0';
        if (strncmp(comanda, "login :", 7) == 0 || strcmp(comanda, "get-logged-users") == 0 ||
            strncmp(comanda, "get-proc-info : ", 16) == 0 || strcmp(comanda, "logout") == 0 ||
            strcmp(comanda, "quit") == 0)
        {
            if (write(fd_client, comanda, strlen(comanda)) == -1)
            {
                perror("write fifo_client");
                exit(4);
            }
            //close(fd_client);
        }
        else
        {
            perror("nu ai introdus o comanda valabila!\n");
            // exit(5);
        }

        if (strcmp(comanda, "quit") == 0)
        {
            printf("ai iesit din sesiune!\n");
            // write(fd_client, comanda, 256);
            return 0;
        }




        // clientul primeste raspuns de la server
        printf("acum astept raspuns de la server..\n");
        if ((fd_server1 = open(fifo_server, O_RDONLY)) == -1)
        {
            perror("deschidere fifo_server");
            exit(1);
        }
        int lung = read(fd_server1, raspuns, 256);
        raspuns[lung] = '\0';
        printf("raspunsul de la server: %s\n", raspuns);
        /*char buffer[128];
        ssize_t bytes_read;
        // citim raspunsul de la server din fifo
        while ((bytes_read = read(fd_server1, buffer, sizeof(buffer) - 1)) > 0)
        {
            buffer[bytes_read] = '\0';                    // Terminăm șirul
            printf("raspunsul de la server: %s", buffer); // Afișăm mesajul primit
            printf("\n");
        }*/
        //close(fd_server1);

    } // while (test);

    return 0;
}