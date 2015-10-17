 /**
  * @author Colin Hansen
  * @date September 15, 2015
  * @version 1.0
  */
#include <unistd.h>
#include <string.h>
#include <errno.h>

/**
 * Prints the provivded string to standard out
 *
 * @param a The string to print
 */
void printc(char* a){
  char* newl="\n";
  write(1, a, strlen(a));
  write(1, newl, strlen(newl));
};

/**
 * Reads a string from standard in until a new line character.  Then replaces
 * new line with a NULL byte.
 *
 * @return A NULL terminated string containing input from the user.
 */
char* readc(){
  int iread;
  char* buffer= (char *)malloc(1024);
  iread= read(0, buffer, 1024);
  if (iread == -1)
    printc(strerror(errno));
  else{
    int i;
    for(i=0; i<strlen(buffer); i++){
      if(buffer[i]=='\n'){
        buffer[i]='\0';
        break;
      }
    }
  }
  return buffer;
};


/**
 * Changes current working directory
 *
 * @param input A string array containing the command 'cd' and the name of the
 * directory that the user wishes to change to
 *
 * @return Either it returns the name of the new current working directory or
 * an empty string if there was an error
 */
char* cd(char** input){
  char* cdarg;
  cdarg = input[1];
  if(chdir(cdarg)==0){
    return(cdarg);
  }
  else{
    printc(strerror(errno));
    char* none= "";
    return(none);
  }
};


/**
 *Prints the current working directory to the screen.
 */
void prdir(){
  char cwd[1024];
  if(getcwd(cwd, sizeof(cwd)) !=NULL){
    write(1, cwd, strlen(cwd));
    write(1, "$ ", 2);
  }
  else{
    perror("getcwd() error");
  }
}



/**
 * Parses the input string (using space delimiters) storing the values
 * into an array
 *
 * @param input The string containing the user's input
 *
 * @return A NULL terminated array of strings containing the parsed input
 */
char** parse(char *input){
  char* inchar = "<";
  char* outchar = ">";
  char** pInput= (char **)malloc(1024);
  int skipCheck = 0;
  int i = 0;
  int j = 0;
  while (input[i] != '\0'){
    if (input [i]=='<'){
      if(i==0){
        pInput[j]=inchar;
        j++;
      }
      else{
        pInput[j+1] = inchar;
        skipCheck = 1;
      }
      input [i] = ' ';
    }

    if (input[i] == '>'){
      if(i==0){
        pInput[j] = outchar;
        j++;
      }
      else{
        pInput[j+1] = outchar;
        skipCheck = 1;
      }
      input[i] = ' ';
    }
    if (input[i]==' '){
      input[i]='\0';
      if(strncmp(input,"\0", strlen(input)) != 0 ){
        pInput[j] = input;
        if(skipCheck == 0){
          j++;
        }
        else{
          j = j+2;
          skipCheck = 0;
        }
      }
      input = input + i + 1;
      i=0;
    }
    else{
      i++;
    }
  }
  if(strncmp(input," ", strlen(input)) !=0){
    pInput[j] = input;
    j++;
  }
  pInput[j]= NULL;
  return pInput;
};


/**
 * Scans through an array of strings of parsed input and checks for correct
 * input redirection
 *
 * @param input A string array of parsed user input
 *
 * @return An integer of 0 (no input redirection), 1 (correct input redirection)
 * or -1 (incorrect input redirection) is returned.
 */
int inFind(char** input){
  int inCount = 0;
  int i=0;
  int infile = -1;
  while(input[i]!=NULL){
    if(input[i]=="<"){
      infile = i+1;
      inCount++;
    } 
    i++;
  }
  if (inCount > 1){
    infile = -2;
  }
  return infile;
};

/**
 * Scans through a string array of parsed input and checks for correct 
 * output redirection
 *
 * @param input A string array of parsed user input
 *
 * @return An integer of -1 (no output redirection), 
 * 1 or 2 (correct output redirection), or -2 (incorrect input redirection) is returned.
 */
int outFind(char** input){
  int outCount = 0;
  int i= 0;
  int outfile = -1;
  int outCheck = 0;
  while (input[i] != NULL){
    if (input[i]==">"){ 
      if (outCheck == 0){
        outfile = i+1;
        if((outCount == 0)&&(input[i+1]!=">")){
          outCheck=-1;
        }
        outCount++;
      }
      else{
        outfile = -2;
        return outfile;
      }
    }
    i++;
  }
  if(outCount>2){
    outfile = -2;
  }
  if((outCount==2) && (outCheck == -1)){
    outfile = -2;
  }
  return outfile;

};

/**
 * Creates an array of strings from user input composed of the shell command and its arguments
 *
 * @param input A string array of the parsed user input
 *
 * @return A string array containing the shell command and its arguments
 */
char** argSep(char** input){
  int i=0;
  char** args=(char **)malloc(1024);

  while((input[i]!=NULL)&&(input[i]!=">")&&(input[i]!="<")){
    args[i] = input[i];
    i++;
  }
  args[i]=NULL;
  return args;
};

/**
 * Checks if the output redirection is meant for appending or truncating
 *
 * @param input A string array of parsed user input
 *
 * @return An integer is returned: 0 to signal truncate, 1 to signal append, and -1 to signal neither
 */
int appendCheck(char** input){
  int i = 0;
  int outCount = 0;

  while(input[i]!=NULL){
    if(input[i]==">"){
      outCount++;
    }
    i++;
  }
  if(outCount == 1){
    return 0;
  }
  if(outCount == 2){
    return 1;
  }
  else{
    return -1;
  }

};
