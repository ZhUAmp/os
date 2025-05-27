#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>

#define MAX_INPUT 1024
#define MAX_ARGS 100


int is_internal_command(char *cmd);


void execute_internal(char **args, int background, int redirect, char *outfile, int append);


void execute_command(char **args, int background, int redirect, char *outfile, int append);


void shell_loop();

// Печать справки по встроенным командам
void print_help();

int main(int argc, char *argv[]) {
    // Устанавливаем переменную окружения shell — текущий каталог
    setenv("shell", getcwd(NULL, 0), 1);

    // Если передан файл с командами — выполняем пакетно
    if (argc > 1) {
        FILE *fp = fopen(argv[1], "r");
        if (!fp) {
            perror("batch file");
            exit(1);
        }

        char line[MAX_INPUT];
        while (fgets(line, sizeof(line), fp)) {
            printf(">> %s", line);
            // Выполнение команды через sh -c (чтобы поддержать сложные команды)
            char *args[] = {"/bin/sh", "-c", line, NULL};
            execute_command(args, 0, 0, NULL, 0);
        }
        fclose(fp);
        exit(0);
    }

    shell_loop();
    return 0;
}

void shell_loop() {
    char input[MAX_INPUT];
    char *args[MAX_ARGS];
    char *token;
    int background;
    int redirect;
    char *outfile;
    int append;


    while (1)
    {

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
            } else if (strcmp(token, ">>") == 0)
            {
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
            execute_command(args, background, redirect, outfile, append);
        }
    }
}

void execute_command(char **args, int background, int redirect, char *outfile, int append) {
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
    } else if (pid == 0) {
        // В дочернем процессе

        // Устанавливаем переменную окружения parent — текущий каталог
        char shell_path[1024];
        getcwd(shell_path, sizeof(shell_path));
        setenv("parent", shell_path, 1);

        // Обработка перенаправления вывода
        if (redirect && outfile != NULL) {
            int fd;
            if (append) {
                // Открываем файл на дозапись (создаем если не существует)
                fd = open(outfile, O_WRONLY | O_CREAT | O_APPEND, 0644);
            } else {
                // Открываем файл с усечением (перезаписываем)
                fd = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            }
            if (fd < 0) {
                perror("open");
                exit(EXIT_FAILURE);
            }

            // Перенаправляем стандартный вывод на файл
            if (dup2(fd, STDOUT_FILENO) < 0) {
                perror("dup2");
                close(fd);
                exit(EXIT_FAILURE);
            }
            close(fd);
        }

        execvp(args[0], args);
        perror("execvp");
        exit(EXIT_FAILURE);
    } else {
        // В родительском процессе
        if (!background)
            waitpid(pid, NULL, 0);
    }
}

void execute_internal(char **args, int background, int redirect, char *outfile, int append) {
    FILE *out = NULL;

    // Если нужно перенаправить вывод встроенной команды — открываем файл
    if (redirect && outfile != NULL) {
        if (append)
            out = freopen(outfile, "a", stdout);
        else
            out = freopen(outfile, "w", stdout);
    }

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
        // Очистка экрана ANSI escape кодом
        printf("\033[H\033[J");
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
        extern char **environ;
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
