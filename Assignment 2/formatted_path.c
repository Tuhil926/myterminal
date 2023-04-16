#include <string.h>
#include <unistd.h>

#define BLUE "\001\033[36m\002"
#define MAGENTA "\001\033[35m\002"
#define DEFAULT "\001\033[0m\002"

void get_path(char *path){
	char pwd[256];
	char username[64];
	char hostname[64];
	path[0] = '\0';

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
			//printf("%s@%s:~%s$ ", username, hostname, new_pwd);
			strcat(path, BLUE);
			strcat(path, username);
			strcat(path, "@");
			strcat(path, hostname);
			strcat(path, DEFAULT);
			strcat(path, ":");
			strcat(path, MAGENTA);
			strcat(path, "~");
			strcat(path, new_pwd);
			strcat(path, DEFAULT);
			strcat(path, "$ ");
			return;
		}
	}

	//printf("%s@%s:%s$ ", username, hostname, pwd);
	strcat(path, BLUE);
	strcat(path, username);
	strcat(path, "@");
	strcat(path, hostname);
	strcat(path, DEFAULT);
	strcat(path, ":");
	strcat(path, MAGENTA);
	strcat(path, pwd);
	strcat(path, DEFAULT);
	strcat(path, "$ ");
	return;
}