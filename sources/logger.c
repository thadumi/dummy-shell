//
// Created by thadumi on 5/23/18.
//

#include "../headers/logger.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "../headers/util/utiliteas.h"
#include "../headers/config.h"

#define COMMENT_TOKEN '#'
#define KEYWORD_START_TOKEN '$'

#define NAME_TOKEN "$name"
#define ARGS_TOKEN "$args"
#define ARGS_LENGHT_TOKEN "$argsnr"
#define EXECUTED_IN_PIPE_TOKEN "$wipipe"
#define EXIT_STATUS_TOKEN "$status"
#define START_AT_TOKEN "$start"
#define END_AT_TOKEN "$end"
#define PID_TOKEN "$pid"

#define NAME_TOKEN_TYPE "%s"
#define ARGS_TOKEN_TYPE "%s"
#define ARGS_LENGHT_TOKEN_TYPE "%d"
#define EXECUTED_IN_PIPE_TOKEN_TYPE "%d"
#define EXIT_STATUS_TOKEN_TYPE "%d"
#define START_AT_TOKEN_TYPE "%s"
//"[%d %d %d  %d:%d:%d]"
#define END_AT_TOKEN_TYPE "%s"
#define PID_TOKEN_TYPE "%ld"

#define TOKENS 9
#define MAX_TOKEN_LENGT


const char * const TOKENS_LIST[][10] = {
            {NAME_TOKEN, NAME_TOKEN_TYPE},
            {ARGS_TOKEN, ARGS_TOKEN_TYPE},
            {ARGS_LENGHT_TOKEN, ARGS_LENGHT_TOKEN_TYPE},
            {EXECUTED_IN_PIPE_TOKEN, EXECUTED_IN_PIPE_TOKEN_TYPE},
            {EXIT_STATUS_TOKEN, EXIT_STATUS_TOKEN_TYPE},
            {START_AT_TOKEN, START_AT_TOKEN_TYPE},
            {END_AT_TOKEN, END_AT_TOKEN_TYPE},
            {PID_TOKEN, PID_TOKEN_TYPE}
        };

#define foreach_token for(int i = 0; i < TOKENS; i++)
#define tokval TOKENS_LIST[i][0]
#define toktype TOKENS_LIST[i][1]

extern char* path_template_file;
extern char* path_output_file;
extern char* output_file_write_mode;
extern char* path_error_file;

int check_template_path() {
    printf("file: #%s#\n", path_template_file);
    FILE* tmp;
    if(!(tmp = fopen(path_template_file, "r"))) {
        perror("error opening the template file");
        return FALSE;
    } else fclose(tmp);

    return TRUE;
}

char** var_to_inject(char *line) {
    ssize_t count_word_lenght(char* ws) {
        char* we = ws;
        while((*we++ != ' ') && (*we++ != '\n') );

        return we - ws -1;
    }

    char** to_inject = malloc(sizeof(char*) * TOKENS + 1);
    int tokens = 0;

    char* ix = line;
    while((ix = strchr(ix, KEYWORD_START_TOKEN)) != NULL ) {
        ssize_t tl = count_word_lenght(ix);
        char* token = malloc(sizeof(char) * (tl + 1));
        memcpy(token, ix, sizeof(char) * tl);
        token[tl] = '\0';

        //printf("\n\ttoken: %s\n", token);
        //char* nl = strsr(line, token, NAME_TOKEN_TYPE);
        //printf("\n\t");printf(nl, "nome_proc");
        //free(token);
        //free(nl);

        ix++;
        to_inject[tokens++] = token;
    }

    to_inject[tokens] = NULL;
    return to_inject;
}

/*char* generate_templete_file() {
    long word_in_file() {
        FILE* fp = fopen(path_template_file, "r");

        if(!fp) return -1;

        long c = 0;
        while(fgetc(fp)) c++;

        fclose(fp);
        return c;
    }


    long buffer_size = word_in_file();

    if(buffer_size <= 0) abort(); //TODO: fix errors code

    char* buffer = malloc(sizeof(char) * buffer_size);

    FILE* fp;

    return NULL;
}*/

/*int parse_template() {
    FILE* template_file = fopen(path_template_file, "r");

    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    while ( !feof(template_file) &&
            (read = getline(&line, &len, template_file)) != -1) {
        strcr(line, COMMENT_TOKEN, '\0'); //remove the comments
        printf("\norg: %s ", line);
        var_to_inject(line);
    }

    if(line) free(line);
    fclose(template_file);
    return 0;
}*/

static char** lines;

#define LOGGER_INITIALIZATION_OK 0
#define LOGGER_INITIALIZATION_FAIL_WITH_FILE_ERROR 10
#define LOGGER_ERROR_INVALID_TOKEN 10 << 1

int init_logger() {
    FILE* template_file = fopen(path_template_file, "r");

    if(!template_file)
        return LOGGER_INITIALIZATION_FAIL_WITH_FILE_ERROR;

    lines = malloc(sizeof(char*) * 51); //TODO: make dynamical the allocation

    char* line = NULL;
    size_t len = 0;
    ssize_t read;

    int line_count = 0;

    while ( !feof(template_file) &&
            (read = getline(&line, &len, template_file)) != -1) {

        char* new_line = malloc(sizeof(char) * read + 1);
        strcpy(new_line, line);

        lines[line_count++] = new_line;
    }

    lines[line_count] = NULL;

    if(line) free(line);
    fclose(template_file);


    //tokens validation
    int cl = 0;
    line = lines[cl];
    while(line != NULL) {
        strcr(line, COMMENT_TOKEN, '\0');

        char**tokens = var_to_inject(line);

        int token = 0;

        while(tokens[token] != NULL) {
            bool any_match = FALSE;

            foreach_token{ //lockup it's a valid token
                //printf("compering %s with %s \n", tokens[token], tokval);
                if(strcmp(tokens[token], tokval) == 0) {
                    any_match = TRUE;
                    break;
                }
            };

            if(any_match == FALSE) {//if it is not a valid token report
                printf("Error parsing the template file %s\n", path_template_file);
                printf("The line \"%s\", has the invalid token %s\n", line, tokens[token]);

                abort(); //TODO: change it with errore logs and autogenerated file
            } else printf("The token %s is ok\n", tokens[token]);

            token++;
        }

        line = lines[++cl];
    }
    cl = 0;
    line = lines[cl];

    return LOGGER_INITIALIZATION_OK;
}

static FILE* of = NULL;
static FILE* ef = NULL;

void log_job(job job) {
    if(of == NULL) of = fopen(path_output_file, output_file_write_mode);
    if(ef == NULL) ef = fopen(path_error_file, output_file_write_mode);
    if(!of) {
        printf("Error opening the file %s for writing the log information", path_output_file);
        abort();
    }

    fprintf(of, "New job executed at $job_start_time");
    fprintf(of, "with command: %s\n", job->command);
    fprintf(of, "with gpid: %d\n", job->pgid);
    fprintf(of, "with status: status");
    fprintf(of, "with execution mode: %s\n", "foregound");
    fprintf(of, "with the following processes:\n");

    foreach_proc_as_p_of(job) {
        int cl = 0;
        char *line = lines[cl];
        while(line != NULL) {
            if(strlen(line) == 0) { //line empty === the line was a comment
                line = lines[++cl];
                continue;
            }
            //ship the empty line -> comments
            char** tokens = var_to_inject(line);

            int token = 0;
            char* nl = strsr(line, " ", " "); //to create a copy of the line
            //char *nl_old =nl;
            while(tokens[token] != NULL) {
                //nl_old = nl;
                //if(tokens)
                //HERE CHECK TOKEN AND CHANGE STRING TYPE


                fprintf(of, "\t");

                if(strcmp(tokens[token], NAME_TOKEN) == 0)
                    nl = strsr(nl, tokens[token], p->argv[0]);
                else if(strcmp(tokens[token], ARGS_TOKEN) == 0) {
                    //printf("args\n");
                    size_t buff_size = 0;
                    int i = 1;
                    char *arg;
                    while( (arg = p->argv[i++]) != NULL) {
                        buff_size += strlen(arg) * sizeof(char);
                    }
                    //printf("if\n");

                    if(buff_size == 0 ) //no command

                        nl = strsr(nl, tokens[token], "null args");

                    //else if(buff_size == sizeof(p->argv[0])) //command without args
                    //   nl = strsr(nl, tokens[token], "no args");

                    else {
                        int argc = i;
                        char* tmp = malloc(buff_size + (argc * sizeof(char)));
                        tmp[0] = '\0';
                        i = 1;
                        while(p->argv[i] != NULL) {
                            printf("#%s#", tmp);
                            strcat(tmp, p->argv[i++]);

                            if(i < argc) strcat(tmp, " ");
                        }

                        nl = strsr(nl, tokens[token], tmp);
                        free(tmp);
                    }

                } else if(strcmp(tokens[token], ARGS_LENGHT_TOKEN) == 0) {
                    int argc = 1;

                    while(p->argv[argc++] != NULL);

                    nl = strsr(nl, tokens[token], ARGS_LENGHT_TOKEN_TYPE);
                    sprintf(nl, nl, argc - 2);

                } else if(strcmp(tokens[token], EXECUTED_IN_PIPE_TOKEN) == 0) {
                    nl = strsr(nl, tokens[token], EXECUTED_IN_PIPE_TOKEN_TYPE);
                    sprintf(nl, nl, p->next ? 1 : 0);
                } else if(strcmp(tokens[token], EXIT_STATUS_TOKEN) == 0) {
                    nl = strsr(nl, tokens[token], EXIT_STATUS_TOKEN_TYPE);
                    sprintf(nl, nl, p->status);
                } else if(strcmp(tokens[token], START_AT_TOKEN) == 0) {
                    nl = strsr(nl, tokens[token], START_AT_TOKEN_TYPE);
                    char* temp = nl;
                    nl = malloc(strlen(nl) * sizeof(char) + sizeof(char) * 100);
                    memcpy(nl, temp, strlen(temp) * sizeof(char));
                    free(temp);
                    struct tm* timeinfo = localtime( &(p->sat));


                    char buffer[50];
                    memset(buffer, '\0', sizeof(char) * 50);
                    memcpy(buffer, asctime(timeinfo), 24 * sizeof(char));
                    buffer[24] = '\n';
                    buffer[25] = '\0';
                    sprintf(nl, nl, buffer);

                } else if(strcmp(tokens[token], PID_TOKEN) == 0) {
                    nl = strsr(nl, tokens[token], PID_TOKEN_TYPE);

                    char* temp = nl;
                    nl = malloc(strlen(nl) * sizeof(char) + sizeof(char) * 50);
                    memcpy(nl, temp, strlen(temp) * sizeof(char));
                    free(temp);
                    sprintf(nl, nl, (long) p->pid);
                    strcat(nl, "\n");
                }
                token++;
            }
            printf("%s",nl);
            fprintf(of, nl);
            free(nl);
            line = lines[++cl];

            if(p->status != 0) {
                fprintf(ef, "Error: process %d exit with code %d\n", p->pid, p->status);
            }
        }

        fprintf(of, "\n");
    }

    //fclose(of);

}


void close_logger() {
    if(of) fclose(of);
    if(ef) fclose(ef);
}