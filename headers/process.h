//
// Created by thadumi on 4/24/18.
//

#include "util/collections/linkedl.h"
#include "util/utiliteas.h"
#include <termios.h>

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
typedef unsigned int exec_mode;

//constants for exec_mode
#define BACKGROUND 1u<<1u
#define FOREGROUND 1u<<2u

#define STD_INPUT  0    /* file descriptor for standard input  */
#define STD_OUTPUT 1    /* file descriptor for standard output */
#define READ  0         /* read file descriptor from pipe  */
#define WRITE 1         /* write file descriptor from pipe */


typedef node commands_in_pipe;

/*struct command{
    char* name;
    char* args;

    exec_mode execMode;
    void* thread;
};

struct {
    
};*/

struct _proc {
    char **argv; //for exec
    pid_t pid;   //process id
    bool completed; //true if the process has finished its job
    bool stopped; //true if the process has stopped
    int status; //reported status value
    time_t sat; //start at
    time_t eat; //end at
    struct _proc *next; //next process in pipeline
};

typedef struct _proc* process;

struct _job {
    char *command;
    struct _proc *head; //llist of processes in this job
    pid_t pgid; //process group id
    bool notified;
    struct termios tmodes; //saved terminal modes
    int strin, stout, strerr; //i/o channels

    struct _job *next; //next active job
};

typedef struct _job *job;

#define foreach_proc_as_p_of(job) for(struct _proc* p = (job)->head; p; p = p->next)

void launch_job(job job, exec_mode mode);
void do_job_notification(void);
#endif //SHELL_PROCESSE_H