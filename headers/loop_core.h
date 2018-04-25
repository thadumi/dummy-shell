//
// Created by thadumi on 4/24/18.
//

#ifndef SHELL_LOOP_CORE_H
#define SHELL_LOOP_CORE_H

#include "processe.h"

#define FALSE 0
#define TRUE !FALSE
//extern char *builtin_str[];
//extern int (*builtin_fun[]) (char **);

//void set_functions(char **str, int (*(*fun[])) (char **));

//int __NSH_NUM_BUILTIN(char **builtin_str);

//#define NSH_NUM_BUILTIN __NSH_NUM_BUILTIN(builtin_str)

char *nsh_read_line(void);

char **nsh_split_line(char *line);

int nsh_execute(char **args);

//TODO: make extern setter const pointer for prompt config and  starting_msg

/**
 * print the start message
 */
void nsh_start_msg(void);

/**
 * print the prompt in the form "<user>@<host> <cwd> >"
 */
void nsh_prompt(void);

#define LSH_RL_BUFSIZE 1024 //size for the input buffer in bytes

/**
 * Read the user input. It will end when enter is pressed
 * @return user input line
 */
char *nsh_read_line(void);


#define LSH_TOK_BUFFER 64
#define LSH_TOK_DELIM " \t\r\n\a"

/**
 * TODO: add quoting
 * @param line to parse
 * @return
 */
char **nsh_split_line(char *line);

int nsh_launch(char **args);

int nsh_execute(char **args);



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

/**
 * TODO: pipeline implementation
 * A pipe is an array of two file descriptors.
 * 1st descriptor si for read data and the 2nd for write data
 * The pipe's files are buffers into the kernel. By default when you try
 * to write into a pipe that is already full, the write call will wait until
 * it will be available space into the buffer. To change this behavior use
 * fcntl(pipe[1], F_SETFL, O_NONBLOCK);. In this way when the buffer is full the write
 * method will return an error (write(...) <= 0)
 */
//#define NEW_PIPE(name) int (name)[2];
typedef int pfd[2];

#define open_pipe(pfd) int __pipe_error = pipe((pfd))
#define if_open_error if(__pipe_error < 0)
int nsh_pipe();

#endif //SHELL_LOOP_CORE_H
