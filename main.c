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

    printf("Usage: shell [OPTION]\n"
           "Runs the Shell, if OPTION is empty the default values are used.\n"
           "\n"
           "Possible OPTION parameters:\n"
           "\t--welcomeMessage\t\t\t\tspecify the welcome message when the shell starts\n"
           "\n"
           "\t--inputFile\t\t\t\tthe name/path of the template file\n"
           "\n"
           "\t--outputFile \t\t\t\t\tthe name/path of the output file where all the output \n"
           "\t\t\t\t\t\t\t\t\tof the shell are written\n"
           "\n"
           "\t--errorFile\t\t\t\tthe name/path of the error file where all the errors \n"
           "\t\t\t\t\t\t\t\t\tthat occur when the shell is running are written\n"
           "\n"
           "\t--outputFileMode\t\t\t\tthe mode to open the output file and error file. Use:\n"
           "\t\t\t\t\t\t\t\t\t\t\"w\"   -  simple writing (file need not exist)\n"
           "\t\t\t\t\t\t\t\t\t\t\"wa\"  -  writing with append (file need not exist)\n"
           "\t\t\t\t\t\t\t\t\t\t\"w+\"  -  simple writing (overwrite existing output file)\n"
           "\t\t\t\t\t\t\t\t\t\t\"wa+\" -  writing with append (append if there is an old output file)");
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

int main(int argc, char** argv){
    //call the shell program.
    nsh(argc, argv);
}
