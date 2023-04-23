#include <stdio.h>
#include <dirent.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <pwd.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/sysinfo.h>
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
	char STIME[32];
	long int STIME_int;
	int TTY_num;
	char TTY[10];
	long int TIME;
	char time_str[10];
	int sid;
};

void get_time_str(time_t time, char* str){
	char* full_time = ctime(&time);
	full_time[16] = '\0';
	strcpy(str, full_time + 11);
}

void make_to_hms_format(long int time_in_sec, char* str){
	int sec = time_in_sec % 60;
	time_in_sec /= 60;
	int min = time_in_sec % 60;
	time_in_sec /= 60;
	int hrs = time_in_sec % 60;
	strcpy(str, "00:00:00");
	str[0] = hrs/10 + 48;
	str[1] = hrs%10 + 48;
	str[3] = min/10 + 48;
	str[4] = min%10 + 48;
	str[6] = sec/10 + 48;
	str[7] = sec%10 + 48;
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
	if (opts->F){
		printf("%20s %6d %6d %3d %10ld %10ld %2d %5s %5s %s %s\n", process->username, process->pid, process->ppid, process->C, process->size, process->RSS, process->PSR, process->STIME, process->TTY, process->time_str, process->process_name);
	}else if (opts->f){
		printf("%20s %6d %6d %3d %5s %5s %s\n", process->username, process->pid, process->ppid, process->C, process->STIME, process->TTY, process->process_name);		
	}else{
		printf("%6d %5s %s %s\n", process->pid, process->TTY, process->time_str, process->process_name);
	}
}

void print_headings(struct ps_opt* opts){
	if (opts->F){
		printf("%20s %6s %6s %3s %10s %10s %3s %5s %5s %8s %s\n", "UID", "PID", "PPID", "C", "SIZE", "RSS", "PSR", "STIME", "TTY", "TIME", "CMD");
	}else if (opts->f){
		printf("%20s %6s %6s %3s %5s %5s %s\n", "UID", "PID", "PPID", "C", "STIME", "TTY", "CMD");		
	}else{
		printf("%6s %5s %8s %s\n", "PID", "TTY", "TIME", "CMD");
	}
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
	struct sysinfo sys_info;
	sysinfo(&sys_info);
	long uptime = sys_info.uptime;
	long now = time(NULL);
	long boot_time = now - uptime;
	uid_t uid;
	long int stime = 0;
	long int utime = 1;

	while ((dir = readdir(d)) != NULL){
		//printf("runnin2\n");
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
				char alph;
				int len = 0;
				//fscanf(process, "%s ", processes[n].process_name);
				while ((alph = fgetc(process)) != ')'){
					processes[n].process_name[len] = alph;
					len++;
				}
				processes[n].process_name[strlen(processes[n].process_name)] = '\0';
			}else if (i == 3){
				fscanf(process, "%d ", &(processes[n].ppid));
			}else if (i == 38){
				fscanf(process, "%d ", &(processes[n].PSR));
			//}else if (i == 14){
			// 	//printf("hsdjf\n");
			// 	fscanf(process, "%ld ", &stime);
			// 	get_time_str(stime, processes[n].STIME);
			// 	//printf("bhifdd\n");
			}else if (i == 13){
				fscanf(process, "%ld ", &utime);
				fscanf(process, "%ld ", &stime);
				//stime /= sysconf(_SC_CLK_TCK);
				make_to_hms_format((stime + utime)/sysconf(_SC_CLK_TCK), processes[n].time_str);
				i++;
			}else if (i == 21){
				fscanf(process, "%ld ", &(processes[n].STIME_int));
				processes[n].STIME_int /= sysconf(_SC_CLK_TCK);
				get_time_str(boot_time + processes[n].STIME_int, processes[n].STIME);
			}else if (i == 5){
				fscanf(process, "%d ", &(processes[n].sid));
			}
			else{
				fscanf(process, "%*s ");
			}
		}

		fclose(process);

		sprintf(stats_path, "/proc/%s/statm", dir->d_name);
		process = fopen(stats_path, "r");
		if (process == NULL){
			printf("Error opening file\n");
		}

		fscanf(process, "%ld", &(processes[n].size));
		fscanf(process, "%ld", &(processes[n].RSS));
		processes[n].RSS *= 4;

		fclose(process);

		sprintf(stats_path, "/proc/%s/status", dir->d_name);
		process = fopen(stats_path, "r");
		if (process == NULL){
			printf("Error opening file\n");
		}

		char line[1000];
		while(fgets(line, 1000, process)){
			if (sscanf(line, "Uid:\t%d", &(processes[n].UID))){
				break;
			}
		}
		fclose(process);

		sprintf(stats_path, "/proc/%s/fd/0", dir->d_name);
		int fd = open(stats_path, 'r');
		processes[n].TTY_num = fd;
		char* tty = ttyname(fd);
		if (tty == NULL){
			strcpy(processes[n].TTY, "?");	
		}else{
			strcpy(processes[n].TTY, ttyname(fd) + 5);
		}

		//printf("runnin\n");
		struct passwd* pwd = getpwuid(processes[n].UID);
		if (pwd != NULL){
			strcpy(processes[n].username, pwd->pw_name);
		}else{
			//printf("ghjgj");
			strcpy(processes[n].username, "NULL");
		}
		processes[n].C = processes[n].STIME_int != 0? ((stime + utime)*100/sysconf(_SC_CLK_TCK)/(now - (boot_time + processes[n].STIME_int))):0;
		n++;
	}
	closedir(d);

	struct ps_opt opts;
	if (parse_ps_opt(options, &opts)){
		return 1;
	}
	sprintf(stats_path, "/proc/%d/fd/0", getpid());
	int fd = open(stats_path, 'r');
	char* tty = ttyname(fd) + 5;
	close(fd);

	//printf("runnin\n");
	print_headings(&opts);
	for (int i = 0; i < n; i++){
		if (opts.e || (opts.a && processes[i].TTY[0] != '?' && processes[i].pid != processes[i].sid ) || (strcmp(processes[i].TTY, tty) == 0)){
			print_processes(&processes[i], &opts);
		}
	}
	//printf("number of processes: %d\n", n);
	return 0;
}
