#include <string.h>
#include "myls/myls.h"

int custom_commands(char* command){
	char *options;
	int length = strlen(command);
	if (length == 4 || length > 4 && command[4] == ' '){
		if (command[0] == 'm' && command[1] == 'y' && command[2] == 'l' && command[3] == 's'){
			options = command + 5;
			return myls(options);
		}
	}

	return 1;
}