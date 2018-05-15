#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_CHAR 256
#define SEPARATOR_CHAR '='
#define DEBUG 1				// no output print if 0

typedef struct {
	char w_msg[MAX_CHAR];
	char user[MAX_CHAR];
	char f_in[MAX_CHAR];
	char f_out[MAX_CHAR];
}shellConf;

typedef struct {
	
}logConf;

typedef struct {
	shellConf* shell;
	logConf* log;
}config;

void load_config(FILE *cf) {
	char temp[MAX_CHAR];
	
	fgets(temp, MAX_CHAR, cf);
	config->shellConf.w_msg = config_parser(temp);

	fgets(temp, MAX_CHAR, cf);
	config->shellConf.user = config_parser(temp);

	fgets(temp, MAX_CHAR, cf);
	config->shellConf.f_in = config_parser(temp);

	fgets(temp, MAX_CHAR, cf);
	config->shellConf.f_out = config_parser(temp);
}

char* config_parser(char* str){
	char final_str[MAX_CHAR];
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



int main(int argc, char const *argv[]) {
	
	FILE *configFile;
	configFile = fopen("config");

	if (configFile == NULL) {
		printf("Error on configuration loading\n");
		return -2;
	}

	load_config(configFile);

	if (DEBUG) {
		printf("%s\n", config->shellConf.w_msg);
		printf("%s\n", config->shellConf.user);
		printf("%s\n", config->shellConf.f_in);
		printf("%s\n", config->shellConf.fout);
	}


	fclose(configFile);

	return 0;
}