#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h> 

#define INFINITE 10000000
#define true 1


struct node
{
	int x;
	int y;
	int edge[10];
	int point;
};

struct query
{
	int start;
	int dest;
	int path[10];
};

struct node *node_array_final;
struct MinPathNode
{
	int prevNode;
	int Node;
};

int MAX_VERTEX_NUM=0;
int startpoint=-1;
////////length array keep first times (startpoint to every point length);lenght1 array keep temp length
double* length1;
double* length;
double* templength;
double calculate(struct node a, struct node b)
{
	return sqrt( (a.x-b.x)*(a.x-b.x)+(a.y-b.y)*(a.y-b.y) );
}
///////calculate point_pos to every point length;
void CountLength(int point_pos)
{
	int i,IsEageId;
	int edgeNum=node_array_final[point_pos].point;
	for (i=0;i<MAX_VERTEX_NUM;i++)
	{
	  templength[i]=INFINITE;
	}
	for(i=0;i<edgeNum;i++)
	{
		IsEageId=node_array_final[point_pos].edge[i];
		templength[IsEageId]=calculate(node_array_final[point_pos],node_array_final[IsEageId]);
	//	printf("\n开始到节点%d,Path为:%f",IsEageId,templength[IsEageId]);
	}
	if (point_pos==startpoint)
	{
		memcpy(length,templength,MAX_VERTEX_NUM*sizeof(double));
	}else
	{
		memcpy(length1,templength,MAX_VERTEX_NUM*sizeof(double));
	}
}

void dijkstra(int temp_start,int temp_dest)
{
	if(temp_start==temp_dest) 
	{
		printf("Shortest distance%d\n",0);
		return;
	}
	double minlength=INFINITE;
	int locationmin;
	int* final=(int *)malloc(sizeof(int)*MAX_VERTEX_NUM);
	int i=0,j=0,k=0;

    struct MinPathNode*  pathNode=(struct MinPathNode *)malloc(sizeof(struct MinPathNode)*MAX_VERTEX_NUM);
	startpoint=temp_start;
	///////////////////////////init node info
	for(j=0; j<MAX_VERTEX_NUM;j++)
	{
		final[j] = 0;
	}
	for (j=0;j<MAX_VERTEX_NUM;j++)
	{

		pathNode[j].prevNode=-1;
		
		pathNode[j].Node=j;
	}
	///////////////////////////init node info
	length=(double *)malloc(sizeof(double)*MAX_VERTEX_NUM);
    templength=(double *)malloc(sizeof(double)*MAX_VERTEX_NUM);
	CountLength(temp_start);	///////calculate startpoint to every point length;
	for(i=0;i<MAX_VERTEX_NUM;i++)
	{
		if (length[i]<minlength)
		{
			minlength=length[i];
			locationmin=i;
		}
	}
	if (minlength==INFINITE)
	{
		printf("no path!\n");
		return;
	}
	final[locationmin]=1;/////////
    pathNode[0].Node=locationmin;//////keep min path node
	pathNode[0].prevNode=-1;///////keep init path node's prevnode
	length1=(double *)malloc(sizeof(double)*MAX_VERTEX_NUM);
	while (locationmin!=temp_dest)
	{
		minlength=INFINITE;
		for (j=0;j<MAX_VERTEX_NUM;j++)
		{
			if (length[j]!=INFINITE)
			{
				for (k=0;k<MAX_VERTEX_NUM;k++)
				{
                   CountLength(j);//////
				   if ((length[j]+length1[k])<length[k])/////
				   {
				   	length[k]=length[j]+length1[k];
				   	pathNode[k].prevNode=j;
				   	pathNode[k].Node=k;
				   }
				}

			}
		}

		for (i=0;i<MAX_VERTEX_NUM;i++)
		{
			if ((0==final[i])&&(length[i]<=minlength))
			{
				minlength=length[i];
				locationmin=i;
			}
		}
		if (minlength==INFINITE)
		{
			printf("no path!\n");
			return;
		}
		final[locationmin]=1;
		//printf("\n已包入S集合中：%d,",locationmin);
	}
	////////////////////////output min path info
	free(length1);
	free(length);
	free(final);
	printf("Path%d ",temp_dest);
	int temp=temp_dest;
	while(1)
	{
		if (temp==-1)
		{
			printf("%d ",temp_start);
			break;
		}
		for (i=0;i<MAX_VERTEX_NUM;i++)
		{
			if (temp==pathNode[i].Node)
			{
				int tempNode=pathNode[i].prevNode;
				if (tempNode!=-1)
				{
					printf("%d ",pathNode[i].prevNode);
				}
				break;
			}
		}
		temp=pathNode[i].prevNode;
	}
	printf("Shortest distance%f\n",minlength);
	free(pathNode);
	////////////////////////output min path info

}

int main(int argc, char** argv)
{
	
	if(argc != 3)
	{
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
	if(fgets(line, 80, fr) != NULL) 
	{
		sscanf(line,"%d %d",&nodes,&edges);
		//printf("%d %d\n",nodes,edges);
	}
	MAX_VERTEX_NUM=nodes;
	struct node *node_array = (struct node *)malloc(sizeof(struct node)*nodes);
	while(i < nodes)
	{
		if(fgets(line, 80, fr) != NULL)
		{
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
	
	while(i< edges)
	{
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
		for(j = 0; j < node_array[i].point;j++)
		{
			for(k = 1;k < node_array[i].point; k++)
			{
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
		printf("%d: ", i);
		for(j = 0; j < node_array[i].point;j++)
		{
			printf("%d ", node_array[i].edge[j]);
		}
		printf("\n");
	}
	
	
	fclose(fr);
	node_array_final=node_array;
	
	int query_num = 0;
	int temp_start=0;
	int temp_dest=0;
	char line1[80]="";
	//read the query file
	FILE *fr2 = fopen(queries, "rt");
	if(fgets(line1, 80, fr2) != NULL)
	{
		sscanf(line1,"%d",&query_num);
	}
	
	struct query *query_array = (struct query *)malloc(sizeof(struct query)*query_num);
	
	for(i = 0;i < query_num;i++)
	{
		if(fgets(line1, 80, fr2) != NULL)
		{
			struct query temp_query;
			sscanf(line1,"%d %d",&(temp_query.start),&(temp_query.dest));
			query_array[i] = temp_query;
		//	printf("query_array:%d %d\n",query_array[i].start,query_array[i].dest);
			
		}
	}
	fclose(fr2);
	for(i = 0;i < query_num; i ++)
	{
		printf("query_array:%d %d\n",query_array[i].start,query_array[i].dest);
		temp_start = query_array[i].start;
		temp_dest = query_array[i].dest;
		dijkstra(temp_start,temp_dest);
	//	printf("\n%d,",i);
	}
	free(node_array);
	free(query_array);

	return 0;
}

