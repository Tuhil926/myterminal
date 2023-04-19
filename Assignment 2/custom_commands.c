#include <string.h>
#include "myls/myls.h"
#include "myps/myps.h"
#include "mycp/mycp.h"

int custom_commands(char* command){
	char *options;
	int length = strlen(command);
	if (length == 4 || length > 4 && command[4] == ' '){
		if (command[0] == 'm' && command[1] == 'y' && command[2] == 'l' && command[3] == 's'){
			options = command + 5;
			myls(options);
			return 0;
		}
		if (command[0] == 'm' && command[1] == 'y' && command[2] == 'p' && command[3] == 's'){
			options = command + 5;
			myps(options);
			return 0;
		}
		if (command[0] == 'm' && command[1] == 'y' && command[2] == 'c' && command[3] == 'p'){
			options = command + 5;
			mycp(options);
			return 0;
		}
	}

	return 1;
}
