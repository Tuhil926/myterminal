#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>
// options to include: -e -a -f -F

struct ps_opt{
	bool e;
	bool a;
	bool F;
	bool f;
};

struct process{
	char process_name[64];
	int pid;
	//char username[32];
	//char groupname[32];
	//char size[32];
};

int parse_opt(char* options, struct ps_opt * opts){
	opts->e = false;
	opts->a = false;
	opts->F = false;
	opts->f = false;

	char* temp = options - 1;

	char* tokens[16];
	int number_of_tokens = 0;

	while (*temp != '\0'){
		//printf("%c, %c\n", *temp, *(temp + 1));
		if (*temp == ' ' && *(temp + 1) != ' '){
			tokens[number_of_tokens] = temp + 1;
			number_of_tokens++;
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
			printf("invalid argument: %s\n", character);
			return 1;
		}
		character++;
		while (*character != ' ' && *character != '\0'){
			if (*character == 'e'){
				opts->e = true;
			}else if (*character == 'a'){
				opts->a = true;
			}else if (*character == 'F'){
				opts->F = true;
			}else if (*character == 'f'){
				opts->f = true;
			}else{
				printf("Invalid option: %c\n", *character);
				return 1;
			}
			character++;
		}
	}
	return 0;
}

void print_processes(struct process* process, struct ps_opt* opts){
	printf("%d\n", process->pid);
}

int myps(char* options){
	DIR *d;
	struct dirent *dir;
	d = opendir("/proc");
	struct process processes[256];
	if (!d){
		printf("error opening current directory.\n");
		return 1;
	}
	int n = 0; // number of processes


	while ((dir = readdir(d)) != NULL){
		FILE* process = fopen(dir->d_name, "r");
		for (int i = 0; i < 52; i++){
			if (i == 0){
				fscanf("%d", &(process.pid));
			}else{
				fscanf("%*s");
			}
		}

		n++;
	}
	closedir(d);

	struct ps_opt opts;
	if (parse_opt(options, &opts)){
		return 1;
	}

	for (int i = 0; i < number_of_files; i++){
		print_stats(&files[i], &opts);
	}
	
	return 0;
}
