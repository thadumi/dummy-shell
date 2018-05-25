//
// Created by thadumi on 5/23/18.
//

#include "../headers/config.h"
#include "../headers/util/utiliteas.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* welcome_mesasge;
char* prompt_message_template;
char* path_template_file;
char* path_output_file;
char* output_file_write_mode;


void load_configuration(char* confi_path) {
    FILE* cf = fopen(confi_path, "r");

    if(!cf) {
        printf("Error opening the configuration file, will be use the default configuration");
        return;
    }

    char* line = NULL;
    size_t len = 0;
    ssize_t read;

    while ( !feof(cf) &&
            (read = getline(&line, &len, cf)) != -1) {
        char* eq = strchr(line, '=');
        *eq = '\0'; //divide the line in two parts
        char* value = trimwhitespace(eq + 1);

        if(strcmp(line, "welcome_message") == 0) {

            welcome_mesasge = malloc(sizeof(char) * strlen(value));
            strcpy(welcome_mesasge, value);

        } else if(strcmp(line, "prompt_msg") == 0) {

            prompt_message_template = malloc(sizeof(char) * strlen(value));
            strcpy(prompt_message_template, value);

        } else if(strcmp(line, "input_file") == 0) {

            path_template_file = malloc(sizeof(char) * strlen(value));
            strcpy(path_template_file, value);

        } else if(strcmp(line, "output_file") == 0) {

            path_output_file = malloc(sizeof(char) * strlen(value));
            strcpy(path_output_file, value);

        } else if(strcmp(line, "output_file_write_mode") == 0) {
            output_file_write_mode = malloc(sizeof(char) * strlen(value));
            strcpy(output_file_write_mode, value);
        }

        //if(value) free(value);

    }
}


        //if(value) free(value);
