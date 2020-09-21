#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{

    pid_t pid;
    int fd[2];
    char str[81];

    close(0); close(1);

    pipe(fd);

    pid  = fork();

    if(pid == 0){  
        (void) execl("/bin/date", "/bin/date", 0);
    } else if (pid > 0){
       bzero(str,81);
       read (0, str, 81);
       write (2, str, strlen(str));    
    }
    return 0;
}

