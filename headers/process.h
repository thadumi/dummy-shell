//
// Created by thadumi on 4/24/18.
//

#ifndef SHELL_PROCESSE_H
#define SHELL_PROCESSE_H

#define CREATE_CHILD pid_t child_pid = fork();
#define IF_CHILD_CREATION_ERROR if(child_pid < 0) {
#define CHILD_CODE } else if(child_pid == 0) {
#define END_CHILD_CODE }
#define PARENT_CODE else {
#define END_PARENT_CODE }

#define WAIT_FOR_CHILD pid_t __wpid; int __status; \
    do \
        __wpid = waitpid(child_pid, &__status, WUNTRACED); \
    while(!WIFEXITED(__status) && !WIFSIGNALED(__status));

typedef char exec_mode;

#define BACKGROUND 1<<1
#define FOREGROUND 1<<2

#endif //SHELL_PROCESSE_H
