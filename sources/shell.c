//
// Created by thadumi on 4/25/18.
//

#include <stdlib.h>
#include <stdio.h>
#include "../headers/shell.h"
#include "../headers/loop_core.h"
#include "../headers/logger.h"
#include "../headers/config.h"
#include "../headers/cli_arguments_parser.h"

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

//char *path_template_file = "../log.template";
//char *path_output_file = "";
//char* output_file_open_mode = "";

int nsh(int argc, char** argv) {
    char *tmp=malloc(strlen(getenv("HOME") + strlen("/.os-shell/.config") +1));
    strcpy(tmp, getenv("HOME"));
    printf("tmp: %s", tmp);
    strcat(tmp, "/.os-shell/.config");
    printf("tmp: %s", tmp);

    getfile(argc, argv);

    load_configuration(tmp);
    free(tmp);



    if(!check_template_path()) {
        printf("error in template path");
    } else printf("ok template path");
    printf("\n");

    init_logger();


    nsh_config();
    nsh_loop();

    close_logger();

    return _nsh_exit();
}