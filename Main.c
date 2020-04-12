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
unsigned int BIG_PENALTY;

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
  int* is_visited;
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
int getAdjacencyOfAgent(Tuple new_position, int agent_id, Agent* agents);

/*
* Get the h value depending on the history of cells visited to calculate f value.
*/
int getHValue(Agent* agent, int* h_global, Tuple new_position_tuple);

/*
* Plans the next cell to move based on repulsion and h values.
*/
char chooseNextCell(Agent* agent,Agent* agents ,Obstacle* obstacles,  int* h_global);

/*
* Update the given agents location based on the next position indicator char.
* U: up, R: right and so on.
*/
void updateAgentLocation(Agent* agent, char nex_position);

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
  BIG_PENALTY = NUMBER_OF_CELLS;

  // init agents
  Agent* agents = malloc(sizeof(Agent)*NUMBER_OF_AGENTS+1);
  for(int i = 1; i < NUMBER_OF_AGENTS+1; i++){
    agents[i].id = i;
    agents[i].x = 1;
    agents[i].y = 1;
    agents[i].is_visited = malloc(sizeof(int)*NUMBER_OF_CELLS);
    memset(&agents[i].is_visited[0], -1, NUMBER_OF_CELLS*sizeof(int));
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

  // buffer for the next steps
  char* next_agent_steps = malloc(sizeof(char)*NUMBER_OF_AGENTS+1);

  // begin recording
  clock_t start_time = clock();

  // search
  int time_step = 0;
  while(1){
    time_step++;
    printf("Step:%d\n",time_step);
    for(int i=1; i < NUMBER_OF_AGENTS+1; i++){
      next_agent_steps[i] = chooseNextCell(agents+i,agents,obstacles,h_global);
      updateAgentLocation(agents+i,next_agent_steps[i]);
      printf("Agent%d: %c      (%d,%d)\n",i,next_agent_steps[i],agents[i].x,agents[i].y);
    }
    int end = -1;
    for(int i=1; i < NUMBER_OF_AGENTS+1; i++){
      if(agents[i].x == N && agents[i].y == N){
        end = i;
        break;
      }
    }
    if(end > 0){
      clock_t end_time = clock();
      double time_spent = (double)(end_time - start_time) / CLOCKS_PER_SEC;
      printf("Agent%d reached the goal in %f seconds.\n",end,time_spent);
      break;
    }
    printf("\n");
  }

  return 0;
}

char chooseNextCell(Agent* agent,Agent* agents ,Obstacle* obstacles,  int* h_global){
  //           up, right, down, left
  int f_values[4] = {-1,-1,-1,-1};

  // Look-ahead search
  // up
  Tuple t[4];
  t[0] = (Tuple){agent->x,agent->y+1};
  if(isBlocked(t[0],obstacles) == 0){
    f_values[0] = 0;
    int h_temp = getHValue(agent, h_global, t[0]);
    f_values[0] = h_temp==BIG_PENALTY ? h_temp : h_temp +1;
  }
  // right
  t[1]  = (Tuple){agent->x+1,agent->y};
  if(isBlocked(t[1],obstacles) == 0){
    f_values[1] = 0;
    int h_temp = getHValue(agent, h_global, t[1]);
    f_values[1] = h_temp==BIG_PENALTY ? h_temp : h_temp +1;
  }
  // down;
  t[2] = (Tuple){agent->x,agent->y-1};
  if(isBlocked(t[2],obstacles) == 0){
    f_values[2] = 0;
    int h_temp = getHValue(agent, h_global, t[2]);
    f_values[2] = h_temp==BIG_PENALTY ? h_temp : h_temp +1;
  }
  // left
  t[3] = (Tuple){agent->x-1,agent->y};
  if(isBlocked(t[3],obstacles) == 0){
    f_values[3] = 0;
    int h_temp = getHValue(agent, h_global, t[3]);
    f_values[3] = h_temp==BIG_PENALTY ? h_temp : h_temp +1;
  }

  // Choice
  // find smallest f value
  int temp_min = -1;
  for(int j = 0; j<4; j++){
    if(temp_min == -1){
      temp_min = f_values[j];
    }else if(f_values[j] > 0 && f_values[j] < temp_min) {
      temp_min = f_values[j];
    }
  }

  // check if more than one min value
  int min_count = 0;
  int arg_min = 0;
  for (int i = 0; i < 4; i++) {
    if(f_values[i]==temp_min){
      min_count++;
      arg_min = i;
    }
  }

  // step 5 choice of the algorithm
  if(min_count > 1){
    //check if all candidates are within the repulsion range
    int repulsive_range = calculateRepulsiveRange(agent);
    int in_repulsion_flag = 1;
    int max_adj = -1;
    int max_adj_arg = 0;
    int rand_number = rand() % 4;
    for(int i = 0; i<4; i++){
      int new_index = (i + rand_number)%4;
      int adj = getAdjacencyOfAgent(t[new_index],agent->id,agents);
      if(adj>=repulsive_range){
        in_repulsion_flag = -1;
        break;
      }else{
        if(max_adj < adj && temp_min == f_values[new_index]){
          max_adj = adj;
          max_adj_arg = new_index;
        }
      }
    }

    if(in_repulsion_flag>0){
      // overrite arg_min to breake the tie
      arg_min = max_adj_arg;
    }else{
      // choose any that outside of repulsion range
      int rand_number = rand() % 4;
      for (int i = 0; i < 4; i++) {
        int new_index = (i + rand_number)%4;
        int adj = getAdjacencyOfAgent(t[new_index],agent->id,agents);
        if(adj >= repulsive_range && temp_min == f_values[new_index]){
          arg_min = new_index;
          break;
        }
      }
    }
  }

  //estimation update
  int agent_pos = agent->x + agent->y*(N+1);
  int admissible_range = getManhattanDistance((Tuple){agent->x,agent->y},(Tuple){N,N});
  if (temp_min > admissible_range){
    *(h_global+agent_pos) = admissible_range;
  }else{
    *(h_global+agent_pos) = temp_min;
  }

  // find second best estimation
  int second_best = -1;
  for(int j = 0; j<4; j++){
    if(second_best == -1 && f_values[j] != temp_min){
      second_best = f_values[j];
    }else if(f_values[j] > 0 && f_values[j] < second_best && f_values[j] != temp_min) {
      second_best = f_values[j];
    }
  }

  agent->h_local[agent_pos] = second_best == -1 ? BIG_PENALTY : second_best;

  if(arg_min==0){
    return 'U';
  }else if(arg_min==1){
    return 'R';
  }else if(arg_min==2){
    return 'D';
  }else{
    return 'L';
  }
}

void updateAgentLocation(Agent* agent, char nex_position){
  int loc = (agent->x + (agent->y*(N+1)));
  agent->is_visited[loc] = 1;
  if(nex_position=='U'){
    agent->x = agent->x;
    agent->y = agent->y+1;
  }else if(nex_position=='R'){
    agent->x = agent->x+1;
    agent->y = agent->y;
  }else if(nex_position=='D'){
    agent->x = agent->x;
    agent->y = agent->y-1;
  }else{
    agent->x = agent->x-1;
    agent->y = agent->y;
  }
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

int getAdjacencyOfAgent(Tuple new_position, int agent_id, Agent* agents){
  if(NUMBER_OF_AGENTS>1){
    int distance = getManhattanDistance((Tuple){1,1},(Tuple){N,N});
    for(int i = 1; i < NUMBER_OF_AGENTS+1; i++){
      if(i!=agent_id){
        Tuple tuple_2 = (Tuple){(agents+i)->x,(agents+i)->y};
        int temp = getManhattanDistance(new_position,tuple_2);
        if(temp<distance){
          distance = temp;
        }
      }
    }
    return distance;
  }
  return 0;
}

int getHValue(Agent* agent, int* h_global, Tuple new_position_tuple){
  int h_value = -1;
  int new_position = new_position_tuple.y*(N+1)+new_position_tuple.x;
  if((agent->is_visited[new_position])>0){
    h_value = agent->h_local[new_position];
  }else if(h_global[new_position] > 0){
    h_value = h_global[new_position];
  }
  if(h_value==-1){
    h_value = getManhattanDistance(new_position_tuple,(Tuple){N,N});
  }
  return h_value;
}
