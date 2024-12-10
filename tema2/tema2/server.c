/* 
SERVER -- Digital Menu Board
*/

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>
#define dim 1048   // pt dimensiunea mesajelor
int conectat[100]; // verificam daca clientul este connectat sau nu
char msg[dim];     // mesajul de la client

/* portul folosit */

#define PORT 2728

extern int errno; /* eroarea returnata de unele apeluri */

/* functie de convertire a adresei IP a clientului in sir de caractere */
char *conv_addr(struct sockaddr_in address)
{
  static char str[25];
  char port[7];

  /* adresa IP a clientului */
  strcpy(str, inet_ntoa(address.sin_addr));
  /* portul utilizat de client */
  bzero(port, 7);
  sprintf(port, ":%d", ntohs(address.sin_port));
  strcat(str, port);
  return (str);
}

int conectare(char *msg, int fd, int bytes)
{
  char raspuns[dim];
  char nume[dim] = "";

  for (int i = 12; i < dim; i++)
    nume[i - 12] = msg[i]; // conectare : --> 12

  printf("[s-conn]am primit comanda..%s, cu numele %s\n", msg, nume);
  printf("[s-conn]salutare, %s!\n", nume);

  /*pregatim mesajul de raspuns */
  bzero(raspuns, dim);
  strcat(raspuns, "s-a conectat cu succes ");
  strcat(raspuns, nume);
  printf("[s-conn]trimitem mesajul inapoi..%s\n", raspuns);

  if (bytes && write(fd, raspuns, bytes) == -1)
  {
    perror("[s-conn] Eroare la write() catre client.\n");
    return 0;
  }
  conectat[fd] = 1; // clientul de la descriptorul client s-a conectat
  return bytes;
}

int afisare(char *msg, int fd, int bytes)
{
  char raspuns[dim];
  printf("[s-afisare]am primit comanda..%s\n", msg);

  /*pregatim mesajul de raspuns */
  bzero(raspuns, dim);
  strcat(raspuns, "afisam in terminal");
  printf("[s-afisare]trimitem mesajul inapoi..%s\n", raspuns);

  if (bytes && write(fd, raspuns, bytes) == -1)
  {
    perror("[s-afisare] Eroare la write() catre client.\n");
    return 0;
  }
  return bytes;
}

int export(char *msg, int fd, int bytes)
{
  char raspuns[dim];
  printf("[s-export]am primit comanda..%s\n", msg);

  /*pregatim mesajul de raspuns */
  bzero(raspuns, dim);
  strcat(raspuns, "dam export in fisier text");
  printf("[s-export]trimitem mesajul inapoi..%s\n", raspuns);

  if (bytes && write(fd, raspuns, bytes) == -1)
  {
    perror("[s-export] Eroare la write() catre client.\n");
    return 0;
  }
  return bytes;
}

int modificare(char *msg, int fd, int bytes)
{
  char raspuns[dim];
  char clasa[dim] = "";
  char produs[dim] = "";
  char atribut[dim] = "";
  int i, j, k;

  for (i = 13; msg[i] != ' '; i++)
  {
    clasa[i - 13] = msg[i]; // modificare : --> 13
    j = i;
  }
  j = j + 2;
  for (i = j; msg[i] != ' '; i++)
  {
    produs[i - j] = msg[i];
    k = i;
  }
  k = k + 2;
  for (i = k; msg[i] != '\0'; i++)
  {
    atribut[i - k] = msg[i];
  }
  printf("[s-modif]clasa..%s\n", clasa);
  printf("[s-modif]produs..%s\n", produs);
  printf("[s-modif]atribut..%s\n", atribut);

  printf("[s-modif]am primit comanda..%s\n", msg);

  /*pregatim mesajul de raspuns */
  bzero(raspuns, dim);
  strcat(raspuns, "cu ce vrei sa modifici la ");
  strcat(raspuns, atribut);
  strcat(raspuns, "?");
  printf("[s-modif]trimitem mesajul inapoi..%s\n", raspuns);

  if (bytes && write(fd, raspuns, bytes) == -1)
  {
    perror("[s-modif] Eroare la write() catre client.\n");
    return 0;
  }

  wait(1);

  // asteptam mesajul de la client
  char mesaj_modif[dim] = "";
  char buffer[dim];
  bytes = read(fd, mesaj_modif, sizeof(buffer));
  if (bytes < 0)
  {
    perror("[s-modif]Eroare la read() de la client.\n");
    return 0;
  }
  printf("[s-modif]Mesajul a fost receptionat...%s\n", mesaj_modif);

  // elimin newline
  for (int i = 0; i < dim; i++)
    if (mesaj_modif[i] == '\n')
      mesaj_modif[i] = '\0';

  // modificam atributul corespunzator cu informatiile noi primite(urmeaza..)

  printf("[s-modif]am modificat...%s\n", mesaj_modif);

  // trimitem confirmare la client
  bzero(raspuns, dim);
  strcat(raspuns, "am actualizat atributul ");
  strcat(raspuns, atribut);
  strcat(raspuns, " cu modificarile cerute");
  printf("[s-modif]trimitem mesajul inapoi..%s\n", raspuns);

  if (bytes && write(fd, raspuns, bytes) == -1)
  {
    perror("[s-modif] Eroare la write() catre client.\n");
    return 0;
  }

  return bytes;
}

int deconectare(char *msg, int fd, int bytes)
{
  char raspuns[dim]="";

  printf("[s-deconn]am primit comanda..%s\n", msg);

  /*pregatim mesajul de raspuns */
  bzero(raspuns, dim);
  // strcat(raspuns, "[s-deconn]S-a deconectat clientul cu descriptorul \n");
  // strcat(raspuns, fd);
  //strcat(raspuns, '!');
  snprintf(raspuns, dim, "S-a deconectat clientul cu descriptorul %d!\n", fd);
  printf("[s-deconn]trimitem mesajul inapoi..%s\n", raspuns);
  fflush(stdout);

  if (bytes && write(fd, raspuns, bytes) == -1)
  {
    perror("[s-deconn]Eroare la write() catre client.\n");
    return 0;
  }
  conectat[fd] = 0;    // clientul de la descriptorul fd s-a deconectat
  return bytes;
}

int fara_conexiune(char *msg, int fd, int bytes)
{
  char raspuns[dim];
  printf("[s-conex]am primit comanda..%s\n", msg);

  /*pregatim mesajul de raspuns */
  bzero(raspuns, dim);
  strcat(raspuns, "nu te-ai conectat!");
  printf("[s-conex]trimitem mesajul inapoi..%s\n", raspuns);

  if (bytes && write(fd, raspuns, bytes) == -1)
  {
    perror("[s-conex] Eroare la write() catre client.\n");
    return 0;
  }
  return bytes;
}

int eroare_sintactica(char *msg, int fd, int bytes)
{
  char raspuns[dim];
  printf("[s-err]am primit comanda..%s\n", msg);

  /*pregatim mesajul de raspuns */
  bzero(raspuns, dim);
  strcat(raspuns, "eroare la instructiune!");
  printf("[s-err]trimitem mesajul inapoi..%s\n", raspuns);

  if (bytes && write(fd, raspuns, bytes) == -1)
  {
    perror("[s-err] Eroare la write() catre client.\n");
    return 0;
  }
  return bytes;
}

/* programul */
int main()
{
  struct sockaddr_in server; /* structurile pentru server si clienti */
  struct sockaddr_in from;
  fd_set readfds;    /* multimea descriptorilor de citire */
  fd_set actfds;     /* multimea descriptorilor activi */
  struct timeval tv; /* structura de timp pentru select() */
  int sd, client;    /* descriptori de socket */
  int optval = 1;    /* optiune folosita pentru setsockopt()*/
  int fd;            /* descriptor folosit pentru
                  parcurgerea listelor de descriptori */
  int nfds;          /* numarul maxim de descriptori */
  int len;           /* lungimea structurii sockaddr_in */

  /* creare socket */
  if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
  {
    perror("[server] Eroare la socket().\n");
    return errno;
  }

  /*setam pentru socket optiunea SO_REUSEADDR */
  setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

  /* pregatim structurile de date */
  bzero(&server, sizeof(server));

  /* umplem structura folosita de server */
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = htonl(INADDR_ANY);
  server.sin_port = htons(PORT);

  /* atasam socketul */
  if (bind(sd, (struct sockaddr *)&server, sizeof(struct sockaddr)) == -1)
  {
    perror("[server] Eroare la bind().\n");
    return errno;
  }

  /* punem serverul sa asculte daca vin clienti sa se conecteze */
  if (listen(sd, 10) == -1)
  {
    perror("[server] Eroare la listen().\n");
    return errno;
  }

  /* completam multimea de descriptori de citire */
  FD_ZERO(&actfds);    /* initial, multimea este vida */
  FD_SET(sd, &actfds); /* includem in multime socketul creat */

  tv.tv_sec = 1; /* se va astepta un timp de 1 sec. */
  tv.tv_usec = 0;

  /* valoarea maxima a descriptorilor folositi */
  nfds = sd;

  printf("[server] Asteptam la portul %d...\n", PORT);
  fflush(stdout);

  /* servim in mod concurent clientii... */
  while (1)
  {
    /* ajustam multimea descriptorilor activi (efectiv utilizati) */
    bcopy((char *)&actfds, (char *)&readfds, sizeof(readfds));

    /* apelul select() */
    if (select(nfds + 1, &readfds, NULL, NULL, &tv) < 0)
    {
      perror("[server] Eroare la select().\n");
      return errno;
    }
    /* vedem daca e pregatit socketul pentru a-i accepta pe clienti */
    if (FD_ISSET(sd, &readfds))
    {
      /* pregatirea structurii client */
      len = sizeof(from);
      bzero(&from, sizeof(from));

      /* a venit un client, acceptam conexiunea */
      client = accept(sd, (struct sockaddr *)&from, &len);

      /* eroare la acceptarea conexiunii de la un client */
      if (client < 0)
      {
        perror("[server] Eroare la accept().\n");
        continue;
      }

      if (nfds < client) /* ajusteaza valoarea maximului */
        nfds = client;

      /* includem in lista de descriptori activi si acest socket */
      FD_SET(client, &actfds);

      printf("[server] S-a conectat clientul cu descriptorul %d, de la adresa %s.\n", client, conv_addr(from));
      fflush(stdout);

      //    acum clientul este conectat la sever(s-a facut conexiunea prin sockets),
      // mai departe verificam daca se logheaza
    }
    /* vedem daca e pregatit vreun socket client pentru a trimite raspunsul */
    for (fd = 0; fd <= nfds; fd++) /* parcurgem multimea de descriptori */
    {
      /* este un socket de citire pregatit? */ // si clientul de la descriptor connectat?
      if (fd != sd && FD_ISSET(fd, &readfds))
      {
        // verificam instructiunea pe care o primeste serverul daca este valida
        char buffer[dim];
        int bytes;
        bytes = read(fd, msg, sizeof(buffer));
        if (bytes < 0)
        {
          perror("[server]Eroare la read() de la client.\n");
          return 0;
        }
        printf("[server]Mesajul a fost receptionat...%s\n", msg);
        // verifcam instructiunile

        //          CONECTARE
        if (strncmp(msg, "conectare : ", 12) == 0)
        {
          if (conectare(msg, fd, bytes))
            printf("[server]am executat comanda conn\n");
          else
            printf("[server]sintaxa comenzii este: conectare : <nume>\n");
        } //        AFISARE
        else if (conectat[fd] == 1)
        {
          if (strcmp(msg, "afisare") == 0)
          {
            if (afisare(msg, fd, bytes))
              printf("[server]am executat comanda afisare\n");
            else
              printf("[server]sintaxa comenzii este: afisare\n");
          } //        EXPORT
          else if (strcmp(msg, "export") == 0 && conectat[fd] == 1)
          {
            if (export(msg, fd, bytes))
              printf("[server]am executat comanda export\n");
            else
              printf("[server]sintaxa comenzii este: export\n");
          } //        MODIFICARE
          else if (strncmp(msg, "modificare : ", 13) == 0 && conectat[fd] == 1) //"modificare : "--> 13
          {
            if (modificare(msg, fd, bytes))
              printf("[server]am executat comanda modificare\n");
            else
              printf("[server]sintaxa comenzii este: modificare : <categorie> <denumire_produs> <atribut_produs>\n");
          } //        DECONECTARE
          else if (strcmp(msg, "deconectare") == 0 && conectat[fd] == 1)
          {
            if (deconectare(msg, fd, bytes))
            {
              printf("[server]am executat comanda deconn\n");
              close(fd);           /* inchidem conexiunea cu clientul */
              FD_CLR(fd, &actfds); /* scoatem si din multime */
            }
            else
              printf("[server]sintaxa comenzii este: deconectare\n");
          }
          else
            eroare_sintactica(msg,fd,bytes);
        }
        else
          fara_conexiune(msg, fd, bytes);
      }
    } /* for */
  } /* while */
} /* main */