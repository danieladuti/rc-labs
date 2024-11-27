// SERVER

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>  //exit
#include <string.h> //strcmp
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/socket.h>
#include <utmp.h>
#include <sys/uio.h>
#include <stdbool.h>

/*struct utmp  //AM INCERCAT...
{
    char ut_user[UT_NAMESIZE]; // username
    char ut_host[UT_HOSTSIZE]; // hostname
    struct
    {
        int32_t tv_sec;  //secunde
        int32_t tv_usec; //microsecunde
    } ut_tv;             //timpul logarii
};*/

const char fifo_client[] = "fifo_client";
const char fifo_server[] = "fifo_server";
int fd_server;
int fd_client;
int fd_username;

int main()
{
    printf("Astept primirea unei comenzi de la client...\n");
    // deschidem citirea comezilor din client
    if ((fd_client = open(fifo_client, O_RDONLY)) == -1)
    {
        perror("deschidere fifo_client");
        exit(1);
    }

    // creez fifo din server ca sa trimit raspunsul la client
    if (mkfifo(fifo_server, 0600) == -1)
    {
        if (errno == EEXIST)
        {
            perror("fifo_server exista deja!\n");
            // exit(1);
        }
        else
        {
            perror("eroare la crearea fifo_server");
            // exit(2);
        }
    }

    if ((fd_server = open(fifo_server, O_WRONLY)) == -1)
    {
        perror("deschidere fifo_server");
        // exit(3);
    }

    // deschid fisierul cu useri
    if ((fd_username = open("usernames.txt", O_RDONLY)) == -1)
    {
        perror("eroare la deschiderea fisierului");
        exit(4);
    }

    int test = 1, useri_log = 0;
    while (test)
    {
        char comanda[256] = "", raspuns[256] = "";
        read(fd_client, comanda, 256);

        printf("%s\n", comanda);

        if (strcmp(comanda, "quit") == 0)
        {
            printf("ai iesit din sesiune!\n");
            return 0;
        }
        else if (strncmp(comanda, "login :", 6) == 0) //// LOGIN : <USERNAME>
        {
            int sockets[2], pid;
            socketpair(AF_UNIX, SOCK_STREAM, 0, sockets);
            pid = fork();
            if (pid) // tata
            {
                close(sockets[0]); // inchidem socket-ul copilului

                write(sockets[1], comanda, 256);
                int lung = read(sockets[1], raspuns, 256);
                raspuns[lung] = '\0';

                // validam lungimea mesajului cu prefixul
                char aux[256];
                strcpy(aux, raspuns);
                char *nr = strtok(aux, "-");
                // printf("%d == %ld", atoi(nr), strlen(raspuns) - 1 - strlen(nr));
                if (atoi(nr) == strlen(raspuns) - 1 - strlen(nr))
                {
                    char *mesaj = &raspuns[2];
                    if (strcmp(mesaj, "Succes") == 0)
                        useri_log = 1;
                    write(fd_server, mesaj, strlen(mesaj));
                }
                else
                {
                    char *mesaj = "eroare la logare!";
                    write(fd_server, mesaj, strlen(mesaj));
                }
                close(sockets[1]);
            }
            else // copil
            {
                close(sockets[1]); // socket parinte
                char comanda_parinte[256];

                int lung = read(sockets[0], comanda_parinte, 256);
                comanda_parinte[lung] = '\0';
                // printf("%s", comanda_parinte);

                char *username = &comanda_parinte[8]; // iau username din comanda
                char buffer[256];
                int fd_username = open("usernames.txt", O_RDONLY);
                int l = read(fd_username, buffer, 256);
                buffer[l] = '\0';
                // printf("%s", buffer);

                // verif daca exista vreun username in fisier
                char *token = strtok(buffer, " ");
                while (token)
                {
                    if (strcmp(token, username) == 0)
                    {
                        strcpy(raspuns, "6");
                        strcat(raspuns, "-");
                        strcat(raspuns, "Succes");
                    }
                    token = strtok(NULL, " ");
                }

                write(sockets[0], raspuns, strlen(raspuns));

                close(sockets[0]);
                exit(1);
            }
        }
        else //// GET-LOGGED-USERS
        {
            if (strcmp(comanda, "get-logged-users") == 0)
            {
                if (useri_log == 1)
                { // creez pipe pentru comunicarea intre tata si copil
                    int pp1[2], pc2[2];
                    pipe(pp1);
                    pipe(pc2);
                    pid_t pid = fork();

                    if (pid) // tata
                    {
                        close(pp1[0]);
                        close(pc2[1]);

                        char info_copil[256];
                        int lung = read(pc2[0], info_copil, 256);
                        info_copil[lung] = '\0';

                        // validam lungimea mesajului cu prefixul
                        char aux[256];
                        strcpy(aux, info_copil);
                        char *nr = strtok(aux, "-");

                        if (atoi(nr) == strlen(info_copil) - 1 - strlen(nr))
                        {
                            char *mesaj;
                            if (strlen(nr) >= 2)
                                mesaj = &info_copil[3];
                            else
                                mesaj = &info_copil[2];
                            write(fd_server, mesaj, 254);
                        }
                        else
                        {
                            char *mesaj = "eroare la get-logged-users!";
                            write(fd_server, mesaj, strlen(mesaj));
                        }
                    }
                    else // copil
                    {
                        close(pp1[1]);
                        close(pc2[0]);
                        char copieBuffer[1024] = "";

                        struct utmp *info;
                        setutent();
                        while ((info = getutent()))
                        {
                            char info_user[1024];
                            snprintf(info_user, sizeof(info_user), "User->%s, Host->%s, Time->%c",
                                     info->ut_user, info->ut_host, info->ut_tv.tv_sec);
                        }
                        endutent();

                        char mesaj_tata[256];
                        int lung = strlen(copieBuffer);
                        char ch[10];
                        sprintf(ch, "%d", lung);
                        strcpy(mesaj_tata, ch);
                        strcat(mesaj_tata, "-");
                        strcat(mesaj_tata, copieBuffer);

                        write(pc2[1], mesaj_tata, strlen(mesaj_tata));
                        exit(0);
                    }
                }
                else
                {
                    char *mesaj = "nu te-ai logat! acceseaza mai intai login : <username>";
                    write(fd_server, mesaj, strlen(mesaj));
                }
            }
            else // GET-PROC-INFO : <PID>
                if (strncmp(comanda, "get-proc-info : ", 16) == 0)
                {
                    if (useri_log == 1)
                    {
                        const char fifo_get_proc1[] = "fifo_get_proc1", fifo_get_proc2[] = "fifo_get_proc2";
                        int fd_get_proc1, fd_get_proc2, pid;
                        if (mkfifo(fifo_get_proc1, 0600) == -1)
                        {
                            if (errno == EEXIST)
                            {
                                perror("fifo_get_proc1 exista deja!\n");
                                // exit(1);
                            }
                            else
                            {
                                perror("eroare la crearea fifo_get_proc1");
                                // exit(2);
                            }
                        }

                        if ((fd_get_proc1 = open(fifo_get_proc1, O_WRONLY)) == -1)
                        {
                            perror("deschidere fifo_get_proc1");
                            exit(3);
                        }

                        if (mkfifo(fifo_get_proc2, 0600) == -1)
                        {
                            if (errno == EEXIST)
                            {
                                perror("fifo_get_proc2 exista deja!\n");
                                // exit(1);
                            }
                            else
                            {
                                perror("eroare la crearea fifo_get_proc2");
                                // exit(2);
                            }
                        }

                        if ((fd_get_proc2 = open(fifo_get_proc2, O_WRONLY)) == -1)
                        {
                            perror("deschidere fifo_get_proc2");
                            exit(3);
                        }

                        pid = fork();

                        if (pid) // tata
                        {
                            //....
                        }
                        else // copil
                        {
                            //....
                        }
                    }
                    else
                    {
                        char *mesaj = "nu te-ai logat! acceseaza mai intai login : <username>";
                        write(fd_server, mesaj, strlen(mesaj));
                        close(fd_server);
                    }
                }
                else // LOGOUT
                    if (strcmp(comanda, "logout") == 0)
                    {
                        if (useri_log)
                        {
                            useri_log = 0;
                            char *mesaj = "te-ai delogat cu succes!";
                            write(fd_server, mesaj, strlen(mesaj));
                        }
                        else
                        {
                            char *mesaj = "nu te poti deloga pentru ca nu te-ai logat inca!!";
                            write(fd_server, mesaj, strlen(mesaj));
                        }
                    }
            /*else
            {
                char *mesaj = "comanda gresita, mai incearca!";
                write(fd_server, mesaj, sizeof(mesaj));
            }*/
        }
    }

    return 0;
}