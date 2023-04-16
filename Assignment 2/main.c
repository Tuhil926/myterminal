#include "headers.h"

int main()
{
	char* command;
	// path stores the current formatted path to be printed during prompting
	char* path = (char *)malloc(256*sizeof(char));

	while (1){
		get_path(path);
		//get_command(command, commands_list, number_of_commands);
		command = readline(path);
		if (strlen(command) > 0){
			add_history(command);
		}
		if (try_other_commands(command) && custom_commands(command)){
			//system(command);
		}
	}
	return 0;
}