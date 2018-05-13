#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zconf.h>
#include <wait.h>
#include <termios.h>
#include "headers/shell.h"
#include "headers/loop_core.h"


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
    exit(0);
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

int main(){
    //call the shell program.
    nsh();
}
