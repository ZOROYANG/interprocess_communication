#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>

#define BUFFER_SIZE 1024

typedef struct{
  pid_t pid;
  char buf[BUFFER_SIZE];
} SHM;

void handler(int signo){
  printf("writer_ recieve signal: %d\n", signo);
}

int main(){
  key_t key;
  int shmid;
  SHM *shmaddr;
  pid_t peer_id;

  signal(SIGUSR1, handler);

  if((key = ftok(".",'a')) == -1){
    perror("ftok");
    exit(-1);
  }

  if((shmid = shmget(key, sizeof(SHM), 0666 | IPC_CREAT | IPC_EXCL)) == -1){
    if(errno == EEXIST){
      shmid = shmget(key, sizeof(SHM), 0666);
      if((shmaddr = (SHM *)shmat(shmid, NULL, 0)) == (SHM*)-1){
        perror("shmat");
        exit(-1);
      }
      peer_id = shmaddr->pid;
      shmaddr->pid = getpid();
      kill(peer_id, SIGUSR1);
    }else{
      perror("shmget");
      exit(-1);
    }
  }else{
    if((shmaddr = (SHM *)shmat(shmid, NULL, 0)) == (SHM*)-1){
      perror("shmat");
      exit(-1);
    }
    shmaddr->pid = getpid();
    pause();
    peer_id = shmaddr->pid;
  }

  while(1){
    fgets(shmaddr->buf, BUFFER_SIZE, stdin);
    kill(peer_id, SIGUSR1);

    if(strncmp(shmaddr->buf, "quit", 4) == 0){
      break;
    }
    pause();
  }

  if(shmdt(shmaddr) == -1){
    perror("shmdt");
    exit(-1);
  }

  if(shmctl(shmid, IPC_RMID, NULL) == -1){
    perror("release");
    exit(-1);
  }
  exit(0);
}
