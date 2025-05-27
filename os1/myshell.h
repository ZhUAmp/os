#ifndef MYSHELL_H
#define MYSHELL_H

#define MAX_INPUT 1024
#define MAX_ARGS 64

void shell_loop();
void execute_command(char **args, int background);
void execute_internal(char **args, int background, int redirect, char *outfile, int append);
int is_internal_command(char *cmd);
void redirect_output(char *outfile, int append);
void print_help();

#endif
