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


int nsh() {

    nsh_config();
    nsh_loop();

    return _nsh_exit();
}