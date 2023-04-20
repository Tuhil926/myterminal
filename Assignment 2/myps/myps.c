#include <stdio.h>
#include <dirent.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <pwd.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
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
	int ppid;
	int UID;
	char username[64];
	int C;
	long int size;
	long int RSS;
	int PSR;
	char STIME[10];
	char TTY[10];
	char time[8];
};

void get_time_str(time_t time, char* str){
	char* full_time = ctime(&time);
	full_time[16] = '\0';
	strcpy(str, full_time + 11);
}

int parse_ps_opt(char* options, struct ps_opt * opts){
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
	printf("%20s %6d %6d %10ld %10ld %2d %10s %5s %s\n", process->username, process->pid, process->ppid, process->size, process->RSS, process->PSR, process->STIME, process->TTY, process->process_name);
}

int myps(char* options){
	DIR *d;
	struct dirent *dir;
	d = opendir("/proc");
	struct process processes[512];
	if (!d){
		printf("error opening current directory.\n");
		return 1;
	}
	int n = 0; // number of processes

	char stats_path[512];
	uid_t uid;
	long int stime;

	while ((dir = readdir(d)) != NULL){
		if (dir->d_name[0] == '.' || dir->d_name[0] < 48 || dir->d_name[0] > 57){
			continue;
		}
		sprintf(stats_path, "/proc/%s/stat", dir->d_name);
		FILE* process = fopen(stats_path, "r");
		if (!process){
			//printf("Not possible to open\n");
			continue;
		}
		//printf("opening file %s\n", stats_path);
		for (int i = 0; i < 52; i++){
			if (i == 0){
				fscanf(process, "%d ", &(processes[n].pid));
			}else if (i == 1){
				fgetc(process);
				fscanf(process, "%s ", processes[n].process_name);
				processes[n].process_name[strlen(processes[n].process_name) - 1] = '\0';
			}else if (i == 3){
				fscanf(process, "%d ", &(processes[n].ppid));
			}else if (i == 38){
				fscanf(process, "%d ", &(processes[n].PSR));
			}else if (i == 14){
				//printf("hsdjf\n");
				fscanf(process, "%ld ", &stime);
				get_time_str(stime, processes[n].STIME);
				//printf("bhifdd\n");
			}
			else{
				fscanf(process, "%*s ");
			}
		}

		fclose(process);

		sprintf(stats_path, "/proc/%s/statm", dir->d_name);
		process = fopen(stats_path, "r");

		fscanf(process, "%ld", &(processes[n].size));
		fscanf(process, "%ld", &(processes[n].RSS));
		processes[n].RSS *= 4;

		fclose(process);

		sprintf(stats_path, "/proc/%s/status", dir->d_name);
		process = fopen(stats_path, "r");

		char line[1000];
		while(fgets(line, 1000, process)){
			if (sscanf(line, "Uid:\t%d", &(processes[n].UID))){
				break;
			}
		}
		fclose(process);

		sprintf(stats_path, "/proc/%s/fd/0", dir->d_name);
		int fd = open(stats_path, 'r');
		char* tty = ttyname(fd);
		if (tty == NULL){
			strcpy(processes[n].TTY, "?");	
		}else{
			strcpy(processes[n].TTY, ttyname(fd) + 5);
		}
		

		strcpy(processes[n].username, getpwuid(processes[n].UID)->pw_name);
		n++;
	}
	closedir(d);

	struct ps_opt opts;
	if (parse_ps_opt(options, &opts)){
		return 1;
	}

	for (int i = 0; i < n; i++){
		print_processes(&processes[i], &opts);
	}
	printf("number of processes: %d\n", n);
	
	return 0;
}
