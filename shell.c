/**
  * @author Colin Hansen
  * @date September 15, 2015
  * @version 1.0
  */

#include <stdlib.h>
#include "headFiles.h"
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

/**
 * Runs a prototype bash shell with the built in change directory and exit functions.
 *
 * Absolute paths are required from the user when providing bash shell commands.
 */

int main(int argc, char *argv[]){
  char* input;
  char** parseIn;
  int i;
  int infile;
  int outfile;
  char** args;
  int appendOrNah;
  pid_t pid;

  prdir(); 
  input= readc();

  while(strcmp(input,"exit")){
    parseIn = parse(input);
    if(strcmp(parseIn[0],"cd")==0){
      cd(parseIn);
    }
    
    else if (strcmp(parseIn[0],"exit")!=0){
      infile = inFind(parseIn);
      outfile = outFind(parseIn);
      args = argSep(parseIn);
      appendOrNah= appendCheck(parseIn);
      if(args[0]!=NULL){
        pid = fork();
        if(pid ==0){
          // Handles input redirection
          if(infile >-1){
            close(0);
            if (open(parseIn[infile], O_RDONLY)==-1){
              printc(strerror(errno));
              exit(1);
            }
          }

          // Handles output redirection
          if(outfile > -1){
            close(1);
            if(appendOrNah >0){
              // Open file in append mode
              if(open(parseIn[outfile], O_WRONLY|O_APPEND|O_CREAT, S_IRWXU)==-1){
                printc(strerror(errno));
                exit(1);
              }
            }
            else{
              // Open file in truncate mode
              if(open(parseIn[outfile], O_WRONLY|O_TRUNC|O_CREAT, S_IRWXU)==-1){
                exit(1);
              }
            }
          }
          if((infile>-2)&&(outfile>-2)){
            execv(args[0], args);
            printc(strerror(errno));
            exit(1);
          }
          else{
            printc("Invalid command.");
            exit(1);
          }
        }
        else if(pid==-1){
          printc(strerror(errno));
          exit(1);         
        }
        else{
          int status;
          wait(&status);
        }
      }
      free(args);
    }

    free(parseIn);
    free(input);
    prdir();
    input= readc();
  }
  return 0;
};
