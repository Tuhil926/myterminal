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

int man(char* command){
	int length = strlen(command);
	if (length == 4 || length > 4 && command[4] == ' '){
		if (command[0] == 'm' && command[1] == 'y' && command[2] == 'l' && command[3] == 's'){
			print_file("myls/man.txt");
			return 0;
		}
		if (command[0] == 'm' && command[1] == 'y' && command[2] == 'p' && command[3] == 's'){
			print_file("myps/man.txt");
			return 0;
		}
		if (command[0] == 'm' && command[1] == 'y' && command[2] == 'c' && command[3] == 'p'){
			print_file("mycp_mv/man_cp.txt");
			return 0;
		}
		if (command[0] == 'm' && command[1] == 'y' && command[2] == 'm' && command[3] == 'v'){
			print_file("mycp_mv/man_mv.txt");
			return 0;
		}
	}else if (length == 6 || length > 6 && command[6] == ' '){
		if (command[0] == 'm' && command[1] == 'y' && command[2] == 'g' && command[3] == 'r' && command[4] == 'e' && command[5] == 'p'){
			print_file("mygrep/man.txt");
			return 0;
		}
	}
	return 1;
}