#include <stdio.h>
#include <dirent.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <pwd.h>
#include <time.h>
#include <sys/stat.h>
// options to include: -f -i -n -v

struct cp_opt{
	bool f;
	bool i;
	bool n;
	bool v;
	char* filenames[16];
	int number_of_files;
};

struct process{
	char process_name[64];
	int pid;
	int ppid;
	char UID[64];
	int C;
	long int size;
	long int RSS;
	int PSR;
	char STIME[10];
	char TTY[10];
	char time[8];
};

int parse_cp_opt(char* options, struct cp_opt * opts){
	opts->f = false;
	opts->i = false;
	opts->n = false;
	opts->v = false;
	opts->number_of_files = 0;

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
			opts->filenames[opts->number_of_files] = character;
			opts->number_of_files++;
			continue;
		}
		character++;
		while (*character != ' ' && *character != '\0'){
			if (*character == 'f'){
				opts->f = true;
			}else if (*character == 'i'){
				opts->i = true;
			}else if (*character == 'n'){
				opts->n = true;
			}else if (*character == 'v'){
				opts->v = true;
			}else{
				printf("Invalid option: %c\n", *character);
				return 1;
			}
			character++;
		}
	}

	if (opts->number_of_files < 2){
		printf("Not enough argumants!\n");
		return 1;
	}

	return 0;
}

int mycp(char* options, int should_move){
	struct cp_opt opts;
	if (parse_cp_opt(options, &opts)){
		return 1;
	}

	char buff[4096];

	char* destination = opts.filenames[opts.number_of_files - 1];

	struct stat stat_buf;

	for (int i = 0; i < opts.number_of_files - 1; i++){
		if (stat(opts.filenames[i], &stat_buf)){
			printf("File \"%s\" doesn't exist!\n", opts.filenames[i]);
			return 1;
		}else{
			if (S_ISDIR(stat_buf.st_mode)){
				printf("\"%s\" is a directory! This command cannot copy a directory!\n", opts.filenames[i]);
				return 1;
			}
		}
	}

	if (stat(destination, &stat_buf)){
		if (opts.number_of_files != 2){
			printf("Directory \"%s\" doesn't exist!\n", destination);
			return 1;
		}else{
			printf("creating new file!\n");
			// create new file
			FILE* newfile = fopen(opts.filenames[1], "w");
			FILE* original = fopen(opts.filenames[0], "r");
			while (fgets(buff, 4096, original)){
				fputs(buff, newfile);
			}
			fclose(original);
			fclose(newfile);

		}
	}else{
		if (S_ISDIR(stat_buf.st_mode)){
			printf("putting files in this directory!\n");
			// put the files in this directory
			char newfile_path[256];
			for (int i = 0; i < opts.number_of_files - 1; i++){
				sprintf(newfile_path, "%s/%s", destination, opts.filenames[i]);
				FILE* newfile = fopen(newfile_path, "w");
				FILE* original = fopen(opts.filenames[0], "r");
				while (fgets(buff, 4096, original)){
					fputs(buff, newfile);
				}
				fclose(original);
				fclose(newfile);
			}
		}else if (opts.number_of_files != 2){
			printf("Too many files given! cannot copy multiple files to a single file!\n");
			return 1;
		}else{
			if (opts.n){
				printf("The file \"%s\" already exists!\n", destination);
				return 1;
			}else{
				printf("overwriting the file!\n");
				// overwrite the file
				FILE* newfile = fopen(opts.filenames[1], "w");
				FILE* original = fopen(opts.filenames[0], "r");
				while (fgets(buff, 4096, original)){
					fputs(buff, newfile);
				}
				fclose(original);
				fclose(newfile);
			}
		}
	}

	return 0;
}
