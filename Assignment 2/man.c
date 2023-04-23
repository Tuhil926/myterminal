#include <stdio.h>
#include <string.h>

void print_file(char* filename){
	FILE* file = fopen(filename, "r");
	if (file == NULL){
		printf("couldn't find man file\n");
		return;
	}
	char buff[4096];
	while (fgets(buff, 4096, file)){
		printf("%s", buff);
	}
	printf("\n");
	fclose(file);
}

int man(char* command, char* cwd){
	int length = strlen(command);
	char path[1024];
	if (length == 4 || length > 4 && command[4] == ' '){
		//printf("fjds\n");
		if (command[0] == 'm' && command[1] == 'y' && command[2] == 'l' && command[3] == 's'){
			//printf("%s\n", cwd);
			sprintf(path, "%s%s", cwd, "/myls/man.txt");
			print_file(path);
			return 0;
		}
		if (command[0] == 'm' && command[1] == 'y' && command[2] == 'p' && command[3] == 's'){
			sprintf(path, "%s%s", cwd, "/myps/man.txt");
			print_file(path);
			return 0;
		}
		if (command[0] == 'm' && command[1] == 'y' && command[2] == 'c' && command[3] == 'p'){
			sprintf(path, "%s%s", cwd, "/mycp_mv/man_cp.txt");
			print_file(path);
			return 0;
		}
		if (command[0] == 'm' && command[1] == 'y' && command[2] == 'm' && command[3] == 'v'){
			sprintf(path, "%s%s", cwd, "/mycp_mv/man_mv.txt");
			print_file(path);
			return 0;
		}
	}else if (length == 6 || length > 6 && command[6] == ' '){
		if (command[0] == 'm' && command[1] == 'y' && command[2] == 'g' && command[3] == 'r' && command[4] == 'e' && command[5] == 'p'){
			sprintf(path, "%s%s", cwd, "/mygrep/man.txt");
			print_file(path);
			return 0;
		}
	}
	return 1;
}