#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int main(int argc, char *argv[]){
    if (argc != 1){
        printf("Please don't input any parameter\n");
        exit(5);
    }

    int pipe_all[105][2], this_proc_one = 0;
    int child_pid;
    int read_num, write_num;

    pipe(pipe_all[this_proc_one ++]);

    if ((child_pid  = fork()) < 0){
        printf("Err when forking ?? Beginning\n");
        exit(1);
    }

    if (child_pid > 0){
        // first proc
        close(pipe_all[this_proc_one - 1][0]);
        for(int i=2;i<=35;i++) // can support up to 100 !
            write(pipe_all[this_proc_one - 1][1], &i, 4);
        close(pipe_all[this_proc_one - 1][1]);
        wait(0);
    }
    else{
        // second proc
        int recv_num[105], recv_cnt = 0;
        while(1){
            // continue to receive until the first received number is 35
            recv_cnt = 0;
            close(pipe_all[this_proc_one - 1][1]);
            while(read(pipe_all[this_proc_one - 1][0], &read_num, 4) > 0)
                recv_num[recv_cnt ++] = read_num;
            close(pipe_all[this_proc_one - 1][0]);
            
            printf("prime %d\n", recv_num[0]);

            if (recv_cnt == 1)
                break; // final proc
            
            // biuld something for the next proc
            pipe(pipe_all[this_proc_one ++]);

            if ((child_pid  = fork()) < 0){
                printf("Err when forking ?? at Child %d\n", this_proc_one - 1);
                exit(1);
            }

            if (child_pid > 0){
                close(pipe_all[this_proc_one - 1][0]);
                // i from 1, this is so important !
                for(int i=1;i<recv_cnt;i++) if(recv_num[i] % recv_num[0]){
                    write_num = recv_num[i];
                    write(pipe_all[this_proc_one - 1][1], &write_num, 4);
                }
                close(pipe_all[this_proc_one - 1][1]);
                wait(0);
                break;
            }
        }
    }

    exit(0);
}