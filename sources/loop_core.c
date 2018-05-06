//
// Created by thadumi on 4/24/18.
//

#include "../headers/loop_core.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zconf.h>
#include <wait.h>
#include <termios.h>

char host_name[1024];
char *current_dir;

static pid_t GBSH_PID;
static pid_t GBSH_PGID;
static int GBSH_IS_INTERACTIVE;
static struct termios GBSH_TMODES;

void nsh_init(void) {

    /* See if we are running interactively.  */
    GBSH_PID = STDIN_FILENO;
    GBSH_IS_INTERACTIVE = isatty (GBSH_PID);

    if (GBSH_IS_INTERACTIVE) {
        /* Loop until we are in the foreground.  */
        while (tcgetpgrp (GBSH_PID) != (GBSH_PGID = getpgrp ())) kill (- GBSH_PGID, SIGTTIN);

        /**
         * Signals is a software interrupt, ie it can change the flow of a program
         * because when a signal is delivered to a process the process will stop what its doing,
         * either handle or ignore the signal, or terminate, depending on the signal kind.
         * The signals are asynchronous because they may be originated outside of the currently execution process.
         *
         * For example typing killall nano in a shell, it will be transmitted a SIGTERM to all nano processes
         * to kill them all. However a process that receives SIGTERM doesn't actually have to terminate.
         *
         * SIGINT is the signal for CTRL+C
         * SIGTSTP signal for CTRL-Z
         * SIGCONT stands for fg/bg
         * SIGSTOP stop
         * SIGCHLD child stopped or terminated
         * SIGPIPE broken pipe
         * SIGKILL kill
         * SIGQUIT quit from keyboard
         * SIGTTIN/OU tty input/output for background process
         * SIGTSTP top typed at tty
         * (more in sys/signal.h)
         *
         * A process can choose the correct response for each signal, ie it can also ignore them all.
         * By default each signal has an action:
         *      -- TERM -> the process will terminate
         *      -- CORE -> the process will terminate with a core dump file
         *                 that traces the process state at the time of termination
         *      -- IGN  -> the process will ignore the signal
         *      -- STOP -> the process will stop, ie CTRL+Z
         *      -- CONT -> the process will continue form being stopped
         * Signals like SIGKILL and SIGABRT have their default action that can't be changed ("kill 9" is the ultimate kill)
         *
         * int signal(int signum, void (*handler)(int)) set the handler to the signal signum
         *     raise(int signum) send the signum signal to the calling process
         *
         *  SIG_IGN default ignore functions
         *  SIG_DFL restore default signal handler
         *
         * an advance way to set the signal's handler is using the sigaction function
         *      int sigaction(int signum, const struct sigaction *newact, struct sigaction *oldact);
         * this function allows us to handler a signal with reentrant functions to avoid EINTR errors
         *
         * struct sigaction {à
         *      void (*sa_handler)(int);
         *      void (*sa_sigaction)(int, siginfo_t *, void *);
         *      sigset_t sa_mask;
         *      int sa_flags; //SA_RESTART to restart the interrupted function,
         * }
         *
         *
         */

        /* Ignore interactive and job-control signals. */
        if(    signal(SIGINT, SIG_IGN)  == SIG_ERR
               || signal(SIGQUIT, SIG_IGN) == SIG_ERR
               || signal(SIGTSTP, SIG_IGN) == SIG_ERR
               || signal(SIGTTIN, SIG_IGN) == SIG_ERR
               || signal(SIGTTOU, SIG_IGN) == SIG_ERR
               || signal(SIGCHLD, SIG_IGN) == SIG_ERR) {
            perror("signals");
            exit(EXIT_FAILURE);
        }

        /* Put ourselves in our own process group.  */
        GBSH_PGID = getpid ();
        if (setpgid (GBSH_PGID, GBSH_PGID) < 0) {
            perror ("Couldn't put the shell in its own process group");
            exit (EXIT_FAILURE);
        }

        /* Grab control of the terminal, i.e. get foreground control */
        tcsetpgrp (GBSH_PID, GBSH_PGID);

        /* Save default terminal attributes for shell.  */
        tcgetattr (GBSH_PID, &GBSH_TMODES);
    } else {
        printf("Could not make the shell interactive.\n");
        exit(EXIT_FAILURE);
    }


}

int nsh_config(void) {

    nsh_init();

    current_dir = (char*) calloc(1024, sizeof(char)); //create the buffer to store the cwd path
    gethostname(host_name, sizeof(host_name)); //load the hostname

    return 0;
}

//TODO: allow template creation for welcome message and line start prompt
void nsh_start_msg(void) {
    printf("\t\tWelcome\n");
}

void nsh_prompt(void) {
    printf("%s@%s %s > ", getenv("LOGNAME"), host_name, getcwd(current_dir, 1024));
}

#define LSH_RL_BUFSIZE 1024 //size for the input buffer in bytes

static char *nsh_read_line(void) {
    int buffer_size = LSH_RL_BUFSIZE;
    register ssize_t position = 0;
    int c;
    char *buffer = malloc(sizeof(char) * buffer_size);
    memset(buffer, '\0', buffer_size);

    if(!buffer) {
        fprintf(stderr, "nsh: allocation input buffer error\n");
        exit(EXIT_FAILURE); //or return NULL with checks
    }

    /*
     * read the user input and allocate it into the buffer
     */
    while(TRUE) {
        c = getchar(); //read the next char

        if(c == EOF || c == '\n') {   //if it was tipped enter
            buffer[position] = '\0';    //close the buffer
            return buffer;              //and return it
        } else buffer[position++] = c;//else add the new char to the buffer and update the position into the buffer

        if(position >= buffer_size) {             //if the buffer is going to stackoverflow
            buffer_size += LSH_RL_BUFSIZE;          //increment the buffer size
            buffer = realloc(buffer, buffer_size);  //and reallocate it

            if(!buffer) {                           //if the buffer reallocation failed
                fprintf(stderr, "nsh: allocation error\n");
                exit(EXIT_FAILURE);                  //exit with an error code
            }

        }
    }

    return NULL;

    /**
     * with getline
     *
     * char *line = NULL;
     * ssize_t buffer_size = 0;
     * getline(&line, &buffersize, stdin);
     * retrun line;
     */
}


#define LSH_TOK_BUFFER 64
#define LSH_TOK_DELIM " \t\r\n\a|"

/**
 * TODO: add quoting
 * @param line to parse
 * @return
 */
static char **nsh_split_line(char *line) {

    ssize_t buffer_size = LSH_TOK_BUFFER;
    char **tokens = malloc(sizeof(char*) * buffer_size);
    char *token;

    if(!tokens) {
        fprintf(stderr, "nsh: allocation error\n");
        exit(EXIT_FAILURE);
    }

    int position = 0;
    token = strtok(line, LSH_TOK_DELIM);
    while(token != NULL) {
        tokens[position++] = token;

        if (position >= buffer_size) {
            buffer_size += LSH_TOK_BUFFER;
            tokens = realloc(tokens, buffer_size * sizeof(char *));

            if (!tokens) {
                fprintf(stderr, "nsh: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }

        token = strtok(NULL, LSH_TOK_DELIM);
    }
    tokens[position] = NULL;
    return tokens;
}

static int nsh_launch(char **args) {
    CREATE_CHILD
    IF_CHILD_CREATION_ERROR

        perror("fork error");

    CHILD_CODE

        // If we launch non-existing commands we end the process
        if (execvp(args[0],args) == -1){
            printf("Command not found");
            kill(getpid(),SIGTERM);
        }

    END_CHILD_CODE
    PARENT_CODE
        WAIT_FOR_CHILD
    END_PARENT_CODE

    return 1;
}

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

static int nsh_pipe() {
    pfd p;
    char c;

    open_pipe(p);
    if_open_error {
        perror("pipe");
        return 1;
    };

    CREATE_CHILD
    IF_CHILD_CREATION_ERROR
        perror("fork");
        return 1;
    CHILD_CODE
        close(0); //close(stdin);

        dup2(p[0], 0); //copy pipe input descriptor in stdin

        close(p[1]); //the child don't need the writing end

        while(read(0, &c, 1) > 0) write(1, &c, 1);
        exit(0);
    END_CHILD_CODE
    PARENT_CODE
        close(1);
        //duplicate writing end to stdout
        dup2(p[1], 1);

        //close reading end
        close(p[0]);


        //read and read 1 byte from stdin, write byte to pipe
        while(read(0,&c,1) > 0){
            write(1, &c, 1);
        }

        //close the pipe and stdout
        close(p[1]);
        close(1);

        //wait for child
        int status;
        wait(&status);

    END_PARENT_CODE
    return 0;

}

/**
 * nsh_launch with fg/bg support
 * @param args
 * @param mode
 * @return
    tokens[position] = NULL;

    return tokens;
}

static int nsh_launch(char
 */
static int _nsh_launch(char** args, exec_mode mode) {
    CREATE_CHILD
    IF_CHILD_CREATION_ERROR

        perror("fork error");

        //TODO: add group process management
    CHILD_CODE
        //any child processes ignore signal actions by inheritance
        //so every child have to reset its own signals
        //signal (SIGINT, SIG_DFL);
        //signal (SIGQUIT, SIG_DFL);
        //signal (SIGTSTP, SIG_DFL);
        //signal (SIGTTIN, SIG_DFL);
        //signal (SIGTTOU, SIG_DFL);
        //signal (SIGCHLD, SIG_DFL);

        // set the child to ignore SIGINT signals (we want the parent
        // process to handle them with signalHandler_int)
        signal(SIGINT, SIG_IGN);


        // We set parent=<pathname>/simple-c-shell as an environment variable
        // for the child
        //setenv("parent",getcwd(currentDirectory, 1024),1);

        // If it was launched non-existing commands, end the process
        if (execvp(args[0],args) == -1){
            printf("Command not found");
            kill(getpid(),SIGTERM);
        }

    END_CHILD_CODE
    PARENT_CODE

        int status;
        pid_t wpid;

        if(mode == BACKGROUND) {
            do {
                wpid = waitpid(child_pid, &status, WUNTRACED);
            } while (!WIFEXITED(status) && !WIFSIGNALED(status));
        } else {
            // In order to create a background process, the current process
            // should just skip the call to wait. The SIGCHILD handler
            // signalHandler_child will take care of the returning values
            // of the childs.
            printf("Process created with PID: %d\n",child_pid);
        }

    END_PARENT_CODE

    return 1;
}

//#########################################################################

//builtin shell commands

//int nsh_cd(char **args);
//int nsh_help(char **args);
//int nsh_exit(char **args);

//list of the commands and their corresponding functions

/* TODO implement as
struct Builtin {
    char* name;
    int (*func) (char**);
};

struct Builtin builtins[] = {
    { "cd", &nsh_cd },
    { "help", &nsh_help },
    { "exit", &nsh_exit }
};
 */

/**
 * builtin_str, builtin_fun, and builtin_fun_size are global constants
 * that must be defined into the file that will call
 * the nsh() function defined in /headers/shell.h.
 *
 * builtin_str is an array of strings as
 * {
 *  "command_0",
 *  "command_1"
 * }
 * and contains the name with witch a function has to be call from the cli.
 *
 * The functions are defined into builtin_fun. This array contain the pointer to each
 * function that must be in the form int <function-name>(char **args). It always return
 * an exit code and accept and array of string that contains the function arguments.
 * The functions' pointer must fallow the order defined into builtin_str.
 *
 * builtin_fun_size contain the size of the builtin_str and builtin_fun arrays.
 */
extern const char *builtin_str[];

//int (*(*builtin_fun[])) (char **);

extern const int (*builtin_fun[]) (char **);

extern const int builtin_fun_size;

//int __NSH_NUM_BUILTIN (char **builtin_str) {
//    return sizeof(builtin_str) / sizeof(char *);
//}

/*void set_functions(char **str, int (*(*fun[])) (char **)) {
    builtin_str = str;
    builtin_fun = fun;
}*/


static int nsh_execute(char **args) {
    if(args[0] == NULL) {
        return 1;
    }

    for(int i = 0; i < builtin_fun_size; i++) {
        if(strcmp(args[0], builtin_str[i]) == 0)
            return (*builtin_fun[i])(args);
    }

    return nsh_launch(args);
}

int nsh_loop(void) {
    char *line; //buffer for user input
    char **args; //command + arguments buffer
    int status;  //command output status code

    nsh_start_msg();

    do {
        //printf("> ");
        nsh_prompt();
        line = nsh_read_line();
        args = nsh_split_line(line);
        status = nsh_execute(args);

        printf("\n"); //empty line between two commands
        free(line);
        free(args);
    } while(status);

    free(current_dir);

    return status;
}