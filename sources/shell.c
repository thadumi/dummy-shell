//
// Created by thadumi on 4/25/18.
//

#include <stdlib.h>
#include <stdio.h>
#include "../headers/shell.h"
#include "../headers/loop_core.h"

/**
 * Here are done all the operation until the shell will be closed, like
 * free up any memory and terminates eventual children threads.
 * @return
 */
int _nsh_exit(void) {

    return EXIT_SUCCESS;
}

extern char host_name[1024];
extern char *current_dir;


//TODO: gain the CLI arguments for shell config ... if they are unseated
// check if there is a config file populated, otherwise create a default one
// and ask the user if he want to create a new one or main the default settings
// if he want to create a new one make a dialog to understand how he want to create it
// another approach, only in linux is to execute the command "nano .config" so that
// the user will be able to modify manually before the shell loop start.
int nsh() {

    //TODO: insert here the calls to config_function (after the stuffs that it is doing now the config function
    nsh_config();

    nsh_loop();

    return _nsh_exit();
}