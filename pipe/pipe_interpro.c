#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAXSIZE 1024

void reader(FILE* stream){
  char buffer[MAXSIZE];
  while(!feof(stream) && !ferror(stream) && fgets(buffer, sizeof(buffer), stream) != NULL){
    //fputs(buffer, stdout);
    //printf("son: %s", buffer);
  }
}

void writer(const char* message, int count, FILE* stream){
  for(; count > 0; -- count){
    fprintf(stream, "%s\n", message);
    printf("%s\n", message);
    fflush(stream);
    sleep(1);
  }
}

void transfer(FILE* stream){
  FILE* f = fopen("bigdata.in", "r");
  char buffer[MAXSIZE];
  while(!feof(f) && !ferror(f) && fgets(buffer, sizeof(buffer), f) != NULL){
    fprintf(stream, "%s", buffer);
    fflush(stream);
  }
  fclose(f);
}

int main(){
  int fds[2];
  pid_t pid;

  pipe(fds);

  pid = fork();
  if(pid == (pid_t)0){
    FILE* stream;
    close(fds[1]);
    stream = fdopen(fds[0], "r");
    reader(stream);
    printf("son: end\n");
    close(fds[0]);
  }else{
    FILE* stream;
    close(fds[0]);
    stream = fdopen(fds[1], "w");
    transfer(stream);
    //writer("father: Hello, world.", 5, stream);
    printf("father: end\n");
    close(fds[1]);
  }
  return 0;
}
