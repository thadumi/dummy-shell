#ifndef CONFIG_H
#define CONFIG_H

#define MAX_CHAR 256
#define SEPARATOR_CONFIG_CHAR '='
#define CONFIG_FILE_NAME ".config"


typedef struct {
	char welcome_msg[MAX_CHAR];
	char user[MAX_CHAR];
	char file_in[MAX_CHAR];
	char file_out[MAX_CHAR];
}shellConf;

typedef struct {
	
}logConf;

typedef struct {
	shellConf* shell;
	logConf* log;
}config;

void load_config(FILE *f);

char* config_parser(char* str);


#endif