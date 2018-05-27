//
// Created by thadumi on 5/19/18.
//

#include <zconf.h>
#include <signal.h>
#include <errno.h>
#include <wait.h>
#include <time.h>
#include "../headers/process.h"

static job jobs = NULL;

#define foreach_job_as_j for(job j = jobs; j; j = j->next)

//TODO get this from loop_core
int GBSH_PID;
pid_t GBSH_PGID = 0;
struct termios GBSH_TMODES;

extern pid_t GBSH_PID;
extern pid_t GBSH_PGID;
extern int GBSH_IS_INTERACTIVE;
extern struct termios GBSH_TMODES;

job find_job(pid_t pgid) {

    foreach_job_as_j {
        if(j->pgid == pgid) return j;
    }

    return NULL;
}

bool job_is_stopped(job job) {
    foreach_proc_as_p_of(job)
        if(!p->completed && !p->stopped) return 0;
    return TRUE;
}

int job_is_completed(job job) {
    foreach_proc_as_p_of(job)
        if(!p->completed) return FALSE;
    return TRUE;
}

/*
 * Store the status of the process pid that was returned by waitpid.
   Return 0 if all went well, nonzero otherwise.
 */
int mark_process_status(pid_t pid, int status) {
    //printf("mark_process_status of %ld : %d\n", (long)pid, status);
    if(pid > 0 ) {
        foreach_job_as_j {
            foreach_proc_as_p_of(j) {
                if(p->pid == pid) {
                    p->status = status;
                    if(WIFSTOPPED(status)) {
                        //printf("%d stopped \n", pid );
                        p->stopped = TRUE;
                    }
                    else {

                        //printf("%d completed\n ", pid );
                        p->completed = TRUE;
                        if(WIFSIGNALED(status)) fprintf(stderr, "%d: Terminated by signal %d.\n", (int) pid, WTERMSIG (p->status));
                    }

                    if(p->stopped == TRUE || p->completed == TRUE)
                        time(&(p->eat));

                    return 0;
                }
            }
            fprintf (stderr, "No child process %d.\n", pid);
            return -1;
        };
    } else if(pid == 0 || errno == ECHILD) return -1;
    else {
        perror("waitpid");
        return  -1;
    }
}

void update_status(void) {
    int status;
    pid_t pid;

    do
        pid = waitpid(WAIT_ANY, &status, WUNTRACED | WNOHANG);
    while(!mark_process_status(pid, status));
}

void wait_for_job(job job) {
    int status;
    pid_t pid;

    do
        pid = waitpid(WAIT_ANY, &status, WUNTRACED);
    while(!mark_process_status(pid, status)
            && !job_is_stopped(job)
            && !job_is_completed(job));
}

/* Put job j in the foreground.  If cont is nonzero,
   restore the saved terminal modes and send the process group a
   SIGCONT signal to wake it up before we block.
*/
void put_job_in_foreground(job job, int cont) {
    tcsetpgrp(GBSH_PID, job->pgid); //put job in foreground

    if(cont) {
        tcsetattr(GBSH_PID, TCSADRAIN, &job->tmodes);
        if(kill(-job->pgid, SIGCONT) < 0)
            perror("kill");
    }

    wait_for_job(job);

    tcsetpgrp(GBSH_PID, GBSH_PGID); //restore the shell back in foreground

    tcgetattr(GBSH_PID, &job->tmodes);
    tcsetattr(GBSH_PID, TCSADRAIN, (const struct termios*) &GBSH_TMODES);
}

/* Put a job in the background.  If the cont argument is true, send
   the process group a SIGCONT signal to wake it up.
*/
void put_job_in_background(job job, bool cont) {
    if(cont)
        if(kill(-job->pgid, SIGCONT) < 0)
            perror("kill (SIGCONT)");
}

void format_job_info (job j, const char *status)  {
    fprintf (stderr, "%ld (%s): %s\n", (long)j->pgid, status, j->command);
}

extern bool is_builtin(char **args);
extern int execbin(char **args);

void launch_process(process p, pid_t pgid, int infile, int outfile, int errfile, exec_mode mode) {
    pid_t pid;
    if(GBSH_IS_INTERACTIVE) {
        pid = getpid();
        if(pgid == 0) pgid = pid;
        setpgid(pid, pgid);

        if(mode == FOREGROUND) tcsetpgrp(GBSH_PID, pid);

        signal (SIGINT, SIG_DFL);
        signal (SIGQUIT, SIG_DFL);
        signal (SIGTSTP, SIG_DFL);
        signal (SIGTTIN, SIG_DFL);
        signal (SIGTTOU, SIG_DFL);
        signal (SIGCHLD, SIG_DFL);
    }

    if(infile != STDIN_FILENO) {
        dup2(infile, STDIN_FILENO);
        close(infile);
    }

    if(outfile != STDOUT_FILENO) {
        dup2(outfile, STDOUT_FILENO);
        close(outfile);
    }

    if(errfile != STDERR_FILENO) {
        dup2(errfile, STDERR_FILENO);
        close(errfile);
    }
    execvp(p->argv[0], p->argv);

    perror("execvp");
    exit(1);
}

void launch_job(job job, exec_mode mode) {
    /*printf("launching job in %s \n", mode == FOREGROUND ? "foregound" : "background");

        foreach_proc_as_p_of(job)
            printf("process: %s\n", p->argv[0]);
    */
    do_job_notification();

    pid_t pid;
    int _pipe[2], infile, outfile;

    infile = job->strin;
    foreach_proc_as_p_of(job) {
        if (p->next)  {
            if (pipe (_pipe) < 0)  {
                perror ("pipe");
                exit (1);
            }
            outfile = _pipe[1];
        } else outfile = job->stout;

        /* Fork the child processes.  */
        pid = fork();

        if (pid == 0) /* This is the child process.  */
            launch_process (p, job->pgid, infile, outfile, job->strerr, FOREGROUND);
        else if (pid < 0)  { /* The fork failed.  */
            perror ("fork");
            exit (1);
        } else { /* This is the parent process.  */
            p->pid = pid;
            p->sat = time(NULL);
            //printf("ugo pid: %ld", (long) p->pid);
            //time(&(p->sat));
            if (GBSH_IS_INTERACTIVE)  {
                if (!job->pgid)
                    job->pgid = pid;
                setpgid (pid, job->pgid);
            }
        }

        /* Clean up after pipes.  */
        if (infile != job->strin)
            close (infile);
        if (outfile != job->stout)
            close (outfile);
        infile = _pipe[0];
    }

    //format_job_info(job, "launched");

    if (jobs == NULL)
        jobs = job;
    else {
        struct _job *last = jobs;
        while (last->next) last = last->next;

        last->next = job;
    }

    if (!GBSH_IS_INTERACTIVE) wait_for_job(job);
    else if (mode == FOREGROUND) put_job_in_foreground(job, 0);
    else put_job_in_background(job, 0);
}

void do_job_notification(void) {
    //printf("\nnotifications\n\n");
    job next_job = NULL;
    job last_job = NULL;
    job j;

    update_status();

    for(j = jobs; j; j = next_job) {
        //printf("job: %s\n with gpid %ld \n", j->command, (long)j->pgid);

        /*foreach_proc_as_p_of(j) {
            printf("\n\tproc %ld \n", (long) p->pid);
            char *arg;
            int i = 0;
            printf("\t\targv: ");
            while((arg = p->argv[i++]) != NULL) printf("%s, ", arg);
            printf("\n");
            printf("\t\tstatus: %d\n", p->status);
        }*/
        next_job = j->next;

        if(job_is_completed(j)) {
            //format_job_info(j, "completed");

            if(last_job) last_job->next = next_job;
            else jobs = next_job;

            free(j);
        } else if(job_is_stopped(j) && !j->notified) {
            //format_job_info(j, "stopped");
            j->notified = 1;
            last_job = j;
        } else last_job = j;
        //printf("\n");
    };

    //printf("\nend notifications\n");
}

void mark_jos_as_running(job job) {
    foreach_proc_as_p_of(job)
        p->stopped = 0;
    job->notified = 0;
}

void continue_job(job job, exec_mode mode) {
    mark_jos_as_running(job);

    if(mode == FOREGROUND) put_job_in_foreground(job, 1);
    else put_job_in_background(job, 1);
}

