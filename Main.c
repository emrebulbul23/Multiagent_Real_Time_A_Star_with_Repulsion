#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// global variables
static unsigned int ALFA = 10;

unsigned int N;
unsigned int NUMBER_OF_AGENTS;
unsigned int NUMBER_OF_OBSTACLES;
unsigned int NUMBER_OF_CELLS;

/*
* Agent struct.
* -1 represents absent entry in h map.
* 'n' represents not visited cell,
* 'y' represents visited cell.
*/
typedef struct{
  unsigned int id;
  unsigned int x;
  unsigned int y;
  unsigned char* is_visited;
  int* h_local;
} Agent;

/*
* Obstacle struct.
*/
typedef struct{
  unsigned int x;
  unsigned int y;
} Obstacle;

/*
* Position tuple x,y.
*/
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
int isBlocked(Tuple t, Obstacle* obstacles);

/*
* Calculate repulsive range for agent.
*/
float calculateRepulsiveRange(Agent* agent);

/*
* Get adjaceny for the agent with id agent_id.
*/
int getAdjacencyOfAgent(int agent_id, Agent* agents);

/*
* Get the h value depending on the history of cells visited to calculate f value.
*/
int getHValue(Agent* agent, int* h_global, Tuple new_position_tuple);

void chooseNextCell(Agent* agent, Obstacle* obstacles,  int* h_global){
  //           up, right, down, left
  int f_values[4] = {-1,-1,-1,-1};

  // up
  Tuple t = (Tuple){agent->x,agent->y+1};
  if(isBlocked(t,obstacles) == 0){
    f_values[0] = 0;
    f_values[0] = 1 + getHValue(agent, h_global, t);
  }
  // right
  t = (Tuple){agent->x+1,agent->y};
  if(isBlocked(t,obstacles) == 0){
    f_values[1] = 0;
    f_values[1] = 1 + getHValue(agent, h_global, t);
  }
  // down;
  t = (Tuple){agent->x,agent->y-1};
  if(isBlocked(t,obstacles) == 0){
    f_values[2] = 0;
    f_values[2] = 1 + getHValue(agent, h_global, t);
  }
  // left
  t = (Tuple){agent->x-1,agent->y};
  if(isBlocked(t,obstacles) == 0){
    f_values[3] = 0;
    f_values[3] = 1 + getHValue(agent, h_global, t);
  }

  printf("agent %d\n",agent->id);
  for(int i = 0; i<4 ; i++){
    printf("%d ",f_values[i]);
  }
  printf("\n");
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
  N = atoi(strtok(line," "));
  NUMBER_OF_AGENTS = atoi(strtok(NULL," "));
  NUMBER_OF_OBSTACLES = atoi(strtok(NULL," "));
  NUMBER_OF_CELLS = (N+1)*(N+1);

  // init agents
  Agent* agents = malloc(sizeof(Agent)*NUMBER_OF_AGENTS+1);
  for(int i = 1; i < NUMBER_OF_AGENTS+1; i++){
    agents[i].id = i;
    agents[i].x = 1;
    agents[i].y = 1;
    agents[i].is_visited = malloc(sizeof(char)*NUMBER_OF_CELLS);
    memset(agents[i].is_visited, 'n', NUMBER_OF_CELLS*sizeof(char));
    agents[i].h_local = malloc(sizeof(int)*NUMBER_OF_CELLS);
    memset(agents[i].h_local, -1, NUMBER_OF_CELLS*sizeof(int));
  }

  // global h list
  int* h_global = malloc(sizeof(int)*NUMBER_OF_CELLS);
  memset(h_global, -1, NUMBER_OF_CELLS*sizeof(int));

  // the obstacles
  int o = 0;
  Obstacle* obstacles = malloc(sizeof(Obstacle)*NUMBER_OF_OBSTACLES);
  while ((read = getline(&line, &len, filePtr)) != -1) {
    obstacles[o].x = atoi(strtok(line," "));
    obstacles[o].y = atoi(strtok(NULL," "));
    o++;
  }

  chooseNextCell(agents+1,obstacles,h_global);
  printf("adjaceny: %d\n",getAdjacencyOfAgent(1,agents));

  return 0;
}

int getManhattanDistance(Tuple from, Tuple to){
  int diff_x = from.x > to.x ? from.x-to.x : to.x-from.x;
  int diff_y = from.y > to.y ? from.y-to.y : to.y-from.y;
  return diff_x+diff_y;
}

int isBlocked(Tuple t, Obstacle* obstacles){
  // not in the grid
  if(!(0<t.x && t.x<=N && 0<t.y && t.y<=N)){
    return 1;
  }
  // no obstacles
  for(int i=0; i<NUMBER_OF_OBSTACLES; i++){
    if((t.x == obstacles[i].x)&&(t.y == obstacles[i].y))
      return 1;
  }
  return 0;
}

float calculateRepulsiveRange(Agent* agent){
  int h_start = getManhattanDistance((Tuple){1,1},(Tuple){N,N});
  int h_agent = getManhattanDistance((Tuple){agent->x,agent->y},(Tuple){N,N});
  return (ALFA*h_agent)/h_start;
}

int getAdjacencyOfAgent(int agent_id, Agent* agents){
  Tuple tuple_1 = (Tuple){(agents+agent_id)->x,(agents+agent_id)->y};
  int distance = 0;
  for(int i = 1; i < NUMBER_OF_AGENTS+1; i++){
    if(i!=agent_id){
      Tuple tuple_2 = (Tuple){(agents+i)->x,(agents+i)->y};
      int temp = getManhattanDistance(tuple_1,tuple_2);
      if(temp<distance){
        distance = temp;
      }
    }
  }
  return distance;
}

int getHValue(Agent* agent, int* h_global, Tuple new_position_tuple){
  int h_value = -1;
  int new_position = new_position_tuple.y*(N+1)+new_position_tuple.x;
  if(agent->is_visited[new_position]=='y'){
    h_value = agent->h_local[new_position];
  }else if(h_global[new_position] > 0){
    h_value = h_global[new_position];
  }
  if(h_value==-1){
    h_value = getManhattanDistance(new_position_tuple,(Tuple){N,N});
  }
  printf("%d\n",h_value );
  return h_value;
}
