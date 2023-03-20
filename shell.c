#include <stdio.h>
#include <stdlib.h>  
#include <string.h>
#include <unistd.h>
#define MAX_LINE 80 /*The maximum length command*/
#define MAX_INPUT 1024

int main(void) {
    char *args[MAX_LINE/2 + 1]; /*command line arguments*/
    int should_run = 1; /*flag to determine when to exit program*/
    while (should_run) {
        int need_to_wait = 1;
        int index = 0;
        char *p, buff[MAX_INPUT];
        /* reading user input */
        printf("\033[33mosh>\033[0m ");
        fflush(stdout);
        gets(buff);
        /* split to args */
        p = strtok(buff, " ");
        args[index] = p;
        if (args[index] == NULL) continue;
        while (1) {
            p = strtok(NULL, " ");
            if (p == NULL) break;
            else if (strcmp(p, "&") == 0) need_to_wait = 0;
            else args[++index] = p;
        }
        args[++index] = NULL;
        // for (int i=0; i<index; i++) printf("%s\n", args[i]);
        // printf("%d\n", need_to_wait);

        /* After reading user input, the steps are:
        * (1) fork a child process using fork()
        * (2) the child process will invoke execv()
        * (3) if command included &, parent will invoke wait() */
        if (strcmp(args[0], "exit") == 0) {
            should_run = 0;
        } else {
            pid_t pid;
            pid = fork();
            if (pid < 0) {
                fprintf(stderr, "Fork Failed\n");
                return 1;
            } else if (pid == 0) {
                if (execvp(args[0], args)) {
                    fprintf(stderr, "Invalid Command\n");
                    return 1;
                }
            } else {
                if (need_to_wait) while(wait(NULL) != pid);
                else printf("[1]%d\n", pid);
            }
        }
    }
    return 0;
}
