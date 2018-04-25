#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zconf.h>
#include <wait.h>
#include <termios.h>
#include "headers/shell.h"
#include "headers/loop_core.h"

//#include "headers/loop_core.h"


int nsh_cd(char **args) {
    if(args[1] == NULL) {
        fprintf(stderr, "dadajldajkld\n");
    } else {
        if(chdir(args[1]) != 0) {
            perror("lsh");
        }
    }

    return 1;
}

int nsh_help(char **args) {
    printf("dakdladada\n");

    //for(int i = 0; i < NSH_NUM_BUILTIN; i++) {
    //    printf(" %s\n", builtin_str[i]);
    //}

    printf("Use man command for informations on other programs\n");
    return 1;
}

int nsh_exit(char **args) {
    return 0;
}

char* builtin_str[] = {
        "cd",
        "help",
        "exit"
};


int (*builtin_fun[]) (char **) = {
        &nsh_cd,
        &nsh_help,
        &nsh_exit
};

const int builtin_fun_size = sizeof(builtin_str) / sizeof(char *);

int main(){
    //set_functions(&my_builtin_str, &my_builtin_fun);

    nsh();
}
//https://stackoverflow.com/questions/28502305/writing-a-simple-shell-in-c-using-fork-execvp
//https://www.usna.edu/Users/cs/aviv/classes/ic221/s16/lec/17/lec.html