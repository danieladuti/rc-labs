
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/socket.h>
#include <utmp.h>
#include <sys/uio.h>
#include <stdbool.h>

#define dim 100

int fd_username;
char mesaj[dim];

/*
int nr_cuvinte(char t[])
{
    char *p;
    int nrc;
    nrc=0;
    p=strtok(t," ,;.:\"!?()");
    while(p!=NULL)
    {
        nrc++;
        p=strtok(NULL," ,;.:\"!?()");
    }
    return nrc;
}
*/

int username_gasit(char buffer[], char mesaj[])
{
    int bytesf;
    int flag = 0;

    // elimin newline-ul adaugat de la read
    int i = 0;
    while (i < strlen(mesaj))
    {
        if (mesaj[i] == '\n')
        {
            break;
        }
        i++;
    }
    mesaj[i] = '\0';
    printf("[server]Mesajul a fost modificat...%s\n", mesaj);

    // deschid fisierul cu useri
    if ((fd_username = open("usernames.txt", O_RDONLY)) == -1)
    {
        printf("eroare la deschiderea fisierului");
    }

    char *username = &mesaj[0]; // iau username din comanda
    if ((fd_username = open("usernames.txt", O_RDONLY)) == -1)
    {
        printf("eroare la deschiderea fisierului cu usernames.");
    }

    bytesf = read(fd_username, buffer, dim);
    buffer[bytesf] = '\0';

    printf("fiserul contine: %s\n", buffer);
    // printf("%s", buffer);

    // verif daca exista vreun username in fisier
    char *token = strtok(buffer, "\n");
    while (token != NULL)
    {
        printf("am intrat in while!!!!!\n");
        fflush(stdout);
        if (strcmp(token, username) == 0)
        {
            printf("token: %s \n", token);
            fflush(stdout);
            flag = 1;
            break;
        }
        token = strtok(NULL, "\n");
    }

    return flag;
}

int main()
{
    char buffer[dim];
    int bytesi;
    printf("Introdu un username: ");
    fflush(stdout);
    bytesi = read(0, mesaj, sizeof(buffer));
    if (bytesi < 0)
    {
        printf("[server]Eroare la read() de la input.\n");
        return 0;
    }
    printf("[server]Mesajul a fost receptionat...%s\n", mesaj);

    if (username_gasit(buffer, mesaj) == 1)
        printf("userul %s s-a conectat cu succes!\n", mesaj);
    else
        printf("usernameul nu a fost gasit.\n");
    return 0;
}