//
// Created by thadumi on 4/24/18.
//

#ifndef SHELL_PROCESSE_H
#define SHELL_PROCESSE_H

//template code for children management

#define CREATE_CHILD(child_pid) pid_t (child_pid) = fork();
#define IF_CHILD_CREATION_ERROR(child_pid) if((child_pid) < 0) {
#define CHILD_CODE(child_pid) } else if((child_pid) == 0) {
#define END_CHILD_CODE }
#define PARENT_CODE else {
#define END_PARENT_CODE }


#define WAIT_FOR_CHILD(child_pid) pid_t __wpid; int __status; \
    do \
        __wpid = waitpid((child_pid), &__status, WUNTRACED); \
    while(!WIFEXITED(__status) && !WIFSIGNALED(__status));

//define the execution mode of a process
//it can be in foreground or in background
typedef char exec_mode;

//constants for exec_mode
#define BACKGROUND 1<<1
#define FOREGROUND 1<<2

#define STD_INPUT  0    /* file descriptor for standard input  */
#define STD_OUTPUT 1    /* file descriptor for standard output */
#define READ  0         /* read file descriptor from pipe  */
#define WRITE 1         /* write file descriptor from pipe */


#endif //SHELL_PROCESSE_H
