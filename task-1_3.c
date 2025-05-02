#include <stdio.h> //printf
#include <linux/ptrace.h> //ptrace(2)
#include <unistd.h> //fork(2)
#include <sched.h> //pid_t

int forked(char** argv){
    int err = execve(argv[0],  argv, NULL);
    printf("%d", err);
    return err;
}


int main(int argc, char** argv)
{
    //Get program and arguments
    //execve program and capture TID
    pid_t pid = fork();
    if(pid == 0) forked(&argv[1]);
    // //ptrace_attach
    // ptrace(PTRACE_SEIZE, pid, 0, 0);
    // ptrace(PTRACE_INTERRUPT, pid, 0, 0);
    // ptrace(PTRACE_SYSCALL, pid,0, 0);
    // struct ptrace_syscall_info pt_sysinfo;
    // // long ptrace(enum __ptrace_request op, pid_t pid, void *addr, void *data);
    // ptrace(PTRACE_GET_SYSCALL_INFO, pid, sizeof(struct ptrace_syscall_info), &pt_sysinfo);
    // // PTRACE_SYSCALL get syscall retunr
    // //Exit on SYS

    return 0;
}