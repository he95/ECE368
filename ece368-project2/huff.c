#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include "huff.h"


/* print huffman code for every char */
void printf_Huffman_Tree()
{
	int i;
	MY_BYTE ch1,ch2;

	printf("char\tfrequency\thuffman_code\n");
	for (i=0;i<256;i++)
	{
		if (g_myHTree[i].weight>0)
		{
			if ( g_myHTree[i].bytes[0] == g_transStruct.reserve_char)
			{
				if ( i>= 0x20)
					printf("[\'%c\']",i);
				else
					printf("[%02x]",i);

			}
			else
			{
				ch1=g_myHTree[i].bytes[0];	
				ch2=g_myHTree[i].bytes[1];	
				if (ch1 >= ' ' && ch2>= ' ')
				{
					printf("[\'%c%c\']",ch1,ch2);
				}
				else
				{
					printf("[%02x %02x]",ch1,ch2);
				}
			}
			printf("\t%10d\t%s\n",(int)g_myHTree[i].weight,g_myHTree[i].huffman_code);
		}
	}
}

/* Create huffman tree base the content of buff, the buff length is len */
void createHuffmanTree(int mode, char *buff,int len)
{
	typedef struct
	{
		HM_ENCODE_NODE*  arr[256*2];
		int head;
		int tail;
	}SLICE_ARR;  
	
	SLICE_ARR HT_Arr;
	int new_node_idx,i,j,i1,i2;
	int new_len;	//some of double byte be converted to single char, so the new_len is less than len
	int free_coding_count;	//not used char count,the every free char can denote a double byte combination
	int find_reserve_char;
	char ch = 0;
	MY_BYTE reserve_char = '\0';
	
	memset(g_myHTree,0,sizeof(g_myHTree));
	for (i=0;i<256;i++)
		g_myHTree[i].parent=NIL_PT;

	for (i=0;i<len;i++)
	{
		ch=buff[i];
		g_myHTree[(int)ch].weight++;
	}
	
	find_reserve_char=0;
	free_coding_count=0;
	for (j=0,i=0;i<256;i++)
	{
		if ( g_myHTree[i].weight==0 )
		{
			free_coding_count++; //not used char
			if ( !find_reserve_char)
			{
				reserve_char=(MY_BYTE)i;
				find_reserve_char=1;
			}
		}
	}
	
	if ( mode==2 && free_coding_count >=FREE_CODE_LOW_LIMIT && len>4096)
	{
		g_transStruct.reserve_char=reserve_char;
		g_transStruct.coding_space= free_coding_count-1;
		
		//some of double bytes be converted to a char, so the buff length be changed as new_len
		new_len=map_2bytes_to_single_byte((MY_BYTE *)buff,len,&g_transStruct);	
		g_transStruct.encode_mode=2;
	}
	else
	{
		g_transStruct.encode_mode=1;
		g_transStruct.reserve_char=0;
	}
	

	for (j=0,i=0;i<256;i++)
	{
		if (g_myHTree[i].weight>0)
			HT_Arr.arr[j++]= &(g_myHTree[i]); //put the address of element in Huffman Array to array HT_Arr
	}
	
	HT_Arr.head=0;
	HT_Arr.tail=j-1;
	qsort( HT_Arr.arr,SLICE_ARRAY_SIZE(HT_Arr),sizeof(HM_ENCODE_NODE*),cmp_by_weight);	 //sort array order by weight

	j=257; //the position 256 is reserved for root pointer
	while ( SLICE_ARRAY_SIZE(HT_Arr)>=2)
	{
		i1=HT_Arr.head;
		i2=HT_Arr.head+1;
		if (SLICE_ARRAY_SIZE(HT_Arr)==2)  //the new node is root node
			new_node_idx=256;			//keep the root node is the first place
		else
			new_node_idx=j++; 

		g_myHTree[new_node_idx].weight=  HT_Arr.arr[i1]->weight +HT_Arr.arr[i2]->weight;
		g_myHTree[new_node_idx].l_child= HT_Arr.arr[i1] - g_myHTree;
		g_myHTree[new_node_idx].r_child= HT_Arr.arr[i2] - g_myHTree;
		g_myHTree[new_node_idx].parent=NIL_PT;

		HT_Arr.arr[i1]->parent=HT_Arr.arr[i2]->parent=new_node_idx;
		HT_Arr.head+=2;  //remove 2 element from HT_Arr in front

		//insert the new element to proper position
		i=HT_Arr.tail;
		while (i>=HT_Arr.head &&  HT_Arr.arr[i]->weight > g_myHTree[new_node_idx].weight)  
		{
			HT_Arr.arr[i+1] =HT_Arr.arr[i] ;
			i--;
		}  
		HT_Arr.arr[i+1] =  &(g_myHTree[new_node_idx]); //Insert this element in postion i+1
		HT_Arr.tail++; 
	}
}

void build_huffman_code()
{
	char code[256];
	char *pt;
	int i,j,pos,up;
	int total_char_count=0;
	int total_code_size=0;
	
	//The first scan
	for (i=0;i<256;i++)
	{
		if (g_myHTree[i].weight>0)
		{
			total_char_count++;
			j=i;
			while ( g_myHTree[j].parent !=NIL_PT)
			{
				j=g_myHTree[j].parent;
				total_code_size++;
			}
		}
	}
	
	if ( g_myHM_code_buff!=NULL)
	{
		free(g_myHM_code_buff); g_myHM_code_buff=NULL;
	}
	g_myHM_code_buff=(char *)malloc(total_char_count + total_code_size); //each of string need a terminal char \0

	//The second scan
	pt=g_myHM_code_buff;
	for (i=0;i<256;i++)
	{
		if (g_myHTree[i].weight>0)
		{
			code[sizeof(code)-1]=0;	//the last char be set as 0
			pos=sizeof(code)-2;		//put '0' or '1' to code order by from right to left
			j=i;
			while ( g_myHTree[j].parent !=NIL_PT)
			{
				up= g_myHTree[j].parent;
				if ( g_myHTree[up].l_child==j)
					code[pos--]='0'+LEFT_CHILD;
				else
					code[pos--]='0'+RIGHT_CHILD;
				j=g_myHTree[j].parent;
			}
			strcpy(pt,code+pos+1);
			g_myHTree[i].huffman_code=pt;
			pt+= strlen(pt)+1;
		}
	}
}

void Encoding(int mode,int showHuffCode,char *inFile,char *outFile)
{
	FILE *myfp1=NULL;
	FILE *myfp2=NULL;
	MY_UINT32 byteCount;
	char *inBuff=NULL;
	char *outBuff=NULL;
	
	myfp1=fopen(inFile,"rb");
	if (myfp1==NULL)
	{
		printf("can not open file %s\n",inFile);
		goto thisExit;
	}

	fseek(myfp1,0,SEEK_END);
	byteCount=ftell(myfp1);
	inBuff=(char *)malloc(byteCount);
	fseek(myfp1,0,SEEK_SET);
	fread(inBuff,byteCount,1,myfp1);
	fclose(myfp1); myfp1=NULL;

	my_init_transStruct();
	createHuffmanTree(mode,inBuff,byteCount);
	build_huffman_code();
	
	if (showHuffCode )
		printf_Huffman_Tree();
	{
		MY_UINT32 i,bitIdx;
		MY_UINT32 bitCount=0;
		MY_UINT32 nonLeafNode=0;	//non leaf node count
		MY_UINT32 convert_tab_len=0;
		char buff[4];

		if (g_transStruct.encode_mode==2)
		{
			i=256-1;
			while ( g_myHTree[i].weight==0) 
				i--;
			convert_tab_len=i+1;
		}

		for (i=0;i<256*2;i++)
		{
			if ( g_myHTree[i].weight>0)
			{
				if (i<256)  //this node is leaf node
					bitCount += (g_myHTree[i].weight * strlen(g_myHTree[i].huffman_code));
				else        //this node is non-leaf node
					nonLeafNode++;	
			}
		}

		//------------------------------------------------------
		myfp2=fopen(outFile,"wb");
		if (myfp2==NULL)
		{
			printf("can not create file %s\n",outFile);
			goto thisExit;
		}
		
		//write file header, total 16 bytes
		fwrite("HUFF",4,1,myfp2);			//File flag
		fwrite(&nonLeafNode,4,1,myfp2);	//Huffman table node count
		fwrite(&byteCount,4,1,myfp2);		//The source file byte count
		fwrite(&bitCount,4,1,myfp2);		//The bit count of huffman code
		fwrite(&convert_tab_len,4,1,myfp2);		//The 1 byte code to 2-bytes convert table length
		
		memset(buff,0,sizeof(buff));
		buff[0]= g_transStruct.reserve_char;
		fwrite(buff,4,1,myfp2);	//The byte count of huffman code

		for (i=0;i<convert_tab_len;i++)
		{
			fwrite(g_myHTree[i].bytes,2,1,myfp2);	//The byte count of huffman code
		}

		//write huffman table,total nonLeafNode*3 bytes
		for (i=256;i<256*2;i++)
		{
			if ( g_myHTree[i].weight>0)
			{
				buff[0]=0;
				if  ( g_myHTree[i].l_child <256) //the left node is leaf node
					buff[0] |=LEFT_CHILD_MASK;
				
				if  ( g_myHTree[i].r_child <256) //the right node is leaf node
					buff[0] |=RIGHT_CHILD_MASK;

				buff[1]=(MY_BYTE)(g_myHTree[i].l_child & 0xff);
				buff[2]=(MY_BYTE)(g_myHTree[i].r_child & 0xff);
				fwrite(buff,3,1,myfp2);	//The byte count of huffman code
			}
		}

		//write huffman bit data,total (bitCount+7)/8 byte
		outBuff=(char *)malloc((bitCount+7)/8);
		memset(outBuff,0,(bitCount+7)/8);

		i=bitIdx=0;
		while (1)
		{
			MY_BYTE ch;
			MY_BYTE *pHuffmanCode;
			
			ch=inBuff[i++];
			pHuffmanCode=(MY_BYTE *)g_myHTree[ch].huffman_code;
			while ( *pHuffmanCode>0)
			{
				if ( *pHuffmanCode++ =='1')
					outBuff[ bitIdx>>3] |= g_maskArray[ bitIdx & 7];
				bitIdx++;
			}
			if (bitIdx>=bitCount)
				break;
		}
		fwrite(outBuff,(bitCount+7)/8,1,myfp2);	//The byte count of huffman code
		fclose(myfp2); myfp2=NULL;					//close file
	}
	thisExit:
	my_free_transStruct();
	if (myfp1!=NULL)
	{
		fclose(myfp1); myfp1=NULL;
	}
	if (inBuff!=NULL)
	{
		free(inBuff); inBuff=NULL;
	}

	if (myfp2!=NULL)
	{
		fclose(myfp2); myfp2=NULL;
	}
	if (outBuff!=NULL)
	{
		free(outBuff); outBuff=NULL;
	}
}


int main(int argc, char* argv[])
{
	int mode=1;
	int bPrintHashcode=2;
	
	char zipFile[260];
	char srcFile[260];

	if ( argc!=2)
	{
		printf("Usage:\n  ./huff [FILENAME] \n");
		return 1;
	}
	sprintf(srcFile, "%s", argv[1]);
	sprintf(zipFile, "%s.huff", argv[1]);
	
	Encoding(mode,bPrintHashcode,srcFile, zipFile);
	printf("Encoded\n");
	return 0;
}
