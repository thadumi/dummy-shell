#ifndef CONFIG_H
#define CONFIG_H

#define MAXLEN 256
#define SEPARATOR_CONFIG_CHAR '='
#define CONFIG_FILE_NAME ".config"


typedef struct {
	char welcome_msg[MAXLEN];
	char user[MAXLEN];
	char file_in[MAXLEN];
	char file_out[MAXLEN];
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