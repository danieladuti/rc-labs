/*
server.c

modificari :
- verifica comenzile daca sunt valide sau nu si afiseaza un mesaj corespunzator
- respinge :conectare : ,afisare , export , deconectare , si modificare daca nu are destule argumente si
 daca vrei sa te conectezo de mai multe ori
- verifica numele restaurantelor in fisierul txt: restaurante.txt
- am introdus comenzile inserare si stergere(nu ruleaza nimic pt ca trebuie implementate functionalizatile URGENT)
- am modificat la modificare sa aiba numai modificare : <denumire_produs> <atribut_produs(ingrediente/pret)>, fara clasa
pt facilitarea cautarii produselor dupa nume

imbunatatiri ulterioare:
- functionalitatile fiecareia dintre comenzi
  --> adaugarea comenzilor: inserare produs si stergere produs
  --> implementarea meniului in ceva????(fisier.txt sau excel)
  --> sa poata modifica/insera in meniu
  --> afisare/export in terminal si in fisier text a meniului

- bonus: securitate pentru username + parola (macar bulinute cand se pune parola) --> sa spun ca "stiu ca este compromisa aplicatia respectiva, dar restul nu sunt decat daca reuseste sa se infiltreze si acolo"
  --> bonus de securitate avansata(vezi cum se poate -- poate sa nu afiseze deloc in terminal)
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
#include <stdlib.h>
#include <ctype.h>
#include <sys/wait.h>
#include <fcntl.h> // O_RDONLY

#define dim 1048   // pt dimensiunea mesajelor
int conectat[100]; // verificam daca clientul este connectat sau nu
char msg[dim];     // mesajul de la client

/* portul folosit */
#define PORT 2793

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

/*-------------------------------------------------------------------------------------------------


                                      CONECTARE


---------------------------------------------------------------------------------------------------*/

int username_gasit(char nume[])
{
  char buffer[dim];
  int bytesf;
  int flag = 0;
  int fd_username;

  // elimin newline-ul adaugat de la read
  int i = 0;
  while (i < strlen(nume))
  {
    if (nume[i] == '\n')
    {
      break;
    }
    i++;
  }
  nume[i] = '\0';
  printf("[s-username]username-ul a fost modificat...%s\n", nume);

  // deschid fisierul cu useri
  if ((fd_username = open("restaurante.txt", O_RDONLY)) == -1)
  {
    printf("[s-username]eroare la deschiderea fisierului");
  }

  // char *username = &nume[0]; // iau username din comanda
  if ((fd_username = open("restaurante.txt", O_RDONLY)) == -1)
  {
    printf("[s-username]eroare la deschiderea fisierului cu usernames.");
  }

  bytesf = read(fd_username, buffer, dim);
  buffer[bytesf] = '\0';

  printf("[s-username]fiserul contine: %s\n", buffer);
  // printf("%s", buffer);

  // verif daca exista vreun username in fisier
  char *token = strtok(buffer, "\n");
  while (token != NULL)
  {
    printf("[s-username]am intrat in while!!!!!\n");
    fflush(stdout);
    if (strcmp(token, nume) == 0)
    {
      printf("[s-username]token: %s \n", token);
      fflush(stdout);
      flag = 1;
      break;
    }
    token = strtok(NULL, "\n");
  }

  return flag;
}

int conectare(char *msg, int fd, int bytes)
{
  char raspuns[dim];
  char nume[dim] = "";

  for (int i = 12; i < dim; i++)
    nume[i - 12] = msg[i]; // conectare : --> 12

  printf("[s-conn]am primit comanda..%s, cu numele %s\n", msg, nume);
  if (username_gasit(nume) == 1) // am gasit username-ul
  {
    printf("[s-conn]salutare, %s!\n", nume);

    /*pregatim mesajul de raspuns */
    bzero(raspuns, dim);
    strcat(raspuns, "s-a conectat cu succes ");
    strcat(raspuns, nume);
    strcat(raspuns, ".");
    printf("[s-conn]trimitem mesajul inapoi..%s\n", raspuns);

    if (bytes && write(fd, raspuns, bytes) == -1)
    {
      perror("[s-conn] Eroare la write() catre client.\n");
      return 0;
    }
    conectat[fd] = 1; // clientul de la descriptorul client s-a conectat
  }
  else
  {
    printf("[s-conn]username-ul nu e valid!\n");

    /*pregatim mesajul de raspuns */
    bzero(raspuns, dim);
    strcat(raspuns, "username-ul ");
    strcat(raspuns, nume);
    strcat(raspuns, " nu e valid!");
    printf("[s-conn]trimitem mesajul inapoi..%s\n", raspuns);

    if (bytes && write(fd, raspuns, bytes) == -1)
    {
      perror("[s-conn] Eroare la write() catre client.\n");
      return 0;
    }
    conectat[fd] = 0; // clientul de la descriptorul client NU e conectat
  }

  return bytes;
}

/*-------------------------------------------------------------------------------------------------


                                      AFISARE


---------------------------------------------------------------------------------------------------*/

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

/*-------------------------------------------------------------------------------------------------


                                      EXPORT


---------------------------------------------------------------------------------------------------*/

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

/*-------------------------------------------------------------------------------------------------


                                      INSERARE


---------------------------------------------------------------------------------------------------*/

int verif_ins(char *msg, int fd, int bytes) // verificam daca au fost introduse toate argumentele pt comanda inserare
{
  int nr_spatii = 0;

  for (int i = 0; i < strlen(msg); i++)
  {
    if (msg[i] == ' ')
    {
      nr_spatii++;
    }
  }

  if (nr_spatii == 4)
  {
    return 0;
  }
  else
  {
    return 1;
  }
}

int inserare(char *msg, int fd, int bytes)
{
  char raspuns[dim];
  char nume_produs[dim] = "";
  char ingrediente[dim] = "";
  char pret[dim] = "";
  int i, j, k;

  for (i = 11; msg[i] != ' '; i++)
  {
    nume_produs[i - 11] = msg[i]; // "inserare : "--> 11
    j = i;
  }
  j = j + 2;
  for (i = j; msg[i] != ' '; i++)
  {
    ingrediente[i - j] = msg[i];
    k = i;
  }
  k = k + 2;
  for (i = k; msg[i] != '\0'; i++)
  {
    pret[i - k] = msg[i];
  }
  printf("[s-ins]produs..%s\n", nume_produs);
  printf("[s-ins]ingrediente..%s\n", ingrediente);
  printf("[s-ins]pret..%s\n", pret);
  fflush(stdout);
  printf("[s-ins]am primit comanda..%s\n", msg);
  fflush(stdout);

  // verificam daca exista un produs tot cu acest nume(urmeaza..)-------------------------------
  //-------------------------------------------------------------------------------------------
  //---------------------------------URGENT----------------------------------------------------
  //-------------------------------------------------------------------------------------------
  //-------------------------------------------------------------------------------------------
  //-------------------------------------------------------------------------------------------
  /*
    if (verif_nume_produs(nume_produs) == 1) // exista deja produsul cu numele acesta URGENT
    {
      printf("[s-ins]produsul %s exista deja! nu-l poti insera din nou!");
      bzero(raspuns, dim);
      strcat(raspuns, "produsul ");
      strcat(raspuns, nume_produs);
      strcat(raspuns, " exista deja! nu-l poti insera din nou!");
    }
    else // produsul nu exista in meniu
    {
      if (insereaza_produs(nume_produs, ingrediente, pret) == 1) // URGENT functie pt inserare produs in meniu
      {
        printf("[s-ins]produsul %s a fost adaugat cu succes!", nume_produs);
        bzero(raspuns, dim);
        strcat(raspuns, "produsul ");
        strcat(raspuns, nume_produs);
        strcat(raspuns, " a fost adaugat cu succes!");
      }
      else
      {
        printf("[s-ins]eroare la adaugarea produsului %s", nume_produs);
        bzero(raspuns, dim);
        strcat(raspuns, "produsul ");
        strcat(raspuns, nume_produs);
        strcat(raspuns, " nu s-a adaugat, mai incearca o data!");
      }
    }
    */

  // lasam asa sa vedem daca ruleaza.. URGENT
  printf("[s-ins]produsul %s a fost adaugat cu succes!\n", nume_produs);
  bzero(raspuns, dim);
  strcat(raspuns, "produsul ");
  strcat(raspuns, nume_produs);
  strcat(raspuns, " a fost adaugat cu succes!");

  printf("[s-ins]trimitem mesajul inapoi..%s\n", raspuns);

  if (bytes && write(fd, raspuns, bytes) == -1)
  {
    perror("[s-ins] Eroare la write() catre client.\n");
    return 0;
  }

  return bytes;
}

/*-------------------------------------------------------------------------------------------------


                                      MODIFICARE


---------------------------------------------------------------------------------------------------*/
int verif_modificare(char *msg, int fd, int bytes) // verificam daca au fost introduse toate argumentele pt comanda modificare
{
  int nr_spatii = 0;

  for (int i = 0; i < strlen(msg); i++)
  {
    if (msg[i] == ' ')
    {
      nr_spatii++;
    }
  }

  if (nr_spatii == 3)
  {
    return 0;
  }
  else
  {
    return 1;
  }
}

int modificare(char *msg, int fd, int bytes)
{
  char raspuns[dim];
  char produs[dim] = "";
  char atribut[dim] = "";
  int i, j;

  for (i = 13; msg[i] != ' '; i++)
  {
    produs[i - 13] = msg[i]; // modificare : --> 13
    j = i;
  }
  j = j + 2;
  for (i = j; msg[i] != '\0'; i++)
  {
    atribut[i - j] = msg[i];
  }
  printf("[s-modif]produs..%s\n", produs);
  printf("[s-modif]atribut..%s\n", atribut);

  printf("[s-modif]am primit comanda..%s\n", msg);
  fflush(stdout);

  /*pregatim mesajul de raspuns */
  bzero(raspuns, dim);
  strcat(raspuns, "cu ce vrei sa modifici la ");
  strcat(raspuns, atribut);
  strcat(raspuns, "?");
  printf("[s-modif]trimitem mesajul inapoi..%s\n", raspuns);
  fflush(stdout);

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
  strcat(raspuns, " cu ");
  strcat(raspuns, mesaj_modif);
  strcat(raspuns, ".");
  printf("[s-modif]trimitem mesajul inapoi..%s\n", raspuns);

  if (bytes && write(fd, raspuns, bytes) == -1)
  {
    perror("[s-modif] Eroare la write() catre client.\n");
    return 0;
  }

  return bytes;
}

/*-------------------------------------------------------------------------------------------------


                                      STERGERE


---------------------------------------------------------------------------------------------------*/

int verif_sterg(char *msg, int fd, int bytes) // verificam daca au fost introduse toate argumentele pt comanda stergere
{
  int nr_spatii = 0;

  for (int i = 0; i < strlen(msg); i++)
  {
    if (msg[i] == ' ')
    {
      nr_spatii++;
    }
  }

  if (nr_spatii == 2)
  {
    return 0;
  }
  else
  {
    return 1;
  }
}

int stergere(char *msg, int fd, int bytes)
{
  char raspuns[dim];
  char produs[dim] = "";
  int i;

  for (i = 11; msg[i] != '\0'; i++)
  {
    produs[i - 11] = msg[i]; // "stergere : "--> 11
  }

  printf("[s-sterg]produs..%s\n", produs);
  fflush(stdout);
  printf("[s-sterg]am primit comanda..%s\n", msg);
  fflush(stdout);

  // verificam daca exista un produs cu acest nume(urmeaza..)-------------------------------
  //-------------------------------------------------------------------------------------------
  //---------------------------------URGENT----------------------------------------------------
  //-------------------------------------------------------------------------------------------
  //-------------------------------------------------------------------------------------------
  //-------------------------------------------------------------------------------------------
  /*
  if (verif_nume_produs(produs) == 1) // exista deja produsul cu numele acesta -- vezi functia pt username URGENT
  {
    printf("[s-sterg]produsul %s exista!", produs);
    if (sterge_produs(produs) == 1) // produsul a fost sters cu succes
    {
      printf("[s-sterg]produsul %s a fost sters cu succes!");
      bzero(raspuns, dim);
      strcat(raspuns, "produsul ");
      strcat(raspuns, produs);
      strcat(raspuns, " a fost sters cu succes!");
    }
    else // eroare la stergere
    {
      printf("[s-sterg]eroare la stergerea produsului %s", produs);
      bzero(raspuns, dim);
      strcat(raspuns, "produsul ");
      strcat(raspuns, produs);
      strcat(raspuns, " nu s-a sters, mai incearca o data!");
    }
  }
  else // produsul nu exista in meniu
  {
    printf("[s-sterg]produsul NU %s exista!", produs);
    bzero(raspuns, dim);
    strcat(raspuns, "produsul ");
    strcat(raspuns, produs);
    strcat(raspuns, " NU exista! deci nu ai cum sa-l stergi");
  }
  */

  printf("[s-sterg]produsul %s a fost sters cu succes!\n", produs);
  bzero(raspuns, dim);
  strcat(raspuns, "produsul ");
  strcat(raspuns, produs);
  strcat(raspuns, " a fost sters cu succes!");

  printf("[s-sterg]trimitem mesajul inapoi..%s\n", raspuns);

  if (bytes && write(fd, raspuns, bytes) == -1)
  {
    perror("[s-sterg] Eroare la write() catre client.\n");
    return 0;
  }

  return bytes;
}

/*-------------------------------------------------------------------------------------------------


                                      DECONECTARE


---------------------------------------------------------------------------------------------------*/

int deconectare(char *msg, int fd, int bytes)
{
  char raspuns[dim] = "";

  printf("[s-deconn]am primit comanda..%s\n", msg);

  /*pregatim mesajul de raspuns */
  bzero(raspuns, dim);
  // strcat(raspuns, "[s-deconn]S-a deconectat clientul cu descriptorul \n");
  // strcat(raspuns, fd);
  // strcat(raspuns, '!');
  snprintf(raspuns, dim, "S-a deconectat clientul cu descriptorul %d!\n", fd);
  printf("[s-deconn]trimitem mesajul inapoi..%s\n", raspuns);
  fflush(stdout);

  if (bytes && write(fd, raspuns, bytes) == -1)
  {
    perror("[s-deconn]Eroare la write() catre client.\n");
    return 0;
  }
  conectat[fd] = 0; // clientul de la descriptorul fd s-a deconectat
  return bytes;
}

/*-------------------------------------------------------------------------------------------------


                                      verificarea instructiunilor
                                      --> fara_conexiune
                                      --> eroare_sintactica


---------------------------------------------------------------------------------------------------*/
int fara_conexiune(char *msg, int fd, int bytes)
{
  char raspuns[dim];
  printf("[s-err]am primit comanda..%s\n", msg);

  /*pregatim mesajul de raspuns */
  bzero(raspuns, dim);
  strcat(raspuns, "nu te-ai conectat!");
  printf("[s-err]trimitem mesajul inapoi..%s\n", raspuns);

  if (bytes && write(fd, raspuns, bytes) == -1)
  {
    perror("[s-err] Eroare la write() catre client.\n");
    return 0;
  }
  return bytes;
}

int eroare_sintactica(char *msg, int fd, int bytes)
{
  char raspuns[dim];
  int on = 0;
  printf("[s-instr]am primit comanda..%s\n", msg);

  if (msg[13] == '\n')
    printf("da");

  /*pregatim mesajul de raspuns */
  bzero(raspuns, dim);

  //----------------------------- CONECTARE-----------
  if (strncmp(msg, "conectare : ", 12) == 0 && isalpha(msg[12]) == 0) // issalpha !=0 caracter alfabetic
  {
    on = 1;
    printf("[s-instr-conn1]sintaxa comenzii este: \"conectare : <nume>\"\n");
    strcat(raspuns, "sintaxa comenzii este: \"conectare : <nume>\"");
  }
  if (strncmp(msg, "conectare : ", 12) == 0 && isalpha(msg[12]) != 0 && conectat[fd] == 1) // isspace !=0 caracter alfabetic
  {
    on = 1;
    printf("[s-instr-conn2]te-ai conectat deja! nu te mai poti conecta inca o data!\n");
    strcat(raspuns, "te-ai conectat deja! nu te mai poti conecta inca o data!");
  }

  //----------------------------- AFISARE-----------
  if (strncmp(msg, "afisare", 7) == 0 && isspace(msg[7]) != 0 && conectat[fd] == 1) // daca are ceva dupa afisare
  {
    on = 1;
    printf("[s-instr-afis]sintaxa comenzii este: \"afisare\"\n");
    strcat(raspuns, "sintaxa comenzii este: \"afisare\"");
  }

  //----------------------------- EXPORT-----------
  if (strncmp(msg, "export", 6) == 0 && isspace(msg[6]) != 0 && conectat[fd] == 1)
  {
    on = 1;
    printf("[s-instr-exp]sintaxa comenzii este: \"export\"\n");
    strcat(raspuns, "sintaxa comenzii este: \"export\"");
  }

  //-----------------------------INSERARE-----------
  int ins = verif_ins(msg, fd, bytes);
  if (strncmp(msg, "inserare", 8) == 0 && ins == 1 && conectat[fd] == 1)
  {
    on = 1;
    printf("[s-instr-ins]sintaxa comenzii este: \"inserare : <denumire_produs> <ingredient_1,ingredient_2> <pret>\"\n");
    strcat(raspuns, "sintaxa comenzii este: \"inserare : <denumire_produs> <ingredient_1,ingredient_2> <pret>\"");
  }

  if (strncmp(msg, "inserare", 8) == 0 && ins == 1 /*&& verif_nume_produs(nume_produs) == 1*/ && conectat[fd] == 1) // isspace !=0 caracter alfabetic
  {                                                                                                                 // URGENT vezi daca e ok cu verificarea numelui aici. de unde imi dau seama ca exista deja numele asta sau nu? trebuie sa mai verific aici? -- nu cred ca deja verifica in functia mare si zice daca exista deja produsul sau nu
    on = 1;
    printf("[s-instr-ins2]ai inserat deja acest produs!\n");
    strcat(raspuns, "ai inserat deja acest produs!");
  }

  //----------------------------- MODIFICARE-----------
  int modif = verif_modificare(msg, fd, bytes);
  if (strncmp(msg, "modificare", 10) == 0 && modif == 1 && conectat[fd] == 1)
  {
    on = 1;
    printf("[s-instr-modif]sintaxa comenzii este: \"modificare : <categorie> <denumire_produs> <atribut_produs(ingrediente/pret)>\"\n");
    strcat(raspuns, "sintaxa comenzii este: \"modificare : <categorie> <denumire_produs> <atribut_produs(ingrediente/pret)>\"");
  }

  //-----------------------------STERGERE-----------
  int sterg = verif_sterg(msg, fd, bytes);
  if (strncmp(msg, "stergere", 8) == 0 && sterg == 1 && conectat[fd] == 1)
  {
    on = 1;
    printf("[s-instr-sterg]sintaxa comenzii este: \"stergere : <denumire_produs>\"\n");
    strcat(raspuns, "sintaxa comenzii este: \"stergere : <denumire_produs>\"");
  }

  //----------------------------- DECONECTARE-----------
  if (strncmp(msg, "deconectare", 11) == 0 && isspace(msg[11]) != 0 && conectat[fd] == 1)
  {
    on = 1;
    printf("[s-instr-deconn]sintaxa comenzii este: \"deconectare\"\n");
    strcat(raspuns, "sintaxa comenzii este: \"deconectare\"");
  }

  if (on == 1)
  {
    wait(1);

    printf("[s-instr]trimitem mesajul inapoi..%s\n", raspuns);

    if (bytes && write(fd, raspuns, bytes) == -1)
    {
      perror("[s-instr] Eroare la write() catre client.\n");
      return 0;
    }
    return bytes;
  }
  else
    return 0;
}

/*------------------------------------------------------------------------------------------------


                                      programul


--------------------------------------------------------------------------------------------------*/
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

  /* servim in mod concurent (!?) clientii... */
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
        // verificam instructiunile
        if (eroare_sintactica(msg, fd, bytes) == 0)
        {
          //          CONECTARE
          if (strncmp(msg, "conectare : ", 12) == 0 && isalpha(msg[12]) != 0 && conectat[fd] == 0)
          {
            if (conectare(msg, fd, bytes))
              printf("[server]am executat comanda conn\n");
            else
              printf("[server]eroare la comanda conn\n");
          } //        AFISARE
          else if (conectat[fd] == 1)
          {
            if (strcmp(msg, "afisare") == 0 && msg[7] != '\n') // verif daca am scris doar afisare, daca am scris "afisare " o sa dea eroare
            {
              if (afisare(msg, fd, bytes))
                printf("[server]am executat comanda afisare\n");
              else
                printf("[server]eroare la afisare\n");
            } //        EXPORT
            else if (strcmp(msg, "export") == 0 && msg[7] != '\n') // verif daca am scris doar export
            {
              if (export(msg, fd, bytes))
                printf("[server]am executat comanda export\n");
              else
                printf("[server]eroare la export export\n");
            } //        INSERARE
            else if (strncmp(msg, "inserare : ", 11) == 0) //"inserare : "--> 11
            {
              if (verif_ins(msg, fd, bytes) == 0) // daca da 1 inseamna ca clientul nu a introdus destule argumente pt comanda inserare.
              {
                if (inserare(msg, fd, bytes))
                  printf("[server]am executat comanda inserare\n");
                else
                  printf("[server]eroare la inserare\n");
              }
            } //        MODIFICARE
            else if (strncmp(msg, "modificare : ", 13) == 0) //"modificare : "--> 13
            {
              if (verif_modificare(msg, fd, bytes) == 0) // daca da 1 inseamna ca clientul nu a introdus destule argumente pt comanda modificare.
              {
                if (modificare(msg, fd, bytes))
                  printf("[server]am executat comanda modificare\n");
                else
                  printf("[server]eroare la modificare\n");
              }
            } //        stergere
            else if (strncmp(msg, "stergere : ", 11) == 0) //"stergere : "--> 11
            {
              if (verif_sterg(msg, fd, bytes) == 0) // daca da 1 inseamna ca clientul nu a introdus destule argumente pt comanda stergere.
              {
                if (stergere(msg, fd, bytes))
                  printf("[server]am executat comanda stergere\n");
                else
                  printf("[server]eroare la stergere\n");
              }
            } //        DECONECTARE
            else if (strcmp(msg, "deconectare") == 0 && msg[11] != '\n') // verif daca am scris doar deconectare
            {
              if (deconectare(msg, fd, bytes))
              {
                printf("[server]am executat comanda deconn\n");
                close(fd);           /* inchidem conexiunea cu clientul */
                FD_CLR(fd, &actfds); /* scoatem si din multime */
              }
              else
                printf("[server]eroare la deconectare\n");
            }
          }
          else
            fara_conexiune(msg, fd, bytes);
        } // if verif cu instructiuni
      } // if cu socket pregatit
    } /* for */
  } /* while */
} /* main */