#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define   BUF_SIZE   100

int main() 
{
	char   buf[BUF_SIZE];
    pid_t pid;

    pid= fork();

    if (pid < 0){
		printf("\nFork Failed\n");
		return 1;
    } 

	else if (pid == 0){ //child
		//execlp("/bin/ps", "ps -l", NULL);
		printf("Child\n");
	}
	
	else{ // parent
		wait(NULL);
		printf("Child Complete\n");
        sprintf(buf,"Child is done from pid %d\n", pid);
		write(1, buf, strlen(buf));
    }

	return 0;
}