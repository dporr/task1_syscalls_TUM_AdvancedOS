#include <stdio.h> //printf
#include <sys/ptrace.h> //ptrace(2)
#include <sys/wait.h> //waitpid
#include <linux/ptrace.h> //struct ptrace_syscall_info
#include <unistd.h> //fork(2)
#include <sched.h> //pid_t
#include <signal.h> //raise,
#include <stdlib.h> //exit(2)
int forked(char** argv){
    // printf("Inside child %d my parent is %d\n", getpid(), getppid());
    ptrace(PTRACE_TRACEME,0,0,0);
    raise(SIGSTOP);
    int err = execve(argv[0],  argv, NULL);
    printf("%d", err);
    return err;
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
        ptrace(PTRACE_GET_SYSCALL_INFO, pid, sizeof(struct ptrace_syscall_info), &pt_sysinfo);
        if(pt_sysinfo.op == PTRACE_SYSCALL_INFO_ENTRY) 
        {
            printf("Entered syscall n: %lld\n", pt_sysinfo.entry.nr);
        }
        else if (pt_sysinfo.op == PTRACE_SYSCALL_INFO_EXIT)
        {
            printf("Exited syscall return value: %lld\n", pt_sysinfo.exit.rval);
        }else{
            printf("Not stopped at syscall op is: %d\n", pt_sysinfo.op);
        }

        
       
    }

    return 0;
}