#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zconf.h>
#include <wait.h>
#include <termios.h>
#include "headers/shell.h"
#include "headers/loop_core.h"
#include "headers/config.h"

#define DEBUG 1             // no output print if 0


int nsh_cd(char **args) {
    if(args[1] == NULL) {
        fprintf(stderr, "dadajldajkld\n");
    } else {
        if(chdir(args[1]) != 0) {
            perror("nsh");
        }
    }

    return 1;
}

//TODO: create a api for builtin functions' help center
int nsh_help(char **args) {
    //for(int i = 0; i < NSH_NUM_BUILTIN; i++) {
    //    printf(" %s\n", builtin_str[i]);
    //}

    printf("Use man command for informations on other programs\n");
    return 1;
}

int nsh_exit(char **args) {
    return 0;
}

/**
 * in this array must be declared the name of the custom functions
 */
const char* builtin_str[] = {
        "cd",
        "help",
        "exit"
};


/**
 * this array contain the pointers of each custom function.
 * The order has to follow the one defined into builtin_str array.
 */
const int (*builtin_fun[]) (char **) = {
        &nsh_cd,
        &nsh_help,
        &nsh_exit
};

/**
 * this constant define the number of custom functions that was defined
 */
const int builtin_fun_size = sizeof(builtin_str) / sizeof(char *);


void load_config(FILE *cf) {
    char temp[MAX_CHAR];
    
    fgets(temp, MAX_CHAR, cf);
    config->shellConf.welcome_msg = config_parser(temp);

    fgets(temp, MAX_CHAR, cf);
    config->shellConf.user = config_parser(temp);

    fgets(temp, MAX_CHAR, cf);
    config->shellConf.file_in = config_parser(temp);

    fgets(temp, MAX_CHAR, cf);
    config->shellConf.file_out = config_parser(temp);
}

char* config_parser(char* str){
    char final_str[MAX_CHAR];
    int save = 0;
    int j = 0;

    for (int i=0;i<strlen(str);i++) {
        if (str[i] == SEPARATOR_CONFIG_CHAR)
            save = 1;
        if (save == 1)
            final_str[j++] = str[i];
    }
    return final_str;
}

int main(){

    // read congfig file

    FILE *configFile;
    configFile = fopen(CONFIG_FILE_NAME, "r");

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

    //call the shell program.
    nsh();
}
