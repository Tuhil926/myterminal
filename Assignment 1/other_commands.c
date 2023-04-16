#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int try_cd(char* command){
	if (strlen(command) >= 4){
		if (command[0] == 'c' && command[1] == 'd' && command[2] == ' '){
			//printf("changing dir");
			int res = chdir(&(command[3]));
			//printf(" %s", &(command[3]));
			return res;
		}
	}

	else if (strlen(command) == 2 || (strlen(command) > 2 && command[2] == ' ')){
		if (command[0] == 'c' && command[1] == 'd'){
			int res = chdir(getenv("HOME"));
			return res;
		}
	}
	return 1;
}

int try_exit(char* command){
	if (strlen(command) == 4 || (strlen(command) > 4 && command[4] == ' ')){
		if (command[0] == 'e' && command[1] == 'x' && command[2] == 'i' && command[3] == 't'){
			printf("Exiting myshell\n");
			exit(0);
		}
	}
	return 1;
}

int try_other_commands(char* command){
	int res_cd = try_cd(command);
	
	int res_exit = try_exit(command);

	return res_exit && res_cd;
	
}
