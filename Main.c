#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(void){
  srand(time(NULL));
  FILE* filePtr;
  char* line = NULL;
  size_t len = 0;
  ssize_t read;
  if((filePtr = fopen("input.txt","r")) == NULL){
    printf("Error opening input file!");
    exit(1);
  }

  while ((read = getline(&line, &len, filePtr)) != -1) {
    printf("%s",line);
  }

  return 0;
}
