#include "headers.h"

int main()
{
	// path stores the current formatted path to be printed during prompting
	char* path = (char *)malloc(256*sizeof(char));

	char cwd[1024];
	getcwd(cwd, 1024);
	//printf("%s", cwd);

	printf("\t\tENTERING MYSHELL\n");

	while (1){
		get_path(path);
		//get_command(command, commands_list, number_of_commands);
		char* command = readline(path);
		if (strlen(command) > 0){
			add_history(command);
		}
		if (try_other_commands(command, cwd) && custom_commands(command)){
			system(command);
		}
	}
	return 0;
}