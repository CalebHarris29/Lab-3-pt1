#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

/*
 * pipes_processes3.c
 * Program to simulate: cat scores | grep <pattern> | sort
 * Usage: ./pipes_processes3 28
 */

int main(int argc, char *argv[])
{
    int pipe1[2];   /* pipe for cat -> grep  */
    int pipe2[2];   /* pipe for grep -> sort */
    pid_t pid1, pid2;

    /* we expect exactly one argument: the grep pattern */
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <pattern>\n", argv[0]);
        exit(1);
    }

    /* first pipe: parent (cat) writes, child (grep) reads */
    if (pipe(pipe1) == -1) {
        perror("pipe1");
        exit(1);
    }

    pid1 = fork();
    if (pid1 < 0) {
        perror("fork1");
        exit(1);
    }

    if (pid1 == 0) {
        /*************** CHILD: will handle grep and create sort ***************/
        /* second pipe: grep writes to sort */
        if (pipe(pipe2) == -1) {
            perror("pipe2");
            exit(1);
        }

        pid2 = fork();
        if (pid2 < 0) {
            perror("fork2");
            exit(1);
        }

        if (pid2 == 0) {
            /*************** CHILD'S CHILD: becomes sort ***************/
            /* sort reads from pipe2[0], writes to stdout */

            close(pipe2[1]);              /* not writing in sort */
            if (dup2(pipe2[0], STDIN_FILENO) == -1) {
                perror("dup2 sort stdin");
                exit(1);
            }
            close(pipe2[0]);

            execlp("sort", "sort", (char *)NULL);
            perror("execlp sort");
            exit(1);
        }

        /*************** CHILD continues: becomes grep ***************/
        /* grep reads from pipe1[0], writes to pipe2[1] */

        /* stdin <- pipe1[0] */
        close(pipe1[1]);                  /* not writing to pipe1 in grep */
        if (dup2(pipe1[0], STDIN_FILENO) == -1) {
            perror("dup2 grep stdin");
            exit(1);
        }
        close(pipe1[0]);

        /* stdout -> pipe2[1] */
        close(pipe2[0]);                  /* not reading from pipe2 in grep */
        if (dup2(pipe2[1], STDOUT_FILENO) == -1) {
            perror("dup2 grep stdout");
            exit(1);
        }
        close(pipe2[1]);

        /* exec grep <pattern> */
        execlp("grep", "grep", argv[1], (char *)NULL);
        perror("execlp grep");
        exit(1);
    }
    else {
        /*************** PARENT: becomes cat scores ***************/
        /* cat writes to pipe1[1]; its stdout is redirected to pipe1 */

        close(pipe1[0]);                  /* not reading from pipe1 in cat */
        if (dup2(pipe1[1], STDOUT_FILENO) == -1) {
            perror("dup2 cat stdout");
            exit(1);
        }
        close(pipe1[1]);

        execlp("cat", "cat", "scores", (char *)NULL);
        perror("execlp cat");
        exit(1);
    }

    return 0;
}
