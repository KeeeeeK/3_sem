#include <signal.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

char* data = NULL;
int temp = 0;
int total_size = 0;
int cpid, ppid;

int buff_size = 8;

char mask() 
{
   return 1 << (temp % buff_size);
}


void expand(int d) 
{
   static int data_size = 0;
   if(data_size == 0) 
   {
       data = (char*)malloc(sizeof(char));
       data_size = 1;
   }
   while(d < data_size)
       data = realloc(data, data_size *= 2);
}


void set_zero(int n) 
{
    expand(temp / buff_size + 1);
    data[temp / buff_size] &= ~mask();
    temp++;
    kill(ppid, SIGUSR1);
}


void set_one(int n) 
{
    expand(temp / buff_size + 1);
    data[temp / buff_size] |= ~mask();
    temp++;
    kill(ppid, SIGUSR1);
}


void finish(int n) 
{
    int fd = open("output.c", O_CREAT | O_WRONLY, 0600);
    write(fd, data, temp / buff_size);
    exit(0);
}


void send_bit(int n) {
    if(temp / buff_size == total_size)
        return;
    if(data[temp / buff_size] & mask()) 
    {
        temp++;
        kill(cpid, SIGUSR2);
    }
    else 
    {
       temp++;
       kill(cpid, SIGUSR1);
    }
}
int main() 
{
    signal(SIGUSR1, set_zero);
    signal(SIGUSR2, set_one);
    signal(SIGINT, finish);

    ppid = getpid();
    cpid = fork();
    if (cpid == -1)
    {
        perror("fork");
        exit(1);
    }

    if(cpid) {
        signal(SIGUSR1, send_bit);
        signal(SIGINT, SIG_DFL);
        int fd = open("input.c", O_RDONLY);

        expand(buff_size);
        for(int d; d = read(fd, &data[total_size], buff_size); total_size += d)
            expand(total_size + buff_size);
        write(STDOUT_FILENO, data, total_size);
        send_bit(0);
        while(temp / buff_size != total_size)
            sleep(1);
        kill(cpid, SIGINT);
    }
    else
       while(1) sleep(1);
}