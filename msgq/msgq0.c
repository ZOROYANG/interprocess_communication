#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <string.h>
#include <signal.h>
struct msgbuf{
      long mtype;
      char mtext[100];
};
int main(void)
{
      key_t key;
      pid_t pid;
      int msgid;
      struct msgbuf msg;
      key=ftok(".", 0x01);
      if ( (msgid = msgget(key,  IPC_CREAT|0666)) <0 )
      {
            perror("msgget error");
            exit(1);
      }
      //创建一个进程
      if ( (pid = fork()) < 0 )
      {
            perror("fork error");
            exit(1);
      }
      //子进程收信息
      else if (pid==0)
      {
            msg.mtype = 1;
            while(1)
            {
                  memset(msg.mtext, 0, 100);
                  msgrcv(msgid, &msg, 100, 1, 0);  //receive the msg from 1
                  printf("receive:%s\n:", msg.mtext);
                  fflush(stdout);
            }
            exit(0);
      }
      //父进程发信息
      else
      {
            while(1)
            {
                  memset(msg.mtext, 0, 100);
                  printf("father:");
                  fgets(msg.mtext, 100, stdin);
                  if (strncmp("bye", msg.mtext, 3)==0)//如果前3个字符为bye,则退出
                  {
                        kill(pid, SIGSTOP);
                        exit(1);
                  }
                  msg.mtype=1;//send to 1
                  msg.mtext[strlen(msg.mtext)-1]='\0';
                  msgsnd(msgid, &msg, strlen(msg.mtext)+1, 0);
            }
      }
      return 0;
}
