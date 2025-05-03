#define _GNU_SOURCE 1//strerrorname_np(3)
#define _ISOC99_SOURCE 1 //snprintf
#include <stdio.h> //printf
#include <sys/ptrace.h> //ptrace(2)
#include <sys/wait.h> //waitpid
#include <linux/ptrace.h> //struct ptrace_syscall_info
#include <unistd.h> //fork(2)
#include <sched.h> //pid_t
#include <signal.h> //raise,
#include <stdlib.h> //exit(2)
#include <string.h> //strerror(3), strerrorname_np(3)

int forked(char** argv){
    // printf("Inside child %d my parent is %d\n", getpid(), getppid());
    ptrace(PTRACE_TRACEME,0,0,0);
    raise(SIGSTOP);
    int err = execve(argv[0],  argv, NULL);
    exit(err);
}


int main(int argc, char** argv)
{
    pid_t pid = fork();
    if(pid == 0) forked(&argv[1]);
    int status;
    waitpid(pid, &status,0);
    ptrace(PTRACE_SETOPTIONS, pid, 0, PTRACE_O_TRACESYSGOOD);
    while (1) {
        struct ptrace_syscall_info pt_sysinfo = {0};
        ptrace(PTRACE_SYSCALL, pid, 0, 0); // resume until next syscall entry/exit
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            exit(0); // Exit the loop if the child process has exited
        }
        ptrace(PTRACE_GET_SYSCALL_INFO, pid, sizeof(struct ptrace_syscall_info), &pt_sysinfo);
        if(pt_sysinfo.op == PTRACE_SYSCALL_INFO_ENTRY && (pt_sysinfo.entry.nr == 0 || pt_sysinfo.entry.nr == 1) ) 
        {
            char *syscall_args = malloc(120);
            if(syscall_args == NULL){
                perror("malloc");
                exit(EXIT_FAILURE);
            }
            char *syscall_name = (pt_sysinfo.entry.nr == 0) ? "read" : "write";
            snprintf(syscall_args, 120, "%s(%d, 0x%llx, %lld) = ",
                syscall_name,
                (int) pt_sysinfo.entry.args[0],        
                pt_sysinfo.entry.args[1],        
                pt_sysinfo.entry.args[2] );

            ptrace(PTRACE_SYSCALL, pid, 0, 0); 
            waitpid(pid, &status, 0);
            ptrace(PTRACE_GET_SYSCALL_INFO, pid, sizeof(struct ptrace_syscall_info), &pt_sysinfo);
            if(pt_sysinfo.op == PTRACE_SYSCALL_INFO_EXIT){
                if(pt_sysinfo.exit.is_error != 0){
                    fprintf(stderr,"%s -1 %s (%s)\n",
                    syscall_args,
                    strerrorname_np((int) -pt_sysinfo.exit.rval),
                    strerror((int) -pt_sysinfo.exit.rval)
                    );
                    fflush(stderr);
                }else
                {
                    printf("%s %d\n", 
                        syscall_args,
                        (int) pt_sysinfo.exit.rval);
                }
            }
        }
    }

    return 0;
}