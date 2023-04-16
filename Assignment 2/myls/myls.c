#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
// options to include: -l -a -F -S -r

struct ls_opt{
	bool l;
	bool a;
	bool F;
	bool S;
	bool r;
};

int parse_opt(char* options, struct ls_opt * opts){
	opts->l = false;
	opts->a = false;
	opts->F = false;
	opts->S = false;
	opts->r = false;

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
			if (*character == 'l'){
				opts->l = true;
			}else if (*character == 'a'){
				opts->a = true;
			}else if (*character == 'F'){
				opts->F = true;
			}else if (*character == 'S'){
				opts->S = true;
			}else if (*character == 'r'){
				opts->r = true;
			}else{
				printf("Invalid option: %c\n", *character);
				return 1;
			}
			character++;
		}
	}
	return 0;
}

void print_stats(struct stat* file, struct ls_opt* opts, char* filename){
	if (opts->a || (filename[0] != '.')){
		printf("%s", filename);
		if (opts->F){
			if (S_ISDIR(file->st_mode)){
				printf("/");
			}else if(S_IXUSR & file->st_mode){
				printf("*");
			}
		}
		printf("\n");
	}
}

bool compare_stats(struct stat files[256], char* filenames[256], struct ls_opt* opts, int i, int j){
	bool res;
	if (opts->S){
		res = (files[i].st_size < files[j].st_size);
	}else{
		res = (strcmp(filenames[i], filenames[j]) > 0);
	}
	if (opts->r){
		res = !res;
	}
	return res;
}

void sort_stats(struct stat files[256], int number_of_files, char* filenames[256], struct ls_opt* opts){
	struct stat temp_file;
	char* temp_name;
	for (int j = 0; j < number_of_files; j++){
		for (int i = 0; i < number_of_files - 1; i++){
			if (compare_stats(files, filenames, opts, i, i + 1)){
				temp_file = files[i];
				temp_name = filenames[i];

				files[i] = files[i + 1];
				filenames[i] = filenames[i + 1];

				files[i + 1] = temp_file;
				filenames[i + 1] = temp_name;
			}
		}
	}
}

int myls(char* options){
	DIR *d;
	struct dirent *dir;
	d = opendir(".");
	struct stat files[256];
	char* filenames[256];
	if (!d){
		printf("error opening current directory.\n");
		return 1;
	}
	int number_of_files = 0;
	while ((dir = readdir(d)) != NULL){
		struct stat stat_buf;
		stat(dir->d_name, &stat_buf);
		files[number_of_files] = stat_buf;
		char* filename = (char*)malloc(32*sizeof(char));
		strcpy(filename, dir->d_name);
		filenames[number_of_files] = filename;
		number_of_files++;
	}
	closedir(d);

	struct ls_opt opts;
	if (parse_opt(options, &opts)){
		return 1;
	}

	sort_stats(files, number_of_files, filenames, &opts);

	for (int i = 0; i < number_of_files; i++){
		print_stats(&files[i], &opts, filenames[i]);
	}
	
	return 0;
}
