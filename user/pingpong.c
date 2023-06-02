#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]){
    if (argc != 1){
        printf("Please don't input any parameter\n");
        exit(5);
    }

    int pipe_num[2], child_pid, buff[255], wrbuff[255];
    pipe(pipe_num);

    if ((child_pid = fork()) < 0){
        printf("Error in Fork\n");
        exit(1);
    }
    
    int now_pid = getpid();

    if (child_pid == 0){
        // child process
        read(pipe_num[0], buff, sizeof(buff));
        printf("%d: received ping\n", now_pid);
        wrbuff[0] = 1;
        write(pipe_num[1], wrbuff, 1);
    }
    else{
        // father process
        wrbuff[0] = 0;
        write(pipe_num[1], wrbuff, 1);
        read(pipe_num[0], buff, sizeof(buff));
        printf("%d: received pong\n", now_pid);
    }

    exit(0);
}