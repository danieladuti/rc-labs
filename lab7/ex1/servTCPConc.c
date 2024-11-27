 /* servTcpConc.c - Exemplu de server TCP concurent
    Asteapta un nume de la clienti; intoarce clientului sirul
    "Hello nume".
*/

//uite iterativ si concurent si data viitoare: multiplexare + proiecte, sapt 10: tema 2

#include <sys/types.h>
#include <sys/wait.h> //adaugat de mine pt wait
#include <fcntl.h> //adaugat de mine pt O_RDONLY
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/* portul folosit */
#define PORT 2024

/* codul de eroare returnat de anumite apeluri */
extern int errno;

int main ()
{
    struct sockaddr_in server;	// structura folosita de server
    struct sockaddr_in from;
    char msg[1024];		//mesajul primit de la client
    char msgrasp[1024]=" ";        //mesaj de raspuns pentru client
    int sd;			//descriptorul de socket

    /* crearea unui socket */
    if ((sd = socket (AF_INET, SOCK_STREAM, 0)) == -1)
    {
    	perror ("[server]Eroare la socket().\n");
    	return errno;
    }

    /* pregatirea structurilor de date */
    bzero (&server, sizeof (server));
    bzero (&from, sizeof (from));

    /* umplem structura folosita de server */
    /* stabilirea familiei de socket-uri */
    server.sin_family = AF_INET;
    /* acceptam orice adresa */
    server.sin_addr.s_addr = htonl (INADDR_ANY);
    /* utilizam un port utilizator */
    server.sin_port = htons (PORT);

    /* atasam socketul */
    if (bind (sd, (struct sockaddr *) &server, sizeof (struct sockaddr)) == -1)
    {
    	perror ("[server]Eroare la bind().\n");
    	return errno;
    }

    /* punem serverul sa asculte daca vin clienti sa se conecteze */
    if (listen (sd, 1) == -1)
    {
    	perror ("[server]Eroare la listen().\n");
    	return errno;
    }

    /* servim in mod concurent clientii... */
    while (1)
    {
    	int client;
    	int length = sizeof (from);

    	printf ("[server]Asteptam la portul %d...\n",PORT);
    	fflush (stdout);

    	/* acceptam un client (stare blocanta pina la realizarea conexiunii) */
    	client = accept (sd, (struct sockaddr *) &from, &length);

    	/* eroare la acceptarea conexiunii de la un client */
    	if (client < 0)
    	{
    		perror ("[server]Eroare la accept().\n");
    		continue;
    	}

    	int pid;
    	if ((pid = fork()) == -1) {
    		close(client);
    		continue;
    	} else if (pid > 0) {
    		// parinte
    		close(client);
    		while(waitpid(-1,NULL,WNOHANG));
    		continue;
    	} else if (pid == 0) {
    		// copil
    		close(sd);

			// deschid fisierul care trebuie copiat
			int fd_fisier;
			if ((fd_fisier = open("cerinta.txt", O_RDONLY)) == -1)
			{
				perror("eroare la deschiderea fisierului");
				exit(4);
			}

			char buffer[1024];
			ssize_t bytes_read, bytes_written;
			// copiem continutul din fisier in buffer
			while ((bytes_read = read(fd_fisier, buffer, sizeof(buffer) - 1)) > 0)
			{
				bytes_written = write(sd, buffer, bytes_read);
				buffer[bytes_read] = '\0';                    // Terminăm șirul
				printf("raspunsul de la server: %s", buffer); // Afișăm mesajul primit
				printf("\n");
				if (bytes_written != bytes_read) {
				perror("Eroare la scrierea datelor în fișierul de destinație");
				//close(sd);
				return -1;
			}

    		/* s-a realizat conexiunea, se astepta mesajul */
    		bzero (msg, 1024);
    		printf ("[server]Asteptam mesajul...\n");
    		fflush (stdout);

    		/* citirea mesajului */
    		if (read (client, msg, 1024) <= 0)
    		{
    			perror ("[server]Eroare la read() de la client.\n");
    			close (client);	/* inchidem conexiunea cu clientul */
    			continue;		/* continuam sa ascultam */
    		}

    		printf ("[server]Mesajul a fost receptionat...%s\n", msg);

    		/*pregatim mesajul de raspuns */
    		bzero(msgrasp,1024);
    		strcat(msgrasp,"Hello ");
    		strcat(msgrasp,buffer);

    		printf("[server]Trimitem mesajul inapoi...%s\n",msgrasp);


    		/* returnam mesajul clientului */
    		if (write (client, msgrasp, 1024) <= 0)
    		{
    			perror ("[server]Eroare la write() catre client.\n");
    			continue;		/* continuam sa ascultam */
    		}
    		else
    			printf ("[server]Mesajul a fost trasmis cu succes.\n");
    		
    		/* am terminat cu acest client, inchidem conexiunea */
    		close (client);
    		exit(0);
    	}

    }				/* while */
}				/* main */
