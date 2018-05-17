#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../headers/config.h"

#define DEBUG 1				// no output print if 0

char* config_parser(char* str){
	char final_str[MAXLEN];
	int save = 0;
	int j = 0;

	for (int i=0;i<strlen(str);i++) {
		if (str[i] == SEPARATOR_CHAR)
			save = 1;
		if (save == 1)
			final_str[j++] = str[i];
	}
	return final_str;
}

