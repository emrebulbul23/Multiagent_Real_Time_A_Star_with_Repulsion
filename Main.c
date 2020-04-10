#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct{
  unsigned int id;
  unsigned int x;
  unsigned int y;
} Agent;

typedef struct{
  unsigned int x;
  unsigned int y;
} Obstacle;

typedef struct{
  unsigned int x;
  unsigned int y;
} Tuple;

/*
* Returns the manhattan distance, to be used as the heuristic function.
*/
int getManhattanDistance(Tuple from, Tuple to);

/*
* Checks for an obstacle in a given position.
*/
int isBlocked(Tuple t, Obstacle* obstacles, unsigned int number_of_obstacles);

void doStuff(){

}

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

  // get the first line
  getline(&line, &len, filePtr);
  unsigned int n = atoi(strtok(line," "));
  unsigned int number_of_agents = atoi(strtok(NULL," "));
  unsigned int number_of_obstacles = atoi(strtok(NULL," "));
  unsigned int number_of_cells = (n+1)*(n+1);

  // init agents
  Agent* agents = malloc(sizeof(Agent)*number_of_agents+1);
  for(int i = 1; i < number_of_agents+1; i++){
    agents[i].id = i;
    agents[i].x = 1;
    agents[i].y = 1;
  }


  // TODO make h maps here

  // TODO make visited maps here


  // the obstacles
  int o = 0;
  Obstacle* obstacles = malloc(sizeof(Obstacle)*number_of_obstacles);
  while ((read = getline(&line, &len, filePtr)) != -1) {
    obstacles[o].x = atoi(strtok(line," "));
    obstacles[o].y = atoi(strtok(NULL," "));
    o++;
  }

  return 0;
}

int getManhattanDistance(Tuple from, Tuple to){
  int diff_x = from.x > to.x ? from.x-to.x : to.x-from.x;
  int diff_y = from.y > to.y ? from.y-to.y : to.y-from.y;
  return diff_x+diff_y;
}

int isBlocked(Tuple t, Obstacle* obstacles, unsigned int number_of_obstacles){
  for(int i=0; i<number_of_obstacles; i++){
    if((t.x == obstacles[i].x)&&(t.y == obstacles[i].y))
      return 1;
  }
  return 0;
}
