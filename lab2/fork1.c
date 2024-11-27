#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>

int main()
{
    pid_t pid_fiu, pid_parinte;
    int n_pid_fiu;
    sigset_t mask;
    sigfillset(&mask);
    sigdelset (&mask, SIGQUIT);

    //procesul fiu
    if(-1 == (pid_fiu=fork()) )
    {
        perror("Eroare la fork");
        return 1;
    }

    if(pid_fiu == 0)
    {
        n_pid_fiu=getpid();
        if(n_pid_fiu%2 == 0)
        {
            wait(10);
        }
    }
    else 
    {   
        printf("lost");
        pid_parinte=getppid();
        kill(pid_parinte, SIGKILL);
    }
}


/*
int main()
{
    pid_t pid_fiu, pid_parinte;

    //procesul fiu
    if(-1 == (pid_fiu=fork()) )
    {
        perror("Eroare la fork");
        return 1;
    }

    if(getpid()%2 == 0)
    {   
        if(pid_fiu == 0) //am apelat procesul fiu
        {
            printf("fortune");
        }
    }
    else //tot in parinte suntem
    {   
        printf("lost");
        pid_parinte=getppid();
        kill(pid_parinte, SIGKILL);
    }
}
*/
