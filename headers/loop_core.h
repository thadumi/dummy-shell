//
// Created by thadumi on 4/24/18.
//

#ifndef SHELL_LOOP_CORE_H
#define SHELL_LOOP_CORE_H

#include "process.h"
#include "util/utiliteas.h"
#include "util/collections/linkedl.h"


//extern char *builtin_str[];
//extern int (*builtin_fun[]) (char **);

//void set_functions(char **str, int (*(*fun[])) (char **));

//int __NSH_NUM_BUILTIN(char **builtin_str);

//#define NSH_NUM_BUILTIN __NSH_NUM_BUILTIN(builtin_str)

//char *nsh_read_line(void);

//char **nsh_split_line(char *line);

//int nsh_execute(char **args);

//TODO: make extern setter const pointer for prompt config and  starting_msg

/**
 * print the start message
 */
void nsh_start_msg(void);

/**
 * print the prompt in the form "<user>@<host> <cwd> >"
 */
void nsh_prompt(void);


/**
 * Read the user input. It will end when enter is pressed
 * @return user input line
 */
//char *nsh_read_line(void);


//char **nsh_split_line(char *line);

//int nsh_launch(char **args);

//int nsh_execute(char **args);



void nsh_init(void);

/**
 * Reads the config files and initialize the shell
 * @return a code to communicate the load process result
 */

int nsh_config(void);

/**
 * This is the main loop of the shell. Here live the interpreter that reads
 * the commands and executes them.
 * So the shell loop has three steps:
 *  read:  reads the command from standard input
 *  parse: separate the command string into a program and arguments
 *  execute: run the parsed command
 * @return
 */
int nsh_loop(void);

//int nsh_pipe();

#endif //SHELL_LOOP_CORE_H
