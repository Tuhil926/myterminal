#include <stdio.h>
#include <dirent.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <pwd.h>
#include <time.h>
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
	char UID[64];
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
	printf("%s %d %d %ld %ld %d %s %s\n", process->UID, process->pid, process->ppid, process->size, process->RSS, process->PSR, process->STIME, process->process_name);
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

	char stats_path[32];
	uid_t uid;
	long int stime;

	while ((dir = readdir(d)) != NULL){
		if (dir->d_name[0] == '.' || dir->d_name[0] < 48 || dir->d_name[0] > 57){
			continue;
		}
		stats_path[0] = '\0';
		strcpy(stats_path, "/proc/");
		strcpy(stats_path + strlen(stats_path), dir->d_name);
		strcpy(stats_path + strlen(stats_path), "/stat");
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
			}else if (i == 23){
				fscanf(process, "%ld ", &(processes[n].RSS));
			}else if (i == 38){
				fscanf(process, "%d ", &(processes[n].PSR));
			}else if (i == 22){
				fscanf(process, "%ld ", &(processes[n].size));
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

		//printf("%s\n", stats_path);
		printf("bhifdd\n");
		strcpy(processes[n].UID, "why tf doesn't this work");//getpwuid(processes[n].pid)->pw_name);
		printf("works!\n");
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
