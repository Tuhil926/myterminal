#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>
// options to include: -l -a -F -S -r

struct ls_opt{
	bool l;
	bool a;
	bool F;
	bool S;
	bool r;
	bool file_given;
	char file[64];
};

struct file{
	struct stat stats;
	char filename[64];
	char username[32];
	char groupname[32];
	char size[32];
};

int parse_opt(char* options, struct ls_opt * opts){
	opts->l = false;
	opts->a = false;
	opts->F = false;
	opts->S = false;
	opts->r = false;
	opts->file_given = false;
	strcpy(opts->file, ".");

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
			if (opts->file_given){
				printf("Error: too many arguments!\n");
				return 1;
			}
			opts->file_given = true;
			strcpy(opts->file, character);
			continue;
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

void get_permissions_string(struct file* file, char* string){
	if (S_ISDIR(file->stats.st_mode)){
		string[0] = 'd';
	}else if (S_ISLNK(file->stats.st_mode)){
		string[0] = 'l';
	}

	if (S_IRUSR & file->stats.st_mode){
		string[1] = 'r';
	}
	if (S_IWUSR & file->stats.st_mode){
		string[2] = 'w';
	}
	if (S_IXUSR & file->stats.st_mode){
		string[3] = 'x';
	}

	if (S_IRGRP & file->stats.st_mode){
		string[4] = 'r';
	}
	if (S_IWGRP & file->stats.st_mode){
		string[5] = 'w';
	}
	if (S_IXGRP & file->stats.st_mode){
		string[6] = 'x';
	}

	if (S_IROTH & file->stats.st_mode){
		string[7] = 'r';
	}
	if (S_IWOTH & file->stats.st_mode){
		string[8] = 'w';
	}
	if (S_IXOTH & file->stats.st_mode){
		string[9] = 'x';
	}

}

void print_time(time_t time){
	char* full_time = ctime(&time);
	full_time[16] = '\0';
	printf("%s ", full_time + 4);
}

void print_stats(struct file* file, struct ls_opt* opts, int max_usrname_len, int max_grpname_len, int max_size_strlen){
	if (opts->a || (file->filename[0] != '.')){
		if (opts->l){
			char perm_string[10] = "----------";
			get_permissions_string(file, perm_string);
			printf("%s ", perm_string);
			printf("%ld ", file->stats.st_nlink);
			for (int i = 0; i < (-strlen(file->username) + max_usrname_len); i++) {printf(" ");}
			printf("%s ", file->username);
			for (int i = 0; i < (-strlen(file->groupname) + max_grpname_len); i++) {printf(" ");}
				printf("%s ", file->groupname);
			for (int i = 0; i < (max_size_strlen-strlen(file->size)); i++) {printf(" ");}
			printf("%s ", file->size);
			print_time(file->stats.st_mtime);
			printf("%s", file->filename);
		}
		else{
			printf("%s", file->filename);
			if (opts->F){
				if (S_ISDIR(file->stats.st_mode)){
					printf("/");
				}else if(S_IXUSR & file->stats.st_mode){
					printf("*");
				}
			}
		}
		printf("\n");
	}
}

bool compare_stats(struct file files[256], struct ls_opt* opts, int i, int j){
	bool res;
	if (opts->S){
		res = (files[i].stats.st_size < files[j].stats.st_size);
	}else{
		res = (strcmp(files[i].filename, files[j].filename) > 0);
	}
	if (opts->r){
		res = !res;
	}
	return res;
}

void sort_stats(struct file files[256], int number_of_files, struct ls_opt* opts){
	struct file temp_file;
	for (int j = 0; j < number_of_files; j++){
		for (int i = 0; i < number_of_files - 1; i++){
			if (compare_stats(files, opts, i, i + 1)){
				temp_file = files[i];

				files[i] = files[i + 1];

				files[i + 1] = temp_file;
			}
		}
	}
}

int myls(char* options){
	struct ls_opt opts;
	if (parse_opt(options, &opts)){
		return 1;
	}

	DIR *d;
	struct dirent *dir;
	d = opendir(opts.file);
	struct file files[256];
	char* filenames[256];
	if (!d){
		printf("error opening directory.\n");
		return 1;
	}
	int number_of_files = 0;

	int max_usrname_len = 0;
	int max_grpname_len = 0;
	int max_size_strlen = 0;

	int block_size = 0;

	struct stat stat_buf;
	while ((dir = readdir(d)) != NULL){
		stat(dir->d_name, &stat_buf);
		files[number_of_files].stats = stat_buf;
		struct passwd* pwd = getpwuid(stat_buf.st_uid);
		if (pwd != NULL){
			strcpy((files[number_of_files].username), pwd->pw_name);
		}else{
			strcpy((files[number_of_files].username), "NULL");
		}
		struct group* grp = getgrgid(stat_buf.st_gid);
		if (grp != NULL){
			strcpy((files[number_of_files].groupname), grp->gr_name);
		}else{
			strcpy((files[number_of_files].groupname), "NULL");
		}
		sprintf((files[number_of_files].size), "%ld", stat_buf.st_size);
		//printf("%ld", files[number_of_files].stats.st_size);
		if (strlen(files[number_of_files].username) > max_usrname_len){
			max_usrname_len = strlen(files[number_of_files].username);
		}
		if (strlen(files[number_of_files].groupname) > max_grpname_len){
			max_grpname_len = strlen(files[number_of_files].groupname);
		}
		if (strlen(files[number_of_files].size) > max_size_strlen){
			max_size_strlen = strlen(files[number_of_files].size);
		}

		//block_size += stat_buf.st_blocks/2;

		strcpy(files[number_of_files].filename, dir->d_name);
		number_of_files++;
	}
	closedir(d);

	sort_stats(files, number_of_files, &opts);

	for (int i = 0; i < number_of_files; i++){
		if (opts.a || (files[i].filename[0] != '.')){
			block_size += files[i].stats.st_blocks/2;
		}
	}

	if (opts.l){
		printf("Total: %d\n", block_size);
	}

	for (int i = 0; i < number_of_files; i++){
		print_stats(&files[i], &opts, max_usrname_len, max_grpname_len, max_size_strlen);
	}
	
	return 0;
}
