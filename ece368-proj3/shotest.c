#include <stdio.h>
#include <math.h>
#include <stdlib.h>

struct node
{
   int x;
   int y;
   int edge[10];
   int point;
   int prevnode;
   double distance;

};

struct query
{
   int start;
   int dest;
   int path[10];
};


double calculate(struct node a, struct node b){
   return sqrt( (a.x-b.x)*(a.x-b.x)+(a.y-b.y)*(a.y-b.y) );

}
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
char line[80];

//Create map
if(fgets(line, 80, fr) != NULL){
  sscanf(line,"%d %d",&nodes,&edges);
//printf("%d %d\n",nodes,edges);
}

struct node *node_array = malloc(sizeof(struct node)*nodes);
while(i < nodes){
    if(fgets(line, 80, fr) != NULL){
       struct node temp_node;
       sscanf(line,"%d %d %d",&temp, &(temp_node.x),&(temp_node.y));
       node_array[temp] = temp_node;
       i ++;  
   }
}

for(i = 0;i < nodes;i++)
{
   node_array[i].point = 0;
//node_array[i].distance = 32767;
}

i = 0;

while(i< edges){
   if(fgets(line,80,fr) != NULL)
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


for(i = 0;i < nodes;i++)
{
//printf("%d: ", i);
   for(j = 0; j < node_array[i].point;j++){
//printf("%d ", node_array[i].edge[j]);
   }
//printf("\n");
}


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


//int distance = calculate(node_array[1],node_array[2]);
//printf("%d\n", distance);

//printf("%d %d\n",nodes,edges );
//for(i = 0; i < nodes ; i++){
//printf("current node %d \n", i);
   //for(j = 0;j < node_array[i].point; j++ ){
    //printf("%d", node_array[i].edge[j]);
  //printf("adjacent %d \n", node_array[i].edge[j] );
  //printf("distance %d --> %d ", i, node_array[i].edge[j]);
      //node_array[i].distance[j] = calculate(node_array[i],node_array[node_array[i].edge[j]]);
  //printf("%f\n",node_array[i].distance[j]);
  //}
//} 
    int current_node;
    int reachable[nodes][nodes];
    for(i = 0; i <nodes; i++){
        for(j = 0;j< nodes;j++){
            reachable[i][j] = 0;
        }
    }
    for (i = 0; i < nodes;i++)
    {
        for(j = 0;j < node_array[i].point;j++)
        {
            reachable[i][node_array[i].edge[j]] = 1;
        }
    }
    for(i = 0;i < nodes;i++){
        for( j = 0;j< nodes;j++){
            printf("%d ", reachable[i][j]);
        }
        printf("\n");
    }
    for(i = 0; i < query_num; i++){
        current_node = query_array[i].start;
        for(j = 0; j < nodes; j++){
            node_array[j].distance = 32767;
        }
        k = i;
    }



}






