#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#define ACL S_IRWXU | S_IRWXG | S_IRWXO
#define WRITE O_CREAT | O_TRUNC | O_RDWR
#define APPEND O_CREAT | O_RDWR | O_APPEND

/* ======================================== */
int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "輸入有誤\n");
        return -1;
    }

    int index = 0;
    char *command, *str, *args[11];

    command = argv[1];
    str = strtok(command, " ");
    args[index++] = str;
    while (1) {
        str = strtok(NULL, " ");
        if (str == NULL) {
            args[index] = NULL;
            break;
        } else if (strcmp(str, ">") == 0) {
            str = strtok(NULL, " ");
            if (str == NULL) {
                fprintf(stderr, "syntax error\n");
                return -1;
            }
            close(1);
            open(str, WRITE, ACL);
        } else if (strcmp(str, ">>") == 0) {
            str = strtok(NULL, " ");
            if (str == NULL) {
                fprintf(stderr, "syntax error\n");
                return -1;
            }
            close(1);
            open(str, APPEND, ACL);
        } else if (strcmp(str, "<") == 0) {
            str = strtok(NULL, " ");
            if (str == NULL) {
                fprintf(stderr, "syntax error\n");
                return -1;
            }
            close(0);
            int fd = open(str, O_RDONLY);
            if (fd == -1) {
                fprintf(stderr, "%s: No such file or directory\n", str);
                return -1;
            }
        } else {
            args[index++] = str;
        }
    }
    
    int ret = execvp(args[0], args);
    if (ret == -1) {
        fprintf(stderr, "execl error\n");
        return -1;
    }
    return 0;
}
