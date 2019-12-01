#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#define N 100

typedef struct
{
  int ctp[2];//child to parent
  int ptc[2];//parent to child
} dpipe_t;

void init_dpipe(dpipe_t dp)
{
  if ((pipe(dp.ctp)<0) || (pipe(dp.ptc)<0))
  {
    perror("pipe");
    exit(1);
  } 
}

void secure_child(dpipe_t dp)
{
  close(dp.ctp[0]);
  close(dp.ptc[1]);
}

void secure_parent(dpipe_t dp)
{
  close(dp.ctp[1]);
  close(dp.ptc[0]);
}

int main()
{
  dpipe_t dp;
  init_dpipe(dp);
  // is that all??)) we've created needed struct... let's test

  char got[N];
  char mess_ptc[] = "Hi, i'm parent";
  char mess_ctp[] = "Hi, i'm child";
  const pid_t pid = fork();
  switch(pid) 
  {
    case -1:
      perror ("fork"); /* an error occurred */
      exit(1); /*exit parent process*/
      break;
    case 0:
      //child
      printf("C: alive\n");
      char cbuf[N];
      int csize;
      strcpy(cbuf, mess_ctp);
      secure_child(dp);

      write(dp.ctp[1], cbuf, N-1);
      printf("C: sent: %s\n", cbuf);
      while ((csize = read(dp.ptc[0], got, N-1))>0)
      {
        got[csize]='\0';
        printf("C: got: %s\n", got);
      }
      printf("%d\n", errno);
      
      printf("C: died\n");
      exit(0);
      break;
    default:
      //parent
      printf("P: alive\n");
      char pbuf[N];
      int psize;
      int stat;
      strcpy(pbuf, mess_ptc);
      secure_parent(dp);

      write(dp.ptc[1], pbuf, N-1);
      printf("P: sent: %s\n", pbuf);
      while ((psize = read(dp.ctp[0], got, N-1))>0)
      {
        got[psize]='\0';
        printf("P: got: %s\n", got);
      }

      waitpid(pid, &stat, 0);
      printf("P: died\n");
      exit(0);
      break;
  }

  return 0;
}