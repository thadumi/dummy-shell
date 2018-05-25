//
// Created by thadumi on 4/24/18.
//

#include "../headers/loop_core.h"
#include "../headers/util/utiliteas.h"
#include "../headers/process.h"
#include "../headers/util/collections/linkedl.h"
#include "../headers/logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zconf.h>
#include <wait.h>
#include <termios.h>
#include <ctype.h>

char host_name[1024];
char *current_dir;

pid_t GBSH_PID;
pid_t GBSH_PGID;
int GBSH_IS_INTERACTIVE;
struct termios GBSH_TMODES;

void handler(int sig)
{
    pid_t pid;

    pid = wait(NULL);

    printf("handler pid %d exited.\n", pid);
}

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
               || signal(SIGTSTP, SIG_IGN) == SIG_ERR  //TODO: add customs handlers here
               || signal(SIGTTIN, SIG_IGN) == SIG_ERR
               || signal(SIGTTOU, SIG_IGN) == SIG_ERR
               || signal(SIGCHLD, SIG_IGN) == SIG_ERR) { //signal(SIGCHLD, handler);
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
#define LSH_TOK_DELIM " \t\r\n\a"

node nsh_split_commands(char *line) {
    //from the line split they in single commands
    //then they are insert into a list that will be execute ordered in pipe

    node commands = NULL;

    char* cmd = trimwhitespace(line);
    char* next = strchr(cmd, '|');
    char* trim_cmd = trimwhitespace(cmd);

    while(next != NULL) {
        *next = '\0';
        trim_cmd = trimwhitespace(cmd);

        push(&commands, trim_cmd, sizeof(char) * (strlen(trim_cmd) + 1));

        cmd = next + 1;
        next = strchr(cmd, '|');
    }
    trim_cmd = trimwhitespace(cmd);
    push(&commands, trim_cmd, sizeof(char) * (strlen(trim_cmd) + 1));
    return commands;
}

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

static job nsh_parse_jobs(char *line) {
    job new_job = malloc(sizeof(struct _job));

    if(!new_job) {
        perror("can't create new jobs");
        return NULL;
    }

    size_t len = strlen(line) + 1;
    new_job->command = malloc(sizeof(char)*len);
    memcpy(new_job->command, line, len);
    new_job->next = NULL;
    new_job->head = NULL;
    new_job->notified = FALSE;
    new_job->tmodes = GBSH_TMODES;
    new_job->stout = STDOUT_FILENO;
    new_job->strin = STDIN_FILENO;
    new_job->strerr = STDERR_FILENO;

    char* cmd = trimwhitespace(line);
    char* next = strchr(cmd, '|');
    char* trim_cmd;

    while(next != NULL) {
        *next = '\0';
        trim_cmd = trimwhitespace(cmd);

        process p = malloc(sizeof(struct _proc));
        p->next = NULL;
        p->stopped = FALSE;
        p->completed = FALSE;
        p->argv = nsh_split_line(trim_cmd);
        if( new_job->head != NULL) {
            process tmp = new_job->head;
            while(tmp->next) tmp = tmp->next;
            tmp->next = p;
        } else new_job->head = p;

        cmd = next + 1;
        next = strchr(cmd, '|');
    }
    trim_cmd = trimwhitespace(cmd);

    process p = malloc(sizeof(struct _proc));
    p->next = NULL;
    p->stopped = FALSE;
    p->completed = FALSE;
    p->argv = nsh_split_line(trim_cmd);
    if( new_job->head != NULL) {
        process tmp = new_job->head;
        while(tmp->next) tmp = tmp->next;
        tmp->next = p;
    } else new_job->head = p;

    return new_job;
}

static int nsh_launch(char **args) {
    CREATE_CHILD(child_pid)
    IF_CHILD_CREATION_ERROR(child_pid)

        perror("fork error");

    CHILD_CODE(child_pid)

        // If we launch non-existing commands we end the process
        if (execvp(args[0],args) == -1){
            printf("Command not found");
            kill(getpid(),SIGTERM);
        }

    END_CHILD_CODE
    PARENT_CODE
        WAIT_FOR_CHILD(child_pid)
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

//#define open_pipe(pfd) int __pipe_error = pipe((pfd))
//#define if_open_error if(__pipe_error < 0)


//TODO: multiple sequences as in parcer pipe
static int pipe_join(char *com1[], char *com2[]) {
    pfd _pfd;       /* pipe */
    int status;     /* status */

    CREATE_CHILD(child)
    IF_CHILD_CREATION_ERROR(child)

        printf("unble to fork\n");
        return -1;

    CHILD_CODE(child)

        if( pipe(_pfd) < 0 ) { /* can use 'pfd' since address */
            printf("unable to pipe\n");
            return-1;
        }

        CREATE_CHILD(child1)
        IF_CHILD_CREATION_ERROR(child1)

            printf("unable to pipe\n");
            return -1;

        CHILD_CODE(child1)

            close(STD_OUTPUT);

            dup(_pfd[WRITE]);
            close(_pfd[READ]);
            close(_pfd[WRITE]);

            execvp(com1[0], com1);
            printf( "first execvp call failed!\n" );
            return( -1 );

        END_CHILD_CODE
        PARENT_CODE

            close( STD_INPUT );     /* close standard input */

            /* make standard input come from pipe */
            dup( _pfd[READ] );

            close( _pfd[READ] );       /* close file descriptors */
            close( _pfd[WRITE] );

            /* execute command 2 */
            execvp( com2[0], com2 );

            /* if execvp returns, error occured */
            printf( "second execvp call failed!\n" );
            return( -1 );

        END_PARENT_CODE
    END_CHILD_CODE
    PARENT_CODE

        wait(&status);
        return  status;

    END_PARENT_CODE
}

/*
 * Handle commands separatly
 * input: return value from previous command (useful for pipe file descriptor)
 * first: 1 if first command in pipe-sequence (no input from previous pipe)
 * last: 1 if last command in pipe-sequence (no input from previous pipe)
 *
 * EXAMPLE: If you type "ls | grep shell | wc" in your shell:
 *    fd1 = command(0, 1, 0), with args[0] = "ls"
 *    fd2 = command(fd1, 0, 0), with args[0] = "grep" and args[1] = "shell"
 *    fd3 = command(fd2, 0, 1), with args[0] = "wc"
 *
 * So if 'command' returns a file descriptor, the next 'command' has this
 * descriptor as its 'input'.
 */
static int pipe_multijoin(int input, int first, int last, char *args[]) {
    int pipettes[2];

    /* Invoke pipe */
    pipe( pipettes );
    static pid_t pid;
    pid = fork();

    /*
     SCHEME:
         STDIN --> O --> O --> O --> STDOUT
    */

    if (pid == 0) {
        if (first == 1 && last == 0 && input == 0) {
            // First command
            dup2( pipettes[WRITE], STDOUT_FILENO );
        } else if (first == 0 && last == 0 && input != 0) {
            // Middle command
            dup2(input, STDIN_FILENO);
            dup2(pipettes[WRITE], STDOUT_FILENO);
        } else {
            // Last command
            dup2( input, STDIN_FILENO );
        }

        if (execvp( args[0], args) == -1)
            _exit(EXIT_FAILURE); // If child fails
    }

    if (input != 0)
        close(input);

    // Nothing more needs to be written
    close(pipettes[WRITE]);

    // If it's the last command, nothing more needs to be read
    if (last == 1)
        close(pipettes[READ]);

    return pipettes[READ];
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
    CREATE_CHILD(child_pid)
    IF_CHILD_CREATION_ERROR(child_pid)

        perror("fork error");

        //TODO: add group process management
    CHILD_CODE(child_pid)
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
bool is_builtin(char **args) {
    for(int i = 0; i < builtin_fun_size; i++) {
        if(strcmp(args[0], builtin_str[i]) == 0)
            return TRUE;
    }

    return FALSE;
}
int execbin(char **args) {
    for(int i = 0; i < builtin_fun_size; i++) {
        if(strcmp(args[0], builtin_str[i]) == 0)
            return (*builtin_fun[i])(args);
    }

    return -1;
}

static int nsh_execute(char **args, int input, int first, int last) {
    if(args[0] == NULL) {
        return 1;
    }

    for(int i = 0; i < builtin_fun_size; i++) {
        if(strcmp(args[0], builtin_str[i]) == 0)
            return (*builtin_fun[i])(args);
    }

    //return nsh_launch(args);
    return pipe_multijoin(input, first, last, args);
}

int nsh_loop(void) {
    char *line; //buffer for user input
    char **args; //command + arguments buffer
    int status = 1;  //command output status code

    nsh_start_msg();

    do {
        //printf("> ");
        nsh_prompt();
        line = nsh_read_line();
        //commando-interno ? lancialo singolarmente
        //creare job

        job new_job = nsh_parse_jobs(line);

        if(is_builtin(new_job->head->argv)) {
            execbin(new_job->head->argv);
        } else launch_job(new_job, FOREGROUND);

        /*node commands = nsh_split_commands(line);

        const unsigned int commands_number = lenght(commands);
        int input = 0;
        int first  = 1;

        consume(commands, LAMBDA(void _(void* data, int current_index) {
            args = nsh_split_line((char*) data);
            input = nsh_execute(args, input, first, commands_number-1 == current_index ? 1 : 0);

            first = 0;
            free(args);
        }));

        int _s, _r;

        for (int i = 0; i < commands_number; ++i) {//wait for all process to end
            _r = wait(&_s);

            printf("wait %d with %d %d", i, _s, _r);
        }*/


        //args = nsh_split_line(line);
        //status = nsh_execute(args);

        printf("\n"); //empty line between two commands

        do_job_notification();
        log_job(new_job);
        //TODO: free(line);


    } while(status);

    //free(current_dir);

    //return status;
}