#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "輸入有誤\n");
        return -1;
    }

    int index0 = 0, index1 = 0, mode = 0;
    char *command, *str, *args0[11], *args1[11];

    command = argv[1];
    str = strtok(command, " ");
    args0[index0++] = str;
    while (1) {
        str = strtok(NULL, " ");
        if (str == NULL) {
            args0[index0] = NULL;
            args1[index1] = NULL;
            break;
        } else if (strcmp(str, "|") == 0) {
            mode = 1;
        } else {
            if (mode == 0) {
                args0[index0++] = str;
            } else {
                args1[index1++] = str;
            }
        }
    }
    // for (int i=0; i<index0; i++) printf("%s\n", args0[i]);
    // for (int i=0; i<index1; i++) printf("%s\n", args1[i]);

    int fd[2];
    if (pipe(fd) == -1) {
        fprintf(stderr, "Pipe failed");
        return -1;
    }

    pid_t pid2 = fork();
    if (pid2 < 0) {
        fprintf(stderr, "Fork failed");
        return -1;
    } else if (pid2 == 0) { /*child process */
        close(fd[0]);
        close(1);
        dup(fd[1]);
        close(fd[1]);
        int ret = execvp(args0[0], args0);
        if (ret == -1) {
            fprintf(stderr, "Fork failed");
            return -1;
        }
    } else { /* parent process */
        close(fd[1]);
        close(0);
        dup(fd[0]);
        close(fd[0]);
        int ret = execvp(args1[0], args1);
        if (ret == -1) {
            fprintf(stderr, "Fork failed");
            return -1;
        }
    }

    return 0;
}
