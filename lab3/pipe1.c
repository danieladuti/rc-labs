//tatal scrie intr-un pipe un nr
//fiul verifica daca numarul este prim si transmite tatalui prin pipe(yes/no)
//tatl va afisa raspunsul primit

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <math.h>

int nrPrim(int n)
{
    int /*radical = sqrt(n),*/ numarDivizori = 0;
    for(int d = 1; d <= n/2; d++) 
    {
        if(n % d == 0) 
        { 
            int dprim = n / d; 
            if(dprim != d) 
                numarDivizori += 2;
            else 
                numarDivizori++;
        }
    }
    if(numarDivizori == 2) 
        return 1;
    else
        return 2;
}

int main(void)
{
    int pid, p[2], nr;

    // Creare pipe
    if(-1 == pipe(p) )
    {
        perror("Eroare la crearea canalului anonim");  exit(1);
    }

    // Creare proces fiu
    if(-1 == ( pid=fork() ) )
    {
        perror("Eroare la crearea unui proces fiu");  exit(2);
    }

    if(pid)
    { 
        /* Tatăl închide capătul de citire din canal. */

        close(p[0]);

        
        /* Tatăl citește un numar de la tastatură, terminand cu CTRL+D (i.e., EOF în UNIX/Linux),
        *  și il transmite fiului, prin canal */
        /*printf("Introduceti un numar, iar la final apasati CTRL+D.\n");
        if( EOF != (nr=getchar()) )
        {
            write(p[1],&nr,1);
        }
        */

        nr = 23;
        write(p[1],&nr,1);

        /* Tatăl își închide capătul de scriere în canal, pentru ca fiul să poată citi EOF din canal (!!!) */

        close(p[1]);
        wait(NULL);//asteptam sa verifice fiul daca nr este prim sau nu
    }
    else
    {   /* Fiul își închide capătul de scriere în canal, pentru ca ulterior să poată citi EOF din canal (!!!) */
        close(p[1]);

        /* Fiul citește nr, până când detectează EOF în canal, ... */
        if(read(p[0],&nr,1) != 0 )
        {
            if(nrPrim(p[0]) == 1)
            {
                nr=1;
                write(p[1],&nr,1);
            }
            else
            {
                nr=2;
                write(p[1],&nr,1);
            }
        }
        close(p[0]);
        close(p[1]);

    }
    close(p[1]); //inchidem capatul de scriere
    if(read(p[0], &nr,1) == 1)
        printf("yes");
    else
        printf("no");
    return 0;
}


