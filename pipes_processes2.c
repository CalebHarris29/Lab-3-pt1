#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main()
{
    int fd[2];            // pipe for cat -> grep
    pid_t pid;

    // Create the pipe
    if (pipe(fd) < 0) {
        perror("pipe");
        exit(1);
    }

    // Fork a child for grep
    pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(1);
    }

    if (pid == 0) {
        /************* CHILD PROCESS (executes grep) *************/
        
        close(fd[1]); // Close write end, child will read from pipe
        
        // Redirect stdin to read from pipe
        if (dup2(fd[0], STDIN_FILENO) < 0) {
            perror("dup2");
            exit(1);
        }
        close(fd[0]);

        // Execute grep for a fixed pattern (example: "Lakers")
        execlp("grep", "grep", "Lakers", (char*)NULL);

        perror("exec grep");
        exit(1);
    }
    else {
        /************* PARENT PROCESS (executes cat scores) *************/
        
        close(fd[0]); // Close read end, parent writes to pipe

        // Redirect stdout to write into pipe
        if (dup2(fd[1], STDOUT_FILENO) < 0) {
            perror("dup2");
            exit(1);
        }
        close(fd[1]);

        // Execute: cat scores
        execlp("cat", "cat", "scores", (char*)NULL);

        perror("exec cat");
        exit(1);
    }

    return 0;
}
