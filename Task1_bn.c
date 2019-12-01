#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#define N 100 //max len of user string


int main()
{
  char ex[] = "exit";
  printf("if u wanna exit the program --- print \"%s\"\n", ex);
  char del[] = " ,\n";

  while (0==0)
  {
    // main loop
    char s[N];
    char** set_s;
    int status;
    scanf("%[^\n]%*c", s);//this combination of symbols is kind of magic, but it deletes \n from input
    if (strcmp(s, ex) == 0) break;
    const pid_t pid = fork();
    switch(pid) 
    {
      case -1:
        perror ("fork"); /* an error occurred */
        exit(1); /*exit parent process*/
        break;
      case 0:
        //child
        set_s = (char**)malloc(N*sizeof(char*));
        for (int k = 0; k<N; k++) set_s[k] = NULL;
        int k = 0;

        //strtok is very unpythonic func, cause it reminds her previous call
        for (char* p=strtok(s, del); p!=NULL; p=strtok(NULL, del))
        {
          set_s[k] = p;
          k++;
        }
        printf("pid: %d\n", getpid());
        int res = execvp(set_s[0], set_s);
        if (res==-1) 
        {
          perror("execvp");
          exit(42);
        }

        free(set_s);
        break;
      default:
        //parent
        status = 0;
        waitpid(pid, &status, 0);
        printf("ret code: %d\n", WEXITSTATUS(status));
        break;
    }
  }
  return 0;
}
