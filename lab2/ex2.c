#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>

void sighandler (int sig) {
	printf("Got signal\n");
	fflush(stdout);
}

int main(){
	signal(SIGUSR2, sighandler);
	while(1);
}
