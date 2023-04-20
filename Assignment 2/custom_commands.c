#include <string.h>
#include "myls/myls.h"
#include "myps/myps.h"
#include "mycp/mycp.h"
#include "mygrep/mygrep.h"

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
			mycp(options, 0);
			return 0;
		}
		if (command[0] == 'm' && command[1] == 'y' && command[2] == 'm' && command[3] == 'v'){
			options = command + 5;
			mycp(options, 1);
			return 0;
		}
	}else if (length == 6 || length > 6 && command[6] == ' '){
		if (command[0] == 'm' && command[1] == 'y' && command[2] == 'g' && command[3] == 'r' && command[4] == 'e' && command[5] == 'p'){
			options = command + 7;
			mygrep(options);
			return 0;
		}
	}

	return 1;
}
