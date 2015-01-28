#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <float.h>

struct node
{
  int x;
  int y;
  int edge[20];
  int point;
  int distance;
  int prev;
};

struct query
{
  int start;
  int dest;
  int path[20];
};


double calculate(struct node a, struct node b){
  return sqrt( (a.x-b.x)*(a.x-b.x)+(a.y-b.y)*(a.y-b.y) );
}
  

void findpath(int temp_start,int temp_dest,struct node node_array[],int node_size);
void dijkstra(int temp_start,struct node node_array[],int node_size,int Q_array[], int dest);
int findmin(struct node node_array[], int Q_array[], int node_size);
int main(int argc, char** argv){
  if(argc != 3){
    printf("Input format wrong!!!\n");
    return 0;
  }

  char* map = argv[1];
  char* queries = argv[2];

  FILE *fr = fopen(map, "rt");
  //int flag = 1;
  int nodes = 0;
  int edges;
  int temp_edge;
  int i = 0;
  int temp;
  //int target;
  //int source;
  int ori_temp;
  char line[120];
  
  //Create map
  if(fgets(line, 120, fr) != NULL){
    sscanf(line,"%d %d",&nodes,&edges);
    //printf("%d %d\n",nodes,edges);
  }
  
  struct node *node_array = malloc(sizeof(struct node)*nodes);
  while(i < nodes){
       if(fgets(line, 120, fr) != NULL){
       struct node temp_node;
       sscanf(line,"%d %d %d",&temp, &(temp_node.x),&(temp_node.y));
       node_array[temp] = temp_node;
       i ++;  
     }
  }

  for(i = 0;i < nodes;i++)
  {
    node_array[i].point = 0;
  }

  i = 0;

  while(i< edges){
    if(fgets(line,120,fr) != NULL)
    {
        sscanf(line,"%d %d",&temp,&temp_edge);
        node_array[temp].edge[node_array[temp].point] = temp_edge;
        node_array[temp].point ++;
          
        node_array[temp_edge].edge[node_array[temp_edge].point] = temp;
        node_array[temp_edge].point ++;

        ori_temp = temp;

      
    }
    i++;
  }

  int j;
  int k;
  int l;
  // sort the edges

  for(i = 0;i < nodes;i ++)
  {
    for(j = 0; j < node_array[i].point;j++){
      for(k = 1;k < node_array[i].point; k++){
        temp = node_array[i].edge[k];
        l = k - 1;
        while(temp < node_array[i].edge[l] && l >= 0)
        {
          node_array[i].edge[l + 1] = node_array[i].edge[l];
          l = l - 1;
        }
        node_array[i].edge[l + 1] = temp;

      }
    }
  }

/*
  for(i = 0;i < nodes;i++)
  {
    printf("%d: ", i);
    for(j = 0; j < node_array[i].point;j++){
      printf("%d ", node_array[i].edge[j]);
    }
    printf("\n");
  }
  
*/

  fclose(fr);

  
  int query_num = 0;
  //read the query file
  FILE *fr2 = fopen(queries, "rt");
  if(fgets(line, 80, fr2) != NULL){
    sscanf(line,"%d",&query_num);
  }

  struct query *query_array = malloc(sizeof(struct query)*query_num);

  for(i = 0;i < query_num;i++)
  {
    if(fgets(line, 80, fr2) != NULL){
       struct query temp_query;
       sscanf(line,"%d %d",&(temp_query.start),&(temp_query.dest));
       query_array[i] = temp_query;
       //printf("%d %d\n",query_array[i].start,query_array[i].dest);

     }
  }
  fclose(fr2);

  //find the shortest path
  int temp_start;
  int temp_dest;
  for(i = 0;i < query_num; i ++){
    temp_start = query_array[i].start;
    temp_dest = query_array[i].dest;
    findpath(temp_start,temp_dest,node_array,nodes);    
  }

  return 0;

}

void findpath(int temp_start,int temp_dest,struct node node_array[],int node_size){
  int i;
  int Q_array[node_size];
  for (i=0;i<node_size;i++)
  {
    if(i != temp_start)
    {
      node_array[i].distance = 100000;
      

    }
    else
    {
      node_array[i].distance = 0;

    }
    node_array[i].prev = -1;
    Q_array[i] = i;
    //printf("good1\n");
  }

  dijkstra(temp_start,node_array,node_size,Q_array, temp_dest);
}

void dijkstra(int temp_start,struct node node_array[],int node_size,int Q_array[], int dest)
{
  int pathnode[1000];

  int temp_size = node_size;
  while(temp_size != 0){
  int u = findmin(node_array, Q_array,node_size);
  Q_array[u] = -1;
  //printf("Now working on point: %d\n-----------------%d nodes remainning in Q-----------------\n",u,temp_size );
  temp_size--;
  int v,i;
  double alt;
  
  
  for(i = 0;i < 1000;i++)
  {
    pathnode[i] = -1;
  }

  for(i = 0 ; i < node_array[u].point ; i++){

    v = node_array[u].edge[i];
    //printf("Current processing node #%d of %d\n", v, node_array[u].point);
    alt = calculate(node_array[u],node_array[v]) + node_array[u].distance;
    if(alt < node_array[v].distance)
    {
      node_array[v].distance = alt;
      node_array[v].prev = u;
    }   
  }
  //printf("good\n");
  }

  //

  double result = node_array[dest].distance;
  printf("%d\n",(int)result);
  int temp = dest;
  int k = 0;
  
  
  while(temp != -1)
  {
    //printf("%d ",temp);
    pathnode[k] = temp;
    temp = node_array[temp].prev;
    k++;
  }
  int j;
  //printf("path: ");
  
  for(j = k - 1;j >= 0;j--)
  {
    printf("%d ",pathnode[j]);
  }

  printf("\n");
  
}

int findmin(struct node node_array[], int Q_array[], int node_size){
  double min = 1000000000;
  int result;
  int i;
  
  for(i = 0 ; i < node_size ;i++){
    if(Q_array[i] != -1){
      if(node_array[i].distance < min){
        min = node_array[i].distance;
        //printf("-=-=-=%d-=-=-=\n",i);
        result = i;
      }
    }
  }
  //printf("=======%d========\n",result);
  return result;
}

 

 //printf("%d %d\n",nodes,edges);

