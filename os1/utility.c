#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include "myshell.h"

extern char **environ;

void execute_internal(char **args, int background, int redirect, char *outfile, int append) {
    FILE *out = NULL;

    if (redirect && outfile != NULL) {
        if (append)
            out = freopen(outfile, "a", stdout);
        else
            out = freopen(outfile, "w", stdout);
    }

    // ... остальной код без изменений

    if (strcmp(args[0], "cd") == 0) {
        if (args[1]) {
            if (chdir(args[1]) != 0)
                perror("cd");
        } else {
            char cwd[1024];
            getcwd(cwd, sizeof(cwd));
            printf("%s\n", cwd);
        }
    } else if (strcmp(args[0], "clr") == 0) {
        printf("\033[H\033[J");  // ANSI escape codes to clear screen
    } else if (strcmp(args[0], "dir") == 0) {
        DIR *d;
        struct dirent *dir;
        d = opendir(args[1] ? args[1] : ".");
        if (d) {
            while ((dir = readdir(d)) != NULL)
                printf("%s\n", dir->d_name);
            closedir(d);
        } else {
            perror("dir");
        }
    } else if (strcmp(args[0], "environ") == 0) {
        for (char **env = environ; *env; env++)
            printf("%s\n", *env);
    } else if (strcmp(args[0], "echo") == 0) {
        for (int i = 1; args[i]; i++)
            printf("%s ", args[i]);
        printf("\n");
    } else if (strcmp(args[0], "help") == 0) {
        print_help();
    } else if (strcmp(args[0], "pause") == 0) {
        printf("Press Enter to continue...\n");
        while (getchar() != '\n');
    } else if (strcmp(args[0], "quit") == 0) {
        exit(0);
    }

    if (out != NULL)
        fclose(out);
}

int is_internal_command(char *cmd) {
    const char *internal[] = {"cd", "clr", "dir", "environ", "echo", "help", "pause", "quit", NULL};
    for (int i = 0; internal[i]; i++)
        if (strcmp(cmd, internal[i]) == 0)
            return 1;
    return 0;
}

void print_help() {
    printf("Simple myshell help\n");
    printf("Built-in commands:\n");
    printf("cd <dir>\nclr\ndir <dir>\nenviron\necho <text>\nhelp\npause\nquit\n");
}
