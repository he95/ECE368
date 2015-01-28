#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "sorting.h"

typedef struct sort_data{
	double compnum;
	double swapnum;
}sort_data;
  

long *Load_File(char *Filename, int *Size)
{
	FILE *fin = NULL;
	int retval = 0;
	fin = fopen(Filename,"r");

	if(!fscanf(fin,"%d",Size))
	{
		return NULL;
	}

	long * array = NULL;
	array = malloc(sizeof(long) * (*Size));

	int i;
	for(i = 0; i< *Size; i++)
	{
		retval = fscanf(fin, "%ld", array + i);
		if(retval != 1)
		{
			break;
		}
	}

	fclose(fin);
	return array;
}


int Save_File(char *Filename, long *Array, int Size)
{
    int i = 0;    
    FILE *fptr = fopen(Filename,"w");
    
    fprintf(fptr, "%d\n", Size);

    while( i < Size)
    {    
        fprintf(fptr, "%ld\n", Array[i]);   
        i++;
    }

    fclose(fptr);
    return i;
}



double pow_(double base, int power)
{
	double result = 1;
	int i;
	for(i = 1; i <= power; i++)
	{
		result = result * base;
	}
	return result;
}

int * getSeq1(int N,int * size)
{
	int i = 0;
	int temp = 1;
	while(temp < N)
	{
		temp = temp * 2;
		i ++;
	}
	i--;	
	//printf(" i =%d\n", i);
  	int power = i;
  	*size = (power + 2) * (power + 1) / 2;
  	int j,k;
  	int * Seq1= malloc(sizeof(int) * (*size));
  	k = 0;
  	 // k = 0;
  	for (j = power; j >= 0; j--){
    	for (i = j; i >= 0; i--){
    		if((pow_(2, i)) * (pow_(3, j-i))< N && (pow_(2, i)) * (pow_(3, j-i)) > 0)
    		{
      			Seq1[k] = (pow_(2, i)) * (pow_(3, j-i));
      			k++;
      		}
    	}
  	}
	//int k;
	//int temp;
	*size = k;
	for(j = 1; j< *size;j++)
	{
		k = j;
		temp = Seq1[j];
		while((k > 0) && (temp > Seq1[k - 1]))
		{
			//temp = Seq1[k];
			Seq1[k] = Seq1[k - 1];
			//Seq1[k-1] = temp;
			k--;
		}
		Seq1[k] = temp;
	}
	
	return Seq1;
}


long int seq2gen(int value)
{
	if (value == 1){
		return 1;
	}
	//printf("seq2%f\n",pow_(1.3,value) );
	return(pow_(1.3,value));
}


int * getSeq2(int N, int * size)
{
	int i = 1;
	//int temp;
	//printf("entered\n");
	//printf("size =  %d\n", *size);
	int temp = N;
	//printf("entered\n");
	while(temp / 1.3 > 1)
	{
		temp = temp / 1.3;
		i++;
	}
	i++;
	//printf("entered\n");
	*size = i;

	temp = N;
	int j;
	int *seq = malloc(sizeof(int) * i);
	for (j = i; j > 0; j--)
	{
		seq[i - j] = temp / 1.3;
		temp = temp / 1.3;
		if((seq[i - j] == 9) || (seq[i - j] == 10))
		{
			seq[i - j] = 11;
		}
		//printf("seq2[%d]%d\n",i-j,seq[i-j] );
	}

	return seq;
}


void Save_Seq1 (char *Filename, int N)
{
	FILE * fout = fopen(Filename,"w");
	int size = 0;
	int *Seq1 = NULL;
	Seq1 = getSeq1(N,&size);
	int i ;
	for(i = 0; i < size ; i ++)
	{
		fprintf(fout, "%d\n", Seq1[i]);
		//printf("%d\n", Seq1[i]);
	}
	fclose(fout);
	free(Seq1);

	return;
}

void Save_Seq2 (char *Filename, int N)
{
	FILE * fout = fopen(Filename,"w");
	int size = 0;
	int *Seq2 = NULL;
	Seq2 = getSeq2(N,&size);
	int i;
	for(i = 0; i < size - 2; i++)
	{
		fprintf(fout, "%d\n", Seq2[i]);
	}
	fclose(fout);
	free(Seq2);

	return;
}

sort_data insertionsort(long * arr, int size, int gap)
{
	int i,j,k;
	long int temp;

	sort_data index;
	index.compnum = 0;
	index.swapnum = 0;
	for(k = 0; k < gap; k++)
	{
		for (i = gap + k; i < size; i += gap)
		{
			temp = arr[i];
			for(j = i;(j - gap) >= 0; j -= gap)
			{
				index.compnum++;
				if(temp < arr[j - gap])
				{
					/* 
					printf("arr[%d] = %ld\n", j,arr[j]);
					printf("arr[%d] = %ld\n", j-gap,arr[j-gap]);
					*/
					arr[j] = arr[j - gap];
					index.swapnum ++;
					//printf("swapnum = %f \n", index.swapnum);
				}else{
					break;
				}
			}
			arr[j] = temp;
			//index.swapnum++;
		}
	}
	return index;
}

sort_data shellsortinsertion(long * arr, int size, int seq[],int seq_size)
{
	long int i;
	sort_data index,temp;
	index.compnum = 0;
	index.swapnum = 0;
	for (i = 0; i < seq_size; i++){
		temp = insertionsort(arr, size,seq[i]);
		//printf("tempswapnum = %f \n", temp.swapnum);
		index.compnum = index.compnum + temp.compnum;
		index.swapnum = index.swapnum + temp.swapnum;
		//printf("swapnum = %f \n", index.swapnum);
		//printf("compnum = %f \n", index.compnum);
	}

	return index;
}

sort_data bubblesort(long * arr, int size, int gap)
{
	int i,j,k;
	long int temp;
	

	sort_data index;
	index.swapnum = 0;
	index.compnum = 0;
	for( k = 0; k < gap; k++){
		for (i = k + gap; i < size; i += gap){
			
			for( j = gap; j < size; j += gap){
				index.compnum++;	
				//fprintf(stderr,"increased\n");
				//fprintf(stderr,"abcd2\n");
				//fprintf(stderr,"array[%d] = %ld\n",j,arr[j] );
				//fprintf(stderr,"array[%d] = %ld\n",j-gap,arr[j-gap] );
				//fprintf(stderr, "gap = %d\n", gap);
				//fprintf(stderr,"abcd\n");
				if(arr[j] < arr[j - gap]){
					//printf("entered\n");
					temp = arr[j];
					arr[j] = arr[j - gap];
					arr[j - gap] = temp;
					index.swapnum = index.swapnum + 3;
					//flag = 1;
					//printf("swapnum = %f \n", index.swapnum);
					//printf("compnum = %f \n", index.compnum);
				}
			}
			//if(flag == 0){break;}
		}
	}
	
	return index;
}

sort_data shellsortbubble(long * arr, int size, int seq[],int seq_size)
{
	int i;
	sort_data index,temp;
	index.compnum = 0;
	index.swapnum = 0;

	for(i = 0; i < seq_size; i++)
	{

		temp = bubblesort(arr, size, seq[i]);
		index.compnum += temp.compnum;
		index.swapnum += temp.swapnum;
		//printf("swapnum = %f \n", index.swapnum);
		//printf("compnum = %f \n", index.compnum);
	}
	return index;
}

void Shell_Insertion_Sort(long *Array, int Size, double *N_Comp, double *N_Move)
{
	int * seq = NULL;
	int seq_size = 0;
	seq = getSeq1(Size, &seq_size);
	sort_data index;
	index = shellsortinsertion(Array,Size,seq,seq_size);
	*N_Comp = index.compnum;
	*N_Move = index.swapnum;

	return;
}
void Improved_Bubble_Sort(long *Array, int Size, double *N_Comp, double *N_Move)
{
	int * seq = NULL;
	int seq_size = 0;
	//printf("entered\n");
	seq = getSeq2(Size, &seq_size);
	sort_data index;
	index = shellsortbubble(Array, Size, seq, seq_size);
	*N_Move = index.swapnum;
	*N_Comp = index.compnum;

	return;
}













