#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <unistd.h>

void get_path(char*);
int try_other_commands(char*, char*);
int custom_commands(char*);