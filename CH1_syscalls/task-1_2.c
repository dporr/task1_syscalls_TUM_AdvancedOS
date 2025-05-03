/* Export functions here */
/*
Task 1.2

Reuse your wrappers from task 1.1 and replace the syscall function with assembly that performs the system call. You must manage error reporting in the same way the glibc would.

The resulting binary should be a shared library named librw_2.so and should be usable by any application to replace the original glibc implementations of these two wrappers without changing the behaviour of applications.

Useful notes:
https://www.man7.org/linux/man-pages/man2/syscall.2.html

*/
#include <unistd.h>
#include <stdio.h>
#include <syscall.h>
#include <errno.h>
ssize_t write(int fd, const void *buf, size_t nbyte);
ssize_t write(int fd, const void *buf, size_t nbyte){
    ssize_t status_code = -1;
    if(fd <= 0){
        return status_code;
    }
    if(buf == NULL ){
        return status_code;
    }
    if(nbyte <= 0){
        return status_code;
    }
    // return syscall(SYS_write, fd,  buf, nbyte);
    /*Argument order
    x86-64: arg1  arg2 arg3   arg4 arg5 arg6   arg7
            rdi   rsi   rdx   r10   r8    r9    -
    return values:  Syscall #   ret1 ret2   
                    rax         rax  rdx  
    arm64: arg1  arg2   arg3  arg4 arg5 arg6   arg7
            x0    x1    x2    x3    x4    x5    - 
    return values:  Syscall #   ret1 ret2  
                    w8         x0   x1
    */
   
   asm volatile ( "syscall"
                : "=a" (status_code)
                : "a" (SYS_write),
                 "D" (fd),
                 "S" (buf),
                 "d"(nbyte)
                : "rcx", "r11", "memory"   // Clobbered registers
                 );
    if(status_code <= -1){
        errno = status_code * -1;
        //printf("Returning %d", errno);
    }
    return status_code;
}

ssize_t read(int fd, void *buf, size_t count);
ssize_t read(int fd, void *buf, size_t count)
{     
    ssize_t status_code = -1;
    if(fd <= 0){
        return -1;
    }
    if(buf == NULL ){
        return -1;
    }
    if(count <= 0){ 
        return -1;
    }
    asm volatile ( "syscall"
                : "=a" (status_code)
                : "a" (SYS_read),
                 "D" (fd),
                 "S" (buf),
                 "d"(count)
                 );
    if(status_code <= -1){
        errno = status_code * -1;
        //printf("Returning %d", errno);
    }
    return status_code;
}