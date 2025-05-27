#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "myshell.h"

void shell_loop() {
    char input[MAX_INPUT];
    char *args[MAX_ARGS];
    char *token;
    int background;
    int redirect;
    char *outfile;
    int append;

    while (1) {
        char cwd[1024];
        getcwd(cwd, sizeof(cwd));
        printf("%s> ", cwd);
        fflush(stdout);

        if (!fgets(input, MAX_INPUT, stdin))
            break;

        input[strcspn(input, "\n")] = 0;

        token = strtok(input, " \t");
        int i = 0;
        background = 0;
        redirect = 0;
        append = 0;
        outfile = NULL;

        while (token != NULL) {
            if (strcmp(token, "&") == 0) {
                background = 1;
            } else if (strcmp(token, ">") == 0) {
                redirect = 1;
                token = strtok(NULL, " \t");
                outfile = token;
            } else if (strcmp(token, ">>") == 0) {
                redirect = 1;
                append = 1;
                token = strtok(NULL, " \t");
                outfile = token;
            } else {
                args[i++] = token;
            }
            token = strtok(NULL, " \t");
        }
        args[i] = NULL;

        if (args[0] == NULL)
            continue;

        if (is_internal_command(args[0])) {
            execute_internal(args, background, redirect, outfile, append);
        } else {
            execute_command(args, background);
        }
    }
}

void execute_command(char **args, int background) {
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
    } else if (pid == 0) {
        char shell_path[1024];
        getcwd(shell_path, sizeof(shell_path));
        setenv("parent", shell_path, 1);
        execvp(args[0], args);
        perror("execvp");
        exit(EXIT_FAILURE);
    } else {
        if (!background)
            waitpid(pid, NULL, 0);
    }
}

int main(int argc, char *argv[]) {
    setenv("shell", getcwd(NULL, 0), 1);

    if (argc > 1) {
        FILE *fp = fopen(argv[1], "r");
        if (!fp) {
            perror("batch file");
            exit(1);
        }

        char line[MAX_INPUT];
        while (fgets(line, sizeof(line), fp)) {
            printf(">> %s", line);
            // simulate input from file
            char *args[] = {"/bin/sh", "-c", line, NULL};
            execute_command(args, 0);
        }
        fclose(fp);
        exit(0);
    }

    shell_loop();
    return 0;
}
