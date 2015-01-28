#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include "huff.h"

void Decoding(char *huffmanFile,char *oriFile)
{

	FILE *myfp1=NULL;
	FILE *myfp2=NULL;
	
	MY_UINT32 *pDWORD;
	MY_UINT32 i,nonLeafNode;	//nonLeafNode, the non leaf node in huffman tree
	MY_UINT32 bitCount;		//the bit count of huffman code
	MY_UINT32 bitIdx;
	MY_UINT32 oriByteCount;	//original file byte count
	MY_UINT32 convert_tab_len;
	MY_BYTE currNode;			//the index of next node or char ASCII 
	MY_BYTE reserve_byte;
	MY_BYTE convert_tab[256*2];

	HM_DECODE_NODE huffmanTree[256];  //huffmanTree, only the non-leaf node be store in this array
	char buff[32];
	
	char *huffmanData=NULL;
	char *outBuff=NULL;
	
	myfp1=fopen(huffmanFile,"rb");
	if (myfp1==NULL)
	{
		printf("can not open file %s\n",huffmanFile);
		goto thisExit;
	}
	
	//read file header, total 16 bytes
	fread(buff,24,1,myfp1);	//File flag
	if (strncmp(buff,"HUFF",4)!=0)
	{
		printf("The file %s is not huffman zip file\n",huffmanFile);
		goto thisExit;
	}
	pDWORD= (MY_UINT32*)(buff+4);	
	nonLeafNode=*(int*)pDWORD;
	pDWORD= (MY_UINT32*)(buff+8);	
	oriByteCount=*(int*)pDWORD;
	pDWORD= (MY_UINT32*)(buff+12); 
	bitCount=*(int*)pDWORD;
	pDWORD= (MY_UINT32*)(buff+16); 
	convert_tab_len=*(int*)pDWORD;
	reserve_byte=(MY_BYTE)(buff[20]);

	memset(convert_tab,reserve_byte,sizeof(convert_tab));
	if (convert_tab_len>0)
		fread((void *)convert_tab,convert_tab_len*2,1,myfp1);
	
	huffmanData=(char *)malloc((bitCount+7)/8);
	outBuff=(char *)malloc(oriByteCount);
	for (i=0;i<nonLeafNode;i++)
	{
		fread(buff,3,1,myfp1);	//File flag
		huffmanTree[i].flag =buff[0];
		huffmanTree[i].left =buff[1];
		huffmanTree[i].right =buff[2];
	}
	fread(huffmanData,(bitCount+7)/8,1,myfp1);
	fclose(myfp1); myfp1=NULL;

	for (i = 0, currNode = 0,bitIdx = 0; bitIdx<bitCount;bitIdx++)
	{
		MY_BYTE flag;
		MY_BYTE currBit;

		currBit=huffmanData[ bitIdx>>3] & g_maskArray[ bitIdx & 7];
		flag=huffmanTree[currNode].flag;
		if ( currBit )  //the current bit is 1, goto right branch
		{
			if ( !(flag & RIGHT_CHILD_MASK))
				currNode=huffmanTree[currNode].right; //
			else
			{
				//In this case, the right field is ASCII of huffman code
				int bytes=Write_1ByteOr2Byes( huffmanTree[currNode].right, convert_tab,reserve_byte,outBuff+i);
				i+=bytes;	
				currNode=0;
			}
		}
		else  //the current bit is 0, goto left branch
		{
			if (!(flag & LEFT_CHILD_MASK))
				currNode=huffmanTree[currNode].left;
			else
			{
				//In this case, the right field is ASCII of huffman code
				int bytes=Write_1ByteOr2Byes( huffmanTree[currNode].left, convert_tab,reserve_byte,outBuff+i);
				i+=bytes;
				currNode=0;
			}
		}
	}
	
	if (i != oriByteCount)
	{
		printf("Error data\n"); goto thisExit;
	}
	
	myfp2=fopen(oriFile,"wb");//open output file
	if (myfp2==NULL)
	{
		printf("can not create file %s\n",oriFile);
		goto thisExit;
	}

	fwrite(outBuff,oriByteCount,1,myfp2);
	fclose(myfp2); myfp2=NULL; //close output file
	

	thisExit:
	if (myfp1!=NULL)
		{	fclose(myfp1); myfp1=NULL; }
	
	if (huffmanData!=NULL)
		{		free(huffmanData); huffmanData=NULL;}

	if (myfp2!=NULL)
		{		fclose(myfp2); myfp2=NULL; }
	
	if (outBuff!=NULL)
		{	free(outBuff); outBuff=NULL; }

	if ( g_myHM_code_buff!=NULL)
		{	free(outBuff); g_myHM_code_buff=NULL; }
}

int main(int argc, char* argv[])
{
	char zipFile[260];
	char srcFile[260];

	if ( argc!=2)
	{
		printf("argc !=2 \n");
		return 1;
	}
	sprintf(zipFile, "%s", argv[1]);
	sprintf(srcFile, "%s.unhuff", argv[1]);
	
	Decoding(zipFile,srcFile);

	
	printf("Decoded\n");

	return 0;
}
