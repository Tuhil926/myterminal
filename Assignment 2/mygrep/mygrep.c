#include <stdio.h>
#include <dirent.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <pwd.h>
#include <time.h>
#include <sys/stat.h>
// options to include: -i -v -c -n -s

struct grep_opt{
	bool i;
	bool v;
	bool c;
	bool n;
	char* args[16];
	int number_of_args;
};

int parse_grep_opt(char* options, struct grep_opt * opts){
	opts->i = false;
	opts->v = false;
	opts->c = false;
	opts->n = false;
	opts->number_of_args = 0;

	char* temp = options - 1;

	char* tokens[16];
	int number_of_tokens = 0;

	while (*temp != '\0'){
		//printf("%c, %c\n", *temp, *(temp + 1));
		if (*temp == ' ' && *(temp + 1) != ' '){
			tokens[number_of_tokens] = temp + 1;
			number_of_tokens++;
			*temp = '\0';
		}
		temp++;
	}
	//printf("%d\n", number_of_tokens);
	for (int i = 0; i < number_of_tokens; i++){
		char * character = tokens[i];
		if (*character == '\0'){
			continue;
		}
		if (*character != '-'){
			opts->args[opts->number_of_args] = character;
			opts->number_of_args++;
			continue;
		}
		character++;
		while (*character != ' ' && *character != '\0'){
			if (*character == 'i'){
				opts->i = true;
			}else if (*character == 'v'){
				opts->v = true;
			}else if (*character == 'c'){
				opts->c = true;
			}else if (*character == 'n'){
				opts->n = true;
			}else{
				printf("Invalid option: %c\n", *character);
				return 1;
			}
			character++;
		}
	}

	if (opts->number_of_args < 2){
		printf("Not enough argumants!\n");
		return 1;
	}

	return 0;
}

int mygrep(char* options){
	struct grep_opt opts;
	if (parse_grep_opt(options, &opts)){
		return 1;
	}

	char* search_term = opts.args[0];

	struct stat stat_buf;

	FILE* search_file;
	char* line;
	size_t len;
	ssize_t read;

	for (int i = 1; i < opts.number_of_args; i++){
		printf("searching in file: %s\n", opts.args[i]);
		if (stat(opts.args[i], &stat_buf)){
			printf("File \"%s\" doesn't exist!\n", opts.args[i]);
			return 1;
		}else{
			if (S_ISDIR(stat_buf.st_mode)){
				printf("\"%s\" is a directory! This command cannot look through a directory!\n", opts.args[i]);
				return 1;
			}else{
				// search the file for the word
				search_file = fopen(opts.args[i], "r");
				while ((read = getline(&line, &len, search_file)) != -1){
					if (strlen(line) >= strlen(search_term) && strstr(line, search_term) != NULL){
						printf("%s", line);
					}
					free(line);
					//printf("line: %s", line);
				}
				fclose(search_file);
			}
		}
	}

	return 0;
}