#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <termios.h>

void print_path(){
	char pwd[256];
	char username[64];
	char hostname[64];

	getcwd(pwd, sizeof(pwd));
	getlogin_r(username, sizeof(username));
	gethostname(hostname, sizeof(hostname));

	if (strlen(pwd) >= 6 + strlen(username)){
		char first_two_dir[64];
		char home_user_dir[64];
		strncpy(first_two_dir, pwd, 6 + strlen(username));
		strcpy(home_user_dir, "/home/");
		strcpy(home_user_dir + 6, username);
		if (!strcmp(first_two_dir, home_user_dir)){
			char new_pwd[256];
			strcpy(new_pwd, pwd + 6 + strlen(username));
			printf("%s@%s:~%s$ ", username, hostname, new_pwd);
			return;
		}
	}

	printf("%s@%s:%s$ ", username, hostname, pwd);
}

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

void get_command(char* command, char** commands_list, int number_of_commands){
	char c;
	int current_prev_command = number_of_commands;
	int counter = 0;
	int is_command = 0;
	int up_comm = 0;
	while (1){
		c = getchar();

		if (c == '\n'){
			break;
		}

		if (c == 127){
			if (counter > 0){
				printf("\b\b\b   \b\b\b");
				counter--;
				continue;
			}else{
				printf("\b\b  \b\b");
				continue;
			}
		}
		if (up_comm == 2){
			//printf(" %d ", c);
			if (c == 65){
				up_comm = 3;
				//printf("fhs ");
			}else if (c == 66){
				up_comm = 4;
			}else{
				up_comm = 0;
			}
		}
		if (up_comm == 1){
			if (c == 91){
				up_comm = 2;
				//printf("djhsfk ");
			}else{
				up_comm = 0;
			}
		}
		if (c == 27){
			up_comm = 1;
		}

		if (up_comm == 3 || up_comm == 4){
			//printf("up!");
			printf("\b\b\b\b    \b\b\b\b");
			while(counter > 0){
				printf("\b \b");
				counter--;
			}
			if (up_comm == 3 &&current_prev_command > 0){
				current_prev_command--;
			}else if (up_comm == 4 && current_prev_command < number_of_commands - 1){
				current_prev_command++;
			}
			if ((up_comm == 3 || up_comm == 4) && number_of_commands > 0){
				strcpy(command, commands_list[current_prev_command]);
				printf("%s", command);
				counter = strlen(command);
			}
			up_comm = 0;
			continue;
		}

		if (up_comm == 0){
			printf("\b \b");
			printf("%c", c);
			command[counter] = c;
			//command[counter + 1] = 5;
			counter++;
		}

		//printf("%d", up_comm);
	}
	command[counter] = '\0';
}

int main()
{
	static struct termios oldt, newt;
	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~(ICANON);
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);

	char command[256];
	char* commands_list[256];
	int number_of_commands = 0;

	while (1){
		print_path();
		get_command(command, commands_list, number_of_commands);
		if (try_other_commands(command)){
			system(command);
			//printf("%s", command);
		}

		char* prev_command = (char*)malloc(sizeof(command));
		strcpy(prev_command, command);
		commands_list[number_of_commands] = prev_command;
		number_of_commands++;
		wait(NULL);
	}
	return 0;
}